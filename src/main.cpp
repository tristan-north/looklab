#include "mainwindow.h"
#include "rman.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QSurfaceFormat>
#include <QTimer>

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

    QString fontPath = QCoreApplication::applicationDirPath();
    fontPath += "/fonts/Inter-Regular.ttf";
    int fontId = QFontDatabase::addApplicationFont(fontPath);
    assert(fontId != -1 && "Failed to load font.");

    MainWindow mainWindow;
    mainWindow.resize(800+800+500, 800+500);
    mainWindow.show();

    QTimer::singleShot(0, nullptr, startRender);

    return app.exec();
}

