#include "sliderparam.h"
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>


SliderWidget::SliderWidget(QWidget* parent) : QWidget(parent) {
};


void SliderWidget::paintEvent(QPaintEvent *event) {
        QPainter painter(this);

        QPen pen(Qt::black, 1, Qt::SolidLine);
        painter.setPen(pen);

        QBrush brush(QColor(100,100,200), Qt::SolidPattern);
        painter.setBrush(brush);

        QRect bounds = this->rect();
        QRect drawRect(0, 4, m_value*bounds.width(), 15);
        painter.drawRect(drawRect);
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
