#version 300 es

precision highp float;

in vec2 TexCoord;
in vec3 FragPos;
in vec4 TextColor;

uniform sampler2D image;

out vec4 FragColor;

void main() {
    float alpha = texture(image, TexCoord).b;
    FragColor = vec4(1, 1, 1, texture(image, TexCoord).r);
}