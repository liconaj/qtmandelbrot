#pragma once

#include <QQuickAsyncImageProvider>

namespace mb {
    
class Renderer : public QQuickImageProvider {
public:
    Renderer() : QQuickImageProvider(QQuickImageProvider::Image) {}
    
    QImage requestImage(const QString& id, QSize* size, const QSize &requestedSize) override;
};

}