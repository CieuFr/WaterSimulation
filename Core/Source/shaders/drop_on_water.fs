#version 330 core
out vec4 FragColor;
const float PI = 3.1415926535899793;

in vec2 TexCoords;

uniform sampler2D water;

uniform vec2 center;



void main()
{             
    vec4 info = texture(water, TexCoords);  
    float drop = max(0.0, 1.0 - length( center - TexCoords) / 0.06);
    drop = 0.5 - cos(drop*PI)*0.5;
    info.r += drop*0.2;
    FragColor = info;
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);

}

