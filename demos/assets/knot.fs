#version 330
#extension GL_ARB_explicit_uniform_location : enable

uniform sampler2D tex;
in vec2 uv;
out vec4 fragcolor;

void main()
{
	fragcolor = texture(tex, uv.xy);
}