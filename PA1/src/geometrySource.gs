#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 9) out;

in VS_OUT {
    vec3 normal;
    vec4 worldPos;
} gs_in[];

out vec3 Normal;
out vec3 FragPos;


const float MAGNITUDE = 0.001;

uniform mat4 view_g;
uniform mat4 projection_g;

vec3 getNormal(vec4 v1, vec4 v2, vec4 v3, vec3 n){
    vec3 vector1 = vec3(v1 - v2);
    vec3 vector2 = vec3(v1 - v3);
    vec3 normal = normalize(cross(vector1, vector2));
    float flag = dot(normal, n);
    if (flag < 0) {
        normal = -normal;
    }
    return normal;
}

void buildFur(vec4 v1, vec4 v2, vec4 v3, vec3 n) {
    Normal = getNormal(v1, v2, v3, n);
    FragPos = vec3(v1);
    gl_Position = projection_g * view_g * v1;
    EmitVertex();
    FragPos = vec3(v2);
    gl_Position = projection_g * view_g * v2;
    EmitVertex();
    FragPos = vec3(v3);
    gl_Position = projection_g * view_g * v3;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    vec4 v0 = gs_in[0].worldPos;
    vec4 v1 = gs_in[1].worldPos;
    vec4 v2 = gs_in[2].worldPos;

    vec4 v3 = (v0 + v1 + v2) / 3;

    vec3 n0  = gs_in[0].normal;
    vec3 n1  = gs_in[1].normal;
    vec3 n2  = gs_in[2].normal;

    vec3 n3 = (n0 + n1 + n2) / 3;

    v3 = v3 + vec4(n3, 0.0) * MAGNITUDE;

    buildFur(v0, v1, v3, n3);

    buildFur(v1, v3, v2, n3);

    buildFur(v3, v2, v0, n3);


}