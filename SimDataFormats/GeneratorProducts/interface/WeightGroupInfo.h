#ifndef SimDataFormats_GeneratorProducts_WeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_WeightGroupInfo_h

/** \class PdfInfo
 *
 */
#include <string>
#include <algorithm>
#include <iostream>

namespace gen {
    struct WeightMetaInfo {
        size_t globalIndex;
        size_t localIndex;
        std::string id;
        std::string label;
    };

    enum WeightType {
        kPdfWeights,
        kScaleWeights,
        kMatrixElementWeights,
        kUnknownWeights,
        kShowerWeights,
    };

	class WeightGroupInfo {
        public:
	        WeightGroupInfo() {}
	        WeightGroupInfo(std::string header, std::string name): 
                headerEntry_(header), name_(name), firstId_(-1), lastId_(-1) {}
	        WeightGroupInfo(std::string header): 
                headerEntry_(header), name_(header), firstId_(-1), lastId_(-1) {}

            void copy(const WeightGroupInfo &other) {
                headerEntry_ = other.headerEntry();
                name_ = other.name();
                weightType_ = other.weightType();
                idsContained_ = other.idsContained();
                firstId_ = other.firstId();
                lastId_ = other.lastId();
            }

            WeightGroupInfo(const WeightGroupInfo &other) {
                copy(other);
            }

            WeightGroupInfo& operator=(const WeightGroupInfo &other) {
                copy(other);
                return *this; 
            }

            WeightGroupInfo* clone() const {
                return new WeightGroupInfo(*this);
            }

            WeightMetaInfo weightMetaInfo(int weightEntry) {
                return idsContained_.at(weightEntry);
            }

            WeightMetaInfo weightMetaInfo(std::string wgtId) {
                int weightEntry = weightVectorEntry(wgtId);
                return idsContained_.at(weightEntry);
            }

            int weightVectorEntry(const std::string& wgtId) {
                return weightVectorEntry(wgtId, 0);
            }

            int containsWeight(const std::string& wgtId, int weightEntry) {
                return weightVectorEntry(wgtId, weightEntry) != -1;
            }

            int weightVectorEntry(const std::string& wgtId, int weightEntry) {
                int entry = -1;
                if (!indexInRange(weightEntry)) {
                    size_t orderedEntry = weightEntry - firstId_;
                    if (orderedEntry < idsContained_.size())
                        if (idsContained_.at(orderedEntry).id == wgtId)
                            return orderedEntry;
                }
                auto it = std::find_if(idsContained_.begin(), idsContained_.end(), 
                                [wgtId] (const WeightMetaInfo& w) { return w.id == wgtId; });
                if (it != idsContained_.end())
                    return std::distance(idsContained_.begin(), it);
                return entry;
            }

            void addContainedId(int weightEntry, std::string id, std::string label="") {
                if (firstId_ == -1 || weightEntry < firstId_) {
                    firstId_ = weightEntry;
                    // Reset to reflect that indices will be shifted
                    for (auto& id : idsContained_)
                        id.localIndex = id.globalIndex - firstId_;
                }
                if (weightEntry > lastId_)
                    lastId_ = weightEntry;
                
                WeightMetaInfo info;
                info.globalIndex = weightEntry;
                info.localIndex = weightEntry - firstId_;
                info.id = id;
                info.label = label;

                if (idsContained_.size() < info.localIndex) {
                    idsContained_.resize(info.localIndex);
                    idsContained_.insert(idsContained_.begin()+info.localIndex, info);
                }
                else if (idsContained_.size() == info.localIndex) {
                    idsContained_.push_back(info);
                }
                else {
                    idsContained_.resize(info.localIndex+1);
                    idsContained_[info.localIndex] = info;
                }
            }

            std::vector<WeightMetaInfo> containedIds() const { return idsContained_; }

            void setWeightType(WeightType type) { weightType_ = type; }
            std::string name() { return name_; }

            bool indexInRange(int index) const {
                return (index <= lastId_ && index >= firstId_);
            }

            std::string headerEntry() const { return headerEntry_; }
            std::string name() const { return name_; }
            WeightType weightType() const { return weightType_; }
            std::vector<WeightMetaInfo> idsContained() const { return idsContained_; }
            int firstId() const { return firstId_; }
            int lastId() const { return lastId_; }

        private:
            std::string headerEntry_;
            std::string name_;
            WeightType weightType_;
            std::vector<WeightMetaInfo> idsContained_;
            int firstId_;
            int lastId_;
	};
}

#endif // SimDataFormats_GeneratorProducts_WeightGroupInfo_h

