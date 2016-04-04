#version 150

#define MAX_SHAPES 14

uniform sampler2D	uNeutralSampler[MAX_SHAPES];
uniform int			uNumberOfShapes;
uniform float		uWeights[MAX_SHAPES];
uniform int			uAreas[MAX_SHAPES]; 

in vec2				texCoords;
out vec4			final_color;

void main()
{
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
	//tex[14] = texture2D(uNeutralSampler[14], texCoords);

	final_color			= vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	// If more than 1 shape used, need to blend textures
	if(uNumberOfShapes > 1)
	{
		float totalWeights	= 0.0;
	
		// Per-shape interpolation with neutral
		for(int i=1; i < uNumberOfShapes; i++)
		{
			//	If uAreas == 1 means is an shape with infuence in the upper area
			//	Therefore leave neutral if current texCoords.y < 0.5
			if((uAreas[i] == 1) && (texCoords.y < 0.55))
			{
				float MIN_Y = 0.45;
				float MAX_Y = 0.55;
				float interpolatedWeight = uWeights[i] * clamp((texCoords.y - MIN_Y)/(MAX_Y - MIN_Y), 0.0, 1.0);
				tex[i] = tex[0] * (1.0 - interpolatedWeight) + tex[i] * interpolatedWeight;
			}
			// Lower face
			else if((uAreas[i] == 2) && (texCoords.y > 0.45))
			{
				float MIN_Y = 0.45;
				float MAX_Y = 0.55;
				float interpolatedWeight = 0.0;

				if(texCoords.y < 0.55)
				{
					interpolatedWeight = uWeights[i] * clamp(abs((texCoords.y - MAX_Y))/(abs(MIN_Y-MAX_Y)), 0.0, 1.0);
				}
				
				
				tex[i] = tex[0] * (1.0 - interpolatedWeight) + tex[i] * interpolatedWeight;
				
			}
			else
				tex[i] = tex[0] * (1.0 - uWeights[i]) + tex[i] * uWeights[i];
		
			totalWeights = totalWeights + uWeights[i];
		}

		// Normalization of weights wrt sum of weights
		for(int i=1; i < uNumberOfShapes; i++)
		{
			final_color = final_color + tex[i] * (uWeights[i] / totalWeights);
		}
	}
	//	All weights are 0, render neutral
	else
	{
		final_color =  tex[0];
	}
}