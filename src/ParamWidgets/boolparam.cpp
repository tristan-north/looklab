#include "boolparam.h"
#include "common.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QCheckBox>


BoolParam::BoolParam(const QString name, QWidget* parent) : QWidget(parent) {
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);

    int height = PARAMETER_HEIGHT;
    m_label = new QLabel(name, this);
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_label->setFixedSize(PARAMETER_LABEL_WIDTH, height);
    hbox->addWidget(m_label);

    m_checkBox = new QCheckBox(this);
    // connect(m_textBox, SIGNAL(editingFinished()), this, SLOT(onTextEditValueChanged()));
    hbox->addWidget(m_checkBox);

    hbox->addStretch();
  
    setLayout(hbox);
}
