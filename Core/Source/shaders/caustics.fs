// #version 450 core
// layout(location = 0) out float o_ratio;


// in vec3 oldPosition;
// in vec3 newPosition;


// void main()
// {
//    const float causticsFactor = 0.15;
//    float causticsIntensity = 0.;

//     float oldArea = length(dFdx(oldPosition)) * length(dFdy(oldPosition));
//     float newArea = length(dFdx(newPosition)) * length(dFdy(newPosition));

//     float ratio;

//     // Prevent dividing by zero (debug NVidia drivers)
//     if (newArea == 0.) {
//       // Arbitrary large value
//       ratio = 2.0e+20;
//     } else {
//       ratio = oldArea / newArea;
//     }

//     causticsIntensity = causticsFactor * ratio;
  

//     o_ratio = causticsIntensity;
// 	  o_ratio =1;


// }


#version 450 core
layout(location = 0) out float o_ratio;

in vec2 TexCoords;

uniform sampler2D water;

void main()
{             
    vec3 texCol = texture(water, TexCoords).rgb;      
    o_ratio = texCol.x;
}

