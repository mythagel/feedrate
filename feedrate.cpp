#include "feedrate.h"
#include "binding.h"
#include <vector>
#include <cstdio>

#include <cmath>
#include <cstring>

constexpr double PI = 3.1415926535897932;

// function to assume Zc from Zn...?

// fz function
using fz = function<tag_FeedPerTooth, tag_Feedrate, tag_SpindleSpeed, tag_CutterTeeth>;
template <>
struct impl <fz> {
    double operator()(double Vf, double n, unsigned Zc) const {
        return Vf / (n * Zc);
    }
};

namespace formulas {
// http://www.sandvik.coromant.com/en-us/knowledge/milling/formulas_and_definitions/formulas

/* Dcap/mm      - Cutter diameter at actual depth of cut
 * fz/mm        - feed per tooth
 * Zn           - total cutter teeth
 * Zc           - effective cutter teeth
 * Vf/mm/min    - table feed
 * fn/mm        - feed per revolution
 * ap/mm        - depth of cut
 * Vc/m/min     - Cutting speed
 * Y0           - chip rake angle
 * ae/mm        - working engagement
 * n/rpm        - spindle speed
 * Pc/kW        - net power
 * Mc/Nm        - Torque
 * Q/cm3/min    - Material removal rate
 * hm/mm        - Average chip thickness
 * hex/mm       - Max chip thickness
 * Kr/deg       - Entering angle
 * Dm/mm        - Machined diameter (component diameter)
 * Dw/mm        - Unmachined diameter
 * Vfm/mm/min   - Table feed of tool at Dm (machined diameter)
 */

double Vc(double Dcap, double n) {
    return (Dcap * PI * n) / 1000.0;
}

double n(double Vc, double Dcap) {
    return (Vc * 1000.0) / (PI * Dcap);
}

double fz(double Vf, double n, unsigned Zc) {
    return Vf / (n * Zc);
}

double Q(double ap, double ae, double Vf) {
    return (ap * ae * Vf) / 1000.0;
}

double Vf(double fz, double n, double Zc) {
    return fz * n * Zc;
}

double Mc(double Pc, double n) {
    return (Pc * 30.0 * 1000.0) / (PI * n);
}

double Pc(double ap, double ae, double Vf, double kc) {
    return (ap * ae * Vf * kc) / (60 * 1000000.0);
}

double hm_side(double Kr, double ae, double fz, double Dcap) {
    auto deg2rad = [](double d) { return (d / 180.0) * PI; };
    return (360 * std::sin(deg2rad(Kr)) * ae * fz) / (PI * Dcap * std::acos(deg2rad(1- ((2 * ae) / Dcap) )));
}
double hm_face(double Kr, double ae, double fz, double Dcap) {
    auto deg2rad = [](double d) { return (d / 180.0) * PI; };
    return (180 * std::sin(deg2rad(Kr)) * ae * fz) / (PI * Dcap * std::asin(deg2rad(ae/Dcap)));
}

}


extern "C" bool calculate(const TaggedValue* in, unsigned in_size, TaggedValue* out, unsigned out_size) {
    if (in_size == 0 || out_size == 0)
        return false;

    auto fn = fz();
    if (fn.out == out[0].tag && fn.has_in(in, in_size)) {
        out[0].value = fn(in, in_size);
        return true;
    }

/*    using fn_table = function_table<fz>;

    for (unsigned i = 0; i < out_size; ++i) {
        for (unsigned fn = 0; fn < max; ++fn) {
            if (fn_table::out(fn) == out[i].tag && fn_table::has_in(fn, in, in_size)) {
                // found a function with these output parameters && available input parameters
                out[i].value = fn_table::call(fn, in, in_size);
            }
        }
    }*/

    return false;
}

std::string fcc(unsigned c) {
    return {
        static_cast<char>((c & 0xFF000000ul) >> 24),
        static_cast<char>((c & 0xFF0000ul) >> 16),
        static_cast<char>((c & 0xFF00ul) >> 8),
        static_cast<char>(c & 0xFFul)
    };
}

int main() {
    TaggedValue in[] = {
        {tag_SpindleSpeed, 3000},
        {tag_Feedrate, 400},
        {tag_CutterTeeth, 4}
    };

    TaggedValue out[] = {
        {tag_FeedPerTooth, 0}
    };

    if (calculate(in, 3, out, 1)) {
        for (auto param : out)
            fprintf(stderr, "%s: %f\n", fcc(param.tag).c_str(), param.value);
    } else {
        fprintf(stderr, "Unable to determine all output parameters.\n");
    }
}
