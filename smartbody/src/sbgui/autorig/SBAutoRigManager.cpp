#ifdef WIN32
#define USE_AUTO_RIGGING 1
#endif
#if defined (__linux__) || defined(__APPLE__)
#define USE_AUTO_RIGGING 1
#endif

#include "SBAutoRigManager.h"
#if USE_AUTO_RIGGING
#include "pinocchioApi.h"
#include "lsqSolver.h"
#include "VoxelizerWindow.h"
#endif

#include <vhcl.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sb/SBJoint.h>
#include <sb/SBAssetManager.h>
#include <sbm/sbm_deformable_mesh.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <boost/foreach.hpp>
#include <FL/Fl.H>


#if USE_AUTO_RIGGING
bool SrModelToMesh( SrModel& model, Mesh& mesh, bool sanityCheck = true );
bool AutoRigToSBSk( PinocchioOutput& out, SmartBody::SBSkeleton& sbSk);
bool AutoRigToDeformableMesh(PinocchioOutput& out, SrModel& m, SmartBody::SBSkeleton& sbSk, DeformableMesh& deformMesh);
bool PolyVoxMeshToSrModel( PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh, SrModel& model);
bool PolyVoxMeshToPinoMesh( PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& polyMesh, Mesh& mesh );
void exportPolyVoxMeshToObj( PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh, std::string filename );
void exportCubicMeshToObj( PolyVox::SurfaceMesh<PolyVox::PositionMaterial>& mesh, std::string filename );
void buildVoxelSkinWeights(SrModel& m, SmartBody::SBSkeleton& inSk, VoxelizerWindow& voxelWindow, SkinWeight& skinWeight);
void buildBoneGlowSkinWeights(SrModel& m, SmartBody::SBSkeleton& inSk, VoxelizerWindow& voxelWindow, std::vector<std::map<int,float> >& skinWeight);
void WeightMapToSkinWeight(std::vector<std::map<int,float> >& weightMap, SkinWeight& skinWeight);
#endif

class PinoAutoRigCallback : public PinnocchioCallBack
{
public:
	PinoAutoRigCallback(AutoRigCallBack* callback)
	{
		autoRigCallBack = callback;
	}

	virtual void callbackFunc()
	{
		Fl::check();
	}
	virtual void skeletonCompleteCallBack(std::vector<Vector3>& embedding)
	{
		if (autoRigCallBack)
		{
			SmartBody::SBSkeleton* autoRigSk = new SmartBody::SBSkeleton();
			PinocchioOutput out;
			out.embedding = embedding;
			AutoRigToSBSk(out, *autoRigSk);
			autoRigCallBack->skeletonComplete(autoRigSk);
			Fl::check();
		}
	}


protected:
	AutoRigCallBack* autoRigCallBack;
};

SBAutoRigManager* SBAutoRigManager::_singleton = NULL;
SBAutoRigManager::SBAutoRigManager()
{
	autoRigCallBack = NULL;
}

SBAutoRigManager::~SBAutoRigManager()
{

}


bool SBAutoRigManager::buildAutoRiggingFromPawnMesh( const std::string& pawnName, int riggingType, const std::string& outSkName, const std::string& outDeformableMeshName )
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBPawn* sbPawn = scene->getPawn(pawnName);	
	DeformableMeshInstance* meshInstance = NULL;
	if (sbPawn)
		meshInstance = sbPawn->dStaticMeshInstance_p;
	if (!sbPawn || !meshInstance || meshInstance->getDeformableMesh() == NULL)
	{

		LOG("AutoRigging Fail : No pawn is selected, or the selected pawn does not contain 3D mesh for rigging.");
		return false;
	}
	DeformableMesh* mesh = meshInstance->getDeformableMesh();	
	SrModel& model = mesh->dMeshStatic_p[0]->shape();	

	SrModel scaleModel = SrModel(model);

	

	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	bool autoRigSuccess = false;
	SrMat worldRotation = sbPawn->get_world_offset().get_rotation(); 
	if (!assetManager->getDeformableMesh(outDeformableMeshName))
	{			
		//model.scale(meshInstance->getMeshScale()); // resize the vertices
		SrVec meshScale = meshInstance->getMeshScale();
		for (int i=0;i<scaleModel.V.size();i++)
			scaleModel.V[i] *= meshScale[0];
		for (int i=0;i<scaleModel.V.size();i++)
			scaleModel.V[i] = scaleModel.V[i]*worldRotation;

		if (riggingType == 0)
			autoRigSuccess = buildAutoRiggingVoxels(scaleModel,outSkName,outDeformableMeshName);
		//autoRigSuccess = autoRigManager.buildAutoRiggingVoxelsWithVoxelSkinWeights(scaleModel,skelName,deformMeshName);
		else if (riggingType == 1)
			autoRigSuccess = buildAutoRiggingVoxelsWithVoxelSkinWeights(scaleModel,outSkName,outDeformableMeshName);
		else if (riggingType == 2)
			autoRigSuccess = buildAutoRigging(scaleModel, outSkName, outDeformableMeshName);		
	}
	else
	{
		LOG("Deformable mesh %s already exists. Skip auto-rigging and create the character directly.");
		autoRigSuccess = true;
	}
	return autoRigSuccess;
}

void SBAutoRigManager::transferSkinWeight(SmartBody::SBSkeleton& skel, SrModel& inModel, std::vector<std::map<int,float> >& inWeight, SrModel& outModel, std::vector<std::map<int,float> >& outWeight)
{
	
	vector<Tri3Object> triobjvec;
	for(int i = 0; i < inModel.F.size(); i++) {
		SrVec p1,p2,p3;
		SrModel::Face& F = inModel.F[i];
		p1 = inModel.V[F[0]];
		p2 = inModel.V[F[1]];
		p3 = inModel.V[F[2]];
		Vector3 v1 = Vector3(p1[0],p1[1],p1[2]);
		Vector3 v2 = Vector3(p2[0],p2[1],p2[2]);
		Vector3 v3 = Vector3(p3[0],p3[1],p3[2]);
		triobjvec.push_back(Tri3Object(v1, v2, v3,i));
	}

	ObjectProjector<3, Tri3Object> proj(triobjvec);
	Tri3Object projTri;
	int numBones = skel.getNumJoints();
	outWeight.clear();
	for (unsigned int i=0;i<outModel.V.size();i++)
	{
		std::map<int,float> vtxWeight;
		Vector3 vtxPos = Vector3(outModel.V[i][0],outModel.V[i][1],outModel.V[i][2]);
		Vector3 projPos = proj.projectObj(vtxPos,projTri);	
		Vector3 baryW = barycentricCoord(projPos, projTri);	

		for (int v=0;v<3;v++)
		{
			int vidx = inModel.F[projTri.triIdx][v];
			std::map<int,float>& origWeight = inWeight[vidx];
			std::map<int,float>::iterator mi;
			//for (int k=0;k<origWeight.size();k++)
			for ( mi  = origWeight.begin();
					mi != origWeight.end();
					mi++)
			{	
				if (vtxWeight.find(mi->first) == vtxWeight.end())
					vtxWeight[mi->first] = 0.f;
				vtxWeight[mi->first] += mi->second*baryW[v];		
			}
		}	
		outWeight.push_back(vtxWeight);
	}
}

