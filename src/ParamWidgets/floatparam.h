#pragma once

#include <QWidget>

class QLabel;
class QLineEdit;
class QSlider;
class SliderWidget;

class FloatParam : public QWidget {
    Q_OBJECT

public:
    FloatParam(const QString name, QWidget* parent);
    void setDefault(float defaultValue);

signals:
    void paramChanged(char* paramName, float newValue);

private slots:
    void onSliderValueChanged(float newValue);
    void onTextEditValueChanged();

private:
    QLabel* m_label;
    QLineEdit* m_textBox;
    SliderWidget* m_slider;
    float m_lastValue;
};


// Custom drawn slider widget, used in SliderParam
class SliderWidget : public QWidget {
  Q_OBJECT

public:
    SliderWidget(QWidget* parent);
    void setValue(float newValue);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void sliderMoved(float newValue);

  private:
    float m_value;
    QBrush m_frameBrush;
    QBrush m_sliderBrush;
};
