#include "viewport.h"

mb::PlanePoint mb::pixelToPlane(const Viewport &viewport, PixelPoint pixel)
{
    // Add 0.5 to pixel components to calculate relative to pixel center.
    // This is important when supersampling.
    Real re{viewport.center.re + (pixel.x + 0.5 - (viewport.width / 2.0)) * viewport.scale};
    Real im{viewport.center.im - (pixel.y + 0.5 - (viewport.height / 2.0)) * viewport.scale};

    mb::PlanePoint planePoint{re, im};

    return planePoint;
}

mb::PixelPoint mb::planeToPixel(const Viewport &viewport, PlanePoint point)
{
    // Same formula as pixelToPlane but for x
    Real x{(point.re - viewport.center.re) / viewport.scale - 0.5 + (viewport.width / 2.0)};
    Real y{(-point.im + viewport.center.im) / viewport.scale - 0.5 + (viewport.height / 2.0)};

    mb::PixelPoint pixelPoint{x, y};

    return pixelPoint;
}

bool mb::isValid(const Viewport &viewport)
{
    if (viewport.scale <= 0) {
        return false;
    }
    if (viewport.width <= 0 || viewport.height <= 0) {
        return false;
    }
    return true;
}
