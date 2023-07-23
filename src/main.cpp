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

    int fontId = QFontDatabase::addApplicationFont("fonts/Inter-Regular.ttf");
    assert(fontId != -1 && "Failed to load font.");
//    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
//    printf("Font: %s\n", fontName.toStdString().c_str());
//    printf("fontId %d\n", fontId);
//    qDebug() << fontName;

    MainWindow mainWindow;
    mainWindow.resize(800+800+500, 800+500);
    mainWindow.show();

    QTimer::singleShot(0, nullptr, startRender);

    return app.exec();
}

