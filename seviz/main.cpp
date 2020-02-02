#include "mainwindow.h"

#include <QApplication>
#include <QtWebEngine>

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setApplicationName(QStringLiteral("seviz"));
    QApplication a(argc, argv);
    QtWebEngine::initialize();
    MainWindow w;
    w.show();

    return a.exec();
}
