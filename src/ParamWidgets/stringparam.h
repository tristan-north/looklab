#pragma once

#include <QWidget>

class QLabel;
class QLineEdit;

class StringParam: public QWidget {
    Q_OBJECT

public:
    StringParam(const QString name, QWidget* parent);
    void setDefault(const char* defaultValue);

    // signals:
    //     void paramChanged(float newValue);

    // private slots:
    //     void onSliderValueChanged(float newValue);
    //     void onTextEditValueChanged();

  private:
    QLabel* m_label;
    QLineEdit* m_textBox;
};