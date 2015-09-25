//#version 130
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

//uniform sampler2D	texNeutral;
//uniform sampler2D	texFv;
//uniform sampler2D	texOpen;
//uniform sampler2D	texPBM;
//uniform sampler2D	texShCh;
//uniform sampler2D	texW;
//uniform sampler2D	texWide;
					
//uniform float		wNeutral;
//uniform float		wFv;
//uniform float		wOpen;
//uniform float		wPBM;
//uniform float		wShCh;
//uniform float		wW;
//uniform float		wWide;

uniform float		uWeights[50];	// WARNING! Hardcoded maximum of 50
uniform sampler2D	uTextures[50];	// WARNING! Hardcoded maximum of 50
uniform int			uNumberOfShapes;

void main()
{
		vec4 new		= vec4(0.0, 0.0, 0.0, 1.0);
		
		//vec4 neutral	= texture2D(texNeutral, gl_TexCoord[0].st);
		//vec4 fv		= texture2D(texFv, gl_TexCoord[0].st);
		//vec4 open		= texture2D(texOpen, gl_TexCoord[0].st);
		//vec4 PBM		= texture2D(texPBM, gl_TexCoord[0].st);
		//vec4 ShCh		= texture2D(texShCh, gl_TexCoord[0].st);
		//vec4 W		= texture2D(texW, gl_TexCoord[0].st);
		//vec4 Wide		= texture2D(texWide, gl_TexCoord[0].st);

		//vec4 blendFv		= neutral * (1.0 - wFv) + fv * wFv;
		//vec4 blendOpen	= neutral * (1.0 - wOpen) + open * wOpen;
		//vec4 blendPBM		= neutral * (1.0 - wPBM) + PBM * wPBM;
		//vec4 blendShCh	= neutral * (1.0 - wShCh) + ShCh * wShCh;
		//vec4 blendW		= neutral * (1.0 - wW) + W * wW;
		//vec4 blendWide	= neutral * (1.0 - wWide) + Wide *wWide;
		//
		//float total_weights = wFv + wOpen + wPBM + wShCh + wW + wWide;

		//if(total_weights < 0.0001)
		//{
		//	new		= neutral;
		//} 
		//else
		//{
		//	new		+= wFv/total_weights * blendFv;
		//	new		+= wOpen/total_weights * blendOpen;
		//	new		+= wPBM/total_weights * blendPBM;
		//	new		+= wShCh/total_weights * blendShCh;
		//	new		+= wW/total_weights * blendW;
		//	new		+= wWide/total_weights * blendWide;
		//}

		vec4 tex[50];	//	tex for the original texture colors of this fragment of each texture
		float w[50];	//	w is the weight of each shape
		vec4 blend[50];	//	blend contains the blended texture of each face WRT the neutral (pos 0)

		for(int i=0; i<50; i++) {
			tex[i]		= vec4(0.0, 0.0, 0.0, 1.0);
			blend[i]	= vec4(0.0, 0.0, 0.0, 1.0);
			w[i]		= 0.0;
		}

		
		tex[0]		= texture2D(uTextures[0], gl_TexCoord[0].st);
		tex[1]		= texture2D(uTextures[1], gl_TexCoord[0].st);
		tex[2]		= texture2D(uTextures[2], gl_TexCoord[0].st);
		tex[3]		= texture2D(uTextures[3], gl_TexCoord[0].st);
		tex[4]		= texture2D(uTextures[4], gl_TexCoord[0].st);
		tex[5]		= texture2D(uTextures[5], gl_TexCoord[0].st);
		tex[6]		= texture2D(uTextures[6], gl_TexCoord[0].st);
		//tex[7]		= texture2D(uTextures[7], gl_TexCoord[0].st);
		//tex[8]		= texture2D(uTextures[8], gl_TexCoord[0].st);
		//tex[9]		= texture2D(uTextures[9], gl_TexCoord[0].st);

		w[0]		= uWeights[0];
		w[1]		= uWeights[1];
		w[2]		= uWeights[2];
		w[3]		= uWeights[3];
		w[4]		= uWeights[4];
		w[5]		= uWeights[5];
		w[6]		= uWeights[6];
		//w[7]		= uWeights[7];
		//w[8]		= uWeights[8];
		//w[9]		= uWeights[9];
		
		//for(int i=1; i<uNumberOfShapes; i++) {
		//	w[i] = uWeights[i];
		//}

		// Note: blend0 is neutral, no need to compute
		blend[1]	= tex[0] * (1.0 - w[1]) + tex[1] * w[1];
		blend[2]	= tex[0] * (1.0 - w[2]) + tex[2] * w[2];
		blend[3]	= tex[0] * (1.0 - w[3]) + tex[3] * w[3];
		blend[4]	= tex[0] * (1.0 - w[4]) + tex[4] * w[4];
		blend[5]	= tex[0] * (1.0 - w[5]) + tex[5] * w[5];
		blend[6]	= tex[0] * (1.0 - w[6]) + tex[6] * w[6];
		//blend[7]	= tex[0] * (1.0 - w[7]) + tex[7] * w[7];
		//blend[8]	= tex[0] * (1.0 - w[8]) + tex[8] * w[8];
		//blend[9]	= tex[0] * (1.0 - w[9]) + tex[9] * w[9];

		float total_weights = w[1] + w[2] + w[3] + w[4] + w[5] + w[6] + w[7] + w[8] + w[9];

		if(total_weights < 0.0001)
		{
			new		= tex[0];
		} 
		else
		{
			new		+= w[1]/total_weights * blend[1];
			new		+= w[2]/total_weights * blend[2];
			new		+= w[3]/total_weights * blend[3];
			new		+= w[4]/total_weights * blend[4];
			new		+= w[5]/total_weights * blend[5];
			new		+= w[6]/total_weights * blend[6];

			//for(int i=1; i<uNumberOfShapes; i++) {
			//	new	+= w[i]/total_weights * blend[i];
			//}
		}

		gl_FragColor = new;
}
