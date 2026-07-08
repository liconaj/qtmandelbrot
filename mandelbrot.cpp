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
    return m_imageWidth;
}

void Mandelbrot::setImageWidth(int newImageWidth)
{
    if (m_imageWidth == newImageWidth)
        return;
    m_imageWidth = newImageWidth;
    emit imageWidthChanged();
    startRendering();
}

int Mandelbrot::imageHeight() const
{
    return m_imageHeight;
}

void Mandelbrot::setImageHeight(int newImageHeight)
{
    if (m_imageHeight == newImageHeight)
        return;
    m_imageHeight = newImageHeight;
    emit imageHeightChanged();
    startRendering();
}

double Mandelbrot::centerRe() const
{
    return m_centerRe;
}

void Mandelbrot::setCenterRe(double newCenterRe)
{
    if (qFuzzyCompare(m_centerRe, newCenterRe))
        return;
    m_centerRe = newCenterRe;
    emit centerReChanged();
    startRendering();
}

double Mandelbrot::centerIm() const
{
    return m_centerIm;
}

void Mandelbrot::setCenterIm(double newCenterIm)
{
    if (qFuzzyCompare(m_centerIm, newCenterIm))
        return;
    m_centerIm = newCenterIm;
    emit centerImChanged();
    startRendering();
}

double Mandelbrot::zoom() const
{
    return m_zoom;
}

void Mandelbrot::setZoom(double newZoom)
{
    if (qFuzzyCompare(m_zoom, newZoom))
        return;
    m_zoom = newZoom;
    emit zoomChanged();
    startRendering();
}

int Mandelbrot::maxIterations() const
{
    return m_maxIterations;
}

void Mandelbrot::setMaxIterations(int newMaxIterations)
{
    if (m_maxIterations == newMaxIterations)
        return;
    m_maxIterations = newMaxIterations;
    emit maxIterationsChanged();
    startRendering();
}

void Mandelbrot::renderOnCpu(QPromise<QImage> &promise)
{
    // Make constant copies of parameters to avoid passing pointer of this
    // to concurrent lambda
    const int imageWidth{m_imageWidth};
    const int imageHeight{m_imageHeight};
    const double centerRe{m_centerRe};
    const double centerIm{m_centerIm};
    // Normalize zoom to make it render width independent
    const double zoom{(m_imageWidth / 8.0) * (m_zoom / 100.0)};
    const int maxIterations{m_maxIterations};

    // Pixel format must be of 32 bits to ensure aligning of rows
    QImage image{imageWidth, imageHeight, QImage::Format_ARGB32};
    QRgb *pixels{reinterpret_cast<QRgb *>(image.bits())};

    double centerX{static_cast<double>(imageWidth) / 2.0};
    double centerY{static_cast<double>(imageHeight) / 2.0};

    std::complex<double> center{centerRe, centerIm};

    // Make a list of row indeces to map across threads
    // size of image is not known at compile time so it this list must be dynamic
    QList<int> rowIndices;
    rowIndices.reserve(imageHeight);
    for (int i{}; i < imageHeight; ++i) {
        rowIndices.append(i);
    }

    QtConcurrent::blockingMap(rowIndices, [=, &promise](int pixelY) {
        if (promise.isCanceled()) {
            return;
        }
        QRgb *row{pixels + (pixelY * imageWidth)};
        for (int pixelX{}; pixelX < imageWidth; ++pixelX) {
            double zRe{static_cast<double>(pixelX - centerX) / zoom};
            double zIm{static_cast<double>(pixelY - centerY) / zoom};
            std::complex<double> z0{zRe + center.real(), zIm - center.imag()};

            int iters{escapeTimeIterations(z0, maxIterations)};

            if (iters < maxIterations) {
                int h{static_cast<int>(std::pow(360.0 * iters / maxIterations, 1.5)) % 360};
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

void Mandelbrot::startRendering()
{
    if (m_imageWidth == 0 || m_imageHeight == 0 || m_zoom == 0 || m_maxIterations == 0) {
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

    if (m_imageWidth <= 0 || m_imageHeight <= 0) {
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
