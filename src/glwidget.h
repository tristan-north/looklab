#pragma once

#include "mesh.h"
#include "perftimer.h"
#include "common.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>
#include <QLabel>
#include <vector>


class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    GLWidget(QWidget* parent = nullptr);
    ~GLWidget();

    QSize sizeHint() const override;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    uint createProgram(const char *vsSrc, const char *fragSrc);
    void createBakeBuffer();
    void printShaderCompilationStatus(uint id);
    void printGlErrors(const QString &str);
    void computeNormals(Mesh* mesh, QVector3D* normals);

    int m_bakeRes = BAKRES;
    float m_xRot = 0;
    float m_yRot = 0;
    QVector3D m_camPos = { 0.0f, -0.5f, -2.0f };
    QPoint m_lastMousePos;
    std::vector<QVector4D> strokePositionsAndRadius;

    uint m_numTris = 0;
    uint m_programDefault = 0;
    uint m_programBake = 0;
    uint m_vaoDefault = 0;
    uint m_vaoBake = 0;
    uint m_fboBake = 0;
    int m_modelMatrixLoc = 0;
    int m_viewMatrixLoc = 0;
    int m_projMatrixLoc = 0;
    QMatrix4x4 m_model;
    QMatrix4x4 m_view;
    QMatrix4x4 m_proj;
//    QElapsedTimer m_frameTimer;
    PerfTimer m_frameTimer;
    QLabel* m_frameTimeLabel;
};