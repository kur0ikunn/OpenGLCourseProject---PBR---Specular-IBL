#version 460

layout(early_fragment_tests) in;

in vec3 TexCoord;
out vec4 color;


uniform sampler2D equirectangularMap;

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(TexCoord));
    vec3 texColor = texture(equirectangularMap, uv).rgb;
    
    color = vec4(texColor, 1.0);
}
