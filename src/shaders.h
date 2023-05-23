static const char *vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec4 position;

    uniform mat4 u_MVP;

    void main() {
       gl_Position = u_MVP * position;
    }
)";

static const char *fragmentShaderSource = R"(
    #version 330 core
    out vec4 color;
    void main() {
       color = vec4(0.5, 0.8, 0.4, 1.0);
    }
)";
