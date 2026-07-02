#pragma once

#include <QQuickAsyncImageProvider>

namespace mb {

struct RenderParameters
{
    int renderWidth{640};
    int renderHeight{480};
    int maxIterations{1000};
    double centerReal{-0.5};
    double centerImag{0.0};
    double zoom{100.0};
};

class Renderer : public QQuickImageProvider {
public:
    Renderer(const RenderParameters &parameters)
        : QQuickImageProvider(QQuickImageProvider::Image)
        , m_parameters{parameters}
    {}

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    const RenderParameters &m_parameters;
};

}