#include "glwidget.h"
#include "rman.h"
#include "shaders.h"
#include "tif.h"
#include "geo.h"
#include "perftimer.h"

#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QGuiApplication>
#include <QApplication>

float VIEW_ROTATE_SPEED = 0.2f;
float VIEW_DOLLY_SPEED = 0.01f;
float VIEW_PAN_SPEED = 0.003f;
float VIEW_NEAR_CLIP = 0.1f;
float VIEW_FAR_CLIP = 10.0f;

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    m_frameTimeLabel = new QLabel("", this);
    m_frameTimeLabel->setStyleSheet("color: rgb(100,100,100); background-color: transparent;font-size:11px;");
    m_frameTimeLabel->setFixedSize(55, 20);
    m_frameTimeLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    m_frameTimeLabel->setAutoFillBackground(false);

    setUpdateBehavior(QOpenGLWidget::PartialUpdate); // Tell Qt not to clear the buffers

    setFixedSize(RENDERWIDTH, RENDERHEIGHT);

    setFocus();  // Needed this to recieve key press events

    updateCamXform(false);
}

GLWidget::~GLWidget()
{
    // if (m_programDefault == nullptr)
    //     return;
    // makeCurrent();
    // m_logoVbo.destroy();
    // delete m_programDefault;
    // m_programDefault = nullptr;
    // doneCurrent();
}

void GLWidget::printGlErrors(const QString &str)
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

uint GLWidget::createProgram(const char *vsSrc, const char *fragSrc)
{
    uint program = glCreateProgram();

    uint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSrc, nullptr);
    glCompileShader(vs);
    printShaderCompilationStatus(vs);

    uint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragSrc, nullptr);
    glCompileShader(fs);
    printShaderCompilationStatus(fs);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    printGlErrors("createProgram");
    return program;
}

QSize GLWidget::sizeHint() const
{
    return {1024, 1024};
}

static void qNormalizeAngle(float& angle)
{
    angle = std::fmod(angle, 360.0f);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);  // Enable backface culling
    glCullFace(GL_BACK);
    glFrontFace(GL_CW); // Alembic winding order is Clockwise

    qInfo() << "Vendor: " << reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    qInfo() << "Renderer: " << reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    qInfo() << "Version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qInfo() << "GLSL Version: " << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    qInfo() << "";
    glClearColor(0.18, 0.18, 0.18, 1);

    createBakeBuffer();

    m_programDefault = createProgram(vertexShaderSource, fragmentShaderSource);
    m_programBake = createProgram(bakeVtxShaderSrc, bakeFragShaderSrc);
    glUseProgram(m_programDefault);

    m_modelMatrixLoc = glGetUniformLocation(m_programDefault, "u_model");
    m_viewMatrixLoc = glGetUniformLocation(m_programDefault, "u_view");
    m_projMatrixLoc = glGetUniformLocation(m_programDefault, "u_proj");

    m_numTris = geo::getNumIndices() / 3;

    // Create buffers containing the geo data
    uint posBuffer;
    glGenBuffers(1, &posBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glBufferData(GL_ARRAY_BUFFER, geo::getNumPoints() * sizeof(QVector3D), geo::getPositions(), GL_STATIC_DRAW);

    uint normalsBuffer;
    glGenBuffers(1, &normalsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
    glBufferData(GL_ARRAY_BUFFER, geo::getNumPoints() * sizeof(QVector3D), geo::getNormals(), GL_STATIC_DRAW);

    uint uvsBuffer;
    glGenBuffers(1, &uvsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvsBuffer);
    glBufferData(GL_ARRAY_BUFFER, geo::getNumPoints() * sizeof(QVector2D), geo::getUVs(), GL_STATIC_DRAW);

    uint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, geo::getNumIndices() * sizeof(uint), geo::getIndices(), GL_STATIC_DRAW);

    // Even tho we only care about 3D, openGL will pad the ubo as a vec4
    QVector4D strokes[50];
    GLuint strokeBuffer;
    glGenBuffers(1, &strokeBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, strokeBuffer);
    glBufferData(GL_UNIFORM_BUFFER, MAX_STROKE_POINTS * sizeof(QVector4D), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, strokeBuffer);  // Bind stroke buffer to uniform buffer location 0

    // Set up the normal default VAO
    glGenVertexArrays(1, &m_vaoDefault);
    glBindVertexArray(m_vaoDefault);

    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    // Set up the bake VAO
    glGenVertexArrays(1, &m_vaoBake);
    glBindVertexArray(m_vaoBake);

    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, uvsBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    printGlErrors("init");
}

