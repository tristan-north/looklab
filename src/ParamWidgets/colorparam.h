#pragma once

#include <QWidget>

class QLabel;
class QLineEdit;

class ColorParam: public QWidget {
    Q_OBJECT

public:
    ColorParam(const QString name, QWidget* parent);

// signals:
//     void paramChanged(float newValue);

// private slots:
//     void onSliderValueChanged(float newValue);
//     void onTextEditValueChanged();

private:
    QLabel* m_label;
    QLineEdit* m_textBoxR;
    QLineEdit* m_textBoxG;
    QLineEdit* m_textBoxB;
};