#pragma once

#include <QFutureWatcher>
#include <QImage>
#include <QQuickItem>

namespace mb {

struct Parameters
{
    QSize imageSize;
    double centerRe;
    double centerIm;
    double zoom;
    int maxIterations;
};

struct IterationHistogram
{
    int total;
    QVector<int> frequencies;
    QVector<int> cumulativeFrequencies;
};

class IterationBuffer
{
public:
    IterationBuffer() = default;

    IterationBuffer(const QSize &imageSize, int maxIterations)
        : m_imageSize{imageSize}
        , m_buffer(imageSize.width() * imageSize.height())
        , m_maxIterations{maxIterations}
    {}

    int width() const { return m_imageSize.width(); }

    int height() const { return m_imageSize.height(); }

    int size() const { return m_imageSize.width() * m_imageSize.height(); }

    std::size_t indexOf(int x, int y) const { return x + (m_imageSize.width() * y); }

    int &at(int index) { return m_buffer[index]; }
    const int &at(int index) const { return m_buffer[index]; }

    int &at(int x, int y) { return m_buffer[indexOf(x, y)]; }
    const int &at(int x, int y) const { return m_buffer[indexOf(x, y)]; }

    int maxIterations() const { return m_maxIterations; }
    void setMaxIterations(int newMaxIterations) { m_maxIterations = newMaxIterations; }

    auto begin() { return m_buffer.begin(); }
    auto end() { return m_buffer.end(); }

    auto begin() const { return m_buffer.begin(); }
    auto end() const { return m_buffer.end(); }

    auto cbegin() const { return m_buffer.cbegin(); }
    auto cend() const { return m_buffer.cend(); }

private:
    QSize m_imageSize;
    QVector<int> m_buffer;
    int m_maxIterations;
};

class Mandelbrot : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QSize imageSize READ imageSize WRITE setImageSize NOTIFY imageSizeChanged FINAL)
    Q_PROPERTY(double centerRe READ centerRe WRITE setCenterRe NOTIFY centerReChanged FINAL)
    Q_PROPERTY(double centerIm READ centerIm WRITE setCenterIm NOTIFY centerImChanged FINAL)
    Q_PROPERTY(double zoom READ zoom WRITE setZoom NOTIFY zoomChanged FINAL)
    Q_PROPERTY(int maxIterations READ maxIterations WRITE setMaxIterations NOTIFY
                   maxIterationsChanged FINAL)
public:
    explicit Mandelbrot(QQuickItem *parent = nullptr);

    QSize imageSize() const;
    void setImageSize(const QSize &newImageSize);

    double centerRe() const;
    void setCenterRe(double newCenterRe);

    double centerIm() const;
    void setCenterIm(double newCenterIm);

    double zoom() const;
    void setZoom(double newZoom);

    int maxIterations() const;
    void setMaxIterations(int newMaxIterations);

signals:
    void imageSizeChanged();

    void centerReChanged();

    void centerImChanged();

    void zoomChanged();

    void maxIterationsChanged();

private:
    void requestRender();
    void renderOnCpu(QPromise<QImage> &promise);

    bool canRequestRender();

    Parameters m_parameters;

    QFutureWatcher<IterationBuffer> m_computeIterationsWatcher;
    QImage m_image;

    QSize m_imageSize;

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;
};

} // namespace mb