bool SBAutoRigManager::updateSkinWeightFromCharacterMesh( const std::string& charName, int weightType )
{
	bool computeWeightSuccess = false;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBPawn* sbChar = scene->getPawn(charName);	
	
	DeformableMeshInstance* meshInstance = NULL;
	if (sbChar)
		meshInstance = sbChar->dMeshInstance_p;
	if (!sbChar || !meshInstance || meshInstance->getDeformableMesh() == NULL)
	{

		LOG("Update skin weight fail, the character doesn't have skin mesh.");
		return false;
	}

	DeformableMesh* mesh = meshInstance->getDeformableMesh();	
	SrModel& model = mesh->dMeshStatic_p[0]->shape();	

	std::string jointNameMap[] = {"spine4", "spine3", "spine2", "spine1", "base", "spine5", "skullbase", "head", "r_hip", "r_knee", "r_ankle", "r_forefoot", "r_toe", "l_hip", "l_knee", "l_ankle", "l_forefoot", "l_toe", "r_shoulder", "r_elbow", "r_wrist", "r_hand", "l_shoulder", "l_elbow", "l_wrist", "l_hand" };

	// build voxel mesh
	int voxelSize = 150;
	VoxelizerWindow* voxelWindow = new VoxelizerWindow(0,0,voxelSize,voxelSize,"voxelWindow");
	voxelWindow->initVoxelizer(&model,voxelSize);
	voxelWindow->show();
	while(!voxelWindow->isFinishBuildVoxels())
	{
		Fl::check();
	}
	voxelWindow->hide();
	PolyVox::SimpleVolume<uint8_t>* voxels = voxelWindow->getVoxels();
	voxelWindow->saveVoxels("test.binvox");	

	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> lowResMesh;

	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>* voxelMesh = voxelWindow->getNormalizedVoxelMesh();

#define VOXEL_MESH_SIMPLIFICATION 0

#if VOXEL_MESH_SIMPLIFICATION
	PolyVox::MeshDecimator<PolyVox::PositionMaterialNormal> decimator(voxelMesh,&lowResMesh, 0.95);
	decimator.execute();	
#endif

	Mesh m, origMesh;
	Skeleton sk = SmartBodySkeleton();
	bool isValidModel = PolyVoxMeshToPinoMesh(*voxelMesh,m);
	SrModelToMesh(model,origMesh, false);
	if (!isValidModel) return false; // no auto-rigging if the model is not valid
	SmartBody::SBSkeleton* sbSk = sbChar->getSkeleton();

	SrMat worldOffsetMat = sbChar->get_world_offset();
	sbChar->setWorldOffset(SrMat::id);
	// update skin weight based on voxel mesh
	if (weightType == 1) // bone glow skin weight
	{

		DeformableMesh* deformMesh = meshInstance->getDeformableMesh();
		SkinWeight* sw = deformMesh->skinWeights[0];
		sw->numInfJoints.clear();
		sw->weightIndex.clear();
		sw->bindWeight.clear();
		sw->jointNameIndex.clear();
		
		std::vector<std::map<int,float> > skinWeightMap;
		buildBoneGlowSkinWeights(model, *sbSk, *voxelWindow, skinWeightMap);
		WeightMapToSkinWeight(skinWeightMap,*sw);

		SmartBody::SBSkeleton* sbOrigSk = sbSk;
		for (int k=0;k<sw->infJointName.size();k++)
		{
			// manually add all joint names
			SmartBody::SBJoint* joint = sbOrigSk->getJointByName(sw->infJointName[k]);

			SrMat gmatZeroInv = joint->gmatZero().rigidInverse();						
			sw->bindPoseMat[k] = gmatZeroInv;
		}

	} 
	else
	{
		LOG("m scale = %f, toAdd = %f %f %f", m.scale, m.toAdd[0], m.toAdd[1], m.toAdd[2]);
		// update skinning weights
		std::vector<Vector3> embedPos;
		// hard coded the number of embedding for now		
		sbSk->updateGlobalMatricesZero();		
		embedPos.resize(26);
		for (int i=0;i<26;i++)
		{
			const std::string& jname = jointNameMap[i];
			SmartBody::SBJoint* joint = sbSk->getJointByMappedName(jname);
			if (joint)
			{
				SrVec pos = joint->gmatZero().get_translation();
				for (int k=0;k<3;k++)
					embedPos[i][k] = pos[k]*m.scale+m.toAdd[k];	
				LOG("Sk EmbedPos[%d] = %f %f %f", i, embedPos[i][0],embedPos[i][1],embedPos[i][2]);
			}
		}
		Skeleton sk = SmartBodySkeleton();
		//PinocchioOutput out = autorigVoxelTransfer(sk, m, origMesh);//autoRigSkinWeightOnly(sk, m, origMesh, embedPos);
		PinocchioOutput out = autoRigSkinWeightOnly(sk, m, origMesh, embedPos);


		for (int i=0;i<26;i++)
		{
			Vector3 pos = out.embedding[i];
			LOG("Pino EmbedPos[%d] = %f %f %f", i, pos[0],pos[1],pos[2]);
		}

		int prevJointIdx[] = { 1, 2, 3, 4, -1, 0, 5, 6,  4, 8, 9, 10, 11, 4, 13, 14, 15, 16, 0, 18, 19, 20, 0, 22, 23, 24};
		std::vector<int> boneIdxMap;
		for (int i=0;i< (int) out.embedding.size();i++)
		{
			int boneIdx = prevJointIdx[i];
			if (boneIdx < 0)
				continue;
			else
				boneIdxMap.push_back(boneIdx);
		}

		LOG("transfer skin weights");
		DeformableMesh* deformMesh = meshInstance->getDeformableMesh();
		SkinWeight* sw = deformMesh->skinWeights[0];
		sw->numInfJoints.clear();
		sw->weightIndex.clear();
		sw->bindWeight.clear();
		sw->jointNameIndex.clear();
		for (unsigned int i=0;i< (size_t) model.V.size();i++)
		{
			//LOG("vertex id = %d",i);
			Vector<double, -1> v = out.attachment->getWeights(i);
			//LOG("out.attachment->getWeight() = %d",v.size());
			double maxD = -1.0;
			int maxIdx = -1;
			std::map<int, float> weights;
			for(int j = 0; j < v.size(); ++j) {
				double d = floor(0.5 + v[j] * 10000.) / 10000.;
				if (d > 0.01) // remove small weights
				{
					int boneIdx = boneIdxMap[j];
					if (weights.find(boneIdx) == weights.end())
					{
						weights[boneIdx] = 0.f;
					}
					weights[boneIdx] += (float) d;
				}		
			}
			//LOG("after copying weights");
			std::map<int,float>::iterator mi;
			sw->numInfJoints.push_back(weights.size());
			for ( mi  = weights.begin(); mi != weights.end(); mi++)
			{
				sw->weightIndex.push_back(sw->bindWeight.size());
				sw->bindWeight.push_back(mi->second);
				sw->jointNameIndex.push_back(mi->first);				
			}
			//LOG("after convert weights");
		}
		LOG("after transfer skin weights");
		sw->normalizeWeights();
		
		LOG("after normalize weights");


		SmartBody::SBSkeleton* sbOrigSk = sbSk;
		for (int k=0;k<sw->infJointName.size();k++)
		{
			// manually add all joint names
			SmartBody::SBJoint* joint = sbOrigSk->getJointByName(sw->infJointName[k]);
			
			SrMat gmatZeroInv = joint->gmatZero().rigidInverse();						
			sw->bindPoseMat[k] = gmatZeroInv;
		}
	}

	// reset the world offset matrix
	sbChar->setWorldOffset(worldOffsetMat);


	return computeWeightSuccess;

}


bool SBAutoRigManager::buildAutoRiggingVoxelsWithVoxelSkinWeights( SrModel& inModel, std::string outSkName, std::string outDeformableMeshName )
{
#if USE_AUTO_RIGGING
	int voxelSize = 150;
	VoxelizerWindow* voxelWindow = new VoxelizerWindow(0,0,voxelSize,voxelSize,"voxelWindow");
	voxelWindow->initVoxelizer(&inModel,voxelSize);
	voxelWindow->show();
	while(!voxelWindow->isFinishBuildVoxels())
	{
		Fl::check();
	}
	voxelWindow->hide();
	PolyVox::SimpleVolume<uint8_t>* voxels = voxelWindow->getVoxels();
	voxelWindow->saveVoxels("test.binvox");	

	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> lowResMesh;

	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>* voxelMesh = voxelWindow->getNormalizedVoxelMesh();

#define VOXEL_MESH_SIMPLIFICATION 0

#if VOXEL_MESH_SIMPLIFICATION
	PolyVox::MeshDecimator<PolyVox::PositionMaterialNormal> decimator(voxelMesh,&lowResMesh, 0.95);
	decimator.execute();	
#endif

	//SrModel tempMesh;
	//PolyVoxMeshToSrModel(mesh,tempMesh);
	exportPolyVoxMeshToObj(*voxelMesh,"testVol.obj"); // for debuging only
	//exportPolyVoxMeshToObj(lowResMesh,"testVolLowRes.obj");


	if (autoRigCallBack)
	{
		PolyVox::SurfaceMesh<PolyVox::PositionMaterial>* cubicMesh = voxelWindow->getNormalizedCubicMesh();
		exportCubicMeshToObj(*cubicMesh,"testVolCubic.obj");
		SrModel srCubicMesh; srCubicMesh.import_obj("testVolCubic.obj");
		srCubicMesh.computeNormals(0);
		autoRigCallBack->voxelComplete(srCubicMesh);

		Fl::check();
	}


	Mesh m, origMesh;
	Skeleton sk = SmartBodyNewSkeleton();
	bool isValidModel = PolyVoxMeshToPinoMesh(*voxelMesh,m);
	SrModelToMesh(inModel,origMesh, false);
	if (!isValidModel) return false; // no auto-rigging if the model is not valid


	PinoAutoRigCallback* pinoCallback = new PinoAutoRigCallback(autoRigCallBack);
	PinnocchioCallBackManager::singletonPtr()->setCallBack(pinoCallback);
	PinocchioOutput out = autorigVoxelTransfer(sk,m,origMesh);	
	PinnocchioCallBackManager::singletonPtr()->setCallBack(NULL);

	//PinocchioOutput out = autorig(sk,m);	
	//PinocchioOutput out = autorigVoxelTransfer(sk,m,origMesh, false); // don't compute the skin weights
	LOG("embedding = %d",out.embedding.size());
	if (out.embedding.size() == 0)
		return false;	

	

	for(int i = 0; i < (int)out.embedding.size(); ++i)
		out.embedding[i] = (out.embedding[i] - m.toAdd) / m.scale;
	SmartBody::SBSkeleton* sbSk = new SmartBody::SBSkeleton();
	//sbSk->setName("testAutoRig.sk");
	//sbSk->setFileName()
	bool isValidSkeleton = AutoRigToSBSk(out, *sbSk);	

	// build deformable model using voxel skinning weights
	SbmDeformableMeshGPU* deformMesh = new SbmDeformableMeshGPU();
	deformMesh->setName(outDeformableMeshName);
		
	std::string meshName = (const char*) inModel.name;
	SrSnModel* srSnModelDynamic = new SrSnModel();
	SrSnModel* srSnModelStatic = new SrSnModel();
	srSnModelDynamic->shape(inModel);
	srSnModelStatic->shape(inModel);
	srSnModelDynamic->changed(true);
	srSnModelDynamic->visible(false);
	srSnModelStatic->shape().name = meshName.c_str();
	srSnModelDynamic->shape().name = meshName.c_str();	
	deformMesh->dMeshStatic_p.push_back(srSnModelStatic);
	deformMesh->dMeshDynamic_p.push_back(srSnModelDynamic);
	srSnModelDynamic->ref();
	srSnModelStatic->ref();

	LOG("Build Voxel Skin Weights");
	SkinWeight* sw = new SkinWeight();
	sw->sourceMesh = meshName;	
	// build voxel skin weights
	//buildVoxelSkinWeights(inModel, *sbSk, *voxelWindow, *sw);
	SrModel voxelSrModel;
	PolyVoxMeshToSrModel(*voxelMesh, voxelSrModel);
	std::vector<std::map<int,float> > skinWeightMap;
#if 0
	buildBoneGlowSkinWeights(inModel, *sbSk, *voxelWindow, skinWeightMap);
#else
	std::vector<std::map<int,float> > voxelWeightMap;
	buildBoneGlowSkinWeights(voxelSrModel, *sbSk, *voxelWindow, voxelWeightMap);
	transferSkinWeight(*sbSk, voxelSrModel, voxelWeightMap, inModel, skinWeightMap);
#endif

	WeightMapToSkinWeight(skinWeightMap, *sw);
	deformMesh->skinWeights.push_back(sw);
	LOG("after normalize weights");


	SmartBody::SBSkeleton* sbOrigSk = sbSk;
	for (int k=0;k<sbOrigSk->getNumJoints();k++)
	{
		// manually add all joint names
		SmartBody::SBJoint* joint = sbOrigSk->getJoint(k);
		sw->infJointName.push_back(joint->getName());
		sw->infJoint.push_back(joint);
		SrMat gmatZeroInv = joint->gmatZero().rigidInverse();						
		sw->bindPoseMat.push_back(gmatZeroInv);
	}

	//bool isValidDeformableMesh = AutoRigToDeformableMesh(out, inModel, *sbSk, *deformMesh);
		
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	sbSk->ref();
	sbSk->skfilename(outSkName.c_str());				
	sbSk->setName(outSkName.c_str());
	deformMesh->skeletonName = outSkName;

	assetManager->addSkeleton(sbSk);
	assetManager->addDeformableMesh(outDeformableMeshName, deformMesh);

	if (autoRigCallBack)
	{
		deformMesh->buildVertexBufferGPU();
		autoRigCallBack->skinComplete(assetManager->getDeformableMesh(outDeformableMeshName));
		Fl::check();
	}

	delete voxelWindow;
	return true;	
#else
	return false;
#endif

}



