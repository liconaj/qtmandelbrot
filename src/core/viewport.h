#pragma once

#include "numeric.h"

namespace mb {

struct PixelPoint
{
    Real x;
    Real y;

    constexpr explicit PixelPoint(Real x_, Real y_)
        : x{x_}
        , y{y_} {};

    bool operator==(const PixelPoint &) const = default;
};

struct PlanePoint
{
    Real re;
    Real im;

    constexpr explicit PlanePoint(Real re_, Real im_)
        : re{re_}
        , im{im_} {};

    bool operator==(const PlanePoint &) const = default;
};

struct Viewport
{
    PlanePoint center;
    Real scale;
    int width;
    int height;

    constexpr explicit Viewport(PlanePoint center_, Real scale_, int width_, int height_)
        : center{center_}
        , scale{scale_}
        , width{width_}
        , height{height_} {};

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

/**
 * @brief Checks whether a viewport has valid parameters
 *
 * @param viewport
 * @return true
 * @return false
 */
[[nodiscard]]
bool isValid(const Viewport &viewport);

} // namespace mb
