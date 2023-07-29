#include "floatparam.h"
#include "common.h"
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>


FloatParam::FloatParam(const QString name, QWidget* parent) : QWidget(parent) {
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setMargin(0);

    int height = PARAMETER_HEIGHT;
    m_label = new QLabel(name, this);
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_label->setFixedSize(PARAMETER_LABEL_WIDTH, height);
    hbox->addWidget(m_label);
    hbox->addSpacing(PARAMETER_SPACE_AFTER_LABEL);

    m_textBox = new QLineEdit(this);
    m_textBox->setFixedSize(50, height);
    connect(m_textBox, SIGNAL(editingFinished()), this, SLOT(onTextEditValueChanged()));
    hbox->addWidget(m_textBox);

    m_slider = new SliderWidget(this);
    connect(m_slider, SIGNAL(sliderMoved(float)), this, SLOT(onSliderValueChanged(float)));
    hbox->addWidget(m_slider);

    setLayout(hbox);
}


void FloatParam::setDefault(float defaultValue) {
    m_textBox->setText(QString::number(defaultValue));
    m_slider->setValue(defaultValue);
    m_lastValue = defaultValue;
}

void FloatParam::onSliderValueChanged(float newValue) {
    if(newValue == m_lastValue)
        return;
    
    m_textBox->setText(QString::number(newValue));

    char* paramName = (char*)malloc(m_label->text().length()+1);
    strcpy(paramName, qPrintable(m_label->text()));
    emit paramChanged(paramName, newValue);

    m_lastValue = newValue;
}

void FloatParam::onTextEditValueChanged() {
    m_textBox->clearFocus();
    
    float valueAsFloat = m_textBox->text().toFloat();
    m_textBox->setText(QString::number(valueAsFloat));
    
    if(valueAsFloat == m_lastValue)
        return;
    
    m_slider->setValue(valueAsFloat);

    char* paramName = (char*)malloc(m_label->text().length()+1);
    strcpy(paramName, qPrintable(m_label->text()));
    emit paramChanged(paramName, valueAsFloat);

    m_lastValue = valueAsFloat;
}

// Slider Widget (used in Slider Param)
SliderWidget::SliderWidget(QWidget* parent) : QWidget(parent),
                                              m_frameBrush(QColor(30, 30, 30), Qt::SolidPattern),
                                              m_sliderBrush(QColor(PARAMETER_FILL_COLOR), Qt::SolidPattern) {
}

void SliderWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect bounds = this->rect();

    // Draw frame
    painter.setBrush(m_frameBrush);
    painter.setPen(QColor(45,45,45));
    QRectF frameRect(0.5, 3.5, bounds.width()-1, bounds.height()-7);
    painter.drawRoundedRect(frameRect, 4, 4);

    // Draw value slider
    QPainterPath path;
    path.addRoundedRect(frameRect, 4, 4);
    painter.setClipPath(path);
    painter.setBrush(m_sliderBrush);
    painter.drawRect(0, 0, m_value*bounds.width(), height());  // This would normally cover the entire widget

}

void SliderWidget::mousePressEvent(QMouseEvent *event) {
    mouseMoveEvent(event);
}

void SliderWidget::mouseMoveEvent(QMouseEvent *event) {
    QPoint pos = event->pos();
    m_value = event->pos().x() / (float)this->rect().width();
    
    // Clamp
    m_value = m_value < 0.0f ? 0.0f : m_value;
    m_value = m_value > 1.0f ? 1.0f : m_value;

    // Round to 2 decimal places
    m_value *= 100.0f;
    m_value = roundf(m_value);
    m_value /= 100.0f;
        
    update();
    emit sliderMoved(m_value);
}

void SliderWidget::setValue(float newValue) {
    m_value = newValue;
    update();
}
