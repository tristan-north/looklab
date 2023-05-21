#include "mainwindow.h"
#include <glwidget.h>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>

MainWindow::MainWindow()
{
    setCentralWidget(new GLWidget(this));
}

