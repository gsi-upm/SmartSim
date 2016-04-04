#ifndef SBM_DEFORMABLE_MESH_H
#define SBM_DEFORMABLE_MESH_H

#include <sb/SBTypes.h>
#include <vector>
#include <string>
#include <map>
#include <sr/sr_sn_shape.h>
#include <sk/sk_skeleton.h>
#include <sr/sr_model.h>
#include <sb/SBAsset.h>
#include <sb/SBCharacter.h>

#include "external/glm/glm/glm.hpp"

#define USE_SKIN_WEIGHT_SIZE_8 0

#ifdef WIN32
	//#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <wingdi.h>
	#include <GL/gl.h>
#elif defined(SB_IPHONE)
    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>
#elif defined(__APPLE__) || defined(__APPLE_CC__)
       #include <OpenGL/gl.h>
//       #include <Carbon/Carbon.h>
       #define APIENTRY
#elif defined(__FLASHPLAYER__)
	#include <GL/gl.h>
#elif defined(__ANDROID__)
	//#include <GLES/gl.h>
	#include <GLES2/gl2.h>
	//#include "wes_gl.h"
#else
	#include <GL/gl.h>
	#include <GL/glx.h>
#endif

typedef std::vector<SkJoint*> SkJointList;

class SkinWeight
{
public:
	std::vector<std::string>	infJointName;	// name array
	std::vector<SkJoint*>		infJoint;         // corresponding joint for each infJointName
	std::vector<float>			bindWeight;		// weight array
	std::vector<SrMat>			bindPoseMat;	// each joint, binding pose transformation matrix
	SrMat						bindShapeMat;	// overall bind shape transformation matrix
	std::string					sourceMesh;		// skin Source Name
	std::vector<unsigned int>	numInfJoints;	// number of influenced joints for very vertex
	std::vector<unsigned int>	weightIndex;	// looking up the weight according to this index
	std::vector<unsigned int>	jointNameIndex;	// looking up the joint name according to this index

public:
	SBAPI SkinWeight();
	
	SBAPI ~SkinWeight();
	SBAPI void normalizeWeights();
	SBAPI void copyWeights(SkinWeight* copy, const std::string& morphName);
};

class SbmSubMesh
{
public:
	SrMaterial  material;
	bool isHair;
	std::string matName;
	std::string texName;
	std::string normalMapName;
	std::string specularMapName;
	int numTri;
#if defined(__ANDROID__) || defined(SB_IPHONE)
	std::vector<SrModel::Face> triBuf;
#else
	std::vector<SrVec3i> triBuf;
#endif
};

class DeformableMeshInstance;
namespace SmartBodyBinary
{
	class StaticMesh;
}

struct BlendShapeData
{
	std::vector<std::pair<int, SrVec> > diffV;
	std::vector<std::pair<int, SrVec> > diffN;
};

/* This class is used to simulate and represent deformed mesh
   for Smartbody Characters.
*/
class DeformableMesh : public SmartBody::SBAsset
{
public:
//	std::string                 meshName;
	std::vector<SrSnModel*>		dMeshDynamic_p;
	std::vector<SrSnModel*>		dMeshStatic_p;
	std::vector<SkinWeight*>	skinWeights;
	std::map<std::string, std::vector<SrSnModel*> > blendShapeMap;	// the key store the base shape name, vector stores morph target SrModels. first one in the vector is always the base one
	std::map<std::string, std::vector<std::string> > morphTargets;	// stores a vector of morph target names, first one is always the base one
	std::vector<BlendShapeData> optimizedBlendShapeData;						// stores optimized information when calculating blend shapes; list of vertices affected, and their differential vector and normal amounts
	
	std::string                 skeletonName;						// binding skeleton for this deformable model
	SkSkeleton*					skeleton;							// pointer to current skeleton
	bool						binding;							// whether in deformable mesh mode
	// unrolled all vertices into a single buffer for faster GPU rendering
	bool initStaticVertexBuffer, initSkinnedVertexBuffer;	
	std::vector<SrVec>          posBuf;	
	std::vector<SrVec>          normalBuf;
	std::vector<SrVec>          tangentBuf;
	std::vector<SrVec>          binormalBuf;
	std::vector<SrVec>          skinColorBuf;
	std::vector<SrVec>          meshColorBuf;
	std::vector<SrVec2>         texCoordBuf;	
	std::vector<SrVec3i>        triBuf;
	std::vector<SbmSubMesh*>    subMeshList;

