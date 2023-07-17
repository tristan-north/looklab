#include "sliderwidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QVBoxLayout>

SliderWidget::SliderWidget(const QString name, QWidget* parent) : QWidget(parent) {
    QHBoxLayout* hbox = new QHBoxLayout;

    int height = 25;
    m_label = new QLabel(name, this);
    m_label->setAlignment(Qt::AlignRight);
    m_label->setFixedSize(90, height);
    hbox->addWidget(m_label);

    m_textBox = new QLineEdit(this);
    m_textBox->setFixedSize(50, height);
    connect(m_textBox, SIGNAL(returnPressed()), this, SLOT(onTextEditValueChanged()));
    hbox->addWidget(m_textBox);

    m_slider = new QSlider(Qt::Horizontal, this);
    connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    hbox->addWidget(m_slider);

    setLayout(hbox);
}
void SliderWidget::onSliderValueChanged(int newValue) {
    float valueAsFloat = newValue/99.0f;
    m_textBox->setText(QString::number(valueAsFloat, 'f', 2));
    emit paramChanged(valueAsFloat);
}
void SliderWidget::onTextEditValueChanged() {
    float valueAsFloat = m_textBox->text().toFloat();
    m_slider->setValue(valueAsFloat*99);
    emit paramChanged(valueAsFloat);
}
