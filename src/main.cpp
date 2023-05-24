#include <QApplication>
#include <QDesktopWidget>
#include <QSurfaceFormat>

#include "glwidget.h"
#include "mainwindow.h"

#include "geo.h"

int main(int argc, char *argv[])
{

    Example1_MeshIn();

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

    return app.exec();
}
