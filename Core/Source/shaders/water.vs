#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform sampler2D water;

uniform float u_dx;
uniform float u_dz;

void main()
{
    vec4 waterMeshInfo = texture2D(water, aTexCoords);
    TexCoords = aTexCoords;
    vec3 newPos = aPos;
    newPos.y = waterMeshInfo.r*2;
    WorldPos = vec3(model * vec4(newPos, 1.0));

    vec3 p1;
    vec3 p2;
    vec3 p3;
    vec3 p4;

   vec2 dx = vec2(u_dx, 0.0);

   vec2 dz = vec2(0.0, u_dz);

    if((TexCoords.x - u_dx) > 0){
        vec4 w = texture2D(water, aTexCoords - dx);
        p1.y =  w.r*2;
    } else {
        p1.y = newPos.y;
    }

    if((TexCoords.y - u_dz) > 0){
        vec4 w1 = texture2D(water, aTexCoords - dz);
        p2.y =  w1.r*2;
    } else {
        p2.y = newPos.y;
    }

    if((TexCoords.y + u_dz) < 1) {
        vec4 w1 = texture2D(water, aTexCoords - dz);
        p3.y =  w1.r*2;
    } else {
        p3.y = newPos.y;
    }
    if((TexCoords.x + u_dx) < 1){
        vec4 w1 = texture2D(water, aTexCoords - dx);
        p4.y =  w1.r*2;
    } else {
        p4.y = newPos.y;
    }

    p1 += vec3(-1,0,0);
    p3 += vec3(1,0,0);
    p2 += vec3(0,0,-1);
    p4 += vec3(0,0,1);

    vec3 newNormal = normalize(cross(p3-p1,p4-p2));

    Normal = normalMatrix * newNormal;   

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}

