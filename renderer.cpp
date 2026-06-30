#include "renderer.h"
#include <complex>
#include <qcontainerfwd.h>
#include <qlogging.h>
#include <qpoint.h>
#include <qrgb.h>

namespace mb {

namespace {

int escapeTimeIterations(std::complex<double> z0, int maxIterations)
{
    std::complex<double> z{z0};
    int iterations{};
    while ((z.real() * z.real()) + (z.imag() * z.imag()) <= 4 && iterations < maxIterations) {
        z = (z * z) + z0;
        iterations++;
    }
    return iterations;
};

}

QImage Renderer::requestImage(const QString& id, QSize* size, const QSize &requestedSize)
{
    Q_UNUSED(id);

    qDebug("Generating mandelbrot image...");

    const int defaultWidth {1280};
    const int defaultHeight {900};
    const int width{requestedSize.width() > 0 ? requestedSize.width() : defaultWidth};
    const int height{requestedSize.height() > 0 ? requestedSize.height() : defaultHeight};
    const double zoom{200};

    // Pixel format must be of 32 bits to ensure aligning
    QImage image(width, height, QImage::Format_ARGB32);

    double centerX{static_cast<double>(width) / 2};
    double centerY{static_cast<double>(height) / 2};
    std::complex<double> zOffset{-0.5, 0.0};

    QRgb* pixels {reinterpret_cast<QRgb*>(image.bits())};

    const int maxIterations {1000};
    for (int i{}; i < width * height; ++i) {
        int pixelX{i % width};
        int pixelY{i / width};

        double zReal{(static_cast<double>(pixelX) - centerX) / zoom};
        double zImag{(static_cast<double>(pixelY) - centerY) / zoom};
        std::complex<double> z0{zReal + zOffset.real(), zImag + zOffset.imag()};

        int iters{escapeTimeIterations(z0, maxIterations)};

        if (iters == maxIterations) {
            pixels[i] = qRgb(0, 0, 0);
        } else {
            pixels[i] = qRgb(255, 0, 0);
        }
    }

    if (size) {
        *size = image.size();
    }
    return image;
};

}