#include "colorparam.h"
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>


ColorParam::ColorParam(const QString name, QWidget* parent) : QWidget(parent) {
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);

    int height = 25;
    m_label = new QLabel(name, this);
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_label->setFixedSize(90, height);
    hbox->addWidget(m_label);

    m_textBoxR = new QLineEdit(this);
    m_textBoxR->setFixedSize(50, height);
    // connect(m_textBox, SIGNAL(editingFinished()), this, SLOT(onTextEditValueChanged()));
    hbox->addWidget(m_textBoxR);

    m_textBoxG = new QLineEdit(this);
    m_textBoxG->setFixedSize(50, height);
    hbox->addWidget(m_textBoxG);
  
    m_textBoxB = new QLineEdit(this);
    m_textBoxB->setFixedSize(50, height);
    hbox->addWidget(m_textBoxB);
  
    setLayout(hbox);
}