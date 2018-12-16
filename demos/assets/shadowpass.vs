#version 330
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 1) in vec4 vertexposition;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vertexposition;
}
