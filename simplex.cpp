#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <utility>

// http://csg.sph.umich.edu/abecasis/class/815.20.pdf

template <unsigned DIMENSION, typename Fn>
class Simplex {
private:
    double m_simplex[DIMENSION + 1][DIMENSION];
    Fn m_func;

    template <std::size_t... I>
    double evaluate_fn(const double* params, std::index_sequence<I...>) const {
        return m_func(params[I]...);
    }

public:
    Simplex(const std::array<double, DIMENSION>& point, Fn func)
     : m_func(func) {
        for (unsigned i = 0; i < DIMENSION + 1; ++i)
            for (unsigned j = 0; j < DIMENSION; ++j)
                m_simplex[i][j] = point[j];

        for (unsigned i = 0; i < DIMENSION; ++i)
            m_simplex[i][i] += 1.0;
    }

    void evaluate(std::array<double, DIMENSION+1>& fx) const {
        for (unsigned i = 0; i < DIMENSION+1; ++i)
            fx[i] = evaluate_fn(m_simplex[i], std::make_index_sequence<DIMENSION>{});
    }

    static void extremes(const std::array<double, DIMENSION+1>& fx, unsigned* ihi, unsigned* ilo, unsigned* inhi) {
        if (fx[0] > fx[1]) {
            *ihi = 0;
            *ilo = *inhi = 1;
        } else {
            *ihi = 1;
            *ilo = *inhi = 0;
        }

        for (unsigned i = 2; i < DIMENSION+1; ++i) {
            if (fx[i] <= fx[*ilo]) {
                *ilo = i;
            } else if (fx[i] > fx[*ihi]) {
                *inhi = *ihi;
                *ihi = i;
            } else if (fx[i] > fx[*inhi]) {
                *inhi = i;
            }
        }
    }

    void bearings(std::array<double, DIMENSION>& midpoint, std::array<double, DIMENSION>& line, unsigned ihi) const {
        std::fill(begin(midpoint), end(midpoint), 0.0);
        for (unsigned i = 0; i < DIMENSION+1; ++i) {
            if (i != ihi) {
                for (unsigned j = 0; j < DIMENSION; ++j)
                    midpoint[j] += m_simplex[i][j];
            }
        }
        for (unsigned j = 0; j < DIMENSION; ++j) {
            midpoint[j] /= DIMENSION;
            line[j] = m_simplex[ihi][j] - midpoint[j];
        }
    }


    bool update(unsigned ihi, double* fmax, const std::array<double, DIMENSION>& midpoint, const std::array<double, DIMENSION>& line, double scale) {
        std::array<double, DIMENSION> next;
        for (unsigned i = 0; i < DIMENSION; ++i)
            next[i] = midpoint[i] + scale * line[i];
        double fx = evaluate_fn(next.data(), std::make_index_sequence<DIMENSION>{});
        if (fx < *fmax) {
            for (unsigned i = 0; i < DIMENSION; ++i)
                m_simplex[ihi][i] = next[i];
            *fmax = fx;
            return true;
        }
        return false;
    }

    void contract(std::array<double, DIMENSION+1>& fx, unsigned ilo) {
        for (unsigned i = 0; i < DIMENSION+1; ++i) {
            if (i != ilo) {
                for (unsigned j = 0; j < DIMENSION; ++j)
                    m_simplex[i][j] = (m_simplex[ilo][j] + m_simplex[i][j]) * 0.5;
                fx[i] = evaluate_fn(m_simplex[i], std::make_index_sequence<DIMENSION>{});
            }
        }
    }

    void get(std::array<double, DIMENSION>& point, unsigned ilo) {
        for (unsigned j = 0; j < DIMENSION; ++j)
            point[j] = m_simplex[ilo][j];
    }
};

template <int DIMENSION, typename Fn>
double amoeba(std::array<double, DIMENSION>& point, Fn func, double tol) {
    auto check_tol = [tol](double fmax, double fmin) {
        constexpr double ZEPS = 1e-10;
        double delta = fabs(fmax - fmin);
        double accuracy = (fabs(fmax) + fabs(fmin)) * tol;
        return (delta < (accuracy + ZEPS));
    };

    unsigned ihi;
    unsigned ilo;
    unsigned inhi;

    std::array<double, DIMENSION+1> fx;
    std::array<double, DIMENSION> midpoint;
    std::array<double, DIMENSION> line;

    auto simplex = Simplex<DIMENSION, Fn>(point, func);
    simplex.evaluate(fx);

    while (true) {
        simplex.extremes(fx, &ihi, &ilo, &inhi);
        simplex.bearings(midpoint, line, ihi);
        if (check_tol(fx[ihi], fx[ilo]))
            break;
        simplex.update(ihi, &fx[ihi], midpoint, line, -1.0);
        if (fx[ihi] < fx[ilo])
            simplex.update(ihi, &fx[ihi], midpoint, line, -2.0);
        else if (fx[ihi] >= fx[inhi])
            if (!simplex.update(ihi, &fx[ihi], midpoint, line, 0.5))
                simplex.contract(fx, ilo);
    }

    simplex.get(point, ilo);
    auto fmin = fx[ilo];
    return fmin;
}
/*
 * initial point is avg of min and max
 * function evaluates parameters and returns MRR
 * */

double optimise() {
    std::array<double, 3> params;
    params[0];
    params[1];
    params[2];
    params[3];
    return 0;
}

struct parameter_range
{
    double min;
    double max;
};

struct parameter {
    double value;
    parameter_range range;

    parameter(double value, parameter_range range) 
     : value(value), range(range) {}

    bool valid() const {
        return value >= range.min && value <= range.max;
    }
};

int main()
{
    auto fn = [](double x, double y, double z) {
        return x*x + y*y + z*z;
    };

    std::array<double, 3> params;
    params[0] = 0.5;
    params[1] = 0.5;
    params[2] = 0.5;

    amoeba<params.size()>(params, fn, 1e-7);
    for (auto param : params)
        fprintf(stderr, "%f ", param);
    fprintf(stderr, "\n");
}
