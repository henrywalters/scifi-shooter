#version 300 es

precision highp float;

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texture;
layout (location = 3) in vec3 a_pos;
layout (location = 4) in vec3 a_velocity;
layout (location = 5) in vec4 a_startColor;
layout (location = 6) in vec4 a_endColor;
layout (location = 7) in float a_startTime;
layout (location = 8) in float a_aliveFor;
layout (location = 9) in vec3 a_gravity;
layout (location = 10) in float a_scale;
layout (location = 11) in int a_initialized;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float currentTime;

uniform vec3 position;
uniform bool positionRelative;

out float AliveFor;
out float ElapsedTime;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 StartColor;
out vec4 EndColor;

void main() {

    ElapsedTime = currentTime - a_startTime;
    AliveFor = a_aliveFor;

    vec3 Velocity = a_velocity + a_gravity * ElapsedTime;
    vec3 particlePosition = a_pos + Velocity * ElapsedTime;

    if (positionRelative) {
        particlePosition += position;
    }

    gl_Position = projection * view * model * vec4(particlePosition + vec3(a_vertex.x * a_scale, a_vertex.y * a_scale, a_vertex.z * a_scale), 1.0);
    Normal = a_normal;
    FragPos = vec3(gl_Position);
    StartColor = a_startColor;
    EndColor = a_endColor;
    TexCoord = a_texture;

    if (a_initialized == 0) {
        StartColor = vec4(0, 0, 0, 0);
    }
}