void GLWidget::createBakeBuffer()
{
    glGenFramebuffers(1, &m_fboBake);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboBake);

    // Create a texture to use as the backbuffer
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_bakeRes, m_bakeRes, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attach the texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    // Check that the framebuffer is complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qCritical() << "Error creating framebuffer for baking.";
    }

    // Rebind the main framebuffer that Qt created for us.
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
}

void GLWidget::paintGL()
{
    m_frameTimer.restart();

    glUseProgram(m_programDefault);
    glBindVertexArray(m_vaoDefault);

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

        strokePositionsAndRadius.reserve(MAX_STROKE_POINTS);
        if(strokePositionsAndRadius.size() < MAX_STROKE_POINTS)
            strokePositionsAndRadius.emplace_back(worldPosition, 1.0f);

        glBufferSubData(GL_UNIFORM_BUFFER, 0, MAX_STROKE_POINTS * sizeof(QVector4D), strokePositionsAndRadius.data());
        printGlErrors("Update Stroke Buffer");

        if(strokePositionsAndRadius.size() % 100 == 0)
            qDebug() << "Num stroke points: " << strokePositionsAndRadius.size();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    printGlErrors("paintGL start");

    glDrawElements(GL_TRIANGLES, m_numTris * 3, GL_UNSIGNED_INT, nullptr);
    printGlErrors("glDrawArrays");

    glFinish(); // This blocks until all gl commands have finished. Using for frame timing.
    m_frameTimeLabel->setText(QString::number(m_frameTimer.elapsedMSec(), 'f', 2).append(" ms"));
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

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    /*
    // Bake to tif
    PerfTimer perfTimer;

    makeCurrent();

    glViewport(0, 0, m_bakeRes, m_bakeRes);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fboBake);
    glUseProgram(m_programBake);
    glBindVertexArray(m_vaoBake);

    glClear(GL_COLOR_BUFFER_BIT);
    printGlErrors("Render to bake framebuffer start");

    glDrawElements(GL_TRIANGLES, m_numTris * 3, GL_UNSIGNED_INT, nullptr);
    printGlErrors("Bake glDrawArrays");

    glFinish(); // This blocks until all gl commands have finished. Using for frame timing.
    perfTimer.printElapsedMSec("Render UV bake time: ");

    perfTimer.restart();
    std::vector<unsigned char> pixels(m_bakeRes*m_bakeRes);
    perfTimer.printElapsedMSec("Time to create pixel array for writeTif: ");
    perfTimer.restart();
    glReadPixels(0, 0, m_bakeRes, m_bakeRes, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
    perfTimer.printElapsedMSec("Time for glReadPixels: ");
    printGlErrors("UV bake read pixels.");
    writeTif(pixels, m_bakeRes);
    */
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

        updateCamXform(true);
    }

    // Dolly view
    else if (event->buttons() == Qt::RightButton) {
        m_camPos.setZ(m_camPos.z() + VIEW_DOLLY_SPEED * (dx + dy));
        updateCamXform(true);
    }

    // Pan view
    else if (event->buttons() == Qt::MiddleButton) {
        m_camPos.setX(m_camPos.x() + VIEW_PAN_SPEED * dx);
        m_camPos.setY(m_camPos.y() - VIEW_PAN_SPEED * dy);
        updateCamXform(true);
    }

    update();
    m_lastMousePos = event->pos();
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W)
    {
//        std::vector<unsigned char> pixels(width()*height());
//        glReadPixels(0, 0, width(), height(), GL_RED, GL_UNSIGNED_BYTE, pixels.data());
//        assert(width() == height() && "Image must be square.");
//        writeTif(pixels, width());
    }
    else
        QWidget::keyPressEvent(event);
}

void GLWidget::updateCamXform(bool alsoUpdateRman) {
    m_view.setToIdentity();
    m_view.translate(m_camPos);
    m_view.rotate(m_xRot, 1, 0, 0);
    m_view.rotate(m_yRot, 0, 1, 0);

    if(alsoUpdateRman)
        rmanSetCamXform(m_view);
}

