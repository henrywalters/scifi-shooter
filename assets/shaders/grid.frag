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
    vec2 uv = TexCoord * 2. - 1.;

    float modX = mod(gl_FragCoord.x, 10.);
    float modY = mod(gl_FragCoord.y, 10.);

    if (modX < 1.) {
       FragColor = Color;
    }

    FragColor = vec4(gl_FragCoord.xy, 0., 1.0);

}