bool SBAutoRigManager::buildAutoRiggingVoxels( SrModel& inModel, std::string outSkName, std::string outDeformableMeshName )
{
	
#if USE_AUTO_RIGGING
	inModel.computeNormals();
	int voxelSize = 250;
	VoxelizerWindow* voxelWindow = new VoxelizerWindow(0,0,voxelSize,voxelSize,"voxelWindow");
	voxelWindow->initVoxelizer(&inModel,voxelSize);
	voxelWindow->show();
	while(!voxelWindow->isFinishBuildVoxels())
	{
		voxelWindow->draw();
		Fl::check();
	}
	voxelWindow->hide();
	PolyVox::SimpleVolume<uint8_t>* voxels = voxelWindow->getVoxels();
	voxelWindow->saveVoxels("test.binvox");	

	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> lowResMesh;

	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>* voxelMesh = voxelWindow->getNormalizedVoxelMesh();
	
#define VOXEL_MESH_SIMPLIFICATION 0

#if VOXEL_MESH_SIMPLIFICATION
	PolyVox::MeshDecimator<PolyVox::PositionMaterialNormal> decimator(voxelMesh,&lowResMesh, 0.95);
	decimator.execute();	
#endif

	//SrModel tempMesh;
	//PolyVoxMeshToSrModel(mesh,tempMesh);
	exportPolyVoxMeshToObj(*voxelMesh,"testVol.obj");

	

	if (autoRigCallBack)
	{
		PolyVox::SurfaceMesh<PolyVox::PositionMaterial>* cubicMesh = voxelWindow->getNormalizedCubicMesh();
		exportCubicMeshToObj(*cubicMesh,"testVolCubic.obj");
		SrModel srCubicMesh; srCubicMesh.import_obj("testVolCubic.obj");
		srCubicMesh.computeNormals(0);
		autoRigCallBack->voxelComplete(srCubicMesh);
		
		Fl::check();
	}
	
	
	//exportPolyVoxMeshToObj(lowResMesh,"testVolLowRes.obj");

	Mesh m, origMesh;
	Skeleton sk = SmartBodyNewSkeleton();
	bool isValidModel = PolyVoxMeshToPinoMesh(*voxelMesh,m);
	SrModelToMesh(inModel,origMesh, false);
	if (!isValidModel) return false; // no auto-rigging if the model is not valid
	
	//PinocchioOutput out = autorig(sk,m);	

	PinoAutoRigCallback* pinoCallback = new PinoAutoRigCallback(autoRigCallBack);
	PinnocchioCallBackManager::singletonPtr()->setCallBack(pinoCallback);
	PinocchioOutput out = autorigVoxelTransfer(sk,m,origMesh);	
	PinnocchioCallBackManager::singletonPtr()->setCallBack(NULL);

	//LOG("embedding = %d",out.embedding.size());
	if (out.embedding.size() == 0)
		return false;	

	for(int i = 0; i < (int)out.embedding.size(); ++i)
		out.embedding[i] = (out.embedding[i] - m.toAdd) / m.scale;
	SmartBody::SBSkeleton* sbSk = new SmartBody::SBSkeleton();
	//sbSk->setName("testAutoRig.sk");
	//sbSk->setFileName()
	bool isValidSkeleton = AutoRigToSBSk(out, *sbSk);	
	SbmDeformableMeshGPU* deformMesh = new SbmDeformableMeshGPU();
	deformMesh->setName(outDeformableMeshName);
	bool isValidDeformableMesh = AutoRigToDeformableMesh(out, inModel, *sbSk, *deformMesh);
	

	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	sbSk->ref();
	sbSk->skfilename(outSkName.c_str());				
	sbSk->setName(outSkName.c_str());
	deformMesh->skeletonName = outSkName;

	assetManager->addSkeleton(sbSk);
	assetManager->addDeformableMesh(outDeformableMeshName, deformMesh);

	//deformMesh->buildSkinnedVertexBuffer();
	if (autoRigCallBack)
	{
		deformMesh->buildVertexBufferGPU();
		autoRigCallBack->skinComplete(assetManager->getDeformableMesh(outDeformableMeshName));
		Fl::check();
	}

	
	delete voxelWindow;
	return true;	
#endif
	return false;
}

bool SBAutoRigManager::buildAutoRigging( SrModel& inModel, std::string outSkName, std::string outDeformableMeshName )
{
#if USE_AUTO_RIGGING
	Mesh m;
	//Skeleton sk = HumanSkeleton(); // default human skeleton from Pinocchio. Should define our own custom skeleton to account for gaze and other behavior
	Skeleton sk = SmartBodyNewSkeleton();
	inModel.computeNormals();
	bool isValidModel = SrModelToMesh(inModel,m);
	if (!isValidModel) return false; // no auto-rigging if the model is not valid
	PinocchioOutput out = autorig(sk,m);	
	if (out.embedding.size() == 0)
		return false; // no embedding

	for(int i = 0; i < (int)out.embedding.size(); ++i)
		out.embedding[i] = (out.embedding[i] - m.toAdd) / m.scale;
	SmartBody::SBSkeleton* sbSk = new SmartBody::SBSkeleton();
	//sbSk->setName("testAutoRig.sk");
	//sbSk->setFileName()
	bool isValidSkeleton = AutoRigToSBSk(out, *sbSk);	
	//LOG("autoRig result, num joints = %d",out.embedding.size());
	//LOG("AutoRig Skeleton = %s", sbSk->saveToString().c_str());
	SbmDeformableMeshGPU* deformMesh = new SbmDeformableMeshGPU();
	deformMesh->setName(outDeformableMeshName);
	bool isValidDeformableMesh = AutoRigToDeformableMesh(out, inModel, *sbSk, *deformMesh);

	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();

	sbSk->ref();
	sbSk->skfilename(outSkName.c_str());				
	sbSk->setName(outSkName.c_str());
	deformMesh->skeletonName = outSkName;

	assetManager->addSkeleton(sbSk);
	assetManager->addDeformableMesh(outDeformableMeshName, deformMesh);
	return true;
#endif
	return false;
}


#if USE_AUTO_RIGGING

typedef PolyVox::SimpleVolume<uint8_t> IntVolume; 
typedef PolyVox::SimpleVolume<float> FloatVolume;
typedef std::map<SrVec3i,float> DistMap;

bool operator<(const SrVec3i& left, const SrVec3i& right) {
	if (left.data[0] < right.data[0]) { return true; }
	if (left.data[0] > right.data[0]) { return false; }

	if (left.data[1] < right.data[1]) { return true; }
	if (left.data[1] > right.data[1]) { return false; }

	return left.data[2] < right.data[2];
}

