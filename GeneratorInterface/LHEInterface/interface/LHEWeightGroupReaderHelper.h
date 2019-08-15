#ifndef GeneratorInterface_LHEInterface_LHEWeightGroupReaderHelper_h
#define GeneratorInterface_LHEInterface_LHEWeightGroupReaderHelper_h

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <fstream>

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"


class LHEWeightGroupReaderHelper {
public:
    LHEWeightGroupReaderHelper() : curWeight(gen::kUnknownWeights) {}

    //// possibly add more versions of this functions for different inputs
    void parseLHEFile(std::string filename);


    gen::WeightGroupInfo* getScaleInfo() {return scaleInfo;}
    edm::OwnVector<gen::WeightGroupInfo> getPdfVector() {return pdfVector;}
    
private:
    // Functions
    std::regex createRegexSearch(std::vector<std::string>);
    std::map<std::string, std::string> getTagsMap(std::string, std::regex);

    // Variables
    gen::WeightType curWeight;
    gen::WeightGroupInfo* scaleInfo;
    edm::OwnVector<gen::WeightGroupInfo> pdfVector;
    std::regex weightStart = std::regex(".*<weightgroup.+>.*");
    std::regex weightEnd = std::regex(".*</weightgroup>.*");
    std::regex weightContent = std::regex("<weight.*>\\s*(.+)</weight>");

};

void
LHEWeightGroupReaderHelper::parseLHEFile(std::string filename) {
    std::ifstream file;
    file.open(filename);

    //// may put in constructor, can have flag to specify these values
    ////     To make this class a little more flexible
    std::vector<std::string> weightGroup = {"name|type", "combine"};
    std::vector<std::string> weightInfo = {"MUF", "id", "MUR", "PDF"};
            
    std::regex groupTags = createRegexSearch(weightGroup);
    std::regex infoTags = createRegexSearch(weightInfo);
    /// end that comment


    std::string line;
    std::smatch m;
    int index = 0;
    while(getline(file, line)) {
	if(std::regex_match(line, weightStart)) {
	    std::string groupLine = line;
	    std::string name = getTagsMap(line, groupTags)["name"];

	    if(name == "Central scale variation")
		curWeight = gen::kScaleWeights;
	    else 
		curWeight = gen::kPdfWeights;

	    /// file weights
	    while(getline(file, line) && !std::regex_match(line, weightEnd)) {
		auto tmp = getTagsMap(line, infoTags);
		std::regex_search(line, m, weightContent);
		std::string content = m[1].str();

		gen::WeightGroupInfo* tmpWeight = nullptr;
		if(curWeight == gen::kScaleWeights) {
		    tmpWeight = new gen::ScaleWeightGroupInfo(groupLine);
		    scaleInfo = tmpWeight;
		}
		else if(curWeight == gen::kPdfWeights) {
		    tmpWeight = new gen::PdfWeightGroupInfo(groupLine);
		    // pdfVector.push_back(tmpWeight);
		}
		tmpWeight->addContainedId(index, tmp["id"], line);
		
		if(curWeight == gen::kPdfWeights) //hate hate hate
		    pdfVector.push_back(tmpWeight);
		index++;
		
	    }
	    curWeight = gen::kUnknownWeights;
	}
    }

}


std::map<std::string, std::string>
LHEWeightGroupReaderHelper::getTagsMap(std::string s, std::regex r) {
    std::smatch m;

    std::map<std::string, std::string> retMap;
    while(std::regex_search(s, m, r)) {
	for(int i = 1; i < m.length() - 1; i += 2) {
	    if(m[i] != "") {
		retMap[m[i]] = m[i + 1];                
	    }
	}
	s = m.suffix().str();
    }
                
    return retMap;
}



std::regex
LHEWeightGroupReaderHelper::createRegexSearch(std::vector<std::string> names) {
    std::string s = "(?:";
    size_t numNames = names.size();
    for(size_t i=0; i < numNames; ++i) {
	s += "\\s*(" + names[i] + ")=\"([^\"]+)\"";
	if(i != numNames - 1) {
	    s += "|";
	}
    }
    s += ")";

    return std::regex(s);
}


#endif
