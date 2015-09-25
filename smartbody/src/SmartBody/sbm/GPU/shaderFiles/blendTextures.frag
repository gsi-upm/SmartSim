//#version 140
//
//uniform sampler2D tex0;
//uniform sampler2D tex1;
//uniform float weight;
//
//in vec2 out_texCoord;
//out	vec4 final_color;
//
//
//void main()
//{
//	float clamp_weight	= clamp(weight, 0.0, 1.0);
//	vec4 tex0			= texture(tex0,  out_texCoord);
//	vec4 tex1			= texture(tex1,  out_texCoord);
//	final_color			= (1.0 - clamp_weight) * tex0 + clamp_weight * tex1;
//}

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D texOut;
uniform float alpha;

void main()
{
        vec4 s1			= texture2D(tex0, gl_TexCoord[0].st);
        vec4 s2			= texture2D(tex1, gl_TexCoord[0].st);
		vec4 sOut		= texture2D(texOut, gl_TexCoord[0].st);
        //gl_FragColor	= mix(vec4(s1.r, s1.g, s1.b, s1.a), vec4(s2.r, s2.g, s2.b, 0.5), alpha);
        //gl_FragData[0]= mix(vec4(s1.r, s1.g, s1.b, s1.a), vec4(s2.r, s2.g, s2.b, s2.a), (1-alpha));
        //gl_FragData[1]= mix(vec4(s1.r, s1.g, s1.b, s1.a), vec4(s2.r, s2.g, s2.b, s2.a), (1-alpha));

		float r	= sOut.r + (s2.r - s1.r) * alpha;
		float g	= sOut.g + (s2.g - s1.g) * alpha;
		float b	= sOut.b + (s2.b - s1.b) * alpha;

		//float r	= sOut.r * (1.0 - alpha) + (s2.r) * alpha;
		//float g	= sOut.g * (1.0 - alpha) + (s2.g) * alpha;
		//float b	= sOut.b * (1.0 - alpha) + (s2.b) * alpha;

		gl_FragColor = vec4(r, g, b, 1.0); 

}
