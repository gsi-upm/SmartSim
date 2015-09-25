uniform int			uNumberOfTextures;
uniform int			uIteration;
uniform int			uFaceArea;
uniform float		uWeight;
uniform float		uWeightUpToNow;
uniform float		uTotalWeights;
uniform sampler2D	uNeutralSampler;			// Neutral texture
uniform	sampler2D	uExpressionSampler;			// Current expresion texture
uniform sampler2D	uPreviousResultSampler;		// Result sampler	

void main()
{
		vec4 new			= vec4(0.0, 0.0, 0.0, 1.0);
		
		vec4 texNeutral		= texture2D(uNeutralSampler, gl_TexCoord[0].st);
		vec4 texExpression	= texture2D(uExpressionSampler, gl_TexCoord[0].st);
		vec4 texPrevious	= texture2D(uPreviousResultSampler,  gl_TexCoord[0].st);
		vec4 texBlend		= texNeutral * (1.0 - uWeight) + texExpression * uWeight;

		if(uTotalWeights > 0.0001)
		{
			if(uWeight > 0.0001)
			{
				// The C++ loop that calls this shaders runs
				// from (uNumberOfTextures - 1) to 0, so this is the first iteration
				if(uIteration == uNumberOfTextures-1)	
				{
					//new	= uWeight/uTotalWeights * texBlend;
					new	= texBlend;
				}
				// If not the first iteration, updates previous blend
				else
				{
					//new	= texPrevious + uWeight/uTotalWeights * texBlend;
					new	= ((uWeightUpToNow-uWeight)/uWeightUpToNow) * texPrevious + uWeight/uWeightUpToNow * texBlend;
				}
			}
			else
			{
				if(uIteration == uNumberOfTextures-1)	
				{
					//new	= uWeight/uTotalWeights * texBlend;
					new	= texBlend;
				}
				else 
				{
					new = texPrevious;
				}
			}
		}
		// If not weights, output neutral expression 
		else
		{
			new = texNeutral;
		}
		gl_FragColor		= new;


		float MIN_BLENDING_HEIGHT = 0.45;
		float MAX_BLENDING_HEIGHT = 0.55;

		if((uFaceArea == 1))		// Upper face
		{
			vec4 color_up		= ((1.0-uWeight)/1.0) * texPrevious + uWeight/1.0 * texBlend;
			vec4 color_down		= ((uTotalWeights-uWeight)/uTotalWeights) * texPrevious + uWeight/uTotalWeights * texPrevious;
			
			if((gl_TexCoord[0].t > MIN_BLENDING_HEIGHT) && (gl_TexCoord[0].t < MAX_BLENDING_HEIGHT))
			{
				float weight	= (gl_TexCoord[0].t - MIN_BLENDING_HEIGHT) / (MAX_BLENDING_HEIGHT-MIN_BLENDING_HEIGHT);
				gl_FragColor	= color_up * weight + color_down * (1.0 - weight);
			}
			else if(gl_TexCoord[0].t >= MAX_BLENDING_HEIGHT)
			{
				gl_FragColor	= color_up;
			}
			else
			{
				gl_FragColor	= color_down;
			}
		} 
		else if(uFaceArea == 2)	// Lower face
		{
			vec4 color_area		= ((1.0-uWeight)/1.0) * texPrevious + uWeight/1.0 * texBlend;
			vec4 color_other	= ((uTotalWeights-uWeight)/uTotalWeights) * texPrevious + uWeight/uTotalWeights * texPrevious;
			
			if((gl_TexCoord[0].t > MIN_BLENDING_HEIGHT) && (gl_TexCoord[0].t < MAX_BLENDING_HEIGHT))
			{
				float weight	= (gl_TexCoord[0].t - MIN_BLENDING_HEIGHT) / (MAX_BLENDING_HEIGHT - MIN_BLENDING_HEIGHT);
				gl_FragColor	= color_area * (1.0-weight) + color_other * (weight);
			}
			else if(gl_TexCoord[0].t <= MIN_BLENDING_HEIGHT)
			{
				gl_FragColor	= color_area;
			}
			else
			{
				gl_FragColor	= color_other;
			}
		}

//		if(uFaceArea == 2)
//			gl_FragColor	= vec4(1.0,0.0,0.0,1.0);

//		else if(uFaceArea == 2)	// Lower face
//		{
//			if(gl_TexCoord[0].t < 0.5)
//			{
//				//gl_FragColor	= new;
//				gl_FragColor	= ((1.0-uWeight)/1.0) * texPrevious + uWeight/1.0 * texBlend;
//			}
//			else
//			{
//				//gl_FragColor	= (texPrevious) + uWeight/uTotalWeights * (uTotalWeights/uWeightUpToNow) * texPrevious ;
//				//gl_FragColor	= texPrevious +  uWeight/uTotalWeights * vec4(normalize(texPrevious.rgb),1.0);
//				//gl_FragColor	= texPrevious + uWeight/uTotalWeights * texNeutral;
//				gl_FragColor	= ((uWeightUpToNow-uWeight)/uWeightUpToNow) * texPrevious + uWeight/uWeightUpToNow * texPrevious;
//			}
//		} 
}
