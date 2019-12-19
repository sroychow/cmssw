#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <optional>
#include <iostream>
#include <tinyxml2.h>

namespace {

  // join vector of strings to one string
  std::string join(const std::vector<std::string>& vec, const char* delim) {
    std::stringstream res;
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<std::string>(res, delim));
    return res.str();
  }

  struct LHEWeightInfo {
    std::string id;
    std::string text;
    std::optional<std::string> group = std::nullopt;
  };

  std::vector<LHEWeightInfo> getLHEWeightInfos(LHERunInfoProduct const& lheInfo) {
    std::vector<LHEWeightInfo> out;

    for (auto iter = lheInfo.headers_begin(), end = lheInfo.headers_end(); iter != end; ++iter) {
      if (iter->tag() != "initrwgt") {
        continue;
      }

      tinyxml2::XMLDocument xmlDoc;
      xmlDoc.Parse(("<root>" + join(iter->lines(), "") + "</root>").c_str());
      tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("root");

      for (auto* e = root->FirstChildElement(); e != nullptr; e = e->NextSiblingElement()) {
        if (strcmp(e->Name(), "weight") == 0) {
          // we are here if there is a weight that does not belong to any group
          std::string text = "";
          if (e->GetText())
            text = e->GetText();
          out.push_back({e->Attribute("id"), text});
        }
        if (strcmp(e->Name(), "weightgroup") == 0) {
          std::string groupName = e->Attribute("name");
          for (auto* inner = e->FirstChildElement("weight"); inner != nullptr;
               inner = inner->NextSiblingElement("weight")) {
            // we are here if there is a weight in a weightgroup
            std::string text = "";
            if (inner->GetText())
              text = inner->GetText();
            out.push_back({inner->Attribute("id"), text, groupName});
          }
        }
      }
    }

    return out;
  }

}  // namespace

class LHEWeightsTableProducer : public edm::global::EDProducer<edm::RunCache<std::vector<LHEWeightInfo>>> {
public:
  LHEWeightsTableProducer(edm::ParameterSet const& params)
      : lheInputTag_(params.getParameter<edm::InputTag>("lheInfo")),
        lheToken_(consumes<LHEEventProduct>(params.getParameter<edm::InputTag>("lheInfo"))),
        weightgroups_(params.getParameter<std::vector<std::string>>("weightgroups")),
        lheWeightPrecision_(params.getParameter<int32_t>("lheWeightPrecision")) {
    consumes<LHERunInfoProduct, edm::InRun>(lheInputTag_);
    produces<std::vector<nanoaod::FlatTable>>();
  }

  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {
    // tables for LHE weights, may not be filled
    auto const& lheInfo = iEvent.get(lheToken_);

    auto lheWeightTables = std::make_unique<std::vector<nanoaod::FlatTable>>();
    auto const& weightInfos = *runCache(iEvent.getRun().index());

    double w0 = lheInfo.originalXWGTUP();

    int i = 0;
    if (lheInfo.weights().size() != weightInfos.size()) {
      throw cms::Exception("LogicError", "Weight labels don't have the same size as weights!\n");
    }
    std::unordered_map<std::string, std::pair<std::string, std::vector<float>>> groupsWithWeights;
    for (auto const& weight : lheInfo.weights()) {
      auto& val = weightInfos[i].group ? groupsWithWeights[*weightInfos[i].group] : groupsWithWeights["ungrouped"];
      if(val.first.empty()) {
          val.first += ";id,text";
      }
      val.first += ";" + weightInfos[i].id + "," + weightInfos[i].text;
      val.second.push_back(weight.wgt / w0);
      ++i;
    }
    for (auto const& group : groupsWithWeights) {
      if(std::find(weightgroups_.begin(), weightgroups_.end(), group.first) == weightgroups_.end()) {
          continue;
      }
      std::string name = std::string("LHEWeight_") + group.first;
      std::transform(name.begin(), name.end(), name.begin(), [](char ch) { return ch == ' ' ? '_' : ch; });
      std::string doc = group.first + " (w_var / w_nominal)" + group.second.first;
      lheWeightTables->emplace_back(group.second.second.size(), name, false);
      lheWeightTables->back().addColumn<float>(
          "", group.second.second, doc, nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);
    }

    iEvent.put(std::move(lheWeightTables));
  }

  std::shared_ptr<std::vector<LHEWeightInfo>> globalBeginRun(edm::Run const& iRun,
                                                             edm::EventSetup const&) const override {
    edm::Handle<LHERunInfoProduct> lheInfo;

    auto weightChoice = std::make_shared<std::vector<LHEWeightInfo>>();

    // getByToken throws since we're not in the endRun (see https://github.com/cms-sw/cmssw/pull/18499)
    iRun.getByLabel(lheInputTag_, lheInfo);
    if (lheInfo.isValid()) {
      getLHEWeightInfos(*lheInfo).swap(*weightChoice);
    }

    return weightChoice;
  }

  // nothing to do here
  void globalEndRun(edm::Run const&, edm::EventSetup const&) const override {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("lheInfo", {"externalLHEProducer"})
        ->setComment("tag(s) for the LHE information (LHEEventProduct and LHERunInfoProduct)");
    desc.add<std::vector<std::string>>("weightgroups");
    desc.add<int32_t>("lheWeightPrecision", -1)->setComment("Number of bits in the mantissa for LHE weights");
    descriptions.addDefault(desc);
  }

protected:
  const edm::InputTag lheInputTag_;
  const edm::EDGetTokenT<LHEEventProduct> lheToken_;
  const std::vector<std::string> weightgroups_;
  int lheWeightPrecision_;
};

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(LHEWeightsTableProducer);
