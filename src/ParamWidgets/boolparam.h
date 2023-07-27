#pragma once

#include <QWidget>

class QLabel;
class QCheckBox;

class BoolParam: public QWidget {
    Q_OBJECT

public:
    BoolParam(const QString name, QWidget* parent);

// signals:
//     void paramChanged(float newValue);

// private slots:
//     void onSliderValueChanged(float newValue);
//     void onTextEditValueChanged();

private:
    QLabel* m_label;
    QCheckBox* m_checkBox;
};
