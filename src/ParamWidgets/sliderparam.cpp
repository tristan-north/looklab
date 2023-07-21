#include "sliderparam.h"
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

SliderWidget::SliderWidget(QWidget* parent) : QWidget(parent) {
}

void SliderWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect bounds = this->rect();

    // Draw frame
//    painter.setBrush(Qt::NoBrush);
    QBrush frameBrush(QColor(30, 30, 30), Qt::SolidPattern);
    painter.setBrush(frameBrush);
    painter.setPen(QColor(45,45,45));
    QRectF frameRect(0.5, 3.5, bounds.width()-1, bounds.height()-7);
    painter.drawRoundedRect(frameRect, 4, 4);

    // Draw value slider
    QPainterPath path;
    path.addRoundedRect(frameRect, 4, 4);
    painter.setClipPath(path);
    QBrush brush(QColor(60, 70, 120), Qt::SolidPattern);
    painter.setBrush(brush);
    painter.drawRect(0, 0, m_value*bounds.width(), height());  // This would normally cover the entire widget

}

void SliderWidget::mousePressEvent(QMouseEvent *event) {
        mouseMoveEvent(event);
}

void SliderWidget::mouseMoveEvent(QMouseEvent *event) {
        QPoint pos = event->pos();
        m_value = event->pos().x() / (float)this->rect().width();
        update();
        emit sliderMoved(m_value);
}

SliderParam::SliderParam(const QString name, QWidget* parent) : QWidget(parent) {
    QHBoxLayout* hbox = new QHBoxLayout;

    int height = 25;
    m_label = new QLabel(name, this);
    m_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_label->setFixedSize(90, height);
    hbox->addWidget(m_label);

    m_textBox = new QLineEdit(this);
    m_textBox->setFixedSize(50, height);
    connect(m_textBox, SIGNAL(editingFinished()), this, SLOT(onTextEditValueChanged()));
    hbox->addWidget(m_textBox);

//    m_slider = new QSlider(Qt::Horizontal, this);
//    m_slider->setMaximum(100);
//    connect(m_slider, SIGNAL(sliderMoved(int)), this, SLOT(onSliderValueChanged(int)));
//    hbox->addWidget(m_slider);

    m_slider = new SliderWidget(this);
    connect(m_slider, SIGNAL(sliderMoved(float)), this, SLOT(onSliderValueChanged(float)));
    hbox->addWidget(m_slider);

    setLayout(hbox);
}

void SliderParam::onSliderValueChanged(float newValue) {
    m_textBox->setText(QString::number(newValue, 'f', 2));

    emit paramChanged(newValue);
}

void SliderParam::onTextEditValueChanged() {

    float valueAsFloat = m_textBox->text().toFloat();
//    m_slider->setValue(valueAsFloat*100);
    m_textBox->setText(QString::number(valueAsFloat, 'f', 2));
    m_textBox->clearFocus();

    emit paramChanged(valueAsFloat);
}
