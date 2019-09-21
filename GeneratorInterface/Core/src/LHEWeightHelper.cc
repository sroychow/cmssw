#include "GeneratorInterface/Core/interface/LHEWeightHelper.h"
#include <iostream>

using namespace tinyxml2;

namespace gen {
    std::string
    LHEWeightHelper::toLowerCase(const char* name) {
        std::string returnStr;
        for (size_t i = 0; i < strlen(name); ++i)
            returnStr.push_back(tolower(name[i]));
        return returnStr;
    }

    std::string
    LHEWeightHelper::toLowerCase(const std::string name) {
        std::string returnStr = name;
        transform(name.begin(), name.end(), returnStr.begin(), ::tolower);    
        return returnStr;

        
    }

    void LHEWeightHelper::loadAttributeNames(std::string baseName, std::vector<std::string> altNames) {
        for(auto altname : altNames) {
            nameConvMap[altname] = baseName;
        }
        nameConvMap[baseName] = baseName;
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
        
        std::cout << "Init" << "\n";
        
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
            std::cout << "we have a problem!" << "\n";
        return std::map<std::string , std::string >();
        //do something....
        }

        std::map<std::string, std::string> attMap;
        XMLElement* element = xmlParser.FirstChildElement();
        
        for( const XMLAttribute* a = element->FirstAttribute(); a; a=a->Next()) {
            attMap[nameConvMap[toLowerCase(a->Name())]] = a->Value();
        }
        // get stuff from content of tag if it has anything.
        // always assume format is AAAAA=(  )BBBB    (  ) => optional space
        if (element->GetText() == nullptr) {
            return attMap;
        }
        // This adds "content: " to the beginning of the content. not sure if its a big deal or?
        std::string content = element->GetText();
        attMap["content"] = content;
        
        std::regex reg("(?:(\\S+)=\\s*(\\S+))");
        std::smatch m;
        while(std::regex_search(content, m, reg)) {
        std::string key = nameConvMap[toLowerCase(m.str(1))];
        if (attMap[key] != std::string()) {
            if (m[2] != attMap[key]) {
            std::cout << m.str(2) << " vs " << attMap[key];
            // might do something if content and attribute don't match?
            // but need to be careful since some are purposefully different
            // eg dyn_scale is described in content but just given a number
            }
        }
        else {
            attMap[key] = m.str(2);
        }
        content = m.suffix().str();
        }
        return attMap;
        
    }

    bool
    LHEWeightHelper::isAWeight(std::string line) {
        XMLDocument xmlParser;
        int error = xmlParser.Parse(line.c_str());
        if (error) {
            return false;
            //do something....
        }
        XMLElement* element = xmlParser.FirstChildElement();
        return element;
    }

    void
    LHEWeightHelper::parseWeightGroupsFromHeader(std::vector<std::string> lheHeader) {
        // TODO: Not sure the weight indexing is right here, this seems to more or less
        // count the lines which isn't quite the goal. TOCHECK!
        int index = 0;
        bool foundGroup = false;
        
        for (std::string headerLine : lheHeader) {
            std::cout << "Header line is:" << headerLine;
            headerLine = sanitizeText(headerLine);
                std::cout << "Header line is:" << weightGroups_.size() << " "<< headerLine;
            //TODO: Fine for now, but in general there should also be a check on the PDF weights,
                // e.g., it could be an unknown weight
            
            if (std::regex_match(headerLine, weightGroupStart_)) {
                //std::cout << "Adding new group for headerLine" << std::endl;
                foundGroup = true;
                std::string fullTag = headerLine + "</weightgroup>";
                auto groupMap = getAttributeMap(fullTag);
                std::string name = groupMap["name"];
                    
                if(name.find("Central scale variation") != std::string::npos ||
                name.find("scale_variation") != std::string::npos) {
                    weightGroups_.push_back(new gen::ScaleWeightGroupInfo(name));
                }
                else
                    weightGroups_.push_back(new gen::PdfWeightGroupInfo(name));
            }
            /// file weights
            else if (foundGroup && isAWeight(headerLine)) {
                //std::cout << "Adding new weight for headerLine" << std::endl;
                auto tagsMap = getAttributeMap(headerLine);
                for(auto pair: tagsMap) {
                    std::cout << pair.first << ": " << pair.second << " | ";
                }
                std::cout << "\n";
                
                std::string content = tagsMap["content"];
                if (tagsMap["id"].empty()) {
                    std::cout << "error" << "\n";
                    // should do something
                }
                
                auto& group = weightGroups_.back();
                if (group.weightType() == gen::kScaleWeights) {
                    if (tagsMap["mur"].empty()  || tagsMap["muf"].empty()) {
                        std::cout << "error" << "\n";
                        // something should happen here
                        continue;
                    }
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
            // commented out since code doesn't work with this in....
            // else if(isAWeight(headerLine)) {
            //     // found header. Don't know what to do with it so just shove it into a new weightgroup for now
            //     // do minimum work for it
            //     weightGroups_.push_back(new gen::PdfWeightGroupInfo(headerLine));
            //     auto& group = weightGroups_.back();
            //     auto tagsMap = getAttributeMap(headerLine);
            //     group.addContainedId(index, tagsMap["id"], headerLine);
            //     foundGroup = true;
            //     index++;
            // }
        
            else if(std::regex_match(headerLine, weightGroupEnd_)) {
                foundGroup = false;
            } 
            else {
                    std::cout << "problem!!!" << "\n";
            }
        }
    }
}

