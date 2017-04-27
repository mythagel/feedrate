#ifndef BINDING_H
#define BINDING_H
#include <stdexcept>

namespace detail {

bool all_of(bool arg) {
    return arg;
}
template<typename Arg, typename... Args>
bool all_of(Arg arg, Args... args) {
    return arg && all_of(args...);
}

// Uses parameter pack to find arguments for function and call from generic list of parameters.
template <typename Out, typename...In>
struct bind {
    static unsigned out() {
        return Out::value;
    }

    static bool has_in(const TaggedValue* params, unsigned n) {
        auto contains = [params, n](unsigned tag) {
            for (unsigned i = 0; i < n; ++i)
                if (params[i].tag == tag)
                    return true;
            return false;
        };
        return all_of(contains(In::value)...);
    }

    template <typename Fn>
    static double call(Fn fn, const TaggedValue* params, unsigned n) {
        auto get = [params, n](unsigned tag) {
            for (unsigned i = 0; i < n; ++i)
                if (params[i].tag == tag)
                    return params[i].value;
            throw std::logic_error("Required input tag not present.");
        };
        return fn(get(In::value)...);
    }
};


// Introduce distinct types for value type enumeration
template <unsigned Tag>
struct tag {
    static constexpr unsigned value = Tag;
};
// A list of the above tag types
template <typename...Tags>
struct tag_list { };
// Helpers to expand tag_list back to parameter pack
// base case
template <typename Out, typename...In>
struct bind_tag_list {
    using type = bind<Out, In...>;
};
// specialisation for TagList
template <typename Out, typename...In>
struct bind_tag_list<Out, tag_list<In...>> {
    using type = typename bind_tag_list<Out, In...>::type;
};

}

template <unsigned FunctionTag>
struct traits {};

template <unsigned OutTag, unsigned... InTags>
struct traits_helper {
    using out = detail::tag<OutTag>;
    using in = detail::tag_list<detail::tag<InTags>...>;
};

namespace detail {
// Generic interface type for feedrate functions
template<unsigned FunctionTag>
struct function {
    using type = typename bind_tag_list<typename traits<FunctionTag>::out, typename traits<FunctionTag>::in>::type;

    static unsigned tag() {
        return FunctionTag;
    }

    static unsigned out() {
        return type::out();
    }

    static bool has_in(const TaggedValue* params, unsigned n) {
        return type::has_in(params, n);
    }

    static double call(const TaggedValue* x, unsigned n) {
        return type::template call<>(traits<FunctionTag>::fn, x, n);
    }
};

template <unsigned... FunctionTag>
struct function_table_entry { };
template <unsigned Tag>
struct function_table_entry<Tag> : function<Tag> {
    using base = function<Tag>;

    static unsigned out(unsigned tag) {
        return tag == base::tag() ? base::out() : throw std::logic_error("unknown function");
    }
    static bool has_in(unsigned tag, const TaggedValue* params, unsigned n) {
        return tag == base::tag() ? base::has_in(params, n) : throw std::logic_error("unknown function");
    }
    static double call(unsigned tag, const TaggedValue* x, unsigned n) {
        return tag == base::tag() ? base::call(x, n) : throw std::logic_error("unknown function");
    }
};
template <unsigned Tag, unsigned... FunctionTag>
struct function_table_entry<Tag, FunctionTag...> : function<Tag>, function_table_entry<FunctionTag...> {
    using base = function<Tag>;
    using next = function_table_entry<FunctionTag...>;

    static unsigned out(unsigned tag) {
        return tag == base::tag() ? base::out() : next::out(tag);
    }
    static bool has_in(unsigned tag, const TaggedValue* params, unsigned n) {
        return tag == base::tag() ? base::has_in(params, n) : next::has_in(tag, params, n);
    }
    static double call(unsigned tag, const TaggedValue* x, unsigned n) {
        return tag == base::tag() ? base::call(x, n) : next::call(tag, x, n);
    }
};

}

template <unsigned... FunctionTag>
struct function_table : detail::function_table_entry<FunctionTag...> {
};

#endif
