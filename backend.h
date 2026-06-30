#pragma once

#include "renderer.h"

#include <QImage>
#include <QObject>
#include <QQmlEngine>
#include <QString>

namespace mb {

class Backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int viewportWidth READ viewportWidth WRITE setViewportWidth NOTIFY
                   viewportWidthChanged FINAL)
    Q_PROPERTY(int viewportHeight READ viewportHeight WRITE setViewportHeight NOTIFY
                   viewportHeightChanged FINAL)
    Q_PROPERTY(double centerReal READ centerReal WRITE setCenterReal NOTIFY centerRealChanged FINAL)
    Q_PROPERTY(double centerImag READ centerImag WRITE setCenterImag NOTIFY centerImagChanged FINAL)
    Q_PROPERTY(double zoom READ zoom WRITE setZoom NOTIFY zoomChanged FINAL)
    Q_PROPERTY(int maxIterations READ maxIterations WRITE setMaxIterations NOTIFY
                   maxIterationsChanged FINAL)
    Q_PROPERTY(QUrl source READ source NOTIFY sourceChanged FINAL)
    QML_ELEMENT
public:
    explicit Backend(QObject *parent = nullptr);

    double centerReal() const;
    void setCenterReal(double newCenterReal);

    double centerImag() const;
    void setCenterImag(double newCenterImag);

    double zoom() const;
    void setZoom(double newZoom);

    int maxIterations() const;
    void setMaxIterations(int newMaxIterations);

    QUrl source() const;

    int viewportWidth() const;
    void setViewportWidth(int newViewportWidth);

    int viewportHeight() const;
    void setViewportHeight(int newViewportHeight);

signals:
    void centerRealChanged();

    void centerImagChanged();

    void zoomChanged();

    void maxIterationsChanged();

    void sourceChanged();

    void viewportWidthChanged();

    void viewportHeightChanged();

private:
    int m_revision{};

    RenderParameters m_parameters{};
    Renderer *m_renderer;
    QString m_rendererIdentifier;

    void updateSource();
};

} // namespace mb
