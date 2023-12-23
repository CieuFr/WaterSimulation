#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
//layout (location = 2) in vec2 aNormals;

out vec2 TexCoords;
//out vec3 Normals;


void main()
{
    //Normals = aNormals;
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

