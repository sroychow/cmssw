#ifndef SimDataFormats_GeneratorProducts_WeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_WeightGroupInfo_h

/** \class PdfInfo
 *
 */
#include <string>

namespace gen {
    struct WeightMetaInfo {
        int globalIndex;
        int localIndex;
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
                headerEntry_(header), name_(header), firstId_(0), lastId_(0) {}

            int weightVectorEntry(const std::string& wgtId) {
                return weightVectorEntry(wgtId, 0);
            }

            int weightVectorEntry(const std::string& wgtId, int weightEntry) {
                int orderedEntry = weightEntry - firstId_;
                int entry = -1;
                if (orderedEntry >= 0 && static_cast<size_t>(orderedEntry) < idsContained_.size())
                    if (idsContained_.at(orderedEntry).id == wgtId)
                        return orderedEntry;
                //auto it = std::find(
                return entry;
            }

            void addContainedId(int weightEntry, std::string id, std::string label="") {
                if (firstId_ == -1 || weightEntry < firstId_)
                    firstId_ = weightEntry;
                if (firstId_ == -1 || weightEntry > lastId_)
                    lastId_ = weightEntry;
                
                int orderedEntry = weightEntry - firstId_;
                WeightMetaInfo info;
                info.globalIndex = weightEntry;
                info.localIndex = orderedEntry;
                info.id = id;
                info.label = label;

                idsContained_.insert(idsContained_.begin()+orderedEntry, info);
            }

            std::vector<WeightMetaInfo> containedIds() { return idsContained_; }

            void setWeightType(WeightType type) { weightType_ = type; }

            bool indexInRange(int index) {
                return (index <= lastId_ && index >= firstId_);
            }

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

