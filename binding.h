#ifndef BINDING_H
#define BINDING_H
#include "feedrate.h"
#include <stdexcept>
#include <cstdint>

template <typename> struct bind { };

template <uint32_t Out, uint32_t... In>
struct function {
    using self = function<Out, In...>;

    const uint32_t out = Out;
    const uint32_t in[sizeof...(In)] = { In... };
    const bind<self> fn = {};

    bool has_in(const TaggedValue* params, unsigned n) const {
        auto contains = [params, n](uint32_t tag) {
            for (unsigned i = 0; i < n; ++i)
                if (params[i].tag == tag)
                    return true;
            return false;
        };
        for (auto param : in)
            if (!contains(param))
                return false;
        return true;
    }

    double operator()(const TaggedValue* params, unsigned n) const {
        auto get = [params, n](uint32_t tag) {
            for (unsigned i = 0; i < n; ++i)
                if (params[i].tag == tag)
                    return params[i].value;
            throw std::logic_error("Required input tag not present.");
        };
        return fn(get(In)...);
    }
};

#endif
