#version 300 es

precision highp float;

in vec2 TexCoord;
in vec3 FragPos;
in vec4 Color;

uniform sampler2D image;

out vec4 FragColor;

void main() {

    Color.r = Color.r * 100;

    const float gamma = 2.2;
    vec3 hdrColor = Color.rgb;

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped * 0.25, 1.0);

    //Color.r *= 100.0f;
    //FragColor = Color;
}