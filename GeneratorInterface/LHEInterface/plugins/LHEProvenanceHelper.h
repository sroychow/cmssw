#ifndef FWCore_Sources_LHEProvenanceHelper_h
#define FWCore_Sources_LHEProvenanceHelper_h

#include "DataFormats/Provenance/interface/BranchDescription.h"
#include "DataFormats/Provenance/interface/ProcessHistoryID.h"
#include "DataFormats/Provenance/interface/ProductProvenance.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

namespace lhef {
  class LHERunInfo;
}

namespace edm {
  class ProcessHistoryRegistry;
  class ProductRegistry;
  class TypeID;
  struct LHEProvenanceHelper {
    explicit LHEProvenanceHelper(TypeID const& eventProductType,
                                 TypeID const& runProductType,
                                 TypeID const& weightProductType,
                                 ProductRegistry& productRegistry);
    ParameterSet fillCommonProcessParameterSet();
    void lheAugment(lhef::LHERunInfo const* runInfo);
    ProcessHistoryID lheInit(ProcessHistoryRegistry& processHistoryRegistry);
    BranchDescription const eventProductBranchDescription_;
    BranchDescription const runProductBranchDescription_;
    BranchDescription const weightProductBranchDescription_;
    ProductProvenance eventProductProvenance_;
    ParameterSet const commonProcessParameterSet_;
    ParameterSet processParameterSet_;
  };
}  // namespace edm
#endif
