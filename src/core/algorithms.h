#pragma once

#include "types.h"

namespace mb {

/**
 * @brief Computed the scape time of a point in the Mandelbrot set.
 *
 * Iterated the recurrence z_{n+1} = z_n^2 + c, starting from z_0 = 0, where
 * c = (cRe, cIm). Iteration stops when |z| exceed the bailout radious or the
 * maximum number of iterations is reached.
 *
 * @param cRe Real part of the complex coordinate
 * @param cIm Imaginary part of the complex coordinate
 * @param bailoutRadius Escape radius. Must be greater than zero.
 * @param maxIterations Maximum number of iteratios to perform.
 * @return An EscapeTimeResult containing.
 *      - iterations: Number of iterations performed befor escaping, or
 *        maxIterations if the point did not escape.
 *      - magnitudeSquared: Squared magnitude (|z|^2) of the final iterate.
 */
EscapeTimeResult computeEscapeTime(Real re, Real im, int bailoutRadius, int maxIterations);


/**
 * @brief Computes the smooth iteration count for continuous Mandelbrot coloring.
 *
 * Calculates the normalized escape value nu using the iteration count and the
 * squared magnitude of the first iterate that escaped the bailout radius. This
 * produces a continuous value that can be used to eliminate color banding.
 *
 * @param iterations Number of iterations performed before the point escaped.
 * @param magnitudeSquared Squared magnitude (|z|^2) of the first iterate whose
 *                         magnitude exceeded the bailout radius.
 * @return The smooth iteration count nu.
 *
 * @pre magnitudeSquared must be greater than the squared bailout radius.
 */
Real smoothIterationCount(int iterations, Real magnitudeSquared);

}
