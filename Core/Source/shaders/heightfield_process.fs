#version 450
 
uniform sampler2D u_pos_tex;
uniform sampler2D u_height_tex;
uniform sampler2D u_vel_tex;
out vec3 out_norm;
out vec2 out_tex;
in vec2 v_tex;
 
void main() {
  vec3 center = texture(u_pos_tex, v_tex).rgb;
  vec3 right  = textureOffset(u_pos_tex, v_tex, ivec2(1,  0)).rgb;
  vec3 top    = textureOffset(u_pos_tex, v_tex, ivec2(0, -1)).rgb;
 
#if 1
  vec3 dx = right - center;
  vec3 dy = top - center;
#else
  vec3 dx = center - right;
  vec3 dy = center - top;
#endif
 
  out_norm = normalize(cross(dx, dy));
  out_tex = v_tex;
}
 
//vec3 left   = textureOffset(u_pos_tex, v_tex, vec2(-1.0,  0.0)).rgb;
//vec3 bottom = textureOffset(u_pos_tex, v_tex, vec2( 0.0,  1.0)).rgb;
//vec3 dx = left - right;
//vec3 dy = top - bottom;