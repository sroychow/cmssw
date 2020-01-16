#ifndef GeneratorInterface_LHEInterface_WeightHelper_h
#define GeneratorInterface_LHEInterface_WeightHelper_h

#include "DataFormats/Common/interface/OwnVector.h"
#include "SimDataFormats/GeneratorProducts/interface/GenWeightProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightsInfo.h"
#include <boost/algorithm/string.hpp>

namespace gen {
    struct PdfSetInfo {
        std::string name;
        int lhapdfId;
        PdfUncertaintyType uncertaintyType; 
    };

    class WeightHelper {
    public:
        WeightHelper();
        edm::OwnVector<gen::WeightGroupInfo> weightGroups() {
            return weightGroups_;
        }
        std::unique_ptr<GenWeightProduct> weightProduct(std::vector<gen::WeightsInfo>, float w0);
        std::unique_ptr<GenWeightProduct> weightProduct(std::vector<double>, float w0);
        void setGroupInfo();
        bool currentGroupIsScale();
        bool currentGroupIsMEParam();
        bool currentGroupIsPdf();
        int addWeightToProduct(std::unique_ptr<GenWeightProduct>& product, double weight, std::string name, int weightNum, int groupIndex);
        int findContainingWeightGroup(std::string wgtId, int weightIndex, int previousGroupIndex);
    protected:
        const std::vector<PdfSetInfo> pdfSetsInfo;
        std::map<std::string, std::string> currWeightAttributeMap_;
        std::map<std::string, std::string> currGroupAttributeMap_;
        edm::OwnVector<gen::WeightGroupInfo> weightGroups_;
    };
}

#endif

