#version 300 es

precision highp float;

in vec2 TexCoord;
in vec3 FragPos;
in vec4 Color;
in vec2 Origin;
in vec2 LightPos;
in float Attenuation;

out vec4 FragColor;

void main() {
    float d = length(LightPos - Origin);
    float light = clamp(1.0 / (1.0 + Attenuation * d), 0.0, 1.0);
    if (light > 0.01) {
        FragColor = vec4(Color.rgb, light);
    }

}