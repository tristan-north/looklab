static const char* vertexShaderSource = R"(
#version 330 core

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

in vec3 a_position;
in vec3 a_normal;

out vec3 v_posCam;
out vec3 v_normal;
out vec3 v_posWorld;

void main()
{
    vec4 worldPosition = u_model * vec4(a_position, 1.0);
    gl_Position = u_proj * u_view * worldPosition;

    v_posCam = (u_view * worldPosition).xyz; // Position without proj transform
    v_normal = (u_view * u_model * vec4(a_normal, 0.0)).xyz;
    v_posWorld = worldPosition.xyz;
}
)";


static const char* fragmentShaderSource = R"(
#version 330 core

float fit(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}


layout(std140) uniform strokesBlock {
    vec3[] strokes;
};

in vec3 v_posCam;
in vec3 v_normal;
in vec3 v_posWorld;

out vec4 FragColor;

void main()
{
    vec3 lightDir = normalize(-v_posCam);
    vec3 normal = normalize(v_normal);

    float ndotl = max(dot(normal, lightDir), 0.0);

    vec3 albedo = vec3(0.5, 0.5, 0.5);
    float mask = distance(v_posWorld, strokes[0]);
    mask = fit(mask, 0.03, 0.031, 1, 0.2);
    mask = clamp(mask, 0.2, 1);
    FragColor = vec4(albedo * ndotl * mask, 1.0); 

    FragColor = vec4(pow(FragColor.rgb, vec3(1.0/2.2)), 1.0);
}
)";


/*
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
        vec3 albedo = vec3(0.5, 0.5, 0.5);
        vec3 lightDir = vec3(1.0, 0.0, 0.0);
        lightDir = normalize(lightDir);
        float diff = max(dot(lightDir, nNormal), 0.0);
        color = vec4(albedo * diff, 1.0);

        vec3 gamma = vec3(1.0/2.2);
        color = vec4(pow(color.rgb, gamma), 1.0);
    }
)";
*/