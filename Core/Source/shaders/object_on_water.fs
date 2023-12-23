#version 450 core
layout(location = 0) out float o_height;
layout(location = 1) out float o_velocity;
layout(location = 2) out float o_object;


struct physicObject{
    vec3 position;
    float radius;
};

const int n_objects = 1;
uniform physicObject objects[1];

const float PI = 3.1415926535899793;
in vec2 TexCoords;

uniform sampler2D u_height;
uniform sampler2D u_velocity;
uniform sampler2D u_object;


uniform float cx;
uniform float cz;
uniform float x;
uniform float spacing;
uniform float numZ;
uniform float numX;





void main() 
{
    
float h1 = 1.0/spacing;
float h2 = spacing * spacing;

// pour chaque objet physique de la scene 
for(int i  = 0 ; i< n_objects; i++){
    physicObject object = objects[i];
    vec3 pos = object.position;
    float br = object.radius;

    float x0 = max(0, (cx + floor((pos.x - br) * h1)));
    float x1 = min(numX - 1, cx + floor((pos.x + br) * h1));
    float z0 = max(0, cz + floor((pos.z - br) * h1));
    float z1 = min(numZ - 1, cz + floor((pos.z + br) * h1));

    

} 
  

    float h =  texture(u_height, TexCoords).r;  
    float vel =  texture(u_velocity, TexCoords).r; 
    vec3 info = texture(u_height, TexCoords).rgb;  
    float drop = max(0.0, 1.0 - length( center - TexCoords) / 0.06);
    drop = 0.5 - cos(drop*PI)*0.5;
    info.r += drop*0.2;
    o_height = info.r;
    o_velocity = vel;
}

