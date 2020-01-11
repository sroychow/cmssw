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
#include "FWCore/Utilities/interface/transform.h"


#include <optional>
#include <iostream>
#include <tinyxml2.h>

namespace {
  typedef std::pair<std::vector<gen::WeightGroupData>, std::vector<gen::WeightGroupData>> WeightGroupsToStore;
}  // namespace

class LHEWeightsTableProducer : public edm::global::EDProducer<edm::LuminosityBlockCache<WeightGroupsToStore>> {
public:
  LHEWeightsTableProducer(edm::ParameterSet const& params) :
        lheToken_(consumes<LHEEventProduct>(params.getParameter<edm::InputTag>("lheInfo"))),
        lheWeightToken_(consumes<GenWeightProduct>(params.getParameter<edm::InputTag>("lheWeights"))),
        lheWeightInfoToken_(consumes<GenWeightInfoProduct, edm::InLumi>(params.getParameter<edm::InputTag>("lheWeights"))),
        genWeightToken_(consumes<GenWeightProduct>(params.getParameter<edm::InputTag>("genWeights"))),
        genWeightInfoToken_(consumes<GenWeightInfoProduct, edm::InLumi>(params.getParameter<edm::InputTag>("genWeights"))),
        weightgroups_(edm::vector_transform(params.getParameter<std::vector<std::string>>("weightgroups"),
                    [](auto& c) { return gen::WeightType(c.at(0)); } )),
        maxGroupsPerType_(params.getParameter<std::vector<int>>("maxGroupsPerType")),
        pdfIds_(params.getParameter<std::vector<int>>("pdfIds")),
        lheWeightPrecision_(params.getParameter<int32_t>("lheWeightPrecision")) {
    if (weightgroups_.size() != maxGroupsPerType_.size())
       throw std::invalid_argument("Inputs 'weightgroups' and 'weightgroupNums' must have equal size");
    produces<std::vector<nanoaod::FlatTable>>();
  }

  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {
    // tables for LHE weights, may not be filled
    auto const& lheInfo = iEvent.get(lheToken_);

    edm::Handle<GenWeightProduct> lheWeightHandle;
    iEvent.getByToken(lheWeightToken_, lheWeightHandle);
    const GenWeightProduct* lheWeightProduct = lheWeightHandle.product();
    WeightsContainer lheWeights = lheWeightProduct->weights();

    edm::Handle<GenWeightProduct> genWeightHandle;
    iEvent.getByToken(genWeightToken_, genWeightHandle);
    const GenWeightProduct* genWeightProduct = genWeightHandle.product();
    WeightsContainer genWeights = genWeightProduct->weights();

    auto lheWeightTables = std::make_unique<std::vector<nanoaod::FlatTable>>();
    double w0 = lheInfo.originalXWGTUP();
    auto const& weightInfos = *luminosityBlockCache(iEvent.getLuminosityBlock().index());

    addWeightGroupToTable(lheWeightTables, "LHE", weightInfos.first, lheWeights, w0);
    addWeightGroupToTable(lheWeightTables, "GEN", weightInfos.second, genWeights, w0);

    iEvent.put(std::move(lheWeightTables));
  }

  void addWeightGroupToTable(std::unique_ptr<std::vector<nanoaod::FlatTable>>& lheWeightTables, const char* typeName,
          const std::vector<gen::WeightGroupData>& weightInfos, WeightsContainer& lheWeights, double w0) const {
    size_t typeCount = 0;
    gen::WeightType previousType = gen::WeightType::kUnknownWeights;

    for (const auto& groupInfo : weightInfos) {
        std::string entryName = typeName;
        gen::WeightType weightType = groupInfo.group->weightType();
        if (previousType != weightType)
            typeCount = 0;
        std::string name = weightTypeNames_.at(weightType);
        auto weights = weightType != gen::WeightType::kScaleWeights ? normalizedWeights(lheWeights, groupInfo, w0) :
                            orderedScaleWeights(lheWeights, groupInfo, w0);
        entryName.append(name);
        entryName.append("Weight");
        if (typeCount > 0) {
            entryName.append("AltSet");
            entryName.append(std::to_string(typeCount));
        }

        lheWeightTables->emplace_back(weights.size(), entryName, false);
        lheWeightTables->back().addColumn<float>(
            "", weights, groupInfo.group->name(), nanoaod::FlatTable::FloatColumn, lheWeightPrecision_);

        typeCount++;
        previousType = weightType;
    }
  }


  std::shared_ptr<WeightGroupsToStore> globalBeginLuminosityBlock(edm::LuminosityBlock const& iLumi,
                                                             edm::EventSetup const&) const override {

    // Set equal to the max number of groups
    // subtrack 1 for each weight group you find
    edm::Handle<GenWeightInfoProduct> lheWeightInfoHandle;
    iLumi.getByToken(lheWeightInfoToken_, lheWeightInfoHandle);

    edm::Handle<GenWeightInfoProduct> genWeightInfoHandle;
    iLumi.getByToken(genWeightInfoToken_, genWeightInfoHandle);

    std::unordered_map<gen::WeightType, int> storePerType;
    for (size_t i = 0; i < weightgroups_.size(); i++)
        storePerType[weightgroups_.at(i)] = maxGroupsPerType_.at(i);

    WeightGroupsToStore weightsToStore;
    for (auto weightType : gen::allGenWeightTypes) {
        auto lheWeights = weightDataPerType(lheWeightInfoHandle, weightType, storePerType[weightType]);
        weightsToStore.first.insert(weightsToStore.first.end(), lheWeights.begin(), lheWeights.end());

        auto genWeights = weightDataPerType(genWeightInfoHandle, weightType, storePerType[weightType]);
        weightsToStore.second.insert(weightsToStore.second.end(), genWeights.begin(), genWeights.end());
    }

    return std::make_shared<WeightGroupsToStore>(weightsToStore);
  }

