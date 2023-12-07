#version 450
 
in vec2 a_tex;
uniform mat4 u_pm;
uniform mat4 u_vm;
uniform sampler2D u_pos_tex;
 
void main() {
  vec4 world_pos = vec4(texelFetch(u_pos_tex, ivec2(a_tex), 0).rgb, 1.0);
  gl_Position = u_pm * u_vm * world_pos;
}