#include "algorithms.h"

#include <cmath>

namespace mb {

EscapeTimeResult computeEscapeTime(const Real cRe, const Real cIm, const int bailoutRadius, const int maxIterations)
{
    int iterations = 0;

    const int bailoutRadius2 = bailoutRadius * bailoutRadius;

    Real zRe = 0.0;
    Real zIm = 0.0;
    Real zRe2 = 0.0;
    Real zIm2 = 0.0;

    while (zRe2 + zIm2 <= bailoutRadius2 && iterations < maxIterations) {
        zIm = 2 * zRe * zIm + cIm;
        zRe = zRe2 - zIm2 + cRe;
        zRe2 = zRe * zRe;
        zIm2 = zIm * zIm;
        ++iterations;
    }
    Real zn2 = zRe2 + zIm2;

    return {.iterations = iterations, .magnitudeSquared = zn2};
}

Real smoothIterationCount(const int iterations, const Real magnitudeSquared)
{
    const Real nu = iterations + 1 - std::log2(std::log(magnitudeSquared) / 2.0);
    return nu;
}

}
