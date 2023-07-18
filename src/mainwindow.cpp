#include "mainwindow.h"
#include "glwidget.h"
#include "renderview.h"
#include "parameters.h"
#include "nodegraph.h"
#include <QKeyEvent>
#include <QLabel>
#include <QHBoxLayout>

MainWindow::MainWindow() {
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: rgb(35,35,35);");

    RenderView* renderView = new RenderView();
    GLWidget* glWidget = new GLWidget(this);
    Parameters* parameters = new Parameters(this);
    Nodegraph* nodegraph = new Nodegraph(this);

    QVBoxLayout* vbox = new QVBoxLayout;
    QHBoxLayout* hbox = new QHBoxLayout;
    vbox->addLayout(hbox);
    hbox->addWidget(renderView);
    hbox->addWidget(glWidget);
    hbox->addWidget(parameters);
//    hbox->setSpacing(4);
//    hbox->setContentsMargins(10, 10, 10, 10);
    vbox->addWidget(nodegraph);

    centralWidget->setLayout(vbox);
    setCentralWidget(centralWidget);
    setFocus();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Q) {
        close();
    } else
        QWidget::keyPressEvent(event);
}