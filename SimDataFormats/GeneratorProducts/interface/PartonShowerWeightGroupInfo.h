#ifndef SimDataFormats_GeneratorProducts_PartonShowerWeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_PartonShowerWeightGroupInfo_h

#include <unordered_map>

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

namespace gen {
  class PartonShowerWeightGroupInfo : public WeightGroupInfo {
  public:
    PartonShowerWeightGroupInfo() : PartonShowerWeightGroupInfo("") {}
    PartonShowerWeightGroupInfo(std::string header, std::string name) : WeightGroupInfo(header, name) {
      weightType_ = WeightType::kPartonShowerWeights;
    }
    PartonShowerWeightGroupInfo(std::string header) : PartonShowerWeightGroupInfo(header, header) {}
    PartonShowerWeightGroupInfo(const PartonShowerWeightGroupInfo &other) { copy(other); }
    virtual ~PartonShowerWeightGroupInfo() override {}
    void copy(const PartonShowerWeightGroupInfo &other);
    virtual PartonShowerWeightGroupInfo *clone() const override;
    void updateWeight(int globalIndex, std::string id, std::string subName, bool isUp);

    size_t getUpIndex(std::string weightName) { return weightNameToUpDown[weightName].first; }
    size_t getDownIndex(std::string weightName) { return weightNameToUpDown[weightName].second; }
    std::vector<std::string> getWeightNames() const { return weightNames; }

  private:
    std::unordered_map<std::string, std::pair<size_t, size_t>> weightNameToUpDown;
    std::vector<std::string> weightNames;
    // Is a variation of the functional form of the dynamic scale
  };
}  // namespace gen

#endif
