#include "boolparam.h"
#include "common.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QCheckBox>
#include <qdebug.h>
#include <qnamespace.h>


BoolParam::BoolParam(const QString name, QWidget* parent) : QWidget(parent) {
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);
  
    int height = PARAMETER_HEIGHT;
    m_label = new QLabel(name, this);
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_label->setFixedSize(PARAMETER_LABEL_WIDTH, height);
    hbox->addWidget(m_label);
    hbox->addSpacing(PARAMETER_SPACE_AFTER_LABEL);

    m_checkBox = new QCheckBox(this);
    connect(m_checkBox, SIGNAL(stateChanged(int)), this, SLOT(onCheckedStatusChanged(int)));
    hbox->addWidget(m_checkBox);

    hbox->addStretch();
  
    setLayout(hbox);
}

void BoolParam::setDefault(bool defaultValue) { 
    if(defaultValue)
        m_checkBox->setCheckState(Qt::Checked);
    else
        m_checkBox->setCheckState(Qt::Unchecked);
  
}
void BoolParam::onCheckedStatusChanged(int newState) {
    bool newStateBool = newState != 0;
  
    char* paramName = (char*)malloc(m_label->text().length()+1);
    strcpy(paramName, qPrintable(m_label->text()));
    emit paramChanged(paramName, newStateBool);
}