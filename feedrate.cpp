#include "feedrate.h"
#include "binding.h"
#include <vector>
#include <cstdio>
#include <sstream>
#include <functional>
#include <tuple>
#include <cstdint>

#include <cmath>
#include <cstring>
#include "id.h"
#include "utils.h"


// http://www.sandvik.coromant.com/en-us/knowledge/milling/formulas_and_definitions/formulas
// https://www.ctemag.com/news/articles/understanding-tangential-cutting-force-when-milling
// https://skyciv.com/tutorials/what-is-deflection/
// https://www.engineeringtoolbox.com/cantilever-beams-d_1848.html
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

template <> struct bind <id::F> {
    double operator()(double T, double ap, double ZE, double I, double Fc) const {
        double a = T - ap;
        return ((Fc * pow(a, 3)) / (3*ZE*I)) * (1 + (3*ap) / (2*a));
    }
};

template <> struct bind <id::I> {
    double operator()(double Dcap) const {
        // TODO calculate core diameter from Dcap
        // Or average between core and Dcap
        return (PI * pow(Dcap, 4)) / 64.0;
    }
};

template <> struct bind <id::A> {
    double operator()(double ap, double fz) const {
        return ap * fz;
    }
};

template <> struct bind <id::Fc> {
    double operator()(double sig, double A, double Zc) const {
        return sig * A * Zc;
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

template <uint32_t Out, uint32_t... In>
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
    static constexpr auto t = std::make_tuple
        (
         id::Vc(), 
         id::n(), 
         id::fz(), 
         id::Q(), 
         id::Vf(), 
         id::Mc(), 
         id::Pc(), 
         id::F(), 
         id::I(),
         id::A(),
         id::Fc()
        );

    if (in_size == 0 || out_size == 0)
        return false;

    std::vector<TaggedValue> values(in, in+in_size);

    auto exists = [&](uint32_t tag) {
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
        auto get = [&](uint32_t tag, double& value) {
            for (auto& v : values)
                if (v.tag == tag) {
                    value = v.value;
                    return true;
                }
            return false;
        };

        if (!get(out[i].tag, out[i].value))
            return false;
    }

    return true;
}

