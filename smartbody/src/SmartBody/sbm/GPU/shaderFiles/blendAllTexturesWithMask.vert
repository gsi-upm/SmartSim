//#version 140
//
//uniform mat4 M;			//modelview matrix
//uniform mat4 P;			//projection matrix
//uniform float weight;
//
////input variables:	vertex position
//in vec3 pos0;
//in vec3 pos1;
//in vec2 texCoord;
//
//
////variables to be passed to the fragment shader
//out vec2 out_texCoord;
// 
//void main() {
//	float clamp_weight = clamp(weight, 0.0, 1.0); 
//
//	vec4 world_pos0	= P * M * vec4(pos0, 1);
//	vec4 world_pos1	= P * M * vec4(pos1, 1);
//
//	gl_Position		= (1.0 - clamp_weight) * world_pos0 + clamp_weight * world_pos1;
//	out_texCoord	= texCoord;
//}
out vec2		texCoords;
void main()
{
    texCoords = gl_MultiTexCoord0.xy;
    gl_Position = gl_Vertex;
}
