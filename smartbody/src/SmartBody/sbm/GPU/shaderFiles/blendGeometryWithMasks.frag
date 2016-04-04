#version 150

#define MAX_SHAPES 14

uniform sampler2D	uNeutralSampler[MAX_SHAPES];
uniform int			uNumberOfShapes;
uniform float		uWeights[MAX_SHAPES];
uniform int			uUsedShapeIDs[MAX_SHAPES];
uniform bool		uShowMasks;
uniform bool		uUseMasks;
uniform bool		uOverlayMasks;

in vec2				texCoords;
out vec4			final_color;

// Function used to retreive the color used for each shape, when using the show masking mode.
// which renders the area of influence of each particular shape, when using masking, with a different color
vec4 getColorFromID(int shapeID)
{
	vec4 weightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	if(shapeID == 1)
	{
		weightColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (shapeID == 2)
	{
		weightColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	else if (shapeID == 3)
	{
		weightColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else if (shapeID == 4)
	{
		weightColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	}
	else if (shapeID == 5)
	{
		weightColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	}
	else if (shapeID == 6)
	{
		weightColor = vec4(0.75f, 0.23f, 1.0f, 1.0f);
	}
	else if (shapeID == 7)
	{
		weightColor = vec4(0.75f, 0.0f, 0.4f, 1.0f);
	}
	else if (shapeID == 8)
	{
		weightColor = vec4(0.0f, 0.57f, 0.60f, 1.0f);
	}
	else if (shapeID == 9)
	{
		weightColor = vec4(0.4f, 1.0f, 0.3f, 1.0f);
	}
	else if (shapeID == 10)
	{
		weightColor = vec4(0.9f, 0.10f, 0.55f, 1.0f);
	}
	else if (shapeID == 11)
	{
		weightColor = vec4(0.55f, 0.15f, 0.12f, 1.0f);
	}
	else if (shapeID == 12)
	{
		weightColor = vec4(0.2f, 0.75f, 0.4f, 1.0f);
	}
	else if (shapeID == 13)
	{
		weightColor = vec4(0.2f, 0.4f, 0.8f, 1.0f);
	}
	else
	{
		weightColor = vec4(0.0f, 0.5f, 1.0f, 1.0f);
	}

	return weightColor;
}

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

	final_color					= vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float interpolatedWeight	= 0.0;
	float interpolatedWeights[MAX_SHAPES];

	bool uShowWeights = uShowMasks;

	if(uShowWeights)
	{
		tex[0] = vec4(0.0, 0.0, 0.0, 1.0);
	}

	// If more than 1 shape used, need to blend textures
	if(uNumberOfShapes > 1)
	{
		float totalWeights				= 0.0;
		float totalInterpolatedWeights	= 0.0;

		// Per-shape interpolation with neutral
		for(int i=1; i < uNumberOfShapes; i++)
		{
			interpolatedWeights[i] = 0.0;

			if(!uUseMasks)
			{
				tex[i].a = 1.0f;
			}

			// Interpolates blending weight by masking (encoded in alpha channel)
			interpolatedWeight		= float(uWeights[i]) * float(tex[i].a);
			interpolatedWeights[i] = interpolatedWeight;

			// If showing weights
			if(uShowWeights)
			{
				// Gets current shape ID
				int shapeID = uUsedShapeIDs[i];

				// Gets the color for that particular shape
				vec4 weightColor = getColorFromID(shapeID);
				tex[i] = weightColor;
			}

			// If overlayMasks attribute is on, we want to render a blendedcolor mask on the top of the texture
			if(uOverlayMasks)
			{
				// Gets current shape ID
				int shapeID = uUsedShapeIDs[i];
				vec4 weightColor = getColorFromID(shapeID);
				tex[i] = mix(tex[i], weightColor, 0.4);
			}

			tex[i] = tex[0] * (1.0 - interpolatedWeight) + tex[i] * interpolatedWeight;

			totalWeights				= totalWeights + uWeights[i];
			totalInterpolatedWeights	= totalInterpolatedWeights + interpolatedWeight;
		}


		if(totalInterpolatedWeights < 0.0001)
		{
			final_color = tex[0];
		}
		else
		{
			// Normalization of weights wrt sum of weights
			for(int i=1; i < uNumberOfShapes; i++)
			{
				final_color = final_color + vec4(tex[i].rgb * (interpolatedWeights[i] / totalInterpolatedWeights), 1.0f);
			}
		}
	}
	//	All weights are 0, render neutral
	else
	{
		final_color =  tex[0];
	}
	//final_color = vec4(1.0,0.0,0.0,1.0);
}