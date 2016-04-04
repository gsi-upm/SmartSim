#version 150

#define MAX_SHAPES 14

uniform mat4		uMatrixMV;
uniform mat4 		uMatrixProj;
//uniform mat4 		uTranslate;
uniform mat4 		uRotate;
uniform float		uWeights[MAX_SHAPES];
uniform int			uNumberOfShapes;
uniform sampler2D	uNeutralSampler[MAX_SHAPES];
uniform bool		uUseMasks;
uniform usamplerBuffer	uBorderVertices;

in vec2			aVertexTexcoord;
in vec3			aVertexPosition[MAX_SHAPES];

out vec2		texCoords;



void main()
{
	texCoords	= aVertexTexcoord;
	
	vec4 pos_shape[MAX_SHAPES];
	vec4 diff_shape[MAX_SHAPES];

	//uint isBorder = texelFetch(uBorderVertices, int(gl_VertexID)).r;

	vec4 tex[MAX_SHAPES];

	tex[0]	= texture2D(uNeutralSampler[0], texCoords);
	tex[1]	= texture2D(uNeutralSampler[1], texCoords);
	tex[2]	= texture2D(uNeutralSampler[2], texCoords);
	tex[3]	= texture2D(uNeutralSampler[3], texCoords);
	tex[4]	= texture2D(uNeutralSampler[4], texCoords);
	tex[5]	= texture2D(uNeutralSampler[5], texCoords);
	tex[6]	= texture2D(uNeutralSampler[6], texCoords);
	tex[7]	= texture2D(uNeutralSampler[7], texCoords);
	tex[8]	= texture2D(uNeutralSampler[8], texCoords);
	tex[9]	= texture2D(uNeutralSampler[9], texCoords);
	tex[10] = texture2D(uNeutralSampler[10], texCoords);
	tex[11] = texture2D(uNeutralSampler[11], texCoords);
	tex[12] = texture2D(uNeutralSampler[12], texCoords);
	tex[13] = texture2D(uNeutralSampler[13], texCoords);
	
	//	Computes vertex point in world coordinates
	for(int i=0; i < uNumberOfShapes; i++)
	{
		//pos_shape[i]	= uMatrixProj * uMatrixMV * uTranslate * uRotate  * vec4(aVertexPosition[i], 1.0); 
		pos_shape[i]	= uMatrixProj * uMatrixMV * uRotate  * vec4(aVertexPosition[i], 1.0); 
	}

	//	Differente of each shape wrt neutral
	for(int i=0; i < uNumberOfShapes; i++)
	{
		diff_shape[i]	= pos_shape[i] - pos_shape[0];
	}


	vec4 result_shape = pos_shape[0];
	float interpolatedWeight	= 0.0;

	for(int i=1; i < uNumberOfShapes; i++)
	{
		if(!uUseMasks)
		{
			tex[i].a = 1.0f;
		}

		interpolatedWeight = uWeights[i] * tex[i].a;
		result_shape = result_shape + diff_shape[i] * interpolatedWeight;
	}

	gl_Position = result_shape;
	//gl_Position = pos_shape[0];

//	if(int(isBorder) == 1)
//		gl_Position =  pos_shape[0];
}