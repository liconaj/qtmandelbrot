#include "renderer.h"
#include <complex>

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
    Q_UNUSED(requestedSize);

    const int width{m_parameters.viewportWidth};
    const int height{m_parameters.viewportHeight};
    const double zoom{m_parameters.zoom};

    // Pixel format must be of 32 bits to ensure aligning
    QImage image(width, height, QImage::Format_ARGB32);

    double centerX{static_cast<double>(width) / 2};
    double centerY{static_cast<double>(height) / 2};
    std::complex<double> zOffset{m_parameters.centerReal, m_parameters.centerImag};

    QRgb* pixels {reinterpret_cast<QRgb*>(image.bits())};

    const int maxIterations{m_parameters.maxIterations};
    for (int i{}; i < width * height; ++i) {
        int pixelX{i % width};
        int pixelY{i / width};

        double zReal{(static_cast<double>(pixelX) - centerX) / zoom};
        double zImag{(static_cast<double>(pixelY) - centerY) / zoom};
        std::complex<double> z0{zReal + zOffset.real(), zImag - zOffset.imag()};

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