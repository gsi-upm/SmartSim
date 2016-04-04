//
//	Adds the masking file to the alpha channel of the texture.
//
// Inputs:
//	-uExpressionSampler ->	texture map
//	-uMaskSampler		->	gray scale image that represents the masking to be applied (white: use, blank: don't use)

void main()
{
    gl_TexCoord[0]	= gl_MultiTexCoord0;
    gl_Position		= ftransform();
}
