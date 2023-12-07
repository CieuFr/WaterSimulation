#version 450 core
layout(location = 0) out vec3 FragColor;
in vec2 TexCoords;


uniform sampler2D water;
uniform float delta;


void main() {
    vec3 info = texture(water, TexCoords).rgb;
    vec3 dx = vec3(delta, 0.0, info.r - texture(water, vec2(TexCoords.x + delta, TexCoords.y)).r);
    vec3 dy = vec3(0.0, delta, info.r - texture2D(water, vec2(TexCoords.x, TexCoords.y + delta)).r);
    info.gr = normalize(cross(dx, dy)).xy;
    FragColor = info;
}