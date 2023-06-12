#include "glwidget.h"
#include "shaders.h"
#include "tif.h"

#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QGuiApplication>
#include <QTimer>
#include <QApplication>

float VIEW_ROTATE_SPEED = 0.2f;
float VIEW_DOLLY_SPEED = 0.01f;
float VIEW_PAN_SPEED = 0.003f;
float VIEW_FOV = 45.0;
float VIEW_NEAR_CLIP = 0.1f;
float VIEW_FAR_CLIP = 10.0f;

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    m_frameTimeLabel = new QLabel("", this);
    m_frameTimeLabel->setStyleSheet("QLabel { color : white; }");

    setUpdateBehavior(QOpenGLWidget::PartialUpdate); // Tell Qt not to clear the buffers

    setFixedSize(1024, 1024);

    setFocus();  // Needed this to recieve key press events
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
    while (GLenum error = glGetError()) {
        qCritical() << "[OpenGL error] : " << str << " : " << error;
    }
}

void GLWidget::printShaderCompilationStatus(uint id)
{
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
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

    glUseProgram(program);

    m_modelMatrixLoc = glGetUniformLocation(program, "u_model");
    m_viewMatrixLoc = glGetUniformLocation(program, "u_view");
    m_projMatrixLoc = glGetUniformLocation(program, "u_proj");

    printGlErrors("createProgram");
    return program;
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(1024, 1024);
}

static void qNormalizeAngle(float& angle)
{
    angle = std::fmod(angle, 360.0f);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW); // Alembic winding order is Clockwise

    qInfo() << "Vendor: " << reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    qInfo() << "Renderer: " << reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    qInfo() << "Version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qInfo() << "GLSL Version: " << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    qInfo() << "";
    glClearColor(0.18, 0.18, 0.18, 1);

    m_program = createProgram();

    //Mesh mesh("../testGeo/testCube3.abc");
//    Mesh mesh("../testGeo/pika_kakashi.abc");
    Mesh mesh("../testGeo/buddha_light_autouvs.abc");
//    Mesh mesh("../testGeo/plane_uvs.abc");
    m_numTris = mesh.m_numIndices / 3;

    std::vector<QVector3D> normals(mesh.m_numPositions);
    computeNormals(&mesh, normals.data());

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
    glBufferData(GL_ARRAY_BUFFER, mesh.m_numPositions * sizeof(QVector3D), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    uint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.m_numIndices * sizeof(uint), mesh.m_indices, GL_STATIC_DRAW);

    // Test of sending float array to shaders
    // Even tho we only care about 3D, openGL will pad the ubo as a vec4
    QVector4D strokes[50];
    GLuint strokeBuffer;
    glGenBuffers(1, &strokeBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, strokeBuffer);
    glBufferData(GL_UNIFORM_BUFFER, MAX_STOKE_POINTS * sizeof(QVector4D), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, strokeBuffer);  // Bind stroke buffer to uniform buffer location 0

    printGlErrors("init");
}


