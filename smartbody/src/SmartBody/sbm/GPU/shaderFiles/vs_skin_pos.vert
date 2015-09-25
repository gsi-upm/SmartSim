//#version 120
#extension GL_EXT_gpu_shader4 : enable
uniform samplerBuffer Transform;
<<<<<<< .working
<<<<<<< .working
attribute vec4 BoneID;
attribute vec4 BoneWeight;
varying vec3 OutPos;
=======
attribute vec4 BoneID1,BoneID2;
attribute vec4 BoneWeight1,BoneWeight2;
//varying vec3 OutPos;
varying vec3 normal,lightDir,halfVector;
>>>>>>> .merge-right.r1595
=======
attribute vec4 BoneID1,BoneID2;
attribute vec4 BoneWeight1,BoneWeight2;
//varying vec3 OutPos;
varying vec3 normal,lightDir[2],halfVector[2];
varying float dist[2];
>>>>>>> .merge-right.r1678

//fetch transformation matrix
mat3 GetTransformation(float id)
{
	int idx = int(id);
	mat3 rot;
	for (int i=0;i<3;i++)
	{		
		for (int j=0;j<3;j++)
<<<<<<< .working
<<<<<<< .working
			rot[j][i] = texelFetchBuffer(Transform,(int)(id*9+i*3+j)).x;		
=======
			rot[j][i] = texelFetchBuffer(Transform,(int)(id*16+i*4+j)).x;		
>>>>>>> .merge-right.r1595
=======
			rot[j][i] = texelFetchBuffer(Transform,(idx*16+i*4+j)).x;		
>>>>>>> .merge-right.r1633
	}	
	return rot;
}

vec3 GetTranslation(float id)
{
	int idx = int(id);
	vec3 tran;
<<<<<<< .working
<<<<<<< .working
	tran[0] = texelFetchBuffer(Offset,(int)(id*3+0)).x;
	tran[1] = texelFetchBuffer(Offset,(int)(id*3+1)).x;
	tran[2] = texelFetchBuffer(Offset,(int)(id*3+2)).x;
=======
	tran[0] = texelFetchBuffer(Transform,(int)(id*16+12)).x;
	tran[1] = texelFetchBuffer(Transform,(int)(id*16+13)).x;
	tran[2] = texelFetchBuffer(Transform,(int)(id*16+14)).x;
>>>>>>> .merge-right.r1595
=======
	tran[0] = texelFetchBuffer(Transform,(idx*16+12)).x;
	tran[1] = texelFetchBuffer(Transform,(idx*16+13)).x;
	tran[2] = texelFetchBuffer(Transform,(idx*16+14)).x;
>>>>>>> .merge-right.r1633
	return tran;	
}
<<<<<<< .working
<<<<<<< .working

vec3 TransformPos(vec3 position, float4 boneid, float4 boneweight)
=======
vec3 TransformPos(vec3 position, vec4 boneid, vec4 boneweight)
>>>>>>> .merge-right.r1595
=======
mat3 TransformPos(vec3 position, vec3 normal, vec4 boneid, vec4 boneweight)
>>>>>>> .merge-right.r1678
{
	vec3 pos = vec3(0,0,0);
	vec3 n = vec3(0,0,0);
	mat3 tempT;	
	vec3 tempt;	
	for (int i=0;i<4;i++)
	{
		tempT = GetTransformation(boneid[i]);
		tempt = GetTranslation(boneid[i]);
		pos += (position*tempT+tempt)*boneweight[i]; 		
		n   += (normal*tempT)*boneweight[i];
	}	
	mat3 result;
	result[0] = pos;
	result[1] = n;
	//tempT = GetTransformation(50);
	//pos = tempT[0].xyz;
	return result;
	
}
void main()
{	
	// the following three lines provide the same result
	vec3 pos = vec3(gl_Vertex.xyz);
<<<<<<< .working
<<<<<<< .working
	vec3 tranPos = TransformPos(pos,BoneID,BoneWeight);	
=======
    vec3 tranPos = TransformPos(pos,BoneID1,BoneWeight1) + TransformPos(pos,BoneID2,BoneWeight2);	
>>>>>>> .merge-right.r1595
	//gl_Position = vec4(tranPos,1.0);
	gl_Position = gl_ModelViewProjectionMatrix*vec4(tranPos,1.0);//gl_Vertex;//vec4(pos,1.0);
	//OutPos = tranPos.xyz;	
	lightDir = normalize(vec3(gl_LightSource[0].position));
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
=======
    mat3 skin = TransformPos(pos,gl_Normal,BoneID1,BoneWeight1) + TransformPos(pos,gl_Normal,BoneID2,BoneWeight2);		
	gl_Position = gl_ModelViewProjectionMatrix*vec4(skin[0],1.0);//gl_Vertex;//vec4(pos,1.0);	
	lightDir[0] = normalize(vec3(gl_LightSource[0].position));
	halfVector[0] = normalize(gl_LightSource[0].halfVector.xyz);	
 	dist[0] = 0.0;
 	vec3 posDir = vec3(gl_LightSource[1].position - gl_ModelViewMatrix * vec4(skin[0],1.0));
 	dist[1] = length(posDir);
 	lightDir[1] = normalize(posDir);
 	halfVector[1] = normalize(gl_LightSource[1].halfVector.xyz);
	normal = normalize(gl_NormalMatrix * skin[1]);
	
>>>>>>> .merge-right.r1678
}