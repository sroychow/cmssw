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
        LHEWeightInfoProduct() {
            gen::WeightGroupInfo scaleInfo(
                "<weightgroup name=\"Central scale variation\" combine=\"envelope\">"
            );
            //scaleInfo.isPDF = false;
            //scaleInfo.pdfLabel = "";
            //scaleInfo.pdfLHAID = "";

            gen::WeightGroupInfo cenPdfInfo(
                "<weightgroup name=\"NNPDF31_nnlo_hessian_pdfas\" combine=\"hessian\">"
            );

            //cenPdfInfo.isPDF = true;
            //cenPdfInfo.pdfLabel = "NNPDF31_nnlo_hessian_pdfas";
            //cenPdfInfo.pdfLHAID = "3061000";

            weightGroupsInfo_.push_back(scaleInfo);
            weightGroupsInfo_.push_back(cenPdfInfo);
        };
        std::vector<gen::WeightGroupInfo> getWeightGroupsInfo() { return weightGroupsInfo_; }
        void addWeightGroupInfo(gen::WeightGroupInfo info) {  
            weightGroupsInfo_.push_back(info); 
        }

    private:
        std::vector<gen::WeightGroupInfo> weightGroupsInfo_;


};

#endif // GeneratorWeightInfo_LHEInterface_LHEWeightInfoProduct_h

