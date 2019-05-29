#ifndef SimDataFormats_GeneratorProducts_LHEWeightInfoProduct_h
#define SimDataFormats_GeneratorProducts_LHEWeightInfoProduct_h

#include <iterator>
#include <memory>
#include <vector>
#include <string>

//#include <hepml.hpp>

#include "SimDataFormats/GeneratorProducts/interface/LesHouches.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

class LHEWeightInfoProduct {
    public:
        LHEWeightInfoProduct() {}
	    LHEWeightInfoProduct(const LHEWeightInfoProduct& other) {weightGroupsInfo_ = other.weightGroupsInfo_;}
	    LHEWeightInfoProduct(LHEWeightInfoProduct&& other) {weightGroupsInfo_ = std::move(other.weightGroupsInfo_);}
        ~LHEWeightInfoProduct() {}
        LHEWeightInfoProduct(std::vector<gen::WeightGroupInfo>& weightGroups) {
            weightGroupsInfo_ = weightGroups;
        }
        LHEWeightInfoProduct &operator = (const LHEWeightInfoProduct &other) {weightGroupsInfo_ = other.weightGroupsInfo_; return * this; }
        LHEWeightInfoProduct &operator = (LHEWeightInfoProduct &&other) {weightGroupsInfo_ = std::move(other.weightGroupsInfo_); return *this;}

        std::vector<gen::WeightGroupInfo> getWeightGroupsInfo() { return weightGroupsInfo_; }
        void addWeightGroupInfo(gen::WeightGroupInfo info) {  
            weightGroupsInfo_.push_back(info); 
        }

    private:
        std::vector<gen::WeightGroupInfo> weightGroupsInfo_;


};

#endif // GeneratorWeightInfo_LHEInterface_LHEWeightInfoProduct_h

