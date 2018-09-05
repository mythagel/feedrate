#include "feedrate.h"
#include "utils.h"
#include <cstdio>
#include <vector>

int main() {
    std::vector<TaggedValue> in = {
        {tag_FeedPerTooth, 0.05},   // 4mm 4 flute endmill
        {tag_CuttingSpeed, 150},    // 6061-T6 low-end
        //{tag_SpindleSpeed, 2000},

        {tag_CutterDiameterAtDepthOfCut, 4},
        {tag_CutterTeeth, 4},
        {tag_EffectiveCutterTeeth, 4},
    };

    std::vector<TaggedValue> out = {
        {tag_TableFeed, 0},
        {tag_SpindleSpeed, 0},
    };

    if (calculate(in.data(), in.size(), out.data(), out.size())) {
        for (auto param : out)
            fprintf(stderr, "%s: %f\n", fcc(param.tag).c_str(), param.value);
    } else {
        fprintf(stderr, "Unable to determine all output parameters.\n");
    }
}
