#version 450 core
layout(location = 0) out float o_height;
layout(location = 1) out float o_velocity;

const float PI = 3.1415926535899793;
in vec2 TexCoords;

uniform sampler2D u_height;
uniform sampler2D u_velocity;

uniform vec2 center; 

void main() 
{
    float h =  texture(u_height, TexCoords).r;  
    float vel =  texture(u_velocity, TexCoords).r; 
    vec3 info = texture(u_height, TexCoords).rgb;  
    float drop = max(0.0, 1.0 - length( center - TexCoords) / 0.06);
    drop = 0.5 - cos(drop*PI)*0.5;
    info.r += drop*0.2;
    o_height = info.r;
    o_velocity = vel;
}

