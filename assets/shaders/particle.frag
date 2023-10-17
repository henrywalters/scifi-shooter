#version 300 es

precision highp float;

in float AliveFor;
in float ElapsedTime;
in vec2 TexCoord;
in vec3 FragPos;
in vec4 StartColor;
in vec4 EndColor;

uniform sampler2D image;

out vec4 FragColor;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 0.0000000001;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {

    if (ElapsedTime >= AliveFor) {
        FragColor = vec4(0, 0, 0, 0);
    } else {

        float t = ElapsedTime / AliveFor;

        float a0 = StartColor.a;
        float a1 = EndColor.a;
        float a = a0 + (a1 - a0) * t;

        vec3 hsv0 = rgb2hsv(StartColor.rgb);
        vec3 hsv1 = rgb2hsv(EndColor.rgb);

        vec3 rgb = hsv2rgb(hsv0 + (hsv1 - hsv0) * t);

        FragColor = vec4(rgb, a);
    }

}