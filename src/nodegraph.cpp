#include "nodegraph.h"
#include <QLabel>
#include <QVBoxLayout>

Nodegraph::Nodegraph(QWidget* parent) : QFrame(parent) {
    setMinimumHeight(500);
//    setFrameStyle(QFrame::Panel);
//    setLineWidth(1);
    setStyleSheet(R"(
        QFrame {border-color: rgb(0,0,0); border-style: solid; border-width: 1px; border-radius:6px;}
        )");

    QLabel* label = new QLabel("Nodegraph", this);
    label->setStyleSheet("color: rgb(40,40,40); background-color: transparent; font-size:80px; border-width: 0px;");
//    label->setFixedSize(55, 15);
    label->setAlignment(Qt::AlignCenter);
//    m_frameTimeLabel->setAutoFillBackground(false);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(label);
    setLayout(vbox);
}