void GLWidget::paintGL()
{
    m_frameTimer.start();

    m_view.setToIdentity();
    m_view.translate(m_camPos);
    m_view.rotate(m_xRot, 1, 0, 0);
    m_view.rotate(m_yRot, 0, 1, 0);

    glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, m_model.constData());
    glUniformMatrix4fv(m_viewMatrixLoc, 1, GL_FALSE, m_view.constData());
    glUniformMatrix4fv(m_projMatrixLoc, 1, GL_FALSE, m_proj.constData());

    // If drawing a stroke
    if (QApplication::mouseButtons() == Qt::LeftButton && QApplication::keyboardModifiers() == Qt::NoModifier) { // == means only the left button no other buttons
        QPoint mousePos = mapFromGlobal(cursor().pos());
        
        // Clamp mouse pos between 0 and window width/height
        GLint x = mousePos.x() < 0 ? 0 : mousePos.x();
        x = x > width() ? width() : x;
        GLint y = mousePos.y() < 0 ? 0 : mousePos.y();
        y = y > height() ? height() : y;
        y = height() - y - 1; // Qt and OpenGL have different 0,0 corners
        float depth;
        glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

        QVector3D worldPosition(x, y, depth);
        worldPosition = worldPosition.unproject(m_view, m_proj, QRect(0, 0, width(), height()));

        strokePositionsAndRadius.reserve(MAX_STOKE_POINTS);
        if(strokePositionsAndRadius.size() < MAX_STOKE_POINTS)
            strokePositionsAndRadius.emplace_back(worldPosition, 1.0f);

        glBufferSubData(GL_UNIFORM_BUFFER, 0, MAX_STOKE_POINTS * sizeof(QVector4D), strokePositionsAndRadius.data());
        printGlErrors("Update Stroke Buffer");

        if(strokePositionsAndRadius.size() % 100 == 0)
            qDebug() << "Num stroke points: " << strokePositionsAndRadius.size();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    printGlErrors("paintGL start");

    glDrawElements(GL_TRIANGLES, m_numTris * 3, GL_UNSIGNED_INT, nullptr);
    printGlErrors("glDrawArrays");

    glFinish(); // This blocks until all gl commands have finished. Using for frame timing.
    QString elapsed = QString::number(m_frameTimer.nsecsElapsed() / 1000000.f, 'f', 2);
    m_frameTimeLabel->setText(elapsed.append(" ms"));

}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(VIEW_FOV, GLfloat(w) / h, VIEW_NEAR_CLIP, VIEW_FAR_CLIP);

}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
    m_lastMousePos = event->pos();
    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
    int dx = event->x() - m_lastMousePos.x();
    int dy = event->y() - m_lastMousePos.y();

    // Rotate view
    if ((event->buttons() == Qt::LeftButton) &&
        QGuiApplication::keyboardModifiers() == Qt::AltModifier) {
        m_xRot += VIEW_ROTATE_SPEED * dy;
        qNormalizeAngle(m_xRot);
        m_yRot += VIEW_ROTATE_SPEED * dx;
        qNormalizeAngle(m_yRot);
    }

    // Dolly view
    else if (event->buttons() == Qt::RightButton) {
        m_camPos.setZ(m_camPos.z() + VIEW_DOLLY_SPEED * (dx + dy));
    }

    // Pan view
    else if (event->buttons() == Qt::MiddleButton) {
        m_camPos.setX(m_camPos.x() + VIEW_PAN_SPEED * dx);
        m_camPos.setY(m_camPos.y() - VIEW_PAN_SPEED * dy);
    }

    update();
    m_lastMousePos = event->pos();
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W)
    {
        std::vector<unsigned char> pixels(width()*height());
        glReadPixels(0, 0, width(), height(), GL_RED, GL_UNSIGNED_BYTE, pixels.data());
        assert(width() == height() && "Image must be square.");
        writeTif(pixels, width());
    }
    else
        QWidget::keyPressEvent(event);
}

void GLWidget::computeNormals(Mesh* mesh, QVector3D* normals)
{
    struct tri {
        uint v1;
        uint v2;
        uint v3;
    };

    QElapsedTimer timer;
    timer.start();

    const tri* tris = reinterpret_cast<const tri*>(mesh->m_indices);

    for (uint i = 0; i < m_numTris; i++) {

        QVector3D v1 = mesh->m_positions[tris[i].v1] - mesh->m_positions[tris[i].v2];
        QVector3D v2 = mesh->m_positions[tris[i].v3] - mesh->m_positions[tris[i].v2];

        QVector3D normal = QVector3D::crossProduct(v1, v2);
        normal.normalize();

        // add this normal to each vertex's normal
        normals[tris[i].v1] += normal;
        normals[tris[i].v2] += normal;
        normals[tris[i].v3] += normal;
    }

    // normalize the normals for each vertex
    for (uint i = 0; i < mesh->m_numPositions; ++i) {
        normals[i].normalize();
    }

    qInfo() << "Time to generate normals: " << timer.elapsed() << "ms";

}

