#include "colorparam.h"
#include "ImathVec.h"
#include "common.h"
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <qdebug.h>


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
    connect(m_textBoxR, SIGNAL(editingFinished()), this, SLOT(onTextEditValueChanged()));
    hbox->addWidget(m_textBoxR);

    m_textBoxG = new QLineEdit(this);
    m_textBoxG->setFixedSize(50, height);
    connect(m_textBoxG, SIGNAL(editingFinished()), this, SLOT(onTextEditValueChanged()));
    hbox->addWidget(m_textBoxG);
  
    m_textBoxB = new QLineEdit(this);
    m_textBoxB->setFixedSize(50, height);
    connect(m_textBoxB, SIGNAL(editingFinished()), this, SLOT(onTextEditValueChanged()));
    hbox->addWidget(m_textBoxB);

    hbox->addStretch();
  
    setLayout(hbox);
}

void ColorParam::setDefault(Imath::V3f defaultValue) {
    m_textBoxR->setText(QString::number(defaultValue.x));
    m_textBoxG->setText(QString::number(defaultValue.y));
    m_textBoxB->setText(QString::number(defaultValue.z));

    m_lastValue = Imath::V3f(defaultValue);
}

void ColorParam::onTextEditValueChanged() {
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());
    if (lineEdit->hasFocus()) {
        lineEdit->clearFocus();
        return;
    }

    float valueAsFloat = lineEdit->text().toFloat();
    lineEdit->setText(QString::number(valueAsFloat));

    float valueAsFloatR = m_textBoxR->text().toFloat();
    float valueAsFloatG = m_textBoxG->text().toFloat();
    float valueAsFloatB = m_textBoxB->text().toFloat();
    
    Imath::V3f newValue(valueAsFloatR, valueAsFloatG, valueAsFloatB);
    if(m_lastValue == newValue)
        return;

    
    char* paramName = (char*)malloc(m_label->text().length()+1);
    strcpy(paramName, qPrintable(m_label->text()));
    emit paramChanged(paramName, valueAsFloatR, valueAsFloatG, valueAsFloatB);

    m_lastValue = newValue;
}
