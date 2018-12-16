#version 330
#extension GL_ARB_explicit_uniform_location : enable

in vec2 uv;
out vec4 fragcolor;

void main()
{
	fragcolor = vec4(1,0,0,1);
}