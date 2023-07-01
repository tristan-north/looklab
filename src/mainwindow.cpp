#include "mainwindow.h"
#include "glwidget.h"
#include "renderview.h"
#include <QKeyEvent>
#include <QLabel>
#include <QHBoxLayout>

MainWindow::MainWindow() {
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #202020;");

    RenderView *renderView = new RenderView();
    GLWidget *glWidget = new GLWidget(this);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(renderView);
    layout->addWidget(glWidget);
    layout->setSpacing(4);
    layout->setContentsMargins(0,0,0,0);

    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Q) {
        close();
    } else
        QWidget::keyPressEvent(event);
}