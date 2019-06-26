#include "simplex.h"
#include <random>

struct parameter {
    double value;
    const double min;
    const double max;

    parameter(double value, double min, double max) 
     : value(value), min(min), max(max) {}

    bool valid() const {
        return value >= min && value <= max;
    }
};

template<class Generator>
void peturb(parameter& param, Generator& gen) {
    std::uniform_real_distribution<double> dist(param.min, param.max);
    param.value = dist(gen);
}

/*
 * initial point is avg of min and max
 * function evaluates parameters and returns MRR
 * */
double optimise() {
    std::array<double, 4> params;
    params[0];
    params[1];
    params[2];
    params[3];
    return 0;
}

int main()
{
    auto fn = [](double x, double y, double z) {
        return x*x + y*y + z*z;
    };

    std::array<double, 3> params;
    params[0] = 0.5;
    params[1] = 0.5;
    params[2] = 0.5;

    Simplex::amoeba<params.size()>(params, fn, 1e-7);
    for (auto param : params)
        fprintf(stderr, "%f ", param);
    fprintf(stderr, "\n");
}
