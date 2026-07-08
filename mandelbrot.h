#pragma once

#include <QFutureWatcher>
#include <QImage>
#include <QQuickItem>

namespace mb {

class Mandelbrot : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int renderWidth READ renderWidth WRITE setRenderWidth NOTIFY renderWidthChanged FINAL)
    Q_PROPERTY(
        int renderHeight READ renderHeight WRITE setRenderHeight NOTIFY renderHeightChanged FINAL)
    Q_PROPERTY(double centerRe READ centerRe WRITE setCenterRe NOTIFY centerReChanged FINAL)
    Q_PROPERTY(double centerIm READ centerIm WRITE setCenterIm NOTIFY centerImChanged FINAL)
    Q_PROPERTY(double zoom READ zoom WRITE setZoom NOTIFY zoomChanged FINAL)
    Q_PROPERTY(int maxIterations READ maxIterations WRITE setMaxIterations NOTIFY
                   maxIterationsChanged FINAL)
public:
    explicit Mandelbrot(QQuickItem *parent = nullptr);

    int renderWidth() const;
    void setRenderWidth(int newRenderWidth);

    int renderHeight() const;
    void setRenderHeight(int newRenderHeight);

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
    void renderWidthChanged();

    void renderHeightChanged();

    void centerReChanged();

    void centerImChanged();

    void zoomChanged();

    void maxIterationsChanged();

private:
    void renderOnCpu(QPromise<QImage> &promise);

    int m_renderWidth;
    int m_renderHeight;
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
