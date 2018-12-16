#version 330
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 1) in vec4 vertexposition;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec2 texcoord;

uniform mat4 mvp;
uniform mat4 model;
out vec2 uv;
out vec4 norm;
out vec4 pos;

void main()
{
	gl_Position = mvp * vertexposition;
	pos = model * vertexposition;
	uv = texcoord;
	norm = normal;
}
