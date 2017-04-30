#include "feedrate.h"
#include "binding.h"
#include <vector>
#include <cstdio>
#include <sstream>
#include <functional>
#include <tuple>

#include <cmath>
#include <cstring>
#include "id.h"


// http://www.sandvik.coromant.com/en-us/knowledge/milling/formulas_and_definitions/formulas
constexpr double PI = 3.1415926535897932;

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

template <> struct bind <id::Vc> {
    double operator()(double Dcap, double n) const {
        return (Dcap * PI * n) / 1000.0;
    }
};

template <> struct bind <id::n> {
    double operator()(double Vc, double Dcap) const {
        return (Vc * 1000.0) / (PI * Dcap);
    }
};

template <> struct bind <id::fz> {
    double operator()(double Vf, double n, unsigned Zc) const {
        return Vf / (n * Zc);
    }
};

template <> struct bind <id::Q> {
    double operator()(double ap, double ae, double Vf) const {
        return (ap * ae * Vf) / 1000.0;
    }
};

template <> struct bind <id::Vf> {
    double operator()(double fz, double n, double Zc) const {
        return fz * n * Zc;
    }
};

template <> struct bind <id::Mc> {
    double operator()(double Pc, double n) const {
        return (Pc * 30.0 * 1000.0) / (PI * n);
    }
};

template <> struct bind <id::Pc> {
    double operator()(double ap, double ae, double Vf, double kc) const {
        return (ap * ae * Vf * kc) / (60 * 1000000.0);
    }
};

double hm_side(double Kr, double ae, double fz, double Dcap) {
    auto deg2rad = [](double d) { return (d / 180.0) * PI; };
    return (360 * std::sin(deg2rad(Kr)) * ae * fz) / (PI * Dcap * std::acos(deg2rad(1- ((2 * ae) / Dcap) )));
}
double hm_face(double Kr, double ae, double fz, double Dcap) {
    auto deg2rad = [](double d) { return (d / 180.0) * PI; };
    return (180 * std::sin(deg2rad(Kr)) * ae * fz) / (PI * Dcap * std::asin(deg2rad(ae/Dcap)));
}


std::string fcc(unsigned c) {
    return {
        static_cast<char>((c & 0xFF000000ul) >> 24),
        static_cast<char>((c & 0xFF0000ul) >> 16),
        static_cast<char>((c & 0xFF00ul) >> 8),
        static_cast<char>(c & 0xFFul)
    };
}
template <unsigned Out, unsigned... In>
std::string to_string(const function<Out, In...>& fn) {
    std::stringstream s;
    s << "(";
    for (auto& in : fn.in)
        s << " '" << fcc(in) << "'";
    s << " )";
    s << " -> '" << fcc(fn.out) << "'";
    return s.str();
}


// function to assume Zc from Zn...?

/* List of mappings i.e. functions which calculate output from inputs
 * unknowns can be substituted by functions with that output parameter
 * where multiple functions have that output parameter, then branch to 
 * investigate each option.
 *
 * */

namespace detail {
template <typename Fn, typename T, std::size_t... I>
void for_each(const T& t, Fn fn, std::index_sequence<I...>) {
    auto _ = { (fn(std::get<I>(t)), 0)... };
    (void)_;
}
}
template <typename Fn, typename... Args>
void for_each(const std::tuple<Args...>& t, Fn fn) {
    detail::for_each(t, fn, std::index_sequence_for<Args...>{});
}

extern "C" bool calculate(const TaggedValue* in, unsigned in_size, TaggedValue* out, unsigned out_size) {
    static constexpr auto t = std::make_tuple(id::Vc(), id::n(), id::fz(), id::Q(), id::Vf(), id::Mc(), id::Pc());

    if (in_size == 0 || out_size == 0)
        return false;

    std::vector<TaggedValue> values(in, in+in_size);

    auto exists = [&](unsigned tag) {
        for (auto& v : values)
            if (v.tag == tag) return true;
        return false;
    };

    for (unsigned _ = 0; _ < 10; ++_)
    for_each(t, [&](auto fn) {
        if (!exists(fn.out) && fn.has_in(values.data(), values.size())) {
            fprintf(stderr, "%s\n", to_string(fn).c_str());
            values.push_back({ fn.out, fn(values.data(), values.size())} );
        }
    });

    for (unsigned i = 0; i < out_size; ++i) {
        auto get = [&](unsigned tag, double& value) {
            for (auto& v : values)
                if (v.tag == tag) {
                    value = v.value;
                    return true;
                }
            return false;
        };

        //if (!get(out[i].tag, out[i].value))
        //    return false;
        get(out[i].tag, out[i].value);
    }

    return true;
}


int main() {
    std::vector<TaggedValue> in = {
        {tag_SpindleSpeed, 3000},
        {tag_TableFeed, 400},
        {tag_DepthOfCut, 4},
        {tag_WorkingEngagement, 4}, // slotting

        {tag_CutterDiameterAtDepthOfCut, 4},
        {tag_CutterTeeth, 4},
        {tag_EffectiveCutterTeeth, 4},
    };

    std::vector<TaggedValue> out = {
        {tag_CutterDiameterAtDepthOfCut, 0},
        {tag_FeedPerTooth, 0},
        {tag_CutterTeeth, 0},
        {tag_EffectiveCutterTeeth, 0},
        {tag_TableFeed, 0},
        {tag_FeedPerRevolution, 0},
        {tag_DepthOfCut, 0},
        {tag_CuttingSpeed, 0},
        {tag_ChipRakeAngle, 0},
        {tag_WorkingEngagement, 0},
        {tag_SpindleSpeed, 0},
        {tag_NetPower, 0},
        {tag_Torque, 0},
        {tag_MaterialRemovalRate, 0},
        {tag_AverageChipThickness, 0},
        {tag_MaxChipThickness, 0},
        {tag_EnteringAngle, 0},
        {tag_MachinedDiameter, 0},
        {tag_UnmachinedDiameter, 0},
        {tag_TableFeedAtMachinedDiameter, 0},
        {tag_SpecificCuttingForce, 0}
    };

    if (calculate(in.data(), in.size(), out.data(), out.size())) {
        for (auto param : out)
            fprintf(stderr, "%s: %f\n", fcc(param.tag).c_str(), param.value);
    } else {
        fprintf(stderr, "Unable to determine all output parameters.\n");
    }
}
