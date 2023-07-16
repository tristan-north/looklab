#include "nodegraph.h"
#include <QLabel>
#include <QVBoxLayout>

Nodegraph::Nodegraph(QWidget* parent) : QFrame(parent) {
    setMinimumHeight(500);
    setFrameStyle(QFrame::Panel);
    setLineWidth(1);

    QLabel* label = new QLabel("Nodegraph", this);
    label->setStyleSheet("color: rgb(40,40,40); background-color: transparent; font-size:80px;");
//    label->setFixedSize(55, 15);
    label->setAlignment(Qt::AlignCenter);
//    m_frameTimeLabel->setAutoFillBackground(false);

    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(label);
    setLayout(vbox);
}
