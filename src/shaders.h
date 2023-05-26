static const char *vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec4 position;
    layout(location = 1) in vec3 normalIn;

    out vec3 normal;

    uniform mat4 u_MVP;

    void main() {
        normal = normalIn;
        gl_Position = u_MVP * position;
    }
)";

static const char *fragmentShaderSource = R"(
    #version 330 core

    in vec3 normal;

    out vec4 color;
    void main() {
        vec3 nNormal = normalize(normal); // Need to renormalize after interpolation
        vec3 albedo = vec3(0.3, 0.6, 0.2);
        vec3 lightDir = vec3(-1.0, -1.0, 0.0);
        lightDir = normalize(lightDir);
        float diff = max(dot(lightDir, nNormal), 0.0);
        color = vec4(albedo * diff, 1.0);

        vec3 gamma = vec3(1.0/2.2);
        color = vec4(pow(color.rgb, gamma), 1.0);
    }
)";
