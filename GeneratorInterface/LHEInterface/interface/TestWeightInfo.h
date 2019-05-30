#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include <regex>

std::string parseId(std::string label) {
    auto re = std::regex("id=\"([0-9]*)\"");
    std::smatch matches;
    std::regex_search(label, matches, re);
    return std::string(matches.str(1));
}

gen::WeightGroupInfo getExampleScaleWeights() { 
    gen::WeightGroupInfo scaleInfo(
            "<weightgroup combine=\"envelope\" name=\"Central scale variation\">",
            "centralScaleVariations"
            );
    std::vector<std::string> entries = {
        R"(<weight MUF="1" MUR="1" PDF="263400" id="1"> mur=1 muf=1 </weight>)",
        R"(<weight MUF="2" MUR="1" PDF="263400" id="2"> mur=1 muf=2 </weight>)",
        R"(<weight MUF="0.5" MUR="1" PDF="263400" id="3"> mur=1 muf=0.5 </weight>)",
        R"(<weight MUF="1" MUR="2" PDF="263400" id="4"> mur=2 muf=1 </weight>)",
        R"(<weight MUF="2" MUR="2" PDF="263400" id="5"> mur=2 muf=2 </weight>)",
        R"(<weight MUF="0.5" MUR="2" PDF="263400" id="6"> mur=2 muf=0.5 </weight>)",
        R"(<weight MUF="1" MUR="0.5" PDF="263400" id="7"> mur=0.5 muf=1 </weight>)",
        R"(<weight MUF="2" MUR="0.5" PDF="263400" id="8"> mur=0.5 muf=2 </weight>)",
        R"(<weight MUF="0.5" MUR="0.5" PDF="263400" id="9"> mur=0.5 muf=0.5 </weight>)",
    };
    scaleInfo.setWeightType(gen::kScaleWeights);

    for (size_t i = 0; i < entries.size(); i++) {
        scaleInfo.addContainedId(i, parseId(entries[i]), entries[i]); 
    }
    return scaleInfo;
}

gen::WeightGroupInfo getExampleScaleWeightsOutOfOrder() { 
    gen::WeightGroupInfo scaleInfo(
            "<weightgroup combine=\"envelope\" name=\"Central scale variation\">",
            "centralScaleVariations"
            );
    std::vector<std::string> entries = {
        R"(<weight MUF="1" MUR="1" PDF="263400" id="1"> mur=1 muf=1 </weight>)",
        R"(<weight MUF="2" MUR="1" PDF="263400" id="2"> mur=1 muf=2 </weight>)",
        R"(<weight MUF="0.5" MUR="1" PDF="263400" id="3"> mur=1 muf=0.5 </weight>)",
        R"(<weight MUF="2" MUR="2" PDF="263400" id="5"> mur=2 muf=2 </weight>)",
        R"(<weight MUF="1" MUR="2" PDF="263400" id="4"> mur=2 muf=1 </weight>)",
        R"(<weight MUF="0.5" MUR="2" PDF="263400" id="6"> mur=2 muf=0.5 </weight>)",
        R"(<weight MUF="2" MUR="0.5" PDF="263400" id="8"> mur=0.5 muf=2 </weight>)",
        R"(<weight MUF="1" MUR="0.5" PDF="263400" id="7"> mur=0.5 muf=1 </weight>)",
        R"(<weight MUF="0.5" MUR="0.5" PDF="263400" id="9"> mur=0.5 muf=0.5 </weight>)",
    };
    scaleInfo.setWeightType(gen::kScaleWeights);

    for (size_t i = 0; i < entries.size(); i++) {
        scaleInfo.addContainedId(i, parseId(entries[i]), entries[i]); 
    }
    return scaleInfo;
}

