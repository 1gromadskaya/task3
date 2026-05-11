#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "playercontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<PlayerController>("App.Player", 1, 0, "PlayerController");

    QQmlApplicationEngine engine;

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [](QObject *obj, const QUrl &objUrl) {
                         if (!obj) QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.loadFromModule("App.Player", "Main");

    return app.exec();
}