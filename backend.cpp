#include "backend.h"

#include <QImage>
#include <QMetaObject>
#include <QObject>
#include <QUrlQuery>
#include <qhashfunctions.h>

namespace mb {

Backend::Backend(QObject *parent)
    : QObject{parent}
{
    // Generate a globally unique key using this object's memory address
    m_rendererIdentifier = QString("renderer_%1").arg(reinterpret_cast<quintptr>(this));
    m_renderer = new Renderer(m_parameters);

    QMetaObject::invokeMethod(
        this,
        [this]() {
            if (auto engine{qmlEngine(this)}) {
                engine->addImageProvider(m_rendererIdentifier, m_renderer);
                updateSource();
            }
        },
        Qt::QueuedConnection);
}

double Backend::centerReal() const
{
    return m_parameters.centerReal;
}

void Backend::setCenterReal(double newCenterReal)
{
    if (qFuzzyCompare(m_parameters.centerReal, newCenterReal))
        return;
    m_parameters.centerReal = newCenterReal;
    emit centerRealChanged();

    updateSource();
}

double Backend::centerImag() const
{
    return m_parameters.centerImag;
}

void Backend::setCenterImag(double newCenterImag)
{
    if (qFuzzyCompare(m_parameters.centerImag, newCenterImag))
        return;
    m_parameters.centerImag = newCenterImag;
    emit centerImagChanged();

    updateSource();
}

double Backend::zoom() const
{
    return m_parameters.zoom;
}

void Backend::setZoom(double newZoom)
{
    if (qFuzzyCompare(m_parameters.zoom, newZoom))
        return;
    m_parameters.zoom = newZoom;
    emit zoomChanged();

    updateSource();
}

int Backend::maxIterations() const
{
    return m_parameters.maxIterations;
}

void Backend::setMaxIterations(int newMaxIterations)
{
    if (m_parameters.maxIterations == newMaxIterations)
        return;
    m_parameters.maxIterations = newMaxIterations;
    emit maxIterationsChanged();

    updateSource();
}

QUrl Backend::source() const
{
    QUrl url{QString("image://%1/frame").arg(m_rendererIdentifier)};
    QUrlQuery query{};
    query.addQueryItem("rev", QString::number(m_revision));
    url.setQuery(query);

    return url;
}

int Backend::viewportWidth() const
{
    return m_parameters.viewportWidth;
}

void Backend::setViewportWidth(int newViewportWidth)
{
    if (m_parameters.viewportWidth == newViewportWidth || newViewportWidth == 0)
        return;
    m_parameters.viewportWidth = newViewportWidth;
    emit viewportWidthChanged();

    updateSource();
}

int Backend::viewportHeight() const
{
    return m_parameters.viewportHeight;
}

void Backend::setViewportHeight(int newViewportHeight)
{
    if (m_parameters.viewportHeight == newViewportHeight || newViewportHeight == 0)
        return;
    m_parameters.viewportHeight = newViewportHeight;
    emit viewportHeightChanged();

    updateSource();
}

void Backend::updateSource()
{
    ++m_revision;
    emit sourceChanged();
}

} // namespace mb
