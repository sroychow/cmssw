#ifndef GeneratorInterface_Core_LHEWeightHelper_h
#define GeneratorInterface_Core_LHEWeightHelper_h

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <fstream>

#include "DataFormats/Common/interface/OwnVector.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"

#include <tinyxml2.h>

namespace gen {
    //class LHEWeightHelper : public GenWeightHelper {
    class LHEWeightHelper {
    public:
        LHEWeightHelper();

        //// possibly add more versions of this functions for different inputs
        void parseLHEFile(std::string filename);
        void parseWeightGroupsFromHeader(std::vector<std::string> lheHeader);


        edm::OwnVector<gen::WeightGroupInfo> getWeightGroups() {return weightGroups_;}
    private:
        void loadAttributeNames(std::string baseName, std::vector<std::string> altNames ={});
        std::string toLowerCase(const char*);
        std::string toLowerCase(const std::string);
        std::map<std::string, std::string> getAttributeMap(std::string);
        std::string sanitizeText(std::string);
        bool isAWeight(std::string);
        
        // Variables
        edm::OwnVector<gen::WeightGroupInfo> weightGroups_;
        std::regex weightGroupStart_;
        std::regex weightGroupEnd_;
        std::regex weightContent_;
        
        std::map<std::string, std::string> nameConvMap;
    };
}

#endif

