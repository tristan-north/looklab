#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <QTimer>

static const char *vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec4 position;
    void main() {
       gl_Position = position;
    }
)";

static const char *fragmentShaderSource = R"(
    #version 330 core
    out vec4 color;
    void main() {
       color = vec4(0.5, 0.8, 0.4, 1.0);
    }
)";

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

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    qInfo() << "Vendor: " << reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    qInfo() << "Renderer: " << reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    qInfo() << "Version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qInfo() << "GLSL Version: " << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    qInfo() << "";
    glClearColor(0, 0, 0, 1);

    m_program = createProgram();
    glUseProgram(m_program);

    float positions[9] = {
        -0.8f, -0.8f, 0.0f,
        0.0f, 0.8f, 0.0f,
        0.8f, -0.8f, 0.0f
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    uint m_buffer;
    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);

    printGlErrors("init");
}


void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    printGlErrors("paintGL start");

    glDrawArrays(GL_TRIANGLES, 0, 3);
    printGlErrors("glDrawArrays");
}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_lastPos = event->pos();
}
