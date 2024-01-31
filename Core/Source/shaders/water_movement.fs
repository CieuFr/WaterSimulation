#version 450 core
layout(location = 0) out float o_height;
layout(location = 1) out float o_velocity;

in vec2 TexCoords;

uniform sampler2D u_height;
uniform sampler2D u_velocity;
uniform float u_waveSpeed;
uniform float u_dt;
uniform float u_c;
uniform float u_pd;
uniform float u_vd;
uniform float u_dx;
uniform float u_dz;

void main()
{
   float h = texture(u_height,TexCoords).x;
   float vel = texture(u_velocity,TexCoords).x;

   float sumH = 0.0;
   vec2 dx = vec2(u_dx, 0.0);
   vec2 dy = vec2(0.0, u_dz);

   if((TexCoords.x - u_dx) > 0){
		sumH +=  texture(u_height, TexCoords - dx).r;
	} else {
		sumH += h;
	}

	if((TexCoords.y - u_dz) > 0){
		
		sumH +=  texture(u_height, TexCoords - dy).r;
	} else {
		sumH += h;
	}

	if( (TexCoords.y + u_dz) < 1) {
		sumH +=  texture(u_height, TexCoords + dy).r;
	} else {
		
		sumH += h;
	}

	if((TexCoords.x + u_dx) < 1){
		sumH +=  texture(u_height, TexCoords + dx).r;
	} else {
		sumH += h;
	}

	float newHeight = h + u_pd * (0.25 * sumH - h);
	float newVelocity = (vel + u_dt * u_c * (sumH - 4.0 * h ))*u_vd;

	newHeight = newHeight + newVelocity * u_dt;

	o_height = newHeight;
	o_velocity = newVelocity;

}
