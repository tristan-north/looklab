#include "mainwindow.h"
#include "rman.h"

#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QSurfaceFormat>


int main(int argc, char* argv[])
{
    QSurfaceFormat fmt;
    // fmt.setDepthBufferSize(24);
    fmt.setVersion(3, 3);
    // fmt.setSamples(4);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Looklab");

    MainWindow mainWindow;
    mainWindow.show();

    QTimer::singleShot(0, nullptr, startRender);

    return app.exec();
}

