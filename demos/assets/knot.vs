#version 330
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 1) in vec4 vertexposition;
layout(location = 3) in vec2 texcoord;

uniform mat4 mvp;
out vec2 uv;

void main()
{
	gl_Position = mvp * vertexposition;
	uv = texcoord;
}
