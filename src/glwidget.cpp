#include "glwidget.h"
#include "shaders.h"
#include "mesh.h"
#include <GL/gl.h>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QTimer>
#include <QElapsedTimer>
#include <cmath>
#include <qelapsedtimer.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qvector.h>
#include <qvector3d.h>

float VIEW_ROTATE_SPEED = 0.2f;
float VIEW_DOLLY_SPEED = 0.01f;
float VIEW_FOV = 45.0;
float VIEW_NEAR_CLIP = 0.01f;
float VIEW_FAR_CLIP = 1000.0f;

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
}

GLWidget::~GLWidget()
{
    // if (m_program == nullptr)
    //     return;
    // makeCurrent();
    // m_logoVbo.destroy();
    // delete m_program;
    // m_program = nullptr;
    // doneCurrent();
}

void GLWidget::printGlErrors(QString str)
{
    while(GLenum error = glGetError()) {
        qCritical() << "[OpenGL error] : " << str << " : " << error;
    }
}

void GLWidget::printShaderCompilationStatus(uint id)
{
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length*sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        int type;
        glGetShaderiv(id, GL_SHADER_TYPE, &type);
        QString typeString = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
        qCritical() << typeString << "shader failed to compile"; 
        qCritical() << message;
    }
}

uint GLWidget::createProgram()
{
    uint program = glCreateProgram();

    uint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, nullptr);
    glCompileShader(vs);
    printShaderCompilationStatus(vs);

    uint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fs);
    printShaderCompilationStatus(fs);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(800, 800);
}

static void qNormalizeAngle(float &angle)
{
    if(angle > 360.0f)
        angle = std::fmod(angle, 360.0f);
}

void GLWidget::setXRotation(float angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        update();
    }
}

void GLWidget::setYRotation(float angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        update();
    }
}

void GLWidget::setZCamPos(float zPos)
{
    if( zPos != m_zCamPos ) {
        m_zCamPos = zPos;
        update();
    }
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);  

    qInfo() << "Vendor: " << reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    qInfo() << "Renderer: " << reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    qInfo() << "Version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qInfo() << "GLSL Version: " << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    qInfo() << "";
    glClearColor(0.18, 0.18, 0.18, 1);

    m_program = createProgram();
    glUseProgram(m_program);

    m_MVPMatrixLoc = glGetUniformLocation(m_program, "u_MVP");

    Mesh mesh("../testGeo/pika_kakashi.abc");
    m_numTris = mesh.m_numIndices/3;

    // QVector3D positions[8] = {
    //     {-0.5f,  0.0f,  0.5f},
    //     {0.5f,  0.0f,  0.5f},
    //     {0.5f,  1.0f,  0.5f},
    //     {-0.5f,  1.0f,  0.5f},
    //
    //     {-0.5f,  0.0f, -0.5f},
    //     {0.5f,  0.0f, -0.5f},
    //     {0.5f,  1.0f, -0.5f},
    //     {-0.5f,  1.0f, -0.5f}
    // };

    struct tri {
        uint v1;
        uint v2;
        uint v3;
    };

    // tri tris[12] = {
    //     {0, 1, 2}, {2, 3, 0},
    //     {1, 5, 6}, {6, 2, 1},
    //     {5, 4, 7}, {7, 6, 5},
    //     {4, 0, 3}, {3, 7, 4},
    //     {5, 0, 4}, {0, 5, 1},
    //     {3, 2, 6}, {6, 7, 3}
    // };
    
    QElapsedTimer timer;
    timer.start();

    const tri* tris = reinterpret_cast<const tri*>(mesh.m_indices);

    QVector3D normals[mesh.m_numPositions]; // Need to zero array?
    for (uint i=0; i<m_numTris; i++) {

        QVector3D v1 = mesh.m_positions[tris[i].v2] - mesh.m_positions[tris[i].v3];
        QVector3D v2 = mesh.m_positions[tris[i].v2] - mesh.m_positions[tris[i].v1];

        QVector3D normal = QVector3D::crossProduct(v1, v2);
        normal.normalize();

        // add this normal to each vertex's normal
        normals[tris[i].v1] += normal;
        normals[tris[i].v2] += normal;
        normals[tris[i].v3] += normal;
    }

    // normalize the normals for each vertex
    for (uint i=0; i<mesh.m_numPositions; ++i) {
        normals[i].normalize();
    }

    qInfo() << "Time to generate normals: " << timer.elapsed() << "ms";

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    uint posBuffer;
    glGenBuffers(1, &posBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.m_numPositions * sizeof(QVector3D), mesh.m_positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    uint normalsBuffer;
    glGenBuffers(1, &normalsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.m_numPositions * sizeof(QVector3D), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    uint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.m_numIndices * sizeof(uint), tris, GL_STATIC_DRAW);

    printGlErrors("init");
}


void GLWidget::paintGL()
{
    m_frameTimer.start();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    printGlErrors("paintGL start");

    QMatrix4x4 mvpMatrix;
    QMatrix4x4 modelMatrix;
    QMatrix4x4 viewMatrix;

    modelMatrix.rotate(m_xRot, 1, 0, 0);
    modelMatrix.rotate(m_yRot, 0, 1, 0);

    viewMatrix.translate(QVector3D(0.0f, -0.5f, m_zCamPos));
    mvpMatrix = m_proj * viewMatrix * modelMatrix;
    glUniformMatrix4fv(m_MVPMatrixLoc, 1, GL_FALSE, mvpMatrix.constData());

    glDrawElements(GL_TRIANGLES, m_numTris*3, GL_UNSIGNED_INT, nullptr);
    printGlErrors("glDrawArrays");

    glFinish(); // This blocks until all gl commands have finished.
    QString elapsed = QString::number(m_frameTimer.nsecsElapsed()/1000000.f, 'f', 2);
    qInfo() << "Time to render frame: " << qPrintable(elapsed) << "ms";
}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(VIEW_FOV, GLfloat(w) / h, VIEW_NEAR_CLIP, VIEW_FAR_CLIP);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if( event->buttons() & Qt::LeftButton ) {
        setXRotation(m_xRot + VIEW_ROTATE_SPEED * dy);
        setYRotation(m_yRot + VIEW_ROTATE_SPEED * dx);
    }
    
    if( event->buttons() & Qt::RightButton ) {
        setZCamPos(m_zCamPos + VIEW_DOLLY_SPEED * (dx+dy));
    }

    m_lastPos = event->pos();
}
