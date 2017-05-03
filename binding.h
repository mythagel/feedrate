#ifndef BINDING_H
#define BINDING_H
#include "feedrate.h"
#include <stdexcept>

template <typename> struct bind { };

template <unsigned Out, unsigned... In>
struct function {
    using self = function<Out, In...>;

    const unsigned out = Out;
    const unsigned in[sizeof...(In)] = { In... };
    const bind<self> fn = {};

    bool has_in(const TaggedValue* params, unsigned n) const {
        auto contains = [params, n](unsigned tag) {
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
        auto get = [params, n](unsigned tag) {
            for (unsigned i = 0; i < n; ++i)
                if (params[i].tag == tag)
                    return params[i].value;
            throw std::logic_error("Required input tag not present.");
        };
        return fn(get(In)...);
    }
};

#endif