// compute the voxel distance 
void buildVoxelDistanceMap(SmartBody::SBJoint* joint, VoxelizerWindow& voxelWindow, FloatVolume& distVol, bool centerOnly)
{
	float maxDist = FLT_MAX;
	PolyVox::SimpleVolume<uint8_t>* tagVol = voxelWindow.getVoxels();
	clearVoxels(&distVol, (float)maxDist); // clear all distance to maximum
	std::vector<SrVec3i> allBoneLineSeg;
	SrVec pos1 = joint->getMatrixGlobal().get_translation();
	if (!centerOnly)
	{
		for (int i=0;i<joint->getNumChildren();i++)
		{		
			SrVec pos2 = joint->getChild(i)->getMatrixGlobal().get_translation();
			std::vector<SrVec3i> boneLineSeg = voxelWindow.rasterizeVoxelLine(pos1,pos2);
			allBoneLineSeg.insert(allBoneLineSeg.end(), boneLineSeg.begin(),boneLineSeg.end());
		}
	}
	else
	{
		if (joint->getNumChildren() > 1)
		{			
			SrVec3i jointVoxID = voxelWindow.getVoxelIDFromPosition(pos1);
			allBoneLineSeg.push_back(jointVoxID);
		}
		else // has only one child, take the center of line segment
		{
			SrVec pos2 = (joint->getChild(0)->getMatrixGlobal().get_translation() + pos1)*0.5f;
			SrVec3i jointVoxID = voxelWindow.getVoxelIDFromPosition(pos1);
			allBoneLineSeg.push_back(jointVoxID);			
		}
	}
	
	
	LOG("joint %s, bone voxels = %d", joint->getName().c_str(), allBoneLineSeg.size());
	std::queue<SrVec3i> voxelQueue;
	for (unsigned int i=0;i<allBoneLineSeg.size();i++)
	{
		SrVec3i pos = allBoneLineSeg[i];
		distVol.setVoxelAt(PolyVox::Vector3DInt32(pos[0],pos[1],pos[2]), 0.f);
		voxelQueue.push(pos);
	}
	
	// flood fill the voxel with distances
	while (!voxelQueue.empty())
	{
		SrVec3i cv = voxelQueue.front();
		voxelQueue.pop();
		std::vector<SrVec3i> neighborCells = voxelWindow.getNeighborCells(cv);
		float cvDist = distVol.getVoxelAt(cv[0],cv[1],cv[2]);
		SrVec cvCenter = voxelWindow.getVoxelCenterByID(cv);
		
		for (unsigned int i=0;i<neighborCells.size();i++)
		{
			SrVec3i nv = neighborCells[i];
			uint8_t tag = tagVol->getVoxelAt(nv[0],nv[1],nv[2]);
			float neighborCurDist = distVol.getVoxelAt(nv[0],nv[1],nv[2]);
			SrVec nvCenter = voxelWindow.getVoxelCenterByID(nv);
			//LOG("cv = %d %d %d, nv = %d %d %d", cv[0],cv[1],cv[2], nv[0], nv[1], nv[2]);
			//LOG("cvCenter = %f %f %f, nvCenter = %f %f %f",cvCenter[0],cvCenter[1],cvCenter[2],nvCenter[0],nvCenter[1],nvCenter[2]);
			float neighborNewDist = cvDist + (nvCenter - cvCenter).norm();
			//LOG("curDist = %f, newDist = %f", neighborCurDist, neighborNewDist);
			if (tag != 0 && neighborNewDist < neighborCurDist) // propagate the cell
			{
				distVol.setVoxelAt(nv[0],nv[1],nv[2], neighborNewDist);
				voxelQueue.push(nv);
			}
		}
	}

}


void normalizeBoneWeights( std::vector<std::map<int,float> > &vtxBoneWeights )
{
	std::map<int,float>::iterator mi;
	// normalized weight first
	for (size_t i=0;i<vtxBoneWeights.size();i++)
	{
		std::map<int,float>& vtxWeight = vtxBoneWeights[i];
		float weightSum = 0.f;
		for ( mi  = vtxWeight.begin();
			mi != vtxWeight.end();
			mi++)
		{
			weightSum += mi->second;
		}
		for ( mi  = vtxWeight.begin();
			mi != vtxWeight.end();
			mi++)
		{
			mi->second /= weightSum;
		}
	}
}

void boneWeightInPainting(std::vector<std::map<int,float> >& vtxBoneWeights, std::vector<bool>& badBoneVtxList, SrModel& m)
{
	if (vtxBoneWeights.size() == 0) return;
	int nv = m.V.size();
	std::vector<std::set<int> > vtxNeighbors;
	std::vector<float> vtxAreas;
	std::vector<int> boneIdxs;
	std::map<int,int> validVtxMap;
	std::map<int,float>& boneWeight = vtxBoneWeights[0];
	
	
	//normalizeBoneWeights(vtxBoneWeights);

	std::map<int,float>::iterator bi;
	for ( bi  = boneWeight.begin();
		  bi != boneWeight.end();
		  bi++)
	{
		boneIdxs.push_back(bi->first);
	}


	vtxNeighbors.resize(nv);
	vtxAreas.resize(nv, 0.f);
	// add all vertex neighbors into the list
	for (int i=0;i<m.F.size();i++)
	{
		SrModel::Face& f = m.F[i];
		float faceArea = m.face_area(i);
		if (badBoneVtxList[f[0]] || badBoneVtxList[f[1]] || badBoneVtxList[f[2]])
		{
			for (int j=0;j<3;j++)
			{
				vtxNeighbors[f[j]].insert(f[(j+1)%3]);
				vtxNeighbors[f[j]].insert(f[(j+2)%3]);
				vtxAreas[f[j]] += faceArea;
			}
			for (int j=0;j<3;j++)
			{
				if (validVtxMap.find(f[j]) == validVtxMap.end())
				{
					int validIdx = validVtxMap.size();
					validVtxMap[f[j]] = validIdx;
				}
			}
		}		
	}
	std::vector<std::map<int,float> > vtxNeighborWeights;
	vtxNeighborWeights.resize(nv);
	std::map<int,int>::iterator vi;
	for ( vi  = validVtxMap.begin();
		  vi != validVtxMap.end();
		  vi++)
	{
		float weightSum = 0.f;
		int vidx = vi->first;
		std::set<int>& neighbor = vtxNeighbors[vidx];	
		std::map<int,float>& mapNeighbor = vtxNeighborWeights[vidx];		
		std::set<int>::iterator ni;
		SrVec vPos = m.V[vidx];
		for ( ni  = neighbor.begin();
			  ni != neighbor.end();
			  ni++)
		{
			int nidx = *ni;
			SrVec adjPos = m.V[nidx];
			float vtxWeight = 1.f/((vPos-adjPos).norm()+1e-10f);
			mapNeighbor[nidx] = vtxWeight;
			weightSum += vtxWeight;
		}
		std::map<int,float>::iterator mi;
		for (  mi  = mapNeighbor.begin();
			   mi != mapNeighbor.end();
			   mi++)
		{
			mi->second /= weightSum;
		}
	}

	float constraintWeight = 1.f;	
	vector<vector<pair<int, double> > > A(validVtxMap.size());
	vector<double> D(validVtxMap.size(), 0.), H(validVtxMap.size(), 0.);
	for ( vi  = validVtxMap.begin();
		  vi != validVtxMap.end();
		  vi++)
	{	
		//get laplacian
		int vidx = vi->first;
		int vrow = vi->second;
		double sum = 0.0;				
		std::map<int,float>& mapNeighbor = vtxNeighborWeights[vidx];	
		std::map<int,float>::iterator mi;
		for (  mi  = mapNeighbor.begin();
			   mi != mapNeighbor.end();
			   mi++)
		{		
			sum += mi->second;
			int ncol = validVtxMap[mi->first];

			if(ncol > vrow) //check for triangular here because sum should be computed regardless
				continue;
			A[vrow].push_back(make_pair(ncol, -mi->second));
		}

		A[vrow].push_back(make_pair(vrow, sum + constraintWeight));
		sort(A[vrow].begin(), A[vrow].end());
	}

	
	SPDMatrix Am(A);
	LLTMatrix *Ainv = Am.factor();
	if(Ainv == NULL)
		return;

	for(size_t j = 0; j < boneIdxs.size(); ++j) {
		vector<double> rhs(validVtxMap.size(), 0.);		
		int boneIdx = boneIdxs[j];
		for ( vi  = validVtxMap.begin();
			  vi != validVtxMap.end();
			  vi++)
		{
			int vidx = vi->first;
			int ncol = vi->second;
			if (vtxBoneWeights[vidx][boneIdx] > 0.0f)
				rhs[ncol] = vtxBoneWeights[vidx][boneIdx]*constraintWeight;//*vtxAreas[i];			 
			else
			{
				//LOG("bone = %d, v = %d, rhs = 0", boneIdx, i);
				rhs[ncol] = 0.f;
			}
		}
		Ainv->solve(rhs);           
		for ( vi  = validVtxMap.begin();
			  vi != validVtxMap.end();
			  vi++)
		{
			int vidx = vi->first;
			int ncol = vi->second;
			//if (rhs[i] > 1.f)
			//	rhs[i] = 1.f;
			vtxBoneWeights[vidx][boneIdx] = (float)rhs[ncol];			
		}
	}

// 	for(i = 0; i < nv; ++i) {
// 		double sum = 0.;
// 		for(j = 0; j < (int)nzweights[i].size(); ++j)
// 			sum += nzweights[i][j].second;
// 
// 		for(j = 0; j < (int)nzweights[i].size(); ++j) {
// 			nzweights[i][j].second /= sum;
// 			weights[i][nzweights[i][j].first] = nzweights[i][j].second;
// 		}
// 	}

	delete Ainv;

}

