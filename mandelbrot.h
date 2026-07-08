#pragma once

#include <QFutureWatcher>
#include <QImage>
#include <QQuickItem>

namespace mb {

struct Parameters
{
    unsigned int width;
    unsigned int height;
    double centerRe;
    double centerIm;
    double zoom;
    unsigned int maxIterations;
};

class Mandelbrot : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int imageWidth READ imageWidth WRITE setImageWidth NOTIFY imageWidthChanged FINAL)
    Q_PROPERTY(int imageHeight READ imageHeight WRITE setImageHeight NOTIFY imageHeightChanged FINAL)
    Q_PROPERTY(double centerRe READ centerRe WRITE setCenterRe NOTIFY centerReChanged FINAL)
    Q_PROPERTY(double centerIm READ centerIm WRITE setCenterIm NOTIFY centerImChanged FINAL)
    Q_PROPERTY(double zoom READ zoom WRITE setZoom NOTIFY zoomChanged FINAL)
    Q_PROPERTY(int maxIterations READ maxIterations WRITE setMaxIterations NOTIFY
                   maxIterationsChanged FINAL)
public:
    explicit Mandelbrot(QQuickItem *parent = nullptr);

    int imageWidth() const;
    void setImageWidth(int newImageWidth);

    int imageHeight() const;
    void setImageHeight(int newImageHeight);

    double centerRe() const;
    void setCenterRe(double newCenterRe);

    double centerIm() const;
    void setCenterIm(double newCenterIm);

    double zoom() const;
    void setZoom(double newZoom);

    int maxIterations() const;
    void setMaxIterations(int newMaxIterations);

    Q_INVOKABLE
    void startRendering();

signals:
    void imageWidthChanged();

    void imageHeightChanged();

    void centerReChanged();

    void centerImChanged();

    void zoomChanged();

    void maxIterationsChanged();

private:
    void renderOnCpu(QPromise<QImage> &promise);

    int m_imageWidth;
    int m_imageHeight;
    double m_centerRe;
    double m_centerIm;
    double m_zoom;
    int m_maxIterations;

    QFutureWatcher<QImage> m_renderWatcher;
    QImage m_cpuImage;

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;
};

} // namespace mb