	std::vector<int>			boneCountBuf;
	std::vector<SrVec>          boneColorMap;
	std::vector<SrVec4i>        boneIDBuf[2];
	std::vector<SrVec4>         boneIDBuf_f[2];
	std::vector<SrVec4>         boneWeightBuf[2];
	std::map<std::string,int>   boneJointIdxMap;
	std::vector<SkJoint*>		boneJointList;	
	std::vector<std::string>    boneJointNameList;
	std::vector<SrMat>          bindPoseMatList;	
	std::map<int,std::vector<int> > vtxNewVtxIdxMap;

	// blend shape
	std::map<std::string, std::vector<SrSnModel*> >	visemeShapeMap;
	std::map<std::string, float>	visemeWeightMap;
	std::vector<SrSnModel*>		dMeshBlend_p;
	bool hasVertexColor;	
	bool hasTexCoord;

	SrModel _emptyModel;
public:
	SBAPI DeformableMesh();
	SBAPI virtual ~DeformableMesh();	
	SBAPI void setSkeleton(SkSkeleton* skel);
	SBAPI virtual void update();
	SkinWeight* getSkinWeight(const std::string& skinSourceName);
	SBAPI int getNumMeshes();
	SBAPI const std::string getMeshName(int index);
	SBAPI SrModel& getStaticModel(int index);
	SBAPI int	getMesh(const std::string& meshName);				// get the position given the mesh name
	int getValidSkinMesh(const std::string& meshName);
    /*! Set the visibility state of the deformable geometry,
        The integers mean 1:show, 0:hide, and -1:don't change the visibility state. */
	void set_visibility(int deformableMesh);
	SBAPI virtual bool buildSkinnedVertexBuffer(); // unrolled all models inside this deformable mesh into a GPU-friendly format
	SBAPI bool isSkinnedMesh();
	SBAPI bool saveToSmb(std::string inputFileName);
	SBAPI bool saveToDmb(std::string inputFileName);
	SBAPI bool readFromSmb(std::string inputFileName);
	SBAPI bool readFromDmb(std::string inputFileName);
	// helper function
	void saveToStaticMeshBinary(SmartBodyBinary::StaticMesh* mesh);
	void readFromStaticMeshBinary(SmartBodyBinary::StaticMesh* mesh);
	void loadAllFoundTextures(std::string textureDirectory);
	SBAPI SrVec computeCenterOfMass();
	SBAPI SrBox computeBoundingBox();

	SBAPI void translate(SrVec trans);
	SBAPI void rotate(SrVec trans);
	SBAPI void scale(float factor);
	SBAPI void centralize();
	SBAPI void computeNormals();
	SBAPI void copySkinWeights(DeformableMesh* fromMesh, const std::string& morphName);

};

class DeformableMeshInstance
{
protected:
	DeformableMesh* _mesh;
	//std::vector<SrSnModel*>	dynamicMesh; 
	SkSkeleton*				_skeleton;
	SmartBody::SBCharacter*	_character;		// pointer to current character
	SmartBody::SBPawn*      _pawn;
	bool				  _updateMesh;
	std::vector<SkJointList> _boneJointList;
	SrVec _meshScale;
	int  meshVisibleType;
	bool _recomputeNormal;
	bool _isStaticMesh;

public:
	std::vector<SrVec> _deformPosBuf;	
	std::vector<SrMat>  transformBuffer;	

	GLuint _tempTex;
	GLuint _tempFBO;

	GLuint * _tempTexPairs;
	GLuint * _tempFBOPairs;

	GLuint * _tempTexWithMask;
	GLuint * _tempFBOTexWithMask;

public:
	SBAPI DeformableMeshInstance();
	SBAPI virtual ~DeformableMeshInstance();
	SBAPI void blendShapeStaticMesh();
	SBAPI virtual void setDeformableMesh(DeformableMesh* mesh);
	SBAPI void updateJointList();
	SBAPI virtual void setPawn(SmartBody::SBPawn* pawn);
	SBAPI virtual void setVisibility(int deformableMesh);
	SBAPI virtual void setMeshScale(SrVec scale);
	SBAPI SrVec   getMeshScale() { return _meshScale; }
	SBAPI int    getVisibility();
	SBAPI void    setToStaticMesh(bool isStatic);
	SBAPI bool    isStaticMesh();
	SBAPI SmartBody::SBSkeleton* getSkeleton();	
	SBAPI virtual void update();
	SBAPI virtual void updateFast();
	SBAPI virtual void GPUblendShapes(glm::mat4x4, glm::mat4x4);
	SBAPI virtual void blendShapes();
	SBAPI DeformableMesh* getDeformableMesh() { return _mesh; }
	SBAPI SmartBody::SBCharacter* getCharacter() { return _character; }
	SBAPI SmartBody::SBPawn* getPawn() { return _pawn; }
	SBAPI void updateTransformBuffer();
	void updateSkin(const std::vector<SrVec>& restPos, std::vector<SrVec>& deformPos);
protected:
	void cleanUp();
};

#endif
