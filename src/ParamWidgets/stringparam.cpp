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
    connect(m_textBox, SIGNAL(editingFinished()), this, SLOT(onTextEditValueChanged()));
    hbox->addWidget(m_textBox);

    setLayout(hbox);
}

void StringParam::setDefault(const char* defaultValue) {
    m_textBox->setText(defaultValue);

    strcpy(m_lastStringValue, defaultValue);
    m_lastStringValue[sizeof(m_lastStringValue) - 1] = '\0'; // Ensure null terminated
}

void StringParam::onTextEditValueChanged() {
    m_textBox->clearFocus();

    if (strcmp(m_lastStringValue, qPrintable(m_textBox->text())) == 0) {
        return;
    }
    
    char* paramName = (char*)malloc(m_label->text().length()+1);
    strcpy(paramName, qPrintable(m_label->text()));

    char* stringParam = (char*)malloc(m_textBox->text().length()+1);
    strcpy(stringParam, qPrintable(m_textBox->text()));
    
    emit paramChanged(paramName, stringParam);

    strncpy(m_lastStringValue, qPrintable(m_textBox->text()), sizeof(m_lastStringValue));
    m_lastStringValue[sizeof(m_lastStringValue)-1] = '\0'; // Ensure null terminated
}