#version 300 es

precision highp float;

in vec2 TexCoord;
in vec3 FragPos;
in vec4 Color;
in float Thickness;
in vec2 Cells;

uniform sampler2D image;

out vec4 FragColor;

void main() {

    FragColor = vec4(mod(FragPos.x, Cells.x), mod(FragPos.y, Cells.y), 0., 1.);
    return;

    if (mod(FragPos.x, Cells.x) < Thickness ||
    mod(FragPos.y, Cells.y) < Thickness) {
        FragColor = Color;
    } else {
        FragColor = vec4(0.);
    }
}