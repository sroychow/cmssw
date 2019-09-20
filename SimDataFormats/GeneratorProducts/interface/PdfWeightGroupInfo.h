#ifndef SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

namespace gen {
    enum PdfUncertaintyType {
        kHessianUnc,
        kMonteCarloUnc,
        kUnknownUnc,
    };

    class PdfWeightGroupInfo : public WeightGroupInfo {
        private:
            PdfUncertaintyType uncertaintyType_;
            bool hasAlphasVars_;
            int alphasUpIndex_;
            int alphasDownIndex_;
            std::vector<int> lhapdfIdsContained_;
        public:
            PdfWeightGroupInfo() : WeightGroupInfo() { weightType_ = kPdfWeights; }
	        PdfWeightGroupInfo(std::string header, std::string name) : 
                WeightGroupInfo(header, name) { weightType_ = kPdfWeights; } 
	        PdfWeightGroupInfo(std::string header) : 
                WeightGroupInfo(header) { weightType_ = kPdfWeights; } 
            PdfWeightGroupInfo(const PdfWeightGroupInfo &other) {
                copy(other);
            }
            virtual ~PdfWeightGroupInfo() override {}
            void copy(const PdfWeightGroupInfo &other);
            virtual PdfWeightGroupInfo* clone() const override;

            void setUncertaintyType(PdfUncertaintyType uncertaintyType) { uncertaintyType_ = uncertaintyType; }
            void setHasAlphasVariations(bool hasAlphasVars) { hasAlphasVars_ = hasAlphasVars; }
            void setAlphasUpIndex(int alphasUpIndex) { alphasUpIndex_ = alphasUpIndex; }
            void setAlphasDownIndex(int alphasDownIndex) { alphasDownIndex_ = alphasDownIndex; }
            PdfUncertaintyType uncertaintyType() const { return uncertaintyType_; }
            bool hasAlphasVariations() const { return hasAlphasVars_; }
            bool containsMultipleSets() const { return lhapdfIdsContained_.size() > 1; }
            int alphasUpIndex() const { return alphasUpIndex_; }
            int alphasDownIndex() const { return alphasDownIndex_; }
            void addLhapdfId(int lhaid) { lhapdfIdsContained_.push_back(lhaid); }
            std::vector<int> getLhapdfIdsContained() const { return lhapdfIdsContained_; }
    };
}

#endif // SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h

