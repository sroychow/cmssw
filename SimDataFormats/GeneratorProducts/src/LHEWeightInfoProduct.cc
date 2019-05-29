#include <vector>
#include <string>

#include "SimDataFormats/GeneratorProducts/interface/LHEWeightInfoProduct.h"

LHEWeightInfoProduct::LHEWeightInfoProduct(std::vector<gen::WeightGroupInfo>& weightGroups) {
    weightGroupsInfo_ = weightGroups;
}
        
LHEWeightInfoProduct& LHEWeightInfoProduct::operator=(const LHEWeightInfoProduct &other) {
    weightGroupsInfo_ = other.weightGroupsInfo_; 
    return * this; 
}

LHEWeightInfoProduct& LHEWeightInfoProduct::operator=(LHEWeightInfoProduct &&other) {
    weightGroupsInfo_ = std::move(other.weightGroupsInfo_); 
    return *this;
}

std::vector<gen::WeightGroupInfo> LHEWeightInfoProduct::getWeightGroupsInfo() { 
    return weightGroupsInfo_; 
}

void LHEWeightInfoProduct::addWeightGroupInfo(gen::WeightGroupInfo info) {  
    weightGroupsInfo_.push_back(info); 
}
