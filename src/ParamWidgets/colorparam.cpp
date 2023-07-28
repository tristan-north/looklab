#include "colorparam.h"
#include "common.h"
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>


ColorParam::ColorParam(const QString name, QWidget* parent) : QWidget(parent) {
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);

    int height = PARAMETER_HEIGHT;
    m_label = new QLabel(name, this);
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_label->setFixedSize(PARAMETER_LABEL_WIDTH, height);
    hbox->addWidget(m_label);
    hbox->addSpacing(PARAMETER_SPACE_AFTER_LABEL);

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

    hbox->addStretch();
  
    setLayout(hbox);
}

void ColorParam::setDefault(Imath::V3f defaultValue) {
    m_textBoxR->setText(QString::number(defaultValue.x));
}
