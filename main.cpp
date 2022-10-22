#include <QApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>
#include "bledevice.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    BLEDevice bledevice;

    qDebug() << QFile::exists(":/qml/main.qml");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("bledevice", &bledevice);
    const QUrl url(u"qrc:/BLE_TrainM/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
