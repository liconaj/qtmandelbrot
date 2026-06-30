#pragma once

#include <QQuickAsyncImageProvider>

namespace mb {

struct RenderParameters
{
    int viewportWidth{1280};
    int viewportHeight{900};
    double centerReal{-0.5};
    double centerImag{0.0};
    double zoom{200.0};
    int maxIterations{1000};
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