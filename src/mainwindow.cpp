#include "mainwindow.h"
#include <glwidget.h>
#include <QKeyEvent>

MainWindow::MainWindow()
{
    setCentralWidget(new GLWidget(this));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Q)
    {
        close();
    }
}
