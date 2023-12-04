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


void main()
{
    vec4 waterMeshInfo = texture2D(water, aTexCoords);
    TexCoords = aTexCoords;
    vec3 newPos = aPos;
    newPos.z += waterMeshInfo.r*2;
    WorldPos = vec3(model * vec4(newPos, 1.0));
    Normal = normalMatrix * aNormal;   

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}

