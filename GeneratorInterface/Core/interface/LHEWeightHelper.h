#ifndef GeneratorInterface_Core_LHEWeightHelper_h
#define GeneratorInterface_Core_LHEWeightHelper_h

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <fstream>

#include "SimDataFormats/GeneratorProducts/interface/UnknownWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/MEParamWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "GeneratorInterface/Core/interface/WeightHelper.h"

#include <tinyxml2.h>

namespace gen {
    class LHEWeightHelper : public WeightHelper {
    public:
        LHEWeightHelper();

        //// possibly add more versions of this functions for different inputs
        void parseLHEFile(std::string filename);
        void parseWeightGroupsFromHeader(std::vector<std::string> lheHeader);
    private:
        std::map<std::string, std::string> weightAttributeMapFromHeaderLine(std::string line);
        void loadAttributeNames(std::string baseName, std::vector<std::string> altNames ={});
        std::map<std::string, std::string> getAttributeMap(std::string);
        std::string sanitizeText(std::string);
        bool isAWeight(std::string);
        
        std::regex weightGroupStart_;
        std::regex weightGroupEnd_;
        std::regex weightContent_;
        
        std::map<std::string, std::string> nameConversionMap_;
    };
}

#endif

