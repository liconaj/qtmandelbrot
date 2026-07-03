#include "mandelbrotrenderer.h"

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

MandelbrotRenderer::MandelbrotRenderer(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
}

int MandelbrotRenderer::renderWidth() const
{
    return m_renderWidth;
}

void MandelbrotRenderer::setRenderWidth(int newRenderWidth)
{
    if (m_renderWidth == newRenderWidth)
        return;
    m_renderWidth = newRenderWidth;
    emit renderWidthChanged();
    startRendering();
}

int MandelbrotRenderer::renderHeight() const
{
    return m_renderHeight;
}

void MandelbrotRenderer::setRenderHeight(int newRenderHeight)
{
    if (m_renderHeight == newRenderHeight)
        return;
    m_renderHeight = newRenderHeight;
    emit renderHeightChanged();
    startRendering();
}

double MandelbrotRenderer::centerRe() const
{
    return m_centerRe;
}

void MandelbrotRenderer::setCenterRe(double newCenterRe)
{
    if (qFuzzyCompare(m_centerRe, newCenterRe))
        return;
    m_centerRe = newCenterRe;
    emit centerReChanged();
    startRendering();
}

double MandelbrotRenderer::centerIm() const
{
    return m_centerIm;
}

void MandelbrotRenderer::setCenterIm(double newCenterIm)
{
    if (qFuzzyCompare(m_centerIm, newCenterIm))
        return;
    m_centerIm = newCenterIm;
    emit centerImChanged();
    startRendering();
}

double MandelbrotRenderer::zoom() const
{
    return m_zoom;
}

void MandelbrotRenderer::setZoom(double newZoom)
{
    if (qFuzzyCompare(m_zoom, newZoom))
        return;
    m_zoom = newZoom;
    emit zoomChanged();
    startRendering();
}

int MandelbrotRenderer::maxIterations() const
{
    return m_maxIterations;
}

void MandelbrotRenderer::setMaxIterations(int newMaxIterations)
{
    if (m_maxIterations == newMaxIterations)
        return;
    m_maxIterations = newMaxIterations;
    emit maxIterationsChanged();
    startRendering();
}

void MandelbrotRenderer::renderOnCpu(QPromise<QImage> &promise)
{
    // Make constant copies of parameters to avoid passing pointer of this
    // to concurrent lambda
    const int renderWidth{m_renderWidth};
    const int renderHeight{m_renderHeight};
    const double centerRe{m_centerRe};
    const double centerIm{m_centerIm};
    const double zoom{m_zoom};
    const int maxIterations{m_maxIterations};

    // Pixel format must be of 32 bits to ensure aligning of rows
    QImage image{renderWidth, renderHeight, QImage::Format_ARGB32};
    QRgb *pixels{reinterpret_cast<QRgb *>(image.bits())};

    double centerX{static_cast<double>(renderWidth) / 2.0};
    double centerY{static_cast<double>(renderHeight) / 2.0};

    std::complex<double> center{centerRe, centerIm};

    // Make a list of row indeces to map across threads
    // size of image is not known at compile time so it this list must be dynamic
    QList<int> rowIndices;
    rowIndices.reserve(renderHeight);
    for (int i{}; i < renderHeight; ++i) {
        rowIndices.append(i);
    }

    QtConcurrent::blockingMap(rowIndices, [=, &promise](int pixelY) {
        if (promise.isCanceled()) {
            return;
        }
        QRgb *row{pixels + (pixelY * renderWidth)};
        for (int pixelX{}; pixelX < renderWidth; ++pixelX) {
            double zRe{static_cast<double>(pixelX - centerX) / zoom};
            double zIm{static_cast<double>(pixelY - centerY) / zoom};
            std::complex<double> z0{zRe + center.real(), zIm - center.imag()};

            int iters{escapeTimeIterations(z0, maxIterations)};

            if (iters < maxIterations) {
                // Use componente with prime numbers for uniqueness
                const int r{iters * 7};
                const int g{iters * 13};
                const int b{iters * 21};
                // Components are wrapped around the number 256 by qRgb
                // this is a simple procedurally wrapped color mapping
                row[pixelX] = qRgb(r, g, b);
            } else {
                row[pixelX] = qRgb(0, 0, 0);
            }
        }
    });

    if (!promise.isCanceled()) {
        promise.addResult(image);
    }
}

void MandelbrotRenderer::startRendering()
{
    if (m_renderWidth == 0 || m_renderHeight == 0 || m_zoom == 0 || m_maxIterations == 0) {
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

    QFuture<QImage> future{QtConcurrent::run(&MandelbrotRenderer::renderOnCpu, this)};
    m_renderWatcher.setFuture(future);
}

QSGNode *MandelbrotRenderer::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    auto node{static_cast<QSGSimpleTextureNode *>(oldNode)};
    if (!node) {
        node = new QSGSimpleTextureNode();
    }

    if (m_renderWidth <= 0 || m_renderHeight <= 0) {
        return node;
    }

    if (m_cpuImage.isNull()) {
        return node;
    }

    node->setRect(0, 0, width(), height());

    QSGTexture *texture{window()->createTextureFromImage(m_cpuImage)};
    node->setOwnsTexture(true);
    node->setTexture(texture);

    return node;
}

} // namespace mb
