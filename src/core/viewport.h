#pragma once

#include "numeric.h"

namespace mb {

struct PixelPoint
{
    Real x;
    Real y;

    bool operator==(const PixelPoint &) const = default;
};

struct PlanePoint
{
    Real re;
    Real im;

    bool operator==(const PlanePoint &) const = default;
};

struct Viewport
{
    PlanePoint center;
    Real scale;
    int width;
    int height;

    bool operator==(const Viewport &) const = default;
};

/**
 * @brief Converts a pixel point into a plane point based on viewport
 *
 * @param viewport
 * @param pixel
 * @return PlanePoint
 */
[[nodiscard]]
PlanePoint pixelToPlane(const Viewport &viewport, PixelPoint pixel);

/**
 * @brief Converts a plane point into a pixel point based on viewport
 *
 * @param viewport
 * @param point
 * @return PixelPoint
 */
[[nodiscard]]
PixelPoint planeToPixel(const Viewport &viewport, PlanePoint point);

} // namespace mb
