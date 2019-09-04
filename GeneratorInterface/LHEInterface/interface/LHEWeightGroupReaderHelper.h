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
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"


namespace dylanTest {
    class RegexCreater
    {
    public:
	void addValue(std::string baseName, std::vector<std::string> altNames={}, bool isInContent=false );
	bool processString(std::string);
	std::map<std::string, std::string> getTagMap() {return valueMap;}

    
    private:
	std::string tagRegString = "(?:)";
	std::string contentRegString = "(?:)";

	std::vector<std::string> tagNames;
	std::vector<std::string> contentNames;
     
	std::regex tagRegex;
	std::regex contentRegex;

	std::map<std::string, std::string> valueMap;
    
	std::string orStrings(std::string baseName, std::vector<std::string> altNames);
	void updateRegexSearchTag(std::string matchName);
	void updateRegexSearchContent(std::string matchName);
    
    };


    

    class LHEWeightGroupReaderHelper {
    public:
	LHEWeightGroupReaderHelper();

	//// possibly add more versions of this functions for different inputs
	void parseLHEFile(std::string filename);
	void parseWeightGroupsFromHeader(std::vector<std::string> lheHeader);


	edm::OwnVector<gen::WeightGroupInfo> getWeightGroups() {return weightGroups_;}
    
    private:
	// functions
	std::map<std::string, std::string> getMap_testAll(std::string, std::vector<RegexCreater>);
	
	// Variables
	edm::OwnVector<gen::WeightGroupInfo> weightGroups_;
	std::regex weightGroupStart_;
	std::regex weightGroupEnd_;
	std::regex weightContent_;
	std::regex scaleWeightMatch_;

	std::vector<RegexCreater> regexOptions;
	RegexCreater weightGroupInfo;
    };
}



void
dylanTest::RegexCreater::addValue(std::string baseName, std::vector<std::string> altNames, bool isInContent) {
    std::string nameListString = orStrings(baseName, altNames);

    if (isInContent) {
	contentNames.push_back(baseName);
	updateRegexSearchContent(nameListString);
    }
    else {
	tagNames.push_back(baseName);
	updateRegexSearchTag(nameListString);
    }
}

std::string
dylanTest::RegexCreater::orStrings(std::string baseName, std::vector<std::string> altNames) {
    for(auto name: altNames) {
	baseName += "|" + name;
    }
    return baseName;
}

void
dylanTest::RegexCreater::updateRegexSearchTag(std::string matchName) {
    tagRegString.pop_back(); 	// remove last )
    if(tagRegString.size() > 5) tagRegString += "|";//for fence post issue
	
    tagRegString += "\\s*(" + matchName + ")=\"([^\"]+)\")";
    // Last bit is to ignore case
    tagRegex = std::regex(tagRegString, std::regex_constants::ECMAScript | std::regex_constants::icase );
}

void
dylanTest::RegexCreater::updateRegexSearchContent(std::string matchName) {
    contentRegString.pop_back();    // remove last )
    if(contentRegString.size() > 5) contentRegString += "|";  //for fence post issue
    contentRegString += "(" + matchName + ")\\s*=\\s*(\\S+))";
    // Last bit is to ignore case
    contentRegex = std::regex(contentRegString, std::regex_constants::ECMAScript | std::regex_constants::icase );
}
    
bool
dylanTest::RegexCreater::processString(std::string fullString) {
    valueMap.clear();
    std::smatch m;

    std::string processStr = fullString;
    while(std::regex_search(processStr, m, tagRegex)) {
	for(int i = 1; i < m.length() - 1; i += 2) {
	    if(m[i] != "") {
		valueMap[tagNames.at(i/2)] = m[i + 1];
	    }

	}
	processStr = m.suffix().str();
    }

    std::regex weightContent_ = std::regex("<weight.*>\\s*(.+)\\s*</weight>");
    std::regex_search(fullString, m, weightContent_);
    processStr = m[1];
    while(std::regex_search(processStr, m, contentRegex)) {
	for(int i = 1; i < m.length() - 1; i += 2) {
	    if(m[i] != "") {
		valueMap[contentNames.at(i/2)] = m[i + 1];
	    }
	}
	processStr = m.suffix().str();
    }

    return valueMap.size() == (contentNames.size() + tagNames.size());
}








