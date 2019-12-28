#include "GeneratorInterface/Core/interface/LHEWeightHelper.h"
#include <iostream>

using namespace tinyxml2;

namespace gen {
    void LHEWeightHelper::loadAttributeNames(std::string baseName, std::vector<std::string> altNames) {
        for(auto altname : altNames) {
            nameConversionMap_[altname] = baseName;
        }
        nameConversionMap_[baseName] = baseName;
    }

    std::string
    LHEWeightHelper::sanitizeText(std::string line) {
        std::map<std::string, std::string > replaceMap = {{"&lt;", "<"}, {"&gt;", ">"}};

        for(auto pair: replaceMap) {
        std::string badText = pair.first;
        std::string goodText = pair.second;
        while(line.find(badText) != std::string::npos) {
            size_t spot = line.find(badText);
            line.replace(spot, badText.size(), goodText);
        }
        }
        return line;
    }


    LHEWeightHelper::LHEWeightHelper() {
        weightGroupStart_ = std::regex(".*<weightgroup.+>.*\n*");
        weightGroupEnd_ = std::regex(".*</weightgroup>.*\n*");
        
        /// Might change later, order matters and code doesn't pick choices

        // Used for translating different naming convention to a common one
        loadAttributeNames("muf", {"facscfact"});
        loadAttributeNames("mur", {"renscfact"});
        loadAttributeNames("id");
        loadAttributeNames("pdf", {"pdf set", "lhapdf", "pdfset"});
        loadAttributeNames("dyn_scale");

        loadAttributeNames("combine");
        loadAttributeNames("name", {"type"});

    }

    std::map<std::string, std::string>
    LHEWeightHelper::getAttributeMap(std::string line) {
        XMLDocument xmlParser;
        int error = xmlParser.Parse(line.c_str());
        if (error) {
            return std::map<std::string , std::string >();
        }

        std::map<std::string, std::string> attributeMap;
        XMLElement* element = xmlParser.FirstChildElement();
        
        for( const XMLAttribute* a = element->FirstAttribute(); a; a=a->Next()) {
            auto name = boost::algorithm::to_lower_copy(std::string(a->Name()));
            attributeMap[nameConversionMap_[name]] = a->Value();
        }
        // get stuff from content of tag if it has anything.
        // always assume format is AAAAA=(  )BBBB    (  ) => optional space
        if (element->GetText() == nullptr) {
            return attributeMap;
        }
        // This adds "content: " to the beginning of the content. not sure if its a big deal or?
        std::string content = element->GetText();
        attributeMap["content"] = content;
        
        for (const auto& entry : weightAttributeMapFromHeaderLine(content)) {
            auto rawLabel = boost::algorithm::to_lower_copy(entry.first);
            std::string label = nameConversionMap_[rawLabel];
            if (nameConversionMap_.find(label) != nameConversionMap_.end())
                attributeMap[nameConversionMap_[label]] = entry.second;
            else 
                attributeMap[label] = entry.second;
        }
            
        return attributeMap;
        
    }

    bool
    LHEWeightHelper::isAWeight(std::string line) {
        XMLDocument xmlParser;
        int error = xmlParser.Parse(line.c_str());
        if (error) {
            return false;
            //throw std::invalid_argument("Failed to parse weight info from header! Line was " + line);
        }
        XMLElement* element = xmlParser.FirstChildElement();
        return element;
    }

    std::map<std::string, std::string> 
    LHEWeightHelper::weightAttributeMapFromHeaderLine(std::string line) {
        std::regex reg("(?:(\\S+)=\\s*(\\S+))");
        std::smatch match;
        std::map<std::string, std::string> weightAttributeMap;
        while(std::regex_search(line, match, reg)) {
            weightAttributeMap[match.str(1)] = match.str(2);
            line = match.suffix().str();
        }
        return weightAttributeMap;
    }

    void
    LHEWeightHelper::parseWeightGroupsFromHeader(std::vector<std::string> lheHeader) {
        // TODO: Not sure the weight indexing is right here, this seems to more or less
        // count the lines which isn't quite the goal. TOCHECK!
        int index = 0;
        currGroupAttributeMap_.clear();
        currWeightAttributeMap_.clear();
        
        for (std::string headerLine : lheHeader) {
            headerLine = sanitizeText(headerLine);
            
            if (std::regex_match(headerLine, weightGroupStart_)) {
                if (!currGroupAttributeMap_.empty())
                    setGroupInfo();
                std::string fullTag = headerLine + "</weightgroup>";
                currGroupAttributeMap_ = getAttributeMap(fullTag);
                auto name = currGroupAttributeMap_["name"];
                    
                if (currentGroupIsScale()) {
                    weightGroups_.push_back(std::make_unique<gen::ScaleWeightGroupInfo>(name));
                }
                else if (currentGroupIsPdf()) {
                    weightGroups_.push_back(std::make_unique<gen::PdfWeightGroupInfo>(name));
                }
                else {
                    weightGroups_.push_back(std::make_unique<gen::UnknownWeightGroupInfo>(name));
                }
            }
            else if (std::regex_match(headerLine, std::regex(".*<weight.*>.*\n*"))) {
                std::string fullTag = headerLine;
                if (!std::regex_match(headerLine, std::regex(".*</weight.*>.*\n*")))
                    fullTag = headerLine + "</weight>";
                currWeightAttributeMap_.clear();
                // This shouldn't really happen, but perhaps we find weights outside of weight groups?
                currWeightAttributeMap_ = getAttributeMap(fullTag);
                
                std::string content = currWeightAttributeMap_["content"];
                if (currWeightAttributeMap_["id"].empty()) {
                    std::cout << "error" << "\n";
                    // should do something
                }
                
                if (weightGroups_.empty()) {
                    weightGroups_.push_back(std::make_unique<gen::UnknownWeightGroupInfo>("Unknown"));
                }
                auto& group = weightGroups_.back();
                if (group.weightType() == gen::kScaleWeights) {
                    if (currWeightAttributeMap_["mur"].empty()  || currWeightAttributeMap_["muf"].empty()) {
                        group.setIsWellFormed(false);
                    }
                    else { 
                        try {
                            float muR = std::stof(currWeightAttributeMap_["mur"]);
                            float muF = std::stof(currWeightAttributeMap_["muf"]);
                            auto& scaleGroup = dynamic_cast<gen::ScaleWeightGroupInfo&>(group);
                            scaleGroup.addContainedId(index, currWeightAttributeMap_["id"], headerLine, muR, muF);
                        }
                        catch(std::invalid_argument& e) {
                            group.setIsWellFormed(false);
                            group.addContainedId(index, currWeightAttributeMap_["id"], headerLine);
                        }
                    }
                }
                else
                    group.addContainedId(index, currWeightAttributeMap_["id"], headerLine);
                
                index++;
            }
            else if (std::regex_match(headerLine, weightGroupEnd_)) {
                if (!currGroupAttributeMap_.empty())
                    setGroupInfo();
                currGroupAttributeMap_.clear();
                currWeightAttributeMap_.clear();
            }
            // Should be fine to ignore all other lines? Tend to be either empty or closing tag
        }
    }
}

