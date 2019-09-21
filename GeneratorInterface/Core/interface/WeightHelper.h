#ifndef GeneratorInterface_LHEInterface_WeightHelper_h
#define GeneratorInterface_LHEInterface_WeightHelper_h

#include "DataFormats/Common/interface/OwnVector.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEWeightProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightsInfo.h"

namespace gen {
    class WeightHelper {
    public:
        WeightHelper() {}
        edm::OwnVector<gen::WeightGroupInfo> weightGroups() {return weightGroups_;}
        std::unique_ptr<LHEWeightProduct> weightProduct(std::vector<gen::WeightsInfo>);
        int findContainingWeightGroup(std::string wgtId, int weightIndex, int previousGroupIndex);
    protected:
        edm::OwnVector<gen::WeightGroupInfo> weightGroups_;
    };
}

#endif

