#include "stringparam.h"
#include "common.h"
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>


StringParam::StringParam(const QString name, QWidget* parent) : QWidget(parent) {
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);

    int height = PARAMETER_HEIGHT;
    m_label = new QLabel(name, this);
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_label->setFixedSize(PARAMETER_LABEL_WIDTH, height);
    hbox->addWidget(m_label);
    hbox->addSpacing(PARAMETER_SPACE_AFTER_LABEL);

    m_textBox = new QLineEdit(this);
    // connect(m_textBox, SIGNAL(editingFinished()), this, SLOT(onTextEditValueChanged()));
    hbox->addWidget(m_textBox);

    setLayout(hbox);
}
