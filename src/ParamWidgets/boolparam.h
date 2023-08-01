#pragma once

#include <QWidget>

class QLabel;
class QCheckBox;

class BoolParam: public QWidget {
    Q_OBJECT

public:
    BoolParam(const QString name, QWidget* parent);
    void setDefault(bool defaultValue);

signals:
    void paramChanged(char* paramName, bool newValue);

private slots:
    void onCheckedStatusChanged(int newState);

private:
    QLabel* m_label;
    QCheckBox* m_checkBox;
};
