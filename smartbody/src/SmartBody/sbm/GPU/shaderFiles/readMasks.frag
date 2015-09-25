//
//	Adds the masking file to the alpha channel of the texture.
//
// Inputs:
//	-uExpressionSampler ->	texture map
//	-uMaskSampler		->	gray scale image that represents the masking to be applied (white: use, blank: don't use)

uniform	sampler2D	uExpressionSampler;			// Current expresion texture
uniform	sampler2D	uMaskSampler;

void main()
{
		vec4 new			= vec4(0.0, 0.0, 0.0, 1.0);
		
		vec4 texExpression	= texture2D(uExpressionSampler, gl_TexCoord[0].st);
		vec4 texMask		= texture2D(uMaskSampler, gl_TexCoord[0].st);
		
		//gl_FragColor		= vec4(texMask.rgb, texExpression.r/texExpression.r);
		
		// Copies mask in alpha channel
		gl_FragColor		= vec4(texExpression.rgb, texMask.r);
}
