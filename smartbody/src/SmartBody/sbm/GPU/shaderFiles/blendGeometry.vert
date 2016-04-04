#version 150

#define MAX_SHAPES 14

uniform mat4	uMatrixMV;
uniform mat4 	uMatrixProj;
uniform mat4 	uTranslate;
uniform float	uWeights[MAX_SHAPES];
uniform int		uAreas[MAX_SHAPES];
uniform int		uNumberOfShapes;

uniform usamplerBuffer	uBorderVertices;

in vec2			aVertexTexcoord;
in vec3			aVertexPosition[MAX_SHAPES];

out vec2		texCoords;



void main()
{
	texCoords	= aVertexTexcoord;
	
	vec4 pos_shape[MAX_SHAPES];
	vec4 diff_shape[MAX_SHAPES];

	uint isBorder = texelFetch(uBorderVertices, int(gl_VertexID)).r;
	
	//	Computes vertex point in world coordinates
	for(int i=0; i < uNumberOfShapes; i++)
	{
		pos_shape[i]	= uMatrixProj * uMatrixMV * uTranslate * vec4(aVertexPosition[i], 1.0);
	}

	//	Differente of each shape wrt neutral
	for(int i=0; i < uNumberOfShapes; i++)
	{
		diff_shape[i]	= pos_shape[i] - pos_shape[0];
	}


	vec4 result_shape = pos_shape[0];
	
	for(int i=1; i < uNumberOfShapes; i++)
	{
		// Full face
		if(uAreas[i] == 0)
		{
			result_shape = result_shape + diff_shape[i] * uWeights[i];
		}
		// Upper face
		else if(uAreas[i] == 1)
		{
			if(texCoords.y > 0.45)
			{
				float MIN_Y = 0.45;
				float MAX_Y = 0.55;
				float interpolatedWeight = uWeights[i] * clamp((texCoords.y - MIN_Y)/(MAX_Y - MIN_Y), 0.0, 1.0);

				result_shape = result_shape + diff_shape[i] * interpolatedWeight;
			}
		}
		// Lower face
		else if(uAreas[i] == 2)
		{
			if(texCoords.y < 0.55)
			{
				float MIN_Y = 0.45;
				float MAX_Y = 0.55;
				float interpolatedWeight = uWeights[i] * clamp((abs(texCoords.y - MAX_Y))/(abs(MIN_Y - MAX_Y)), 0.0, 1.0);

				result_shape = result_shape + diff_shape[i] * interpolatedWeight;
			}
		}
	}

	gl_Position = result_shape;

//	if(int(isBorder) == 1)
//		gl_Position =  pos_shape[0];
}