#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D water;
uniform float delta;

void main()
{             
    vec4 info = texture(water, TexCoords);  
    vec2 dx = vec2(delta, 0.0);
    vec2 dy = vec2(0.0, delta);
    float average = (    texture2D(water, TexCoords - dx).r +
					     texture2D(water, TexCoords - dy).r +
						 texture2D(water, TexCoords + dx).r +
					     texture2D(water, TexCoords + dy).r ) * 0.25;
    info.g += (average - info.r)*0.3;
    info.g *= 0.995;
    info.r += info.g;    
    FragColor = info;
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);

}

