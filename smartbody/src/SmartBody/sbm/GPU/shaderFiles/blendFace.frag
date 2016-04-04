#version 140

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform float weight;

in vec2 out_texCoord;
out	vec4 final_color;


void main()
{
	float clamp_weight = clamp(weight, 0.0, 1.0);
	vec4 tex0	= texture(tex0,  out_texCoord);
	vec4 tex1	= texture(tex1,  out_texCoord);
	final_color	= (1.0 - clamp_weight) * tex0 + clamp_weight * tex1;
}