void boneWeightLaplacianSmoothing(std::vector<std::map<int,float> >& vtxBoneWeights, std::vector<float>& vtxCloestDist, SrModel& m)
{
	if (vtxBoneWeights.size() == 0) return;
	int nv = m.V.size();	
	std::vector<std::set<int> > vtxNeighbors;	
	std::map<int,float>& boneWeight = vtxBoneWeights[0];
	std::vector<int> boneIdxs;
	std::map<int,float>::iterator mi;
	for ( mi  = boneWeight.begin();
		 mi != boneWeight.end();
		 mi++)
	{
		boneIdxs.push_back(mi->first);
	}

	//normalizeBoneWeights(vtxBoneWeights);

	vtxNeighbors.resize(nv);
	//vtxAreas.resize(nv, 0.f);
	// add all vertex neighbors into the list
	for (int i=0;i<m.F.size();i++)
	{
		SrModel::Face& f = m.F[i];
		float faceArea = m.face_area(i);
		for (int j=0;j<3;j++)
		{
			vtxNeighbors[f[j]].insert(f[(j+1)%3]);
			vtxNeighbors[f[j]].insert(f[(j+2)%3]);
			//vtxAreas[f[j]] += faceArea;
		}
	}

	std::vector<std::map<int,float> > vtxNeighborWeights;
	vtxNeighborWeights.resize(nv);
	for (int i = 0; i < nv; i++)
	{
		float weightSum = 0.f;
		std::set<int>& neighbor = vtxNeighbors[i];	
		std::map<int,float>& mapNeighbor = vtxNeighborWeights[i];		
		std::set<int>::iterator ni;
		SrVec vPos = m.V[i];
		for ( ni  = neighbor.begin();
			ni != neighbor.end();
			ni++)
		{
			int nidx = *ni;
			SrVec adjPos = m.V[nidx];
			float vtxWeight = 1.f/((vPos-adjPos).norm()+1e-10f);
			mapNeighbor[nidx] = vtxWeight;
			weightSum += vtxWeight;
		}

		std::map<int,float>::iterator mi;
		for (  mi  = mapNeighbor.begin();
			mi != mapNeighbor.end();
			mi++)
		{
			mi->second /= weightSum;
		}
	}

	std::vector<float> tempBoneWeights(nv,0.f);
	int smoothIteration = 20;
	float smoothAlpha = 0.9f;
	for (int iter=0;iter<smoothIteration;iter++)
	{
		LOG("Smooth Iteration = %d",iter);
		for (size_t ibone = 0; ibone < boneIdxs.size(); ibone++)
		{
			int boneIdx = boneIdxs[ibone];
			for (unsigned int i=0;i<tempBoneWeights.size();i++)
			{
				tempBoneWeights[i] = vtxBoneWeights[i][boneIdx]*(1.f-smoothAlpha);
				std::map<int,float>& mapNeighbor = vtxNeighborWeights[i];
				std::map<int,float>::iterator mi;
				for ( mi  = mapNeighbor.begin();
					  mi != mapNeighbor.end();
					  mi++)
				{
					tempBoneWeights[i] += vtxBoneWeights[mi->first][boneIdx]*mi->second*smoothAlpha;
				}
			}

			for (unsigned int i=0;i<tempBoneWeights.size();i++) // copy over the bone weights
			{
				vtxBoneWeights[i][boneIdx] = tempBoneWeights[i];
			}
		}
		//normalizeBoneWeights(vtxBoneWeights);
	}
}

// smoothly propagate weights on the mesh
void boneWeightHarmonicSmoothing(std::vector<std::map<int,float> >& vtxBoneWeights, std::vector<float>& vtxCloestDist, SrModel& m)
{
	//return;
	if (vtxBoneWeights.size() == 0) return;
	int nv = m.V.size();
	std::vector<std::set<int> > vtxNeighbors;
	std::vector<float> vtxAreas;
	std::vector<int> boneIdxs;
	std::map<int,float>& boneWeight = vtxBoneWeights[0];
	std::map<int,float>::iterator mi;
	for ( mi  = boneWeight.begin();
		  mi != boneWeight.end();
		  mi++)
	{
		boneIdxs.push_back(mi->first);
	}

	for (int i=0;i<nv;i++)
	{
		std::map<int,float>& vtxWeight = vtxBoneWeights[i];
		float weightSum = 0.f;
		for ( mi  = boneWeight.begin();
			  mi != boneWeight.end();
			  mi++)
		{
			weightSum += mi->second;
		}

		for ( mi  = boneWeight.begin();
			  mi != boneWeight.end();
			  mi++)
		{
			mi->second /= weightSum;
		}
	}


	vtxNeighbors.resize(nv);
	vtxAreas.resize(nv, 0.f);
	// add all vertex neighbors into the list
	for (int i=0;i<m.F.size();i++)
	{
		SrModel::Face& f = m.F[i];
		float faceArea = m.face_area(i);
		for (int j=0;j<3;j++)
		{
			vtxNeighbors[f[j]].insert(f[(j+1)%3]);
			vtxNeighbors[f[j]].insert(f[(j+2)%3]);
			vtxAreas[f[j]] += faceArea;
		}
	}
	std::vector<std::map<int,float> > vtxNeighborWeights;
	vtxNeighborWeights.resize(nv);
	for (int i = 0; i < nv; i++)
	{
		float weightSum = 0.f;
		std::set<int>& neighbor = vtxNeighbors[i];	
		std::map<int,float>& mapNeighbor = vtxNeighborWeights[i];		
		std::set<int>::iterator ni;
		SrVec vPos = m.V[i];
		for ( ni  = neighbor.begin();
			  ni != neighbor.end();
			  ni++)
		{
			int nidx = *ni;
			SrVec adjPos = m.V[nidx];
			float vtxWeight = 1.f/((vPos-adjPos).norm()+1e-10f);
			mapNeighbor[nidx] = vtxWeight;
			weightSum += vtxWeight;
		}

		std::map<int,float>::iterator mi;
		for (  mi  = mapNeighbor.begin();
			   mi != mapNeighbor.end();
			   mi++)
		{
			mi->second /= weightSum;
		}
	}

	float constraintWeight = 0.8f;
		
	vector<vector<pair<int, double> > > A(nv);
	vector<double> D(nv, 0.), H(nv, 0.);
	vector<int> closest(nv, -1);
	for(int i = 0; i < nv; ++i) {		
		//get laplacian
		double sum = 0.;		
		H[i] = 1.f/(1e-10+vtxCloestDist[i]*vtxCloestDist[i]);
		std::map<int,float>& mapNeighbor = vtxNeighborWeights[i];	
		std::map<int,float>::iterator mi;
		for (  mi  = mapNeighbor.begin();
			mi != mapNeighbor.end();
			mi++)
		{		
			sum += mi->second;
			if(mi->first > i) //check for triangular here because sum should be computed regardless
				continue;
			A[i].push_back(make_pair(mi->first, -mi->second));
		}

		A[i].push_back(make_pair(i, sum + constraintWeight));
		sort(A[i].begin(), A[i].end());
	}

	
	SPDMatrix Am(A);
	LLTMatrix *Ainv = Am.factor();
	if(Ainv == NULL)
		return;

	for(size_t j = 0; j < boneIdxs.size(); ++j) {
		vector<double> rhs(nv, 0.);		
		int boneIdx = boneIdxs[j];
		for(int i = 0; i < nv; ++i) {
			if (vtxBoneWeights[i][boneIdx] > 0.0f)
				rhs[i] = vtxBoneWeights[i][boneIdx]*constraintWeight;//*vtxAreas[i];			 
			else
			{
				//LOG("bone = %d, v = %d, rhs = 0", boneIdx, i);
				rhs[i] = 0.f;
			}
		}
		Ainv->solve(rhs);           
		for(int i = 0; i < nv; ++i) {
			//if (rhs[i] > 1.f)
			//	rhs[i] = 1.f;
			 vtxBoneWeights[i][boneIdx] = (float)rhs[i];			
		}
	}

// 	for(i = 0; i < nv; ++i) {
// 		double sum = 0.;
// 		for(j = 0; j < (int)nzweights[i].size(); ++j)
// 			sum += nzweights[i][j].second;
// 
// 		for(j = 0; j < (int)nzweights[i].size(); ++j) {
// 			nzweights[i][j].second /= sum;
// 			weights[i][nzweights[i][j].first] = nzweights[i][j].second;
// 		}
// 	}

	delete Ainv;
}

void buildHarmonicSkinWeights(SrModel& m, SmartBody::SBSkeleton& inSk, VoxelizerWindow& voxelWindow, SkinWeight& skinWeigh)
{

}

void WeightMapToSkinWeight(std::vector<std::map<int,float> >& weightMap, SkinWeight& skinWeight)
{
	for (unsigned int i=0;i< weightMap.size();i++)
	{		
		std::map<int, float>& weights = weightMap[i];
		std::map<int,float>::iterator mi;
		skinWeight.numInfJoints.push_back(weights.size());
		for ( mi  = weights.begin(); mi != weights.end(); mi++)
		{
			skinWeight.weightIndex.push_back(skinWeight.bindWeight.size());
			skinWeight.bindWeight.push_back(mi->second);
			skinWeight.jointNameIndex.push_back(mi->first);				
		}
		//LOG("after convert weights");
	}
	//LOG("after transfer skin weights");
	skinWeight.normalizeWeights();

}

