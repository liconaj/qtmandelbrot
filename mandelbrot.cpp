#include "mandelbrot.h"

#include <QList>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QtConcurrent>
#include <complex>

namespace mb {

namespace {

int escapeTimeIterations(std::complex<double> z0, int maxIterations)
{
    std::complex<double> z{z0};
    int iterations{};
    while ((z.real() * z.real()) + (z.imag() * z.imag()) <= 4 && iterations < maxIterations) {
        z = (z * z) + z0;
        ++iterations;
    }
    return iterations;
}

void computeIterations(QPromise<IterationBuffer> &promise, const Parameters &parameters)
{
    int imageWidth{parameters.imageSize.width()};
    int imageHeight{parameters.imageSize.height()};

    double centerX{static_cast<double>(imageWidth) / 2.0};
    double centerY{static_cast<double>(imageHeight) / 2.0};

    IterationBuffer iterationBuffer{parameters.imageSize, parameters.maxIterations};

    // Normalize zoom to a scale independent of the imageWidth of the image size
    const double normalizedZoom{(imageWidth / 8.0) * (parameters.zoom / 100.0)};

    // Make a list of row indeces to map across threads
    // size of image is not known at compile time so this list must be dynamic
    QVector<int> rowIndices;
    rowIndices.reserve(iterationBuffer.height());
    for (int i{}; i < iterationBuffer.height(); ++i) {
        rowIndices.append(i);
    }

    QtConcurrent::blockingMap(rowIndices, [&](int pixelY) {
        if (promise.isCanceled()) {
            return;
        }

        for (int pixelX{}; pixelX < imageWidth; ++pixelX) {
            double zRe{static_cast<double>(pixelX - centerX) / normalizedZoom};
            double zIm{static_cast<double>(pixelY - centerY) / normalizedZoom};
            std::complex<double> z0{zRe + parameters.centerRe, zIm - parameters.centerIm};

            int iters{escapeTimeIterations(z0, parameters.maxIterations)};
            iterationBuffer.at(pixelX, pixelY) = iters;
        }
    });

    if (!promise.isCanceled()) {
        promise.addResult(iterationBuffer);
    }
}

QImage renderImage(const IterationBuffer &buffer)
{
    // Pixel format must be of 32 bits to ensure aligning of rows
    QImage image{buffer.width(), buffer.height(), QImage::Format_ARGB32};
    QRgb *pixels{reinterpret_cast<QRgb *>(image.bits())};

    for (int i{}; i < buffer.size(); ++i) {
        const int iterations{buffer.at(i)};

        if (iterations < buffer.maxIterations()) {
            int h{static_cast<int>(std::pow(360.0 * iterations / buffer.maxIterations(), 1.5))
                  % 360};
            int s{255}; // 100%
            int l{static_cast<int>(100)};
            QColor color{QColor::fromHsl(h, s, l)};
            pixels[i] = color.rgba();
        } else {
            pixels[i] = qRgb(0, 0, 0);
        }
    }

    return image;
}

} // namespace

Mandelbrot::Mandelbrot(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
}

QSize Mandelbrot::imageSize() const
{
    return m_parameters.imageSize;
}

void Mandelbrot::setImageSize(const QSize &newImageSize)
{
    if (m_parameters.imageSize == newImageSize)
        return;
    m_parameters.imageSize = newImageSize;
    emit imageSizeChanged();
    requestRender();
}

double Mandelbrot::centerRe() const
{
    return m_parameters.centerRe;
}

void Mandelbrot::setCenterRe(double newCenterRe)
{
    if (qFuzzyCompare(m_parameters.centerRe, newCenterRe))
        return;
    m_parameters.centerRe = newCenterRe;
    emit centerReChanged();
    requestRender();
}

double Mandelbrot::centerIm() const
{
    return m_parameters.centerIm;
}

void Mandelbrot::setCenterIm(double newCenterIm)
{
    if (qFuzzyCompare(m_parameters.centerIm, newCenterIm))
        return;
    m_parameters.centerIm = newCenterIm;
    emit centerImChanged();
    requestRender();
}

double Mandelbrot::zoom() const
{
    return m_parameters.zoom;
}

void Mandelbrot::setZoom(double newZoom)
{
    if (qFuzzyCompare(m_parameters.zoom, newZoom))
        return;
    m_parameters.zoom = newZoom;
    emit zoomChanged();
    requestRender();
}

int Mandelbrot::maxIterations() const
{
    return m_parameters.maxIterations;
}

void Mandelbrot::setMaxIterations(int newMaxIterations)
{
    if (m_parameters.maxIterations == newMaxIterations)
        return;
    m_parameters.maxIterations = newMaxIterations;
    emit maxIterationsChanged();
    requestRender();
}

bool Mandelbrot::canRequestRender()
{
    bool hasSize{m_parameters.imageSize.width() > 0 && m_parameters.imageSize.height() > 0};
    bool hasZoom{m_parameters.zoom > 0};
    bool hasMaxIterations{m_parameters.maxIterations > 0};

    return hasSize && hasZoom && hasMaxIterations;
}

void Mandelbrot::requestRender()
{
    if (!canRequestRender()) {
        return;
    }

    if (m_computeWatcher.isRunning()) {
        m_computeWatcher.cancel();
    }
    // Disconnect previous connections
    m_computeWatcher.disconnect(this);

    // Connect the watcher to trigger update() on the main thread
    connect(&m_computeWatcher, &QFutureWatcher<void>::finished, this, [this]() {
        // Only update if was not canceled
        if (!m_computeWatcher.isCanceled()) {
            m_iterationBuffer = m_computeWatcher.result();
            m_image = renderImage(m_iterationBuffer);
            update();
        }
    });

    QFuture<IterationBuffer> future{QtConcurrent::run(computeIterations, m_parameters)};
    m_computeWatcher.setFuture(future);
}

QSGNode *Mandelbrot::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    auto node{static_cast<QSGSimpleTextureNode *>(oldNode)};
    if (!node) {
        node = new QSGSimpleTextureNode();
    }

    if (!canRequestRender()) {
        return node;
    }

    if (m_image.isNull()) {
        delete node;
        return nullptr;
    }

    node->setRect(0, 0, width(), height());

    QSGTexture *texture{window()->createTextureFromImage(m_image)};
    texture->setFiltering(QSGTexture::Linear);
    node->setOwnsTexture(true);
    node->setTexture(texture);

    return node;
}

} // namespace mb