  std::vector<gen::WeightGroupData> weightDataPerType(edm::Handle<GenWeightInfoProduct>& weightsHandle, 
                                                gen::WeightType weightType, int& maxStore) const {
      std::vector<gen::WeightGroupData> group;
      if (weightType == gen::WeightType::kPdfWeights) {
        for (auto lhaid : pdfIds_) {
            if (auto pdfGroup = weightsHandle->pdfGroupWithIndexByLHAID(lhaid)) {
                group.push_back(pdfGroup.value());
                maxStore--;
                if (maxStore == 0)
                    break;
            }
        }
        return group;
      }

      group = weightsHandle->weightGroupsAndIndicesByType(weightType);

      if (maxStore < 0 || static_cast<int>(group.size()) <= maxStore) {
          // Modify size in case one type of weight is present in multiple products
          maxStore -= group.size();
          return group;
      }
      return std::vector(group.begin(), group.begin()+maxStore);
  }


  std::vector<float> normalizedWeights(WeightsContainer& lheWeights, const gen::WeightGroupData& meGroupInfo, double w0) const {
    std::vector<float> normalizedWeights;
    for (const auto& weight : lheWeights.at(meGroupInfo.index))
        normalizedWeights.push_back(weight/w0);
    return normalizedWeights;
  }

  std::vector<float> orderedScaleWeights(WeightsContainer& lheWeights, const gen::WeightGroupData& scaleGroupInfo, double w0) const {
    const gen::ScaleWeightGroupInfo* scaleGroup = static_cast<const gen::ScaleWeightGroupInfo*>(scaleGroupInfo.group);
    size_t scaleGroupIndex = scaleGroupInfo.index;

    std::vector<float> normalizedWeights;
    std::vector<double>& scaleWeights = lheWeights.at(scaleGroupIndex);

    // nano ordering of mur=0.5 muf=0.5 ; [1] is mur=0.5 muf=1 ; [2] is mur=0.5 muf=2 ; [3] is mur=1 muf=0.5 ; 
    // [4] is mur=1 muf=1 ; [5] is mur=1 muf=2 ; [6] is mur=2 muf=0.5 ; [7] is mur=2 muf=1 ; [8] is mur=2 muf=2 *
    normalizedWeights.push_back(scaleWeights.at(scaleGroup->muR05muF05Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroup->muR05muF1Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroup->muR05muF2Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroup->muR1muF05Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroup->centralIndex())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroup->muR1muF2Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroup->muR2muF05Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroup->muR2muF1Index())/w0);
    normalizedWeights.push_back(scaleWeights.at(scaleGroup->muR2muF2Index())/w0);

    return normalizedWeights;
  }

  // nothing to do here
  virtual void globalEndLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) const override {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("lheInfo", {"externalLHEProducer"})
        ->setComment("tag(s) for the LHE information (LHEEventProduct and LHERunInfoProduct)");
    desc.add<edm::InputTag>("lheWeights");
    desc.add<edm::InputTag>("genWeights");
    desc.add<std::vector<std::string>>("weightgroups");
    desc.add<std::vector<int>>("maxGroupsPerType");
    desc.add<std::vector<int>>("pdfIds");
    desc.add<int32_t>("lheWeightPrecision", -1)->setComment("Number of bits in the mantissa for LHE weights");
    descriptions.addDefault(desc);
  }

protected:
  const edm::EDGetTokenT<LHEEventProduct> lheToken_;
  const edm::EDGetTokenT<GenWeightProduct> lheWeightToken_;
  const edm::EDGetTokenT<GenWeightInfoProduct> lheWeightInfoToken_;
  const edm::EDGetTokenT<GenWeightProduct> genWeightToken_;
  const edm::EDGetTokenT<GenWeightInfoProduct> genWeightInfoToken_;
  const std::vector<gen::WeightType> weightgroups_;
  const std::vector<int> maxGroupsPerType_;
  const std::vector<int> pdfIds_;
  const std::unordered_map<gen::WeightType, std::string> weightTypeNames_ = {
      {gen::WeightType::kScaleWeights, "Scale"},
      {gen::WeightType::kPdfWeights, "Pdf"},
      {gen::WeightType::kMEParamWeights, "MEParam"},
      {gen::WeightType::kPartonShowerWeights, "PartonShower"},
      {gen::WeightType::kUnknownWeights, "Unknown"},
  };

  //std::unordered_map<std::string, int> weightGroupIndices_;
  int lheWeightPrecision_;
};

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(LHEWeightsTableProducer);
