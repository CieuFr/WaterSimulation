#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D tex;

void main()
{             
    vec3 texCol = texture(tex, TexCoords).rgb;      
    FragColor = vec4(texCol.r, 0.0, 0.0, 1.0);
}

