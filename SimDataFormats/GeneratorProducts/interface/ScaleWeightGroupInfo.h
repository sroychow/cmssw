#ifndef SimDataFormats_GeneratorProducts_ScaleWeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_ScaleWeightGroupInfo_h

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include <unordered_map>
#include <vector>

namespace gen {
  class ScaleWeightGroupInfo : public WeightGroupInfo {
  private:
    bool isFunctionalFormVar_;
    std::vector<size_t> muIndices_;

    // Dyn_scale
    // bool hasDyn = false;
    // Maybe have mapping of [muR##muF##index] = vector<dyn_scale_indices>
    // std::unordered_map<size_t, std::vector<size_t>> dynMap;
    void setMuRMuFIndex(WeightMetaInfo& info, float muR, float muF);
    inline int getIndex(int muR, int muF) const { return 3 * muR + muF; }
    inline bool isValidValue(float mu) const { return mu == 0.5 || mu == 1.0 || mu == 2.0; }

  public:
    ScaleWeightGroupInfo() : ScaleWeightGroupInfo("") {}
    ScaleWeightGroupInfo(std::string header, std::string name) : WeightGroupInfo(header, name), muIndices_(9) {
      weightType_ = WeightType::kScaleWeights;
      isFunctionalFormVar_ = false;
    }
    ScaleWeightGroupInfo(std::string header) : ScaleWeightGroupInfo(header, header) {}
    ScaleWeightGroupInfo(const ScaleWeightGroupInfo& other) { copy(other); }
    virtual ~ScaleWeightGroupInfo() override {}
    void copy(const ScaleWeightGroupInfo& other);
    virtual ScaleWeightGroupInfo* clone() const override;

    void setMuRMuFIndex(int globalIndex, std::string id, float muR, float muF);
    void addContainedId(int weightEntry, std::string id, std::string label, float muR, float muF);

    // Is a variation of the functional form of the dynamic scale
    bool isFunctionalFormVariation();
    void setIsFunctionalFormVariation(bool functionalVar) { isFunctionalFormVar_ = functionalVar; }
    size_t centralIndex() const { return muIndices_.at(4); }
    size_t muR1muF2Index() const { return muIndices_.at(5); }
    size_t muR1muF05Index() const { return muIndices_.at(3); }
    size_t muR2muF05Index() const { return muIndices_.at(6); }
    size_t muR2muF1Index() const { return muIndices_.at(7); }
    size_t muR2muF2Index() const { return muIndices_.at(8); }
    size_t muR05muF05Index() const { return muIndices_.at(0); }
    size_t muR05muF1Index() const { return muIndices_.at(1); }
    size_t muR05muF2Index() const { return muIndices_.at(2); }
  };
}  // namespace gen

#endif
