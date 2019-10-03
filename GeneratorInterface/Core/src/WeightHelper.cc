#include "GeneratorInterface/Core/interface/WeightHelper.h"

namespace gen {
    std::unique_ptr<GenWeightProduct> WeightHelper::weightProduct(std::vector<gen::WeightsInfo> weights) {
        auto weightProduct = std::make_unique<GenWeightProduct>();
        weightProduct->setNumWeightSets(weightGroups_.size());
        int weightGroupIndex = 0;
        int weightNum = 0;
        for (const auto& weight : weights) {
            weightGroupIndex = findContainingWeightGroup(weight.id, weightNum, weightGroupIndex);
            if (weightGroupIndex < 0 || weightGroupIndex >= static_cast<int>(weightGroups_.size())) {
                // Needs to be properly handled
                throw std::range_error("Unmatched weight");
            }
            auto group = weightGroups_[weightGroupIndex];
            int entry = group.weightVectorEntry(weight.id, weightNum);
            weightProduct->addWeight(weight.wgt, weightGroupIndex, entry);
            weightNum++;
        }
        return std::move(weightProduct);
    }

    int WeightHelper::findContainingWeightGroup(std::string wgtId, int weightIndex, int previousGroupIndex) {
        // Start search at previous index, under expectation of ordered weights
        previousGroupIndex = previousGroupIndex >=0 ? previousGroupIndex : 0;
        for (int index = previousGroupIndex; 
                index < std::min(index+1, static_cast<int>(weightGroups_.size())); index++) {
            const gen::WeightGroupInfo& weightGroup = weightGroups_[index];
            if (weightGroup.indexInRange(weightIndex) && weightGroup.containsWeight(wgtId, weightIndex)) {
                return static_cast<int>(index);
            }
        }

        // Fall back to unordered search
        int counter = 0;
        for (auto weightGroup : weightGroups_) {
            if (weightGroup.containsWeight(wgtId, weightIndex))
                return counter;
            counter++;
        }
        return -1;
    }
}

