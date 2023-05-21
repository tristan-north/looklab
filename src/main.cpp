#include <QApplication>
#include <QDesktopWidget>
#include <QSurfaceFormat>

#include "glwidget.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("Looklab");

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setVersion(3, 2);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow mainWindow;

    mainWindow.show();

    return app.exec();
}