void buildBoneGlowSkinWeights(SrModel& m, SmartBody::SBSkeleton& inSk, VoxelizerWindow& voxelWindow, std::vector<std::map<int,float> >& skinWeight)
{
	IntVolume* origVoxels = voxelWindow.getVoxels();
	// store the voxel map for each vertex		
	std::map<SrVec3i, std::vector<int> > voxelVtxMap;
	std::map<SrVec3i, std::pair<int,float> > voxClosestBoneMap;
	std::map<SrVec3i, std::pair<int,float> > vox2ndClosestBoneMap;

	m.computeNormals();
	for (int i=0;i<m.V.size();i++)
	{
		SrVec vPos = m.V[i];
		SrVec3i voxID = voxelWindow.getVoxelIDFromPosition(vPos);
		origVoxels->setVoxelAt(PolyVox::Vector3DInt32(voxID[0],voxID[1],voxID[2]),100);
		//vtxVoxelSet.insert(voxID);
		if (voxelVtxMap.find(voxID) == voxelVtxMap.end())
		{
			voxelVtxMap[voxID] = std::vector<int>();
			voxClosestBoneMap[voxID] = std::pair<int,float>(-1,1e30f);
			voxClosestBoneMap[voxID] = std::pair<int,float>(-1,1e30f);
		}
		voxelVtxMap[voxID].push_back(i);		
	}
	std::map<int, std::vector<SrVec3i> > boneVoxelMap;
	std::map<int, std::vector<SrVec3i> > allBoneVoxelMap;
	for (int i=0;i<inSk.getNumJoints();i++) // compute distance map for all joints
	{		
		SmartBody::SBJoint* joint = inSk.getJoint(i);	
		if (joint->getParent() != NULL)
		{
			SrVec pos1 = joint->getMatrixGlobal().get_translation();
			SrVec pos2 = joint->getParent()->getMatrixGlobal().get_translation();
			int parentIdx = joint->getParent()->getIndex();
			if ((pos2-pos1).norm() > 0.f) // if this is a bone with enough length
			{
				std::vector<SrVec3i> boneLineSeg = voxelWindow.rasterizeVoxelLine(pos1,pos2);
				boneVoxelMap[i] = boneLineSeg;
				if (allBoneVoxelMap.find(parentIdx) == allBoneVoxelMap.end())
					allBoneVoxelMap[parentIdx] = std::vector<SrVec3i>();
				std::vector<SrVec3i>& allBoneLineSeg = allBoneVoxelMap[parentIdx];
				allBoneLineSeg.insert(allBoneLineSeg.end(),boneLineSeg.begin(),boneLineSeg.end());
			}			
		}
	}

	std::map<SrVec3i, std::vector<int> >::iterator voxIter;
	std::map<int, DistMap> distanceMapList;		
	std::map<int, DistMap> curvatureMapList;	
	FloatVolume distVoxels(origVoxels->getEnclosingRegion());
	FloatVolume curvatureVoxels(origVoxels->getEnclosingRegion());
	float maxDist = 0.f;
#if 1
	for (int i=0;i<inSk.getNumJoints();i++) // compute distance map for all joints
	{		
		SmartBody::SBJoint* joint = inSk.getJoint(i);
		if (joint->getNumChildren() > 0)
		{
			distanceMapList[joint->getIndex()] = DistMap();
			curvatureMapList[joint->getIndex()] = DistMap();
			DistMap& boneDistMap = distanceMapList[joint->getIndex()];
			DistMap& curvDistMap = curvatureMapList[joint->getIndex()];
			buildVoxelDistanceMap(joint,voxelWindow, distVoxels, false);		
			//buildVoxelDistanceMap(joint,voxelWindow, curvatureVoxels, true);

			for ( voxIter  = voxelVtxMap.begin();
				  voxIter != voxelVtxMap.end();
				  voxIter++)
			{	
				SrVec3i vid = voxIter->first;
				float voxDist = distVoxels.getVoxelAt(vid.data[0],vid.data[1],vid.data[2]);
				//float curvatureDist = curvatureVoxels.getVoxelAt(vid.data[0],vid.data[1],vid.data[2]);
				boneDistMap[vid] = voxDist;	
				//curvDistMap[vid] = curvatureDist;
				if (voxClosestBoneMap[vid].second > voxDist) // compute the closest bone for each vertex voxel
				{
					voxClosestBoneMap[vid] = std::pair<int,float>(joint->getIndex(), voxDist);					
				}								
			}
		}
	}
#endif

#if 1
	std::map<SrVec3i,bool> badVoxelMap;
	std::vector<float> vtxClosestBoneDist;
	vtxClosestBoneDist.resize(m.V.size());
	for ( voxIter  = voxelVtxMap.begin();
		  voxIter != voxelVtxMap.end();
		  voxIter++)
	{
		SrVec3i voxID = voxIter->first;
		SrVec voxelCenter = voxelWindow.getVoxelCenterByID(voxID);
		int cloestBone = voxClosestBoneMap[voxID].first;		
		std::vector<int>& vtxIdx = voxIter->second;
		float voxClosestDist = voxClosestBoneMap[voxID].second;
		for (unsigned int v=0;v<vtxIdx.size();v++)
		{
			SrVec vPos = m.V[vtxIdx[v]];
			float voxDeltaDist = (vPos - voxelCenter).norm();
			vtxClosestBoneDist[vtxIdx[v]] = voxDeltaDist + voxClosestDist;
		}



		std::vector<SrVec3i>& allBoneLineSeg = allBoneVoxelMap[cloestBone];
		float visibleCount = 0.f;
		for (unsigned int b=0;b<allBoneLineSeg.size();b++)
		{
			if (voxelWindow.checkVoxelLineVisible(voxID, allBoneLineSeg[b]))
			{	
				visibleCount += 1.f;
			}
		}
		float closestBoneVisibility = visibleCount/allBoneLineSeg.size();
		if (closestBoneVisibility < 0.6f)
		{
			badVoxelMap[voxID] = true;
		}
		else
		{
			badVoxelMap[voxID] = false;
		}
	}
#endif

	float alpha = 1.0f;
	
	std::vector<bool> badBoneVtxList(m.V.size(), false);
	std::vector<std::map<int,float> > vtxBoneWeights;
	vtxBoneWeights.resize(m.V.size());
	// iterate through all voxels that contain vertices
	for ( voxIter  = voxelVtxMap.begin();
		  voxIter != voxelVtxMap.end();
		  voxIter++)
	{
		SrVec3i voxID = voxIter->first;
		SrVec voxelCenter = voxelWindow.getVoxelCenterByID(voxID);
		std::vector<int>& vtxIdx = voxIter->second;
		std::map<int, std::vector<SrVec3i> >::iterator boneIter;
		for (unsigned int i=0;i<vtxIdx.size();i++)
		{
			vtxBoneWeights[vtxIdx[i]] = std::map<int,float>();
		}

				
		if (badVoxelMap[voxID] == true) // bad voxel, use geodesic distance to compute bone weights ( or just set it to zero, and use heat map ? )
		{		
#if 0
			std::map<int,float> voxelBoneCurvatures;
			std::map<int, DistMap>::iterator curIterator;
			for ( curIterator  = curvatureMapList.begin();
				  curIterator != curvatureMapList.end();
				  curIterator++)
			{
				int jointIdx = curIterator->first;
				SmartBody::SBJoint* joint = inSk.getJoint(jointIdx);
				DistMap& curvDistMap = curIterator->second;
				SrVec jointPos;
				if (joint->getNumChildren() > 1)
				{
					jointPos = joint->getMatrixGlobal().get_translation();
				}
				else
				{
					jointPos = (joint->getChild(0)->getMatrixGlobal().get_translation() + joint->getMatrixGlobal().get_translation())*0.5f;
				}
				float euDist = (jointPos - voxelCenter).norm();

				float curvDist = curvDistMap[voxID];
				float curvature = curvDist/euDist;
				voxelBoneCurvatures[jointIdx] = curvature;
			}
#endif
			
		
			for (unsigned int v=0;v<vtxIdx.size();v++)
			{
				int vIdx = vtxIdx[v];
				badBoneVtxList[vIdx] = true;
				SrVec vPos = m.V[vIdx];				
				float voxDeltaDist = (vPos - voxelCenter).norm();
				std::map<int,float>& weight = vtxBoneWeights[vIdx];	
				std::map<int, DistMap>::iterator distIter;
				for ( distIter  = distanceMapList.begin();
					  distIter != distanceMapList.end();
					  distIter++)
				{	
					int jointIdx = distIter->first;
					DistMap& boneDistMap = distIter->second;
					if (boneDistMap.find(voxID) != boneDistMap.end())
					{
						float d_j = (boneDistMap[voxID] + voxDeltaDist);//extentD;		
						//float curv_j = voxelBoneCurvatures[jointIdx];
						//float w_j = 1.f/(d_j*d_j*pow(curv_j,3.f));//1.f/((1.f-alpha)*d_j + alpha*d_j*d_j);					
						float w_j = 1.f/(d_j*(1-alpha) + d_j*d_j*alpha);
						weight[jointIdx] = 0.f;//w_j; //w_j*w_j;
						//LOG("Unnormalized weight, v = %d, bone = %d, d_j = %f, w = %f", i, jointIdx, d_j, weights[jointIdx]);
					}
					else
					{
						LOG("Error, can not find bone dist map");
						// error, this should not even happen !!
					}
				}
			}
		}
		else // 
		{
			
			std::map<int, DistMap>::iterator distIter;
			std::map<float,int> jointDistanceOrder;
			for ( distIter  = distanceMapList.begin();
				  distIter != distanceMapList.end();
				  distIter++)
			{	
				int jointIdx = distIter->first;
				DistMap& boneDistMap = distIter->second;
				jointDistanceOrder[boneDistMap[voxID]] = jointIdx;
			}


			for (boneIter  = boneVoxelMap.begin();
				 boneIter != boneVoxelMap.end();
				 boneIter++)
			{
				int boneID = boneIter->first;

				SmartBody::SBJoint* joint = inSk.getJoint(boneID);
				int infJointIdx = joint->getParent()->getIndex(); // the bone segment actually corresponds to its parent joint
				SrVec boneDir = joint->getMatrixGlobal().get_translation() - joint->getParent()->getMatrixGlobal().get_translation();
				boneDir.normalize(); // get bone direction

				std::vector<SrVec3i>& boneLine = boneIter->second;
				std::vector<SrVec> visibleBone;
				int biCount = 0;
				bool useBone = false;
				std::map<float,int>::iterator bi;
				for ( bi  = jointDistanceOrder.begin();
					  bi != jointDistanceOrder.end();
					  bi++)
				{
					if (biCount >= 3)
						break;

					if (bi->second == infJointIdx)
						useBone = true;
					biCount++;
				}
						

#if 0
				SmartBody::SBJoint* joint = inSk.getJoint(boneID);
				int infJointIdx = joint->getParent()->getIndex(); // the bone segment actually corresponds to its parent joint		
				float voxelWeight = 0.f;
				for (unsigned int b=0;b<boneLine.size();b++)
				{
					float euDist = voxelWindow.getVoxelEuclideanDist(voxID, boneLine[b]);
					float geoDist = voxelWindow.getVoxelGeodesicDist(voxID, boneLine[b]);
					float curvature = geoDist/euDist;
					float ratio = pow(curvature,3);
					float boneSampleWeight = 1.f/(ratio*(geoDist*geoDist));
					voxelWeight += boneSampleWeight;
				}
				
				for (unsigned int v=0;v<vtxIdx.size();v++)
				{
					int vIdx = vtxIdx[v];
					std::map<int,float>& weight = vtxBoneWeights[vIdx];
					if (weight.find(infJointIdx) == weight.end())
						weight[infJointIdx] = 0.f;
					weight[infJointIdx] += voxelWeight;					
				}
#endif

#if 1
				//if (!useBone) // not using this bone
				//	continue;
				// check the bone visibility with the current voxel			
				for (unsigned int b=0;b<boneLine.size();b++)
				{
					if (voxelWindow.checkVoxelLineVisible(voxID, boneLine[b]))
					{	
						SrVec bonePos = voxelWindow.getVoxelCenterByID(boneLine[b]);
						visibleBone.push_back(bonePos);
					}
				}
				
				for (unsigned int v=0;v<vtxIdx.size();v++)
				{
					int vIdx = vtxIdx[v];
					std::map<int,float>& weight = vtxBoneWeights[vIdx];
					SrVec& vPos = m.V[vIdx];
					SrVec& vN = m.N[vIdx];				
					if (weight.find(infJointIdx) == weight.end())
						weight[infJointIdx] = 0.f;
					for (unsigned int b=0;b<visibleBone.size();b++)
					{
						SrVec vDir = vPos - visibleBone[b];
						SrVec vUnitDir = vDir; vUnitDir.normalize();
						float vLen = vDir.norm2();
						float Rv = (dot(vUnitDir,vN))/vLen;
						float Tv = cross(vUnitDir,boneDir).norm();
						weight[infJointIdx] += (Rv*Tv);
					}
				}
#endif
			}
		}		
	}	

	LOG("In-Painting Skin Weights");
	//boneWeightSmoothing(vtxBoneWeights, vtxClosestBoneDist, m);
	boneWeightInPainting(vtxBoneWeights, badBoneVtxList, m);
	LOG("Smooth Skin Weights");
	boneWeightLaplacianSmoothing(vtxBoneWeights, vtxClosestBoneDist, m);
	LOG("Finish Smooth Skin Weights");
	skinWeight = vtxBoneWeights;
	
}

