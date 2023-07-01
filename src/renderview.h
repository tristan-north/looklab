#pragma once

#include <QLabel>

class RenderView : public QLabel
{
Q_OBJECT

public:
    RenderView();

private slots:
    void updatePixmap();
};
