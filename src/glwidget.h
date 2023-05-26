#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>
#include <QElapsedTimer>


class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void setXRotation(float angle);
    void setYRotation(float angle);
    void setZCamPos(float zPos);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    uint createProgram();
    void printShaderCompilationStatus(uint id);
    void printGlErrors(QString str);
    float m_xRot = 0;
    float m_yRot = 0;
    float m_zRot = 0;
    float m_zCamPos = -4.0f;
    QPoint m_lastPos;
    uint m_numTris = 0;
    uint m_program = 0;
    uint m_vao = 0;
    uint m_MVPMatrixLoc = 0;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_camera;
    QMatrix4x4 m_world;
    QElapsedTimer m_frameTimer;
};
