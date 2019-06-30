#include "feedrate.h"
#include "utils.h"
#include <cstdio>
#include <vector>

double get(unsigned tag, const std::vector<TaggedValue>& v) {
    for (auto& tv : v)
        if (tv.tag == tag)
            return tv.value;
    return 0.0;
}
void set(unsigned tag, double value, std::vector<TaggedValue>& v) {
    for (auto& tv : v)
        if (tv.tag == tag) {
            tv.value = value;
            return;
        }
    v.push_back({tag, value});
}

int main() {
    std::vector<TaggedValue> in = {

        // Tool parameters
        {tag_FeedPerTooth, 0.012},   // 4mm 4 flute endmill slotting in mild steel
        {tag_CutterDiameterAtDepthOfCut, 4},
        {tag_CutterTeeth, 4},
        {tag_CutterOverhang, 20},
        {tag_CutterMaterialElasticity, 650000}, // Carbide, 650 GPa

        // Material parameters
        {tag_CuttingSpeed, 3},    // mild steel low-end
        {tag_SpecificCuttingForce, 1500},
        {tag_MaterialTensileStrength, 440},

        // Cut parameters
        {tag_DepthOfCut, 0.6},
        {tag_WorkingEngagement, 4},
        {tag_EffectiveCutterTeeth, 4},
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
        {tag_TangentialForce, 0},
    };

    while (true) {
        if (calculate(in.data(), in.size(), out.data(), out.size())) {
            for (auto param : out)
                fprintf(stderr, "%s: %f\n", fcc(param.tag).c_str(), param.value);
        } else {
            fprintf(stderr, "Unable to determine all output parameters.\n");
            break;
        }

        double max_torque = 0.07061551834;
        if (get(tag_Torque, out) > max_torque) {
            fprintf(stderr, "over torque!\n");
        }

        // Adjust spindle speed and feedrate to within limits
        double max_rpm = 2800;
        double max_tablefeed = 200;     // arbitary
        if (get(tag_SpindleSpeed, out) > max_rpm) {
            fprintf(stderr, "adjust speed\n");
            double alpha = max_rpm / get(tag_SpindleSpeed, out);
            set(tag_SpindleSpeed, max_rpm, in);
            set(tag_TableFeed, get(tag_TableFeed, out) * alpha, in);
        } else if (get(tag_TableFeed, out) > max_tablefeed) {
            fprintf(stderr, "adjust feed\n");
            double alpha = max_tablefeed / get(tag_TableFeed, out);
            set(tag_SpindleSpeed, get(tag_SpindleSpeed, out) * alpha, in);
            set(tag_TableFeed, max_tablefeed, in);
        } else {
            break;
        }
        fprintf(stderr, "\n");
    }
}
