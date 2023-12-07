#version 450 
 
uniform sampler2D u_height_tex;
uniform sampler2D u_vel_tex;
uniform float u_dt;
out float out_height;
out float out_vel;
in vec2 v_tex;
 
void main() {
 
  float u        = texture(u_height_tex, v_tex).r;
  float u_right  = textureOffset(u_height_tex, v_tex, ivec2( 1.0,  0.0)).r;
  float u_left   = textureOffset(u_height_tex, v_tex, ivec2(-1.0,  0.0)).r;
  float u_top    = textureOffset(u_height_tex, v_tex, ivec2( 0.0, -1.0)).r;
  float u_bottom = textureOffset(u_height_tex, v_tex, ivec2( 0.0,  1.0)).r;
 
  float c = 50.0;
  float f = ((c*c) * ((u_right + u_left + u_top + u_bottom) - (4.0 * u)) ) / 4.0;
 
  float v = texture(u_vel_tex, v_tex).r + f * u_dt;
  out_height = u + v * u_dt;
  out_vel = v * 0.996;
}