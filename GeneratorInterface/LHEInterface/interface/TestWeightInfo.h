#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

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
        scaleInfo.addContainedId(i, std::to_string(i+1), entries[i]); 
    }
    return scaleInfo;
}

