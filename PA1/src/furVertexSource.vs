#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
    vec4 worldPos;
} vs_out;


uniform mat4 view_v;
uniform mat4 model;
uniform mat4 projection_v;

void main()
{
    gl_Position = projection_v * view_v * model * vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vs_out.normal = normalize(normalMatrix * aNormal);
    vs_out.worldPos = model * vec4(aPos, 1.0);
}