void buildVoxelSkinWeights(SrModel& m, SmartBody::SBSkeleton& inSk, VoxelizerWindow& voxelWindow, SkinWeight& skinWeight)
{
	// copy the original voxel shapes
	
	IntVolume* origVoxels = voxelWindow.getVoxels();
	// set all voxels with vertex values	
	std::set<SrVec3i> vtxVoxelSet;
	for (int i=0;i<m.V.size();i++)
	{
		SrVec vPos = m.V[i];
		SrVec3i voxID = voxelWindow.getVoxelIDFromPosition(vPos);
		origVoxels->setVoxelAt(PolyVox::Vector3DInt32(voxID[0],voxID[1],voxID[2]),100);
		vtxVoxelSet.insert(voxID);
	}
	
	// compute distance 
	//std::vector<DistMap> distanceMapList;
	std::map<int, DistMap> distanceMapList;
	FloatVolume distVoxels(origVoxels->getEnclosingRegion());
	float maxDist = 0.f;
	for (int i=0;i<inSk.getNumJoints();i++) // compute distance map for all joints
	{		
		SmartBody::SBJoint* joint = inSk.getJoint(i);
		if (joint->getNumChildren() > 0)
		{
			distanceMapList[joint->getIndex()] = DistMap();
			DistMap& boneDistMap = distanceMapList[joint->getIndex()];
			buildVoxelDistanceMap(joint,voxelWindow, distVoxels, false);
			BOOST_FOREACH(const SrVec3i& vid, vtxVoxelSet)
			{	
				float voxDist = distVoxels.getVoxelAt(vid.data[0],vid.data[1],vid.data[2]);
				boneDistMap[vid] = voxDist;
				if (voxDist > maxDist)
					maxDist = voxDist;
			}
		}
	}

	SrBox bbox;
	m.get_bounding_box(bbox);
	float bboxExtents = bbox.getSize().norm();
	float extentD = bboxExtents*0.01f + maxDist; // increase the maximum distance a little bit to guarantee the normalized distance will fall between epsilon and 1
	float alpha = 0.1f;
	// compute the final skin weights	
	for (unsigned int i=0;i<(size_t) m.V.size();i++)
	{	
		std::map<int, DistMap>::iterator distIter;
		std::map<int, float> weights;
		SrVec vPos = m.V[i];
		SrVec3i vtxVoxID = voxelWindow.getVoxelIDFromPosition(vPos);
		float voxDeltaDist = (vPos - voxelWindow.getVoxelCenterByID(vtxVoxID)).norm();
		for ( distIter  = distanceMapList.begin();
			  distIter != distanceMapList.end();
			  distIter++)
		{	
			int jointIdx = distIter->first;
			DistMap& boneDistMap = distIter->second;
			if (boneDistMap.find(vtxVoxID) != boneDistMap.end())
			{
				float d_j = (boneDistMap[vtxVoxID] + voxDeltaDist)/extentD;
				float w_j = 1.f/((1.f-alpha)*d_j + alpha*d_j*d_j);
				weights[jointIdx] =  w_j*w_j;
				//LOG("Unnormalized weight, v = %d, bone = %d, d_j = %f, w = %f", i, jointIdx, d_j, weights[jointIdx]);
			}
			else
			{
				LOG("Error, can not find bone dist map");
				// error, this should not even happen !!
			}
		}

		std::map<int,float>::iterator mi;
		skinWeight.numInfJoints.push_back(weights.size());
		for ( mi  = weights.begin(); mi != weights.end(); mi++)
		{
			skinWeight.weightIndex.push_back(skinWeight.bindWeight.size());
			skinWeight.bindWeight.push_back(mi->second);
			skinWeight.jointNameIndex.push_back(mi->first);				
		}
		//LOG("after convert weights");
	}
	LOG("after transfer skin weights");
	skinWeight.normalizeWeights();
}

bool AutoRigToDeformableMesh( PinocchioOutput& out, SrModel& m, SmartBody::SBSkeleton& sbSk, DeformableMesh& deformMesh )
{
	//deformMesh.dMeshStatic_p.push_back()
	// setup mesh
	std::string meshName = (const char*) m.name;
	SrSnModel* srSnModelDynamic = new SrSnModel();
	SrSnModel* srSnModelStatic = new SrSnModel();
	srSnModelDynamic->shape(m);
	srSnModelStatic->shape(m);
	srSnModelDynamic->changed(true);
	srSnModelDynamic->visible(false);
	srSnModelStatic->shape().name = meshName.c_str();
	srSnModelDynamic->shape().name = meshName.c_str();	
	deformMesh.dMeshStatic_p.push_back(srSnModelStatic);
	deformMesh.dMeshDynamic_p.push_back(srSnModelDynamic);
	srSnModelDynamic->ref();
	srSnModelStatic->ref();

	// setup skin weights
	//int boneIdxMap[] = { 1, 2, 0, 2, 4, 5, 6, 2, 8, 9, 10, 0, 12, 13, 0, 15, 16  }; // hard coded for HumanSkeleton for now
	//int boneIdxMap[] = { 1, 2, 3, 4, 0, 5, 4, 7, 8, 9, 4, 11, 12, 13, 0, 15, 16, 0, 18, 19};
	//int boneIdxMap[] = {1, 2, 3}
	//int prevJointIdx[] = { 1, 2, 3, 4, -1, 0, 5, 6,  4, 8, 9, 10, 11, 4, 13, 14, 15, 16, 0, 18, 19, 20, 0, 22, 23, 24};
	int prevJointIdx[] = { 1, 2, 3, -1, 0, 4, 5, 6,  3, 8, 9, 10, 11, 3, 13, 14, 15, 16, 0, 18, 19, 20, 0, 22, 23, 24};

	std::vector<int> boneIdxMap;
	for (int i=0;i< (int) out.embedding.size();i++)
	{
		int boneIdx = prevJointIdx[i];
		if (boneIdx < 0)
			continue;
		else
			boneIdxMap.push_back(boneIdx);
	}

	LOG("transfer skin weights");
	
	SkinWeight* sw = new SkinWeight();
	sw->sourceMesh = meshName;	
	for (unsigned int i=0;i< (size_t) m.V.size();i++)
	{
		//LOG("vertex id = %d",i);
		Vector<double, -1> v = out.attachment->getWeights(i);
		//LOG("out.attachment->getWeight() = %d",v.size());
		double maxD = -1.0;
		int maxIdx = -1;
		std::map<int, float> weights;
		for(int j = 0; j < v.size(); ++j) {
			double d = floor(0.5 + v[j] * 10000.) / 10000.;
			if (d > 0.01) // remove small weights
			{
				int boneIdx = boneIdxMap[j];
				if (weights.find(boneIdx) == weights.end())
				{
					weights[boneIdx] = 0.f;
				}
				weights[boneIdx] += (float) d;
			}		
		}
		//LOG("after copying weights");
		std::map<int,float>::iterator mi;
		sw->numInfJoints.push_back(weights.size());
		for ( mi  = weights.begin(); mi != weights.end(); mi++)
		{
			sw->weightIndex.push_back(sw->bindWeight.size());
			sw->bindWeight.push_back(mi->second);
			sw->jointNameIndex.push_back(mi->first);				
		}
		//LOG("after convert weights");
	}
	LOG("after transfer skin weights");
	sw->normalizeWeights();
	deformMesh.skinWeights.push_back(sw);
	LOG("after normalize weights");

	
	SmartBody::SBSkeleton* sbOrigSk = &sbSk;
	for (int k=0;k<sbOrigSk->getNumJoints();k++)
	{
		// manually add all joint names
		SmartBody::SBJoint* joint = sbOrigSk->getJoint(k);
		sw->infJointName.push_back(joint->getName());
		sw->infJoint.push_back(joint);
		SrMat gmatZeroInv = joint->gmatZero().rigidInverse();						
		sw->bindPoseMat.push_back(gmatZeroInv);
	}
	
	return true;
}

bool AutoRigToSBSk( PinocchioOutput& out, SmartBody::SBSkeleton& sbSk)
{
	//std::string jointNameMap[] = {"spine3","spine1", "base", "skullbase", "r_hip", "r_knee", "r_ankle", "r_forefoot", "l_hip", "l_knee", "l_ankle", "l_forefoot", "r_shoulder", "r_elbow", "r_wrist", "l_shoulder", "l_elbow", "l_wrist" };
	//int prevJointIdx[] = { 1, 2, -1, 0, 2, 4, 5, 6, 2, 8, 9, 10, 0, 12, 13, 0, 15, 16};

	// SmartBody skeleton
	//std::string jointNameMap[] = {"spine4", "spine3", "spine2", "spine1", "base", "spine5", "skullbase", "head", "r_hip", "r_knee", "r_ankle", "r_forefoot", "r_toe", "l_hip", "l_knee", "l_ankle", "l_forefoot", "l_toe", "r_shoulder", "r_elbow", "r_wrist", "r_hand", "l_shoulder", "l_elbow", "l_wrist", "l_hand" };
	//int prevJointIdx[] = { 1, 2, 3, 4, -1, 0, 5, 6,  4, 8, 9, 10, 11, 4, 13, 14, 15, 16, 0, 18, 19, 20, 0, 22, 23, 24};

	// new smartbody skeleton
	std::string jointNameMap[] = {"spine3", "spine2", "spine1",  "base", "spine4", "spine5", "skullbase", "head", "r_hip", "r_knee", "r_ankle", "r_forefoot", "r_toe", "l_hip", "l_knee", "l_ankle", "l_forefoot", "l_toe", "r_shoulder", "r_elbow", "r_wrist", "r_hand", "l_shoulder", "l_elbow", "l_wrist", "l_hand" };
	int prevJointIdx[] = { 1, 2, 3, -1, 0, 4, 5, 6,  3, 8, 9, 10, 11, 3, 13, 14, 15, 16, 0, 18, 19, 20, 0, 22, 23, 24};


	// build all joints
	for(int i = 0; i < (int)out.embedding.size(); ++i) // number of joints
	{	
		SmartBody::SBJoint* joint = dynamic_cast<SmartBody::SBJoint*>(sbSk.add_joint(SkJoint::TypeQuat, -1));
		joint->quat()->activate();
		joint->name(jointNameMap[i]);
		joint->extName(jointNameMap[i]);
		joint->extID(jointNameMap[i]);
		joint->extSID(jointNameMap[i]);

		sbSk.channels().add(joint->jointName(), SkChannel::XPos);
		sbSk.channels().add(joint->jointName(), SkChannel::YPos);
		sbSk.channels().add(joint->jointName(), SkChannel::ZPos);
		joint->pos()->limits(SkVecLimits::X, false);
		joint->pos()->limits(SkVecLimits::Y, false);
		joint->pos()->limits(SkVecLimits::Z, false);
		sbSk.channels().add(joint->jointName(), SkChannel::Quat);
		joint->quat()->activate();

		float pos[3];
		for (int j=0;j<3;j++)
		{
			pos[j] = (float) out.embedding[i][j];
			if (prevJointIdx[i] != -1)
			{
				pos[j] -= (float) out.embedding[prevJointIdx[i]][j];
			}
		}	
		joint->setOffset(SrVec(pos[0],pos[1],pos[2]));
	}
	// setup joint hierarchy
	for (unsigned int i=0;i< (size_t) sbSk.getNumJoints();i++)
	{
		SmartBody::SBJoint* joint = sbSk.getJoint(i);
		int parentIdx = prevJointIdx[i];
		if (parentIdx != -1)
		{
			//joint->setParent(sbSk.getJoint(parentIdx));
			sbSk.getJoint(parentIdx)->add_child(joint);
		}
		else // root joint
		{
			sbSk.root(joint);
		}
	}
	LOG("sbSk, num of joints = %d",sbSk.getNumJoints());
	sbSk.updateGlobalMatricesZero();

	return true;
}


bool PolyVoxMeshToPinoMesh( PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& polyMesh, Mesh& mesh )
{
	mesh.vertices.resize(polyMesh.m_vecVertices.size());
	for (size_t i=0;i<polyMesh.m_vecVertices.size();i++)
	{
		PolyVox::PositionMaterialNormal& vtx = polyMesh.m_vecVertices[i];
		mesh.vertices[i].pos = Vector3(vtx.position.getX(),vtx.position.getY(),vtx.position.getZ());
	}

	mesh.edges.resize(polyMesh.getIndices().size());
	for (size_t i=0;i<polyMesh.getIndices().size();i++)
	{
		mesh.edges[i].vertex = polyMesh.m_vecTriangleIndices[i];		
	}

	mesh.fixDupFaces();
	mesh.computeTopology();
	if(!mesh.integrityCheck())
	{		
		//Debugging::out() << "Successfully read " << file << ": " << vertices.size() << " vertices, " << edges.size() << " edges" << endl;
		LOG("Error : SBAutoRigManager::SrModelToMesh Fail.");
		return false;
	}	
	mesh.normalizeBoundingBox();
	mesh.computeVertexNormals();

	return true;
}


bool SrModelToMesh( SrModel& model, Mesh& mesh, bool sanityCheck )
{
	mesh.vertices.resize(model.V.size());
	for (int i=0;i<model.V.size();i++)
	{
		SrPnt& p = model.V[i];
		mesh.vertices[i].pos = Vector3(p.x,p.y,p.z);
	}

	mesh.edges.resize(model.F.size()*3);
	for (int i=0;i<model.F.size();i++)
	{
		SrModel::Face& f = model.F[i];
		mesh.edges[i*3].vertex = f.a;
		mesh.edges[i*3+1].vertex = f.b;
		mesh.edges[i*3+2].vertex = f.c;
	}

	if (sanityCheck)
	{
		mesh.fixDupFaces();
		mesh.computeTopology();
	}	
	if(sanityCheck && !mesh.integrityCheck())
	{		
		//Debugging::out() << "Successfully read " << file << ": " << vertices.size() << " vertices, " << edges.size() << " edges" << endl;
		LOG("Error : SBAutoRigManager::SrModelToMesh Fail.");
		return false;
	}

	mesh.normalizeBoundingBox();
	mesh.computeVertexNormals();

	return true;
}

bool PolyVoxMeshToSrModel( PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh, SrModel& model )
{
	int nVtx = mesh.getNoOfVertices();
	int nTri = mesh.getNoOfIndices()/3;	
	model.V.size(mesh.getNoOfVertices());
	model.F.size(mesh.getNoOfIndices()/3);
	float x,y,z;
	for (int i=0;i<nVtx;i++)
	{
		PolyVox::PositionMaterialNormal& vtx = mesh.m_vecVertices[i];
		x = vtx.position.getX();
		y = vtx.position.getY();
		z = vtx.position.getZ();
		model.V[i] = SrPnt(x,y,z);		
	}
	for (int i=0;i<nTri;i++)
	{
		model.F[i].a = mesh.m_vecTriangleIndices[i*3];
		model.F[i].b = mesh.m_vecTriangleIndices[i*3+1];
		model.F[i].c = mesh.m_vecTriangleIndices[i*3+2];
	}
	model.computeNormals();
	return true;
}

void exportPolyVoxMeshToObj( PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& mesh, std::string filename )
{
	FILE* fp = fopen(filename.c_str(),"wt");
	int nVtx = mesh.getNoOfVertices();
	int nTri = mesh.getNoOfIndices()/3;	

	float x,y,z;
	for (int i=0;i<nVtx;i++)
	{
		PolyVox::PositionMaterialNormal& vtx = mesh.m_vecVertices[i];
		x = vtx.position.getX();
		y = vtx.position.getY();
		z = vtx.position.getZ();
		fprintf(fp,"v %f %f %f\n",x,y,z);
	}

	int a,b,c;
	for (int i=0;i<nTri;i++)
	{
		a = mesh.m_vecTriangleIndices[i*3]+1;
		b = mesh.m_vecTriangleIndices[i*3+1]+1;
		c = mesh.m_vecTriangleIndices[i*3+2]+1;
		fprintf(fp,"f %d %d %d\n",a,b,c);
	}

	fclose(fp);
}

void exportCubicMeshToObj( PolyVox::SurfaceMesh<PolyVox::PositionMaterial>& mesh, std::string filename )
{
	FILE* fp = fopen(filename.c_str(),"wt");
	int nVtx = mesh.getNoOfVertices();
	int nTri = mesh.getNoOfIndices()/3;	

	float x,y,z;
	for (int i=0;i<nVtx;i++)
	{
		PolyVox::PositionMaterial& vtx = mesh.m_vecVertices[i];
		x = vtx.position.getX();
		y = vtx.position.getY();
		z = vtx.position.getZ();
		fprintf(fp,"v %f %f %f\n",x,y,z);
	}

	int a,b,c;
	for (int i=0;i<nTri;i++)
	{
		a = mesh.m_vecTriangleIndices[i*3]+1;
		b = mesh.m_vecTriangleIndices[i*3+1]+1;
		c = mesh.m_vecTriangleIndices[i*3+2]+1;
		fprintf(fp,"f %d %d %d\n",a,b,c);
	}

	fclose(fp);
}

SBAutoRigManager* getAutoRigManager()
{
	return SBAutoRigManager::singletonPtr();
}
#endif