dylanTest::LHEWeightGroupReaderHelper::LHEWeightGroupReaderHelper() {
    weightGroupStart_ = std::regex(".*<weightgroup.+>.*\n*");
    weightGroupEnd_ = std::regex(".*</weightgroup>.*\n*");
    weightContent_ = std::regex("<weight.*>\\s*(.+)\\s*</weight>\n*");
    scaleWeightMatch_ = std::regex(".*(Central scale variation|scale_variation).*\n?");

    std::cout << "Init" << "\n";
    
    /// Might change later, order matters and code doesn't pick choices

    // WZVBS_2017_weightInfo.txt : scale-sometimes
    // WZVBS_private_weightInfo.txt : scale-sometimes
    RegexCreater rc_scale_dyn_pdf;
    rc_scale_dyn_pdf.addValue("muf");
    rc_scale_dyn_pdf.addValue("mur");
    rc_scale_dyn_pdf.addValue("id");
    rc_scale_dyn_pdf.addValue("dyn_scale");
    rc_scale_dyn_pdf.addValue("pdf");
    regexOptions.push_back(rc_scale_dyn_pdf);

    // WZVBS_private_weightInfo.txt : scale-sometimes / other
    // WZVBS_2017_weightInfo.txt : scale-sometimes / other
    // DrellYan_LO_MGMLMv242_2017_weightInfo.txt : all
    RegexCreater rc_scale_pdf;
    rc_scale_pdf.addValue("muf");
    rc_scale_pdf.addValue("mur");
    rc_scale_pdf.addValue("id");
    rc_scale_pdf.addValue("pdf");
    regexOptions.push_back(rc_scale_pdf);

	
    RegexCreater rc_scale;
    rc_scale.addValue("muf");
    rc_scale.addValue("mur");
    rc_scale.addValue("id");
    regexOptions.push_back(rc_scale);


    // ZZTo4L_powheg_2016_weightInfo.txt : scale / missing pdf option though...
    // DrellYan_NLO_MGFXFXv233_2016_weightInfo.txt : scale
    // DrellYan_LO_MGMLMv233_2016_weightInfo.txt : scale
    RegexCreater rc_scaleAlt;
    rc_scaleAlt.addValue("muf", {"facscfact"}, true);
    rc_scaleAlt.addValue("mur", {"renscfact"}, true);
    rc_scaleAlt.addValue("id");
    regexOptions.push_back(rc_scaleAlt);


    // ZZTo4L_powheg_2016_weightInfo.txt : other
    // ZZTo4L_powheg_2017_weightInfo.txt : other
    // DrellYan_NLO_MGFXFXv242_2017_weightInfo.txt : other
    // DrellYan_NLO_MGFXFXv233_2016_weightInfo.txt : other
    RegexCreater rc_pdfAlt;
    rc_pdfAlt.addValue("pdf", {"pdf set", "lhapdf", "pdfset"}, true);
    rc_pdfAlt.addValue("id");
    regexOptions.push_back(rc_pdfAlt);
    
    // DrellYan_LO_MGMLMv233_2016_weightInfo.txt : other
    RegexCreater rc_idOnly;
    rc_idOnly.addValue("id");
    regexOptions.push_back(rc_idOnly);
    
    weightGroupInfo.addValue("name", {"type"});
    weightGroupInfo.addValue("combine");


}

std::map<std::string , std::string>
dylanTest::LHEWeightGroupReaderHelper::getMap_testAll(std::string line, std::vector<RegexCreater> options) {
    for (size_t i = 0; i < options.size(); ++i) {
	if(options[i].processString(line)) {
	    return options[i].getTagMap();
	}
    }
    ////// problem!!!!
    std::cout << "problem!!" << "\n";
    return std::map<std::string , std::string >();
}


