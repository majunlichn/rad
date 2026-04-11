#pragma once

#include <rad/Common/Float.h>

#include <cassert>
#include <cmath>

#include <numbers>

// e = e_v<double>;
// log2e = log2e_v<double>;
// log10e = log10e_v<double>;
// pi = pi_v<double>;
// inv_pi = inv_pi_v<double>; // 1/pi
// inv_sqrtpi = inv_sqrtpi_v<double>; // 1/sqrt(pi)
// ln2 = ln2_v<double>;
// ln10 = ln10_v<double>;
// sqrt2 = sqrt2_v<double>;
// sqrt3 = sqrt3_v<double>;
// inv_sqrt3 = inv_sqrt3_v<double>; // 1/sqrt(3)
// egamma = egamma_v<double>; // the Euler-Mascheroni constant: https://en.wikipedia.org/wiki/Euler%27s_constant
// phi = phi_v<double>; // the golden ratio: (1+sqrt(5))/2 = 1.618033988749...

#if !defined(M_PI)
#define M_E 2.71828182845904523536        // e
#define M_LOG2E 1.44269504088896340736    // log2(e)
#define M_LOG10E 0.434294481903251827651  // log10(e)
#define M_LN2 0.693147180559945309417     // ln(2)
#define M_LN10 2.30258509299404568402     // ln(10)
#define M_PI 3.14159265358979323846       // pi
#define M_PI_2 1.57079632679489661923     // pi/2
#define M_PI_4 0.785398163397448309616    // pi/4
#define M_1_PI 0.318309886183790671538    // 1/pi
#define M_2_PI 0.636619772367581343076    // 2/pi
#define M_2_SQRTPI 1.12837916709551257390 // 2/sqrt(pi)
#define M_SQRT2 1.41421356237309504880    // sqrt(2)
#define M_SQRT1_2 0.707106781186547524401 // 1/sqrt(2)
#endif

namespace rad
{

// The Right Way to Calculate Stuff: https://www.plunk.org/~hatch/rightway.html

// Finds solutions of the quadratic equation at^2 + bt + c = 0; return true if solutions were found.
// https://pbr-book.org/3ed-2018/Utilities/Mathematical_Routines
// https://github.com/mmp/pbrt-v4/blob/master/src/pbrt/util/math.h
template <typename Float>
constexpr bool SolveQuadratic(Float a, Float b, Float c, Float& t0, Float& t1)
{
    // Handle case of $a=0$ for quadratic solution
    if (a == 0) [[unlikely]]
    {
        if (b == 0) [[unlikely]]
        {
            if (c == 0)
            {
                t0 = 0;
                t1 = 0;
                return true;
            }
            else
            {
                return false;
            }
        }
        t0 = t1 = -c / b;
        return true;
    }
    // Find quadratic discriminant: b^2 - 4ac
    Float discrim = std::fma(b, b, -4 * a * c);
    if (discrim < 0)
    {
        return false;
    }
    Float rootDiscrim = std::sqrt(discrim);
    // Compute quadratic _t_ values
    Float q = Float(-0.5) * (b + std::copysign(rootDiscrim, b));
    t0 = q / a;
    t1 = (q != Float(0)) ? (c / q) : t0;
    if (t0 > t1)
    {
        std::swap(t0, t1);
    }
    return true;
}

} // namespace rad
