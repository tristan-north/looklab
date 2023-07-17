#pragma once

#include <QWidget>

class QLabel;
class QLineEdit;
class QSlider;

class SliderWidget : public QWidget {
    Q_OBJECT

public:
    SliderWidget(const QString name, QWidget* parent);

signals:
    void paramChanged(float newValue);

private slots:
    void onSliderValueChanged(int newValue);
    void onTextEditValueChanged();

private:
    QLabel* m_label;
    QLineEdit* m_textBox;
    QSlider* m_slider;
};