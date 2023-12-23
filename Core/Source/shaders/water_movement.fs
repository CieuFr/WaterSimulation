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

/*

	    float dt = 1.0 / 30.0;
		waveSpeed = glm::min(waveSpeed, 0.5f * spacing / dt);
		float c = waveSpeed * waveSpeed / spacing / spacing;
		float pd = glm::min(posDamping * dt, 1.0f);
		float vd = glm::max(0.0f, 1.0f - velDamping * dt);
		for (int i = 0; i < numX; i++) {
			for (int j = 0; j < numZ; j++) {
				int id = i * numZ * j;
				float h = heights[id];
				float sumH = 0.0f;
				sumH += i > 0 ? heights[id - numZ] : h;
				sumH += i < numX - 1 ? heights[id + numZ] : h;
				sumH += j > 0 ? heights[id - 1] : h;
				sumH += j < numZ - 1 ? heights[id + 1] : h;
				velocities[id] += dt * c * (sumH - 4.0 * h);
				heights[id] += (0.25 * sumH - h) * pd;
			}
		}

		for (int i = 0; i < numCells; i++) {
			velocities[i] *= vd;		// velocity damping
			heights[i] += velocities[i] * dt;
		}

*/















/*#version 450 core
layout(location = 0) out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D height;
uniform sampler2D velocity;


uniform float delta;

highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}
void main()
{
    vec4 info = texture2D(water, TexCoords);
    vec2 dx = vec2(delta, 0.0);
    vec2 dy = vec2(0.0, delta);

    float average = (    texture(water, TexCoords - dx).r +
                         texture(water, TexCoords - dy).r +
                         texture(water, TexCoords + dx).r +
                         texture(water, TexCoords + dy).r ) * 0.25;
    info.g += (average - info.r)*0.3;
    info.g *= 0.995;
    info.r += info.g;
    FragColor = info.rgb;
}


*/













/*
#version 450 core
layout(location = 0) out vec3 FragColor;

in vec2 TexCoords;

uniform sampler2D water;
uniform float delta;



highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

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

*/