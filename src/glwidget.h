#pragma once

#include "mesh.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>
#include <QElapsedTimer>
#include <QLabel>
#include <qvector3d.h>


class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    GLWidget(QWidget* parent = nullptr);
    ~GLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    uint createProgram();
    void printShaderCompilationStatus(uint id);
    void printGlErrors(QString str);
    void computeNormals(Mesh* mesh, QVector3D* normals);

    float m_xRot = 0;
    float m_yRot = 0;
    float m_zRot = 0;
    QVector3D m_camPos = { 0.0f, -0.5f, -3.0f };
    QPoint m_lastMousePos;

    QVector3D m_drawPt;

    uint m_numTris = 0;
    uint m_program = 0;
    uint m_vao = 0;
    uint m_modelMatrixLoc = 0;
    uint m_viewMatrixLoc = 0;
    uint m_projMatrixLoc = 0;
    QMatrix4x4 m_model;
    QMatrix4x4 m_view;
    QMatrix4x4 m_proj;
    QElapsedTimer m_frameTimer;
    QLabel* m_frameTimeLabel;
};