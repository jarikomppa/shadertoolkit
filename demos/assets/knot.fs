#version 330
#extension GL_ARB_explicit_uniform_location : enable

uniform sampler2D tex;
uniform vec4 lightpos;

in vec2 uv;
in vec4 norm;
in vec4 pos;
out vec4 fragcolor;

void main()
{
	vec4 lightdir = normalize(pos-lightpos);
	float lit = dot(norm, lightdir);
	fragcolor = texture(tex, uv.xy) * lit;
}