#version 330
#extension GL_ARB_explicit_uniform_location : enable

uniform sampler2D tex;
uniform sampler2D gloss;
uniform sampler2D mirror;
uniform vec2 viewport;
in vec2 uv;
out vec4 fragcolor;

void main()
{
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
	fragcolor = mix(surface, reflection, 0.2);
}