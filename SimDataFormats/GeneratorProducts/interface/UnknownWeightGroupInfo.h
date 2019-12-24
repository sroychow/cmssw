#ifndef SimDataFormats_GeneratorProducts_UnknownWeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_UnknownWeightGroupInfo_h

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

namespace gen {
    class UnknownWeightGroupInfo : public WeightGroupInfo {
        public:
            UnknownWeightGroupInfo() : WeightGroupInfo() { weightType_ = kUnknownWeights; }
	        UnknownWeightGroupInfo(std::string header, std::string name) : 
                WeightGroupInfo(header, name) { weightType_ = kUnknownWeights; isWellFormed_ = false;} 
	        UnknownWeightGroupInfo(std::string header) : 
                WeightGroupInfo(header) { weightType_ = kUnknownWeights; isWellFormed_ = false;} 
            virtual ~UnknownWeightGroupInfo() override {}
            void copy(const UnknownWeightGroupInfo &other);
            virtual UnknownWeightGroupInfo* clone() const override;
    };
}

#endif // SimDataFormats_GeneratorProducts_UnknownWeightGroupInfo_h


