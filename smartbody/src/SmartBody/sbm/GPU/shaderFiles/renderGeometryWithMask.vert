#version 150

#define MAX_SHAPES 14

uniform mat4		uMatrixMV;
uniform mat4 		uMatrixProj;
//uniform mat4 		uTranslate;
uniform mat4 		uRotate;
in vec2			aVertexTexcoord;
in vec3			aVertexPosition;
out vec2		texCoords;

void main()
{
	texCoords	= aVertexTexcoord;
	gl_Position = uMatrixProj * uMatrixMV * uRotate  * vec4(aVertexPosition, 1.0);
	//gl_Position = pos_shape[0];

//	if(int(isBorder) == 1)
//		gl_Position =  pos_shape[0];
}