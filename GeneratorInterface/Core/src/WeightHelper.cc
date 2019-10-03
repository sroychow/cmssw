#include "GeneratorInterface/Core/interface/WeightHelper.h"

namespace gen {
    // TODO: Could probably recycle this code better
    std::unique_ptr<GenWeightProduct> WeightHelper::weightProduct(std::vector<double> weights) {
        auto weightProduct = std::make_unique<GenWeightProduct>();
        weightProduct->setNumWeightSets(weightGroups_.size());
        int weightGroupIndex = 0;
        for (unsigned int i = 0; i < weightGroups_.size(); i++) {
            addWeightToProduct(weightProduct, weights.at(i), "", i, weightGroupIndex);
        }
        return std::move(weightProduct);
    }

    std::unique_ptr<GenWeightProduct> WeightHelper::weightProduct(std::vector<gen::WeightsInfo> weights) {
        auto weightProduct = std::make_unique<GenWeightProduct>();
        weightProduct->setNumWeightSets(weightGroups_.size());
        int weightGroupIndex = 0;
        int i = 0;
        for (const auto& weight : weights) {
            weightGroupIndex = addWeightToProduct(weightProduct, weight.wgt, weight.id, i++, weightGroupIndex);
        }
        return std::move(weightProduct);
    }

    int WeightHelper::addWeightToProduct(std::unique_ptr<GenWeightProduct>& product, 
            double weight, std::string name, int weightNum, int groupIndex) {
        groupIndex = findContainingWeightGroup(name, weightNum, groupIndex);
        auto group = weightGroups_[groupIndex];
        int entry = group.weightVectorEntry(name, weightNum);
        product->addWeight(weight, groupIndex, entry);
        return groupIndex;
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
        // Needs to be properly handled
        throw std::range_error("Unmatched weight");
    }
}