void
dylanTest::LHEWeightGroupReaderHelper::parseLHEFile(std::string filename) {
    std::ifstream file;
    file.open(filename);

    std::string line;
    std::smatch matches;
    // TODO: Not sure the weight indexing is right here, this seems to more or less
    // count the lines which isn't quite the goal. TOCHECK!
    int index = 0;
    while(getline(file, line)) {
	if(std::regex_match(line, weightGroupStart_)) {
	    std::string name = getMap_testAll(line, {weightGroupInfo})["name"];
                
	    //TODO: Fine for now, but in general there should also be a check on the PDF weights,
	    // e.g., it could be an unknown weight
	    
	    if(std::regex_match(name, scaleWeightMatch_)) {
		weightGroups_.push_back(new gen::ScaleWeightGroupInfo(line));
		std::cout << "scale weight" << "\n";
	    }


	    else
		weightGroups_.push_back(new gen::PdfWeightGroupInfo(line));

	    /// file weights
	    while(getline(file, line) && !std::regex_match(line, weightGroupEnd_)) {
		auto tagsMap = getMap_testAll(line, regexOptions);
                
		std::regex_search(line, matches, weightContent_);
		// TODO: Add proper check that it worked
		std::string content = matches[1].str();

		auto& group = weightGroups_.back();
		if (group.weightType() == gen::kScaleWeights) {
		    float muR = std::stof(tagsMap["mur"]);
		    float muF = std::stof(tagsMap["muf"]);
		    auto& scaleGroup = static_cast<gen::ScaleWeightGroupInfo&>(group);
		    scaleGroup.addContainedId(index, tagsMap["id"], line, muR, muF);
		}
		else
		    group.addContainedId(index, tagsMap["id"], line);

		index++;                        
	    }
	}
    }
}

void
dylanTest::LHEWeightGroupReaderHelper::parseWeightGroupsFromHeader(std::vector<std::string> lheHeader) {
    std::smatch matches;
    // TODO: Not sure the weight indexing is right here, this seems to more or less
    // count the lines which isn't quite the goal. TOCHECK!
    int index = 0;
    bool foundGroup = false;
    for (std::string headerLine : lheHeader) {
	std::cout << "Header line is:" << headerLine;
	//TODO: Fine for now, but in general there should also be a check on the PDF weights,
	// e.g., it could be an unknown weight
	//std::cout << "weightGroupStart_ .*<weightgroup.+>.* ... match? " << static_cast<int>(std::regex_match(headerLine, weightGroupStart_)) << std::endl;
	if (std::regex_match(headerLine, weightGroupStart_)) {
	    //std::cout << "Adding new group for headerLine" << std::endl;
	    foundGroup = true;
	    std::string name = getMap_testAll(headerLine, {weightGroupInfo})["name"];
                
	    if(std::regex_match(name, scaleWeightMatch_)) {
		weightGroups_.push_back(new gen::ScaleWeightGroupInfo(headerLine));
		std::cout << "scale weight" << "\n";
	    }
            
	    else
		weightGroups_.push_back(new gen::PdfWeightGroupInfo(headerLine));
	}
	/// file weights
	else if (foundGroup && !std::regex_match(headerLine, weightGroupEnd_)) {
	    //std::cout << "Adding new weight for headerLine" << std::endl;
	    auto tagsMap = getMap_testAll(headerLine, regexOptions);
	    for(auto pair: tagsMap) {
		std::cout << pair.first << ": " << pair.second << " | ";
	    }
	    std::cout << "\n";
	    std::regex_search(headerLine, matches, weightContent_);
	    // TODO: Add proper check that it worked
	    std::string content = matches[1].str();
	    // std::cout << content << "\n";
	    
	    auto& group = weightGroups_.back();
	    if (group.weightType() == gen::kScaleWeights) {
		float muR = std::stof(tagsMap["mur"]);
		float muF = std::stof(tagsMap["muf"]);
		std::cout << tagsMap["id"] << " " << muR << " " << muF << " " << content << "\n";
		auto& scaleGroup = static_cast<gen::ScaleWeightGroupInfo&>(group);
		scaleGroup.addContainedId(index, tagsMap["id"], headerLine, muR, muF);
	    }
	    else
		group.addContainedId(index, tagsMap["id"], headerLine);
	    index++;
	}
	else {
	    foundGroup = false;
	}
    }
}



#endif


