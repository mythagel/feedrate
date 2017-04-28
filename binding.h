#ifndef BINDING_H
#define BINDING_H
#include <stdexcept>

template <typename> struct impl { };

template <unsigned Out, unsigned... In>
struct function {
    using self = function<Out, In...>;

    const unsigned out = Out;
    const unsigned in[sizeof...(In)] = { In... };
    const impl<self> fn = {};

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

/* 
 * output parameter can be derived from input parameters
 * input parameters can be substituted for output parameter
 *
 * generate a plan by substituing output parameters with functions which can
 * derive them. branch (recurse) at each decision point and try to replace until
 * solution(s) appear
 * */

// TODO type which exposes functions

#endif
