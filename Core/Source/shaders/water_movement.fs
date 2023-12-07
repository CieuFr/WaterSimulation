#version 450 core
layout(location = 0) out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D water;
uniform float delta;

void main()
{       
	//	sumH += i > 0 ? heights[id - numZ] : h;
	vec2 dx = vec2(delta, 0.0);
	vec2 dy = vec2(0.0, delta);
	vec3 info =  texture(water, TexCoords).rgb;
	float h = texture(water, TexCoords).r;
	float sumH = 0.0f;

	if((TexCoords.x - delta) > 0){
		sumH +=  texture(water, TexCoords - dx).r;
	} else {
		sumH += h;
	}

	if((TexCoords.y - delta) > 0){
		sumH +=  texture(water, TexCoords - dy).r;
	} else {
		sumH += h;
	}

	if( (TexCoords.y + delta)< 1) {
		sumH +=  texture(water, TexCoords + dy).r;
	} else {
		sumH += h;
	}

	if((TexCoords.x + delta) < 1){
		sumH +=  texture(water, TexCoords + dx).r;
	} else {
		sumH += h;
	}
	
	// TODO dt ? et c ? 
	float velocity = sumH - 4.0 * h;
	if(velocity <=0)
	{
		info.r=0;
	}
	else
	{
		info.r =texture(water, TexCoords ).r;
	}
	FragColor =  info;

  

}

