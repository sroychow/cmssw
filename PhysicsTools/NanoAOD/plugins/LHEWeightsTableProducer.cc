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
#include "SimDataFormats/GeneratorProducts/interface/GenWeightInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenWeightProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"


#include <optional>
#include <iostream>
#include <tinyxml2.h>

namespace {
  struct WeightGroupData {
    size_t index;
    const gen::WeightGroupInfo* group;
  };

  typedef std::unordered_map<std::string, const WeightGroupData> WeightGroupsToStore;
}  // namespace

class LHEWeightsTableProducer : public edm::global::EDProducer<edm::RunCache<WeightGroupsToStore>> {
public:
  LHEWeightsTableProducer(edm::ParameterSet const& params)
      : lheInputTag_(params.getParameter<edm::InputTag>("lheInfo")),
        lheToken_(consumes<LHEEventProduct>(params.getParameter<edm::InputTag>("lheInfo"))),
        lheWeightToken_(consumes<GenWeightProduct>(params.getParameter<edm::InputTag>("lheInfo"))),
        //lheWeightInfoToken_(consumes<GenWeightInfoProduct, edm::InRun>(params.getParameter<edm::InputTag>("lheWeightInfo"))),
        weightgroups_(params.getParameter<std::vector<std::string>>("weightgroups")),
        lheWeightPrecision_(params.getParameter<int32_t>("lheWeightPrecision")) {
    consumes<LHERunInfoProduct, edm::InRun>(lheInputTag_);
    produces<std::vector<nanoaod::FlatTable>>();
  }

  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {
    // tables for LHE weights, may not be filled
    auto const& lheInfo = iEvent.get(lheToken_);

    edm::Handle<GenWeightProduct> lheWeightHandle;
    iEvent.getByToken(lheWeightToken_, lheWeightHandle);
    const GenWeightProduct* lheWeightProduct = lheWeightHandle.product();
    WeightsContainer lheWeights = lheWeightProduct->weights();

    auto lheWeightTables = std::make_unique<std::vector<nanoaod::FlatTable>>();
    auto const& weightInfos = *runCache(iEvent.getRun().index());

    double w0 = lheInfo.originalXWGTUP();

    size_t scaleGroupIndex = weightInfos.at("Scale").index;
    const gen::ScaleWeightGroupInfo* scaleGroupInfo_ = static_cast<const gen::ScaleWeightGroupInfo*>(weightInfos.at("Scale").group);

    std::vector<float> normalizedWeights;
    std::vector<double>& scaleWeights = lheWeights.at(scaleGroupIndex);

    // nano ordering of mur=0.5 muf=0.5 ; [1] is mur=0.5 muf=1 ; [2] is mur=0.5 muf=2 ; [3] is mur=1 muf=0.5 ; 
    // [4] is mur=1 muf=1 ; [5] is mur=1 muf=2 ; [6] is mur=2 muf=0.5 ; [7] is mur=2 muf=1 ; [8] is mur=2 muf=2 *
    normalizedWeights.push_back(scaleWeights.at(scaleGroupInfo_->muR05muF05Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroupInfo_->muR05muF1Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroupInfo_->muR05muF2Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroupInfo_->muR1muF05Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroupInfo_->centralIndex())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroupInfo_->muR1muF2Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroupInfo_->muR2muF05Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroupInfo_->muR2muF1Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroupInfo_->muR2muF2Index())/w0);

    lheWeightTables->emplace_back(normalizedWeights.size(), "LHEScaleWeight", false);
    lheWeightTables->back().addColumn<float>(
          "", normalizedWeights, scaleGroupInfo_->name(), nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);

    iEvent.put(std::move(lheWeightTables));
  }

  std::shared_ptr<WeightGroupsToStore> globalBeginRun(edm::Run const& iRun,
                                                             edm::EventSetup const&) const override {
    edm::Handle<LHERunInfoProduct> lheInfo;


    // getByToken throws since we're not in the endRun (see https://github.com/cms-sw/cmssw/pull/18499)
    iRun.getByLabel(lheInputTag_, lheInfo);
    //if (lheInfo.isValid()) {
    //}
    //iRun.getByToken(lheWeightInfoToken_, lheWeightInfoHandle);
    edm::Handle<GenWeightInfoProduct> lheWeightInfoHandle;
    iRun.getByLabel(lheInputTag_, lheWeightInfoHandle);

    // Should add a search by name function
    auto scaleGroupIndices = lheWeightInfoHandle->weightGroupIndicesByType(gen::kScaleWeights);
    size_t scaleGroupIndex = scaleGroupIndices.front();
    const gen::WeightGroupInfo* scaleGroupInfo = lheWeightInfoHandle->orderedWeightGroupInfo(scaleGroupIndex);
    WeightGroupsToStore weightsToStore = { {"Scale", {scaleGroupIndex, scaleGroupInfo}} };

    return std::make_shared<WeightGroupsToStore>(weightsToStore);
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
  const edm::EDGetTokenT<GenWeightProduct> lheWeightToken_;
  const edm::EDGetTokenT<GenWeightInfoProduct> lheWeightInfoToken_;

  const std::vector<std::string> weightgroups_;

  //std::unordered_map<std::string, int> weightGroupIndices_;
  int lheWeightPrecision_;
};

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(LHEWeightsTableProducer);
