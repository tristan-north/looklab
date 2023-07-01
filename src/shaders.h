static const char* vertexShaderSource = R"(
#version 330 core

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

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
    vec4[1024] strokes;
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

    float mask = 0.0;
    for( int i=0; i<2048; i++ ) {
        if( strokes[i].w == 0.0)
            break;

        float dist = distance(v_posWorld, vec3(strokes[i].xyz));
        if( dist < 0.03 )
            mask += 0.3;
    }
    mask = clamp(mask, 0.2, 1.0);
    

    FragColor = vec4(albedo * ndotl * mask, 1.0);

    // Gamma correction
    FragColor = vec4(pow(FragColor.rgb, vec3(1.0/2.2)), 1.0);
}
)";

//////////////////////////////////////////////////////////
static const char* bakeVtxShaderSrc = R"(
#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

out vec2 v_uv;
out vec3 v_posWorld;

void main()
{
    vec2 clipSpacePos = (a_uv - 0.5) * 2.0;
    gl_Position = vec4(clipSpacePos, 0.0, 1.0);

    v_uv = a_uv;
    v_posWorld = a_position;
}
)";


static const char* bakeFragShaderSrc = R"(
#version 330 core

float fit(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

layout(std140) uniform strokesBlock {
    vec4[1024] strokes;
};

in vec2 v_uv;
in vec3 v_posWorld;

out float FragColor;

void main()
{
    float mask = 0.0;
    for( int i=0; i<2048; i++ ) {
        if( strokes[i].w == 0.0)
            break;

        float dist = distance(v_posWorld, vec3(strokes[i].xyz));
        if( dist < 0.03 )
            mask += 0.3;
    }
    mask = clamp(mask, 0.0, 1.0);

    FragColor = mask;
}
)";
