#pragma once

#include <QWidget>
#include <ImathVec.h>

class QLabel;
class QLineEdit;

class ColorParam: public QWidget {
    Q_OBJECT

public:
    ColorParam(const QString name, QWidget* parent);
    void setDefault(Imath::V3f defaultValue);

signals:
    void paramChanged(char* paramName, float Cx, float Cy, float Cz);

private slots:
    void onTextEditValueChanged();

private:
    QLabel* m_label;
    QLineEdit* m_textBoxR;
    QLineEdit* m_textBoxG;
    QLineEdit* m_textBoxB;
    Imath::V3f m_lastValue;
};