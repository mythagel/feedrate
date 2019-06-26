#include "feedrate.h"
#include "utils.h"
#include <cstdio>
#include <vector>

int main() {
    std::vector<TaggedValue> in = {

        // Tool parameters
        {tag_FeedPerTooth, 0.05},   // 4mm 4 flute endmill
        {tag_CutterDiameterAtDepthOfCut, 4},
        {tag_CutterTeeth, 4},
        {tag_EffectiveCutterTeeth, 2},
        {tag_CutterOverhang, 40},
        {tag_CutterMaterialElasticity, 650000}, // Carbide, 650 GPa

        // Material parameters
        {tag_CuttingSpeed, 150},    // 6061-T6 low-end
        {tag_SpecificCuttingForce, 400},
        {tag_MaterialTensileStrength, 310},

        // Cut parameters
        {tag_DepthOfCut, 4},
        {tag_WorkingEngagement, 4},
    };

    std::vector<TaggedValue> out = {
        // Direct outputs
        {tag_TableFeed, 0},
        {tag_SpindleSpeed, 0},

        // Used for fitness function
        {tag_MaterialRemovalRate, 0},       // cm3/min
        {tag_NetPower, 0},                  // kW
        {tag_Torque, 0},                    // Nm
        {tag_Deflection, 0},                // mm
    };

    if (calculate(in.data(), in.size(), out.data(), out.size())) {
        for (auto param : out)
            fprintf(stderr, "%s: %f\n", fcc(param.tag).c_str(), param.value);
    } else {
        fprintf(stderr, "Unable to determine all output parameters.\n");
    }
}
