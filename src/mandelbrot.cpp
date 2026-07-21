#include "mandelbrot.h"
#include "algorithms.h"

#include <QList>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QtConcurrent>
#include <cmath>

namespace mb {

namespace {

Palette generatePalette()
{
    QList<QColor> palette;
    palette.reserve(255);
    for (int i{}; i < 255; ++i) {
        uchar r, g, b;
        if (i < 32) {
            r = i * 8;
            g = i * 8;
            b = 127 - i * 4;
        } else if (i < 128) {
            r = 255;
            g = 255 - (i - 32) * 8 / 3;
            b = (i - 32) * 4 / 3;
        } else if (i < 192) {
            r = 255 - (i - 128) * 4;
            g = 0 + (i - 128) * 3;
            b = 127 - (i - 128);
        } else {
            r = 0;
            g = 192 - (i - 192) * 3;
            b = 64 + (i - 192);
        }
        QColor color{QColor::fromRgb(r, g, b)};
        palette.append(color);
    }
    return palette;
}

QColor interpolateColor(QColor color1, QColor color2, double factor)
{
    uchar r{static_cast<uchar>(color1.red() + factor * (color2.red() - color1.red()))};
    uchar g{static_cast<uchar>(color1.green() + factor * (color2.green() - color1.green()))};
    uchar b{static_cast<uchar>(color1.blue() + factor * (color2.blue() - color1.blue()))};

    return QColor::fromRgb(r, g, b);
}

void computeIterations(QPromise<IterationBuffer> &promise, const Parameters &parameters)
{
    int imageWidth{parameters.imageSize.width()};
    int imageHeight{parameters.imageSize.height()};

    double centerX{static_cast<double>(imageWidth) / 2.0};
    double centerY{static_cast<double>(imageHeight) / 2.0};

    IterationBuffer iterationBuffer{parameters.imageSize, parameters.maxIterations};

    // Normalize zoom to a scale independent of the imageWidth of the image size
    const double normalizedZoom{(imageWidth / 5.0) * (parameters.zoom / 100.0)};

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
            double cRe{static_cast<double>(pixelX - centerX) / normalizedZoom + parameters.centerRe};
            double cIm{static_cast<double>(pixelY - centerY) / normalizedZoom - parameters.centerIm};
            const int bailoutRadius = 256;

            EscapeTimeResult iters{computeEscapeTime(cRe, cIm, bailoutRadius, parameters.maxIterations)};
            iterationBuffer.at(pixelX, pixelY) = iters;
        }
    });

    if (!promise.isCanceled()) {
        promise.addResult(iterationBuffer);
    }
}

IterationHistogram computeHistogram(const IterationBuffer &iterationBuffer)
{
    int total{};
    QVector<int> frequencies(iterationBuffer.maxIterations());
    QVector<int> cumulativeFrequencies(iterationBuffer.maxIterations());

    for (const EscapeTimeResult &escapeResult : iterationBuffer) {
        if (escapeResult.iterations < iterationBuffer.maxIterations()) {
            ++frequencies[escapeResult.iterations];
            ++total;
        }
    }

    for (int i{}; i < frequencies.count(); ++i) {
        cumulativeFrequencies[i] = frequencies[i];
        if (i > 0) {
            cumulativeFrequencies[i] += frequencies[i - 1];
        }
    }

    return {.total = total,
            .frequencies = frequencies,
            .cumulativeFrequencies = cumulativeFrequencies};
}

QImage renderImage(const IterationBuffer &buffer,
                   const IterationHistogram &histogram,
                   const Palette &palette)
{
    // Pixel format must be of 32 bits to ensure aligning of rows
    QImage image{buffer.width(), buffer.height(), QImage::Format_ARGB32};
    QRgb *pixels{reinterpret_cast<QRgb *>(image.bits())};

    for (int i{}; i < buffer.size(); ++i) {
        const EscapeTimeResult escapeResult{buffer.at(i)};

        if (escapeResult.iterations < buffer.maxIterations()) {
            double density{1.0};
            Real nu = 0;
            if (escapeResult.iterations < buffer.maxIterations()) {
                nu = smoothIterationCount(escapeResult.iterations, escapeResult.magnitudeSquared);
            }
            double scaledNu{nu * density};
            int index{static_cast<int>(std::floor(scaledNu))};
            if (index < 0) {
                index = 0;
            }

            const int paletteIndex{static_cast<int>(index % palette.count())};
            const int nextPaletteIndex{static_cast<int>((index + 1) % palette.count())};

            const double smooth{scaledNu - std::floor(scaledNu)};

            QColor color1{palette.at(paletteIndex)};
            QColor color2{palette.at(nextPaletteIndex)};
            QColor color = interpolateColor(color1, color2, smooth);
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

    if (m_computeIterationsWatcher.isRunning()) {
        m_computeIterationsWatcher.cancel();
    }
    // Disconnect previous connections
    m_computeIterationsWatcher.disconnect(this);

    // Connect the watcher to trigger update() on the main thread
    connect(&m_computeIterationsWatcher, &QFutureWatcher<void>::finished, this, [this]() {
        // Only update if was not canceled
        if (!m_computeIterationsWatcher.isCanceled()) {
            IterationBuffer iterationBuffer = m_computeIterationsWatcher.result();
            IterationHistogram histogram = computeHistogram(iterationBuffer);
            Palette palette = generatePalette();
            m_image = renderImage(iterationBuffer, histogram, palette);
            update();
        }
    });

    QFuture<IterationBuffer> future{QtConcurrent::run(computeIterations, m_parameters)};
    m_computeIterationsWatcher.setFuture(future);
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
