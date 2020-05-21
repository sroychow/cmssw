#ifndef SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "LHAPDF/LHAPDF.h"
#include <iostream>
#include <set>
#include <unordered_map>

namespace gen {
  enum PdfUncertaintyType {
    kHessianUnc,
    kMonteCarloUnc,
    kVariationSet,
    kUnknownUnc,
  };

  class PdfWeightGroupInfo : public WeightGroupInfo {
  private:
    PdfUncertaintyType uncertaintyType_;
    bool hasAlphasVars_;
    int alphasUpIndex_;
    int alphasDownIndex_;
    int parentLhapdfId_ = -1;
    std::vector<int> lhaids;
    int parentLhapdfId(int lhaid) const { return lhaid - LHAPDF::lookupPDF(lhaid).second; }

  public:
    PdfWeightGroupInfo() : WeightGroupInfo() { weightType_ = WeightType::kPdfWeights; }
    PdfWeightGroupInfo(std::string header, std::string name) : WeightGroupInfo(header, name) {
      weightType_ = WeightType::kPdfWeights;
    }
    PdfWeightGroupInfo(std::string header) : WeightGroupInfo(header) { weightType_ = WeightType::kPdfWeights; }
    PdfWeightGroupInfo(const PdfWeightGroupInfo& other) { copy(other); }
    virtual ~PdfWeightGroupInfo() override {}
    void copy(const PdfWeightGroupInfo& other);
    virtual PdfWeightGroupInfo* clone() const override;

    void setUncertaintyType(PdfUncertaintyType uncertaintyType) { uncertaintyType_ = uncertaintyType; }
    void setHasAlphasVariations(bool hasAlphasVars) { hasAlphasVars_ = hasAlphasVars; }
    void setAlphasUpIndex(int alphasUpIndex) { alphasUpIndex_ = alphasUpIndex; }
    void setAlphasDownIndex(int alphasDownIndex) { alphasDownIndex_ = alphasDownIndex; }
    PdfUncertaintyType uncertaintyType() const { return uncertaintyType_; }
    bool hasAlphasVariations() const { return hasAlphasVars_; }
    void addLhaid(int lhaid) { lhaids.push_back(lhaid); }
    std::vector<int>& getLhaIds() { return lhaids; }

    bool isIdInParentSet(int lhaid) const { return parentLhapdfId_ == parentLhapdfId(lhaid); }
    int getParentLhapdfId() const { return parentLhapdfId_; }
    void setParentLhapdfId(int lhaid) { parentLhapdfId_ = lhaid; }

    // need to remove
    bool containsLhapdfId(int lhaid) const { return isIdInParentSet(lhaid); }

    int alphasUpIndex() const { return alphasUpIndex_; }
    int alphasDownIndex() const { return alphasDownIndex_; }
  };
}  // namespace gen

#endif  // SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h
