#include "SimDataFormats/GeneratorProducts/interface/MEParamWeightGroupInfo.h"
#include <iostream>

namespace gen {
  void MEParamWeightGroupInfo::copy(const MEParamWeightGroupInfo& other) { WeightGroupInfo::copy(other); }

  MEParamWeightGroupInfo* MEParamWeightGroupInfo::clone() const { return new MEParamWeightGroupInfo(*this); }
}  // namespace gen
