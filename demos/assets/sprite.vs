#version 330
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 1) in vec4 vertexposition;
layout(location = 2) in vec4 worldpos;
layout(location = 3) in vec4 spritedata;

uniform mat4 proj;
uniform mat4 view;
out vec4 pos;
out vec3 uv;

mat2 rotate2d(float angle)
{
    return mat2(cos(angle),-sin(angle),
                sin(angle),cos(angle));
}

void main()
{
    mat2 r = rotate2d(spritedata.z);
    vec2 s = vertexposition.xy * spritedata.xy;
    s = r * s;

    mat4 mvp = proj * view;

    vec3 cr = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cu = vec3(view[0][1], view[1][1], view[2][1]);    
    vec3 vp = worldpos.xyz + cr * s.x + cu * s.y;
	uv = vec3(vertexposition.xy + vec2(0.5, 0.5), spritedata.w);
	pos = vec4(vp,1);
	gl_Position = mvp * pos;
}
