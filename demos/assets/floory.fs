#version 330
#extension GL_ARB_explicit_uniform_location : enable

uniform sampler2D tex;
uniform sampler2D gloss;
uniform sampler2D mirror;
uniform sampler2D shadowmap;
uniform vec2 viewport;
uniform mat4 shadowmatrix;
in vec2 uv;
in vec4 coord;
out vec4 fragcolor;

float getshadowsample(vec4 scoord)
{
	float shadowdepth = texture(shadowmap, scoord.xy).r;

	if (scoord.w < 0.0 ||
		scoord.x < 0.0 ||
		scoord.y < 0.0 ||
		scoord.x > 1.0 ||
		scoord.y > 1.0)
	{
		return 1.0; // what to do if outside the shadow map area (depends on application)
	}
	if (shadowdepth < scoord.z)
	{
		return 0.5; // what to do if in shadow
	}
	return 1.0; // what to do in light
}

vec2 poissonDisk[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760),
	vec2(-0.91588581, 0.45771432),
	vec2(-0.81544232, -0.87912464),
	vec2(-0.38277543, 0.27676845),
	vec2(0.97484398, 0.75648379),
	vec2(0.44323325, -0.97511554),
	vec2(0.53742981, -0.47373420),
	vec2(-0.26496911, -0.41893023),
	vec2(0.79197514, 0.19090188),
	vec2(-0.24188840, 0.99706507),
	vec2(-0.81409955, 0.91437590),
	vec2(0.19984126, 0.78641367),
	vec2(0.14383161, -0.14100790)
	);


void main()
{
	vec4 shadowcoord = shadowmatrix * coord;
    shadowcoord /= shadowcoord.w;
    float shade = 0;
	for (int i = 0; i < 16; i++)
	{
		vec4 temp = shadowcoord;
		temp.xy += poissonDisk[i] / 200.0;
		shade += getshadowsample(temp);
	}
	shade /= 16;

	vec2 reflectionuv = vec2(gl_FragCoord.x / viewport.x, 1-gl_FragCoord.y / viewport.y);
    vec4 surface = texture(tex, uv.xy);
    vec4 glossness = texture(gloss, uv.xy);
    vec4 reflection = vec4(0);
    for (int i = 0; i < 4; i++)
    {
        vec2 uv2 = reflectionuv + vec2(i,16-i)*(1-glossness.r)*0.05;
        reflection += texture(mirror, uv2.xy);
    }
    reflection /= 4;
	fragcolor = mix(surface, reflection, 0.5) * shade;
}