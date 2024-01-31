#version 450 core
layout(location = 0) out float o_height;
layout(location = 1) out float o_velocity;


const float PI = 3.1415926535899793;
in vec2 TexCoords;

uniform sampler2D u_height;
uniform sampler2D u_velocity;
uniform sampler2D u_object;

uniform float alpha;

void main() 
{
    float h_obj =  texture(u_object, TexCoords).r;  
    float h =  texture(u_height, TexCoords).r;  
    float vel =  texture(u_velocity, TexCoords).r; 

    float bodyChange = h_obj - h;
    float newH = h + alpha * bodyChange;

    o_height = h;
    o_velocity = vel;

   
}

