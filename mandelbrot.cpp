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

} // namespace

Mandelbrot::Mandelbrot(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
}

int Mandelbrot::imageWidth() const
{
    return m_parameters.imageWidth;
}

void Mandelbrot::setImageWidth(int newImageWidth)
{
    if (m_parameters.imageWidth == newImageWidth)
        return;
    m_parameters.imageWidth = newImageWidth;
    emit imageWidthChanged();
    requestRender();
}

int Mandelbrot::imageHeight() const
{
    return m_parameters.imageHeight;
}

void Mandelbrot::setImageHeight(int newImageHeight)
{
    if (m_parameters.imageHeight == newImageHeight)
        return;
    m_parameters.imageHeight = newImageHeight;
    emit imageHeightChanged();
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

void Mandelbrot::renderOnCpu(QPromise<QImage> &promise)
{
    // Make constant copy of parameters to avoid passing pointer of this
    const Parameters parameters{m_parameters};

    double centerX{static_cast<double>(parameters.imageWidth) / 2.0};
    double centerY{static_cast<double>(parameters.imageHeight) / 2.0};

    // Normalize zoom to a scale independent of the imageWidth of the image size
    const double normalizedZoom{(parameters.imageWidth / 8.0) * (parameters.zoom / 100.0)};

    // Pixel format must be of 32 bits to ensure aligning of rows
    QImage image{parameters.imageWidth, parameters.imageHeight, QImage::Format_ARGB32};
    QRgb *pixels{reinterpret_cast<QRgb *>(image.bits())};

    // Make a list of row indeces to map across threads
    // size of image is not known at compile time so it this list must be dynamic
    QList<int> rowIndices;
    rowIndices.reserve(parameters.imageHeight);
    for (int i{}; i < parameters.imageHeight; ++i) {
        rowIndices.append(i);
    }

    QtConcurrent::blockingMap(rowIndices, [=, &promise](int pixelY) {
        if (promise.isCanceled()) {
            return;
        }

        QRgb *row{pixels + (pixelY * parameters.imageWidth)};
        for (int pixelX{}; pixelX < parameters.imageWidth; ++pixelX) {
            double zRe{static_cast<double>(pixelX - centerX) / normalizedZoom};
            double zIm{static_cast<double>(pixelY - centerY) / normalizedZoom};
            std::complex<double> z0{zRe + parameters.centerRe, zIm - parameters.centerIm};

            int iters{escapeTimeIterations(z0, parameters.maxIterations)};

            if (iters < parameters.maxIterations) {
                int h{static_cast<int>(std::pow(360.0 * iters / parameters.maxIterations, 1.5))
                      % 360};
                int s{255}; // 100%
                int l{static_cast<int>(100)};
                QColor color{QColor::fromHsl(h, s, l)};
                row[pixelX] = color.rgba();
            } else {
                row[pixelX] = qRgb(0, 0, 0);
            }
        }
    });

    if (!promise.isCanceled()) {
        promise.addResult(image);
    }
}

void Mandelbrot::requestRender()
{
    if (m_parameters.imageWidth == 0 || m_parameters.imageHeight == 0 || m_parameters.zoom == 0
        || m_parameters.maxIterations == 0) {
        return;
    }

    if (m_renderWatcher.isRunning()) {
        m_renderWatcher.cancel();
    }
    // Disconnect previous connections
    m_renderWatcher.disconnect(this);

    // Connect the watcher to trigger update() on the main thread
    connect(&m_renderWatcher, &QFutureWatcher<void>::finished, this, [this]() {
        // Only update if was not canceled
        if (!m_renderWatcher.isCanceled()) {
            m_cpuImage = m_renderWatcher.result();
            update();
        }
    });

    QFuture<QImage> future{QtConcurrent::run(&Mandelbrot::renderOnCpu, this)};
    m_renderWatcher.setFuture(future);
}

QSGNode *Mandelbrot::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    auto node{static_cast<QSGSimpleTextureNode *>(oldNode)};
    if (!node) {
        node = new QSGSimpleTextureNode();
    }

    if (m_parameters.imageWidth <= 0 || m_parameters.imageHeight <= 0) {
        return node;
    }

    if (m_cpuImage.isNull()) {
        delete node;
        return nullptr;
    }

    node->setRect(0, 0, width(), height());

    QSGTexture *texture{window()->createTextureFromImage(m_cpuImage)};
    texture->setFiltering(QSGTexture::Linear);
    node->setOwnsTexture(true);
    node->setTexture(texture);

    return node;
}

} // namespace mb
