#version 300 es

precision highp float;

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texture;
layout (location = 3) in vec2 a_size;
layout (location = 4) in vec2 a_offset;
layout (location = 5) in vec4 a_color;
layout (location = 6) in vec4 a_model0;
layout (location = 7) in vec4 a_model1;
layout (location = 8) in vec4 a_model2;
layout (location = 9) in vec4 a_model3;

uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 Color;

void main() {

    mat4 model = mat4(1.0);
    model[0] = a_model0;
    model[1] = a_model1;
    model[2] = a_model2;
    model[3] = a_model3;

    gl_Position = projection * view * transpose(model) * vec4(a_vertex.xy * a_size + a_offset, 0.0, 1.0);
    Normal = a_normal;
    FragPos = vec3(gl_Position);
    Color = a_color;
    TexCoord = a_texture;
}