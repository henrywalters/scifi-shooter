#version 420

precision highp float;

in vec2 TexCoord;
in vec3 FragPos;

layout (binding = 0) uniform sampler2D colorTex;
layout (binding = 1) uniform sampler2D lightTex;
layout (binding = 2) uniform sampler2D debugTex;
layout (binding = 3) uniform sampler2D uiTex;

out vec4 FragColor;

void main() {
    float min_light = 0.05f;
    vec4 light_lb = vec4(min_light, min_light, min_light, 1.0);

    vec4 color = texture(colorTex, TexCoord);
    vec4 light = min(max(texture(lightTex, TexCoord), light_lb), vec4(1.0));
    vec4 debug = texture(debugTex, TexCoord);
    vec4 ui = texture(uiTex, TexCoord);

    vec3 diffuse = color.rgb * light.rgb;
    FragColor = vec4(diffuse, color.a) + ui + debug;
}