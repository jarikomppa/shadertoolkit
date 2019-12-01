#version 330
#extension GL_ARB_explicit_uniform_location : enable

uniform sampler2D tex;
in vec3 uv;
out vec4 fragcolor;

void main()
{    
    vec4 t = texture(tex, uv.xy);
    fragcolor = vec4(t.rgb, t.a * uv.z);
}