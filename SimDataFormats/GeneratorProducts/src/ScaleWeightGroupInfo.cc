#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"
#include <stdexcept>
#include <iostream>

namespace gen {
  void ScaleWeightGroupInfo::copy(const ScaleWeightGroupInfo& other) {
    muIndices_ = other.muIndices_;
    WeightGroupInfo::copy(other);
  }

  ScaleWeightGroupInfo* ScaleWeightGroupInfo::clone() const { return new ScaleWeightGroupInfo(*this); }

  void ScaleWeightGroupInfo::addContainedId(int globalIndex, std::string id, std::string label, float muR, float muF) {
    WeightGroupInfo::addContainedId(globalIndex, id, label);
    setMuRMuFIndex(globalIndex, id, muR, muF);
  }

  void ScaleWeightGroupInfo::setMuRMuFIndex(int globalIndex, std::string id, float muR, float muF) {
    auto metaInfo = weightMetaInfoByGlobalIndex(id, globalIndex);
    setMuRMuFIndex(metaInfo, muR, muF);
  }

  void ScaleWeightGroupInfo::setMuRMuFIndex(WeightMetaInfo& info, float muR, float muF) {
    int index = getIndex(muR, muF);
    if (index < 0 || index > 8 || !(isValidValue(muR) && isValidValue(muF))) {
      isWellFormed_ = false;
    }
    muIndices_[index] = info.localIndex;
  }
}  // namespace gen
