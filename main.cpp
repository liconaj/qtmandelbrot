#include "renderer.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.addImageProvider(QLatin1String("renderer"), new mb::Renderer);

    engine.loadFromModule("qtmandelbrot", "Main");

    return QGuiApplication::exec();
}
