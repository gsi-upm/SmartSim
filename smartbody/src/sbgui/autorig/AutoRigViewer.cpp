#include "vhcl.h"
#include "AutoRigViewer.h"
#include "jointmapviewer/RetargetStepWindow.h"
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBAssetManager.h>
#include <sb/SBJointMapManager.h>
#include <sb/SBJointMap.h>
#include <sb/SBBehaviorSetManager.h>
#include <sb/SBBehaviorSet.h>
#include <autorig/SBAutoRigManager.h>
#include <sbm/sbm_deformable_mesh.h>
#include <sk/sk_joint.h>
#include <sr/sr_gl_render_funcs.h>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Input.H>

#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/glut.H>
#include <FL/fl_ask.H>
#include <sr/sr_gl.h>
#include <sr/sr_gl_render_funcs.h>
#include <sr/sr_sphere.h>
#include <sr/sr_sn_shape.h>
#include <sbm/gwiz_math.h>

#include <boost/filesystem.hpp>

#ifndef WIN32
#define _strdup strdup
#endif


AutoRigViewer::AutoRigViewer(int x, int y, int w, int h, char* name) : Fl_Double_Window(x, y, w, h, name)
{	
	begin();	
	int curY = 10;
	int startY = 10;	
	int yDis = 10;
	yDis += 30;
	_choiceVoxelRigging = new Fl_Choice(60, yDis, 120, 25, "Type");
	_choiceVoxelRigging->add("Voxel Weight");
	_choiceVoxelRigging->add("Glow Weight");
	_choiceVoxelRigging->add("Diffusion Weight");
	_choiceVoxelRigging->value(0);

	yDis += 45;
	_buttonAutoRig = new Fl_Button(60, yDis, 120, 25, "Apply AutoRig");
	_buttonAutoRig->callback(ApplyAutoRigCB, this);
	
	modelViewer = new ModelViewer(250, startY, 300, h-startY - 50, "Voxel");	

	skinViewer  = new SkinViewer(650, startY, 300, h-startY-50, "Skin");

	end();
	_deletePawnName = "";
	_characterName = "";
	retargetStepWindow = NULL;
}


void AutoRigViewer::skinComplete( DeformableMesh* defMesh )
{
	if (skinViewer)
		skinViewer->setDeformableMesh(defMesh);
	Fl::check();
}

void AutoRigViewer::skeletonComplete( SmartBody::SBSkeleton* sk )
{
	if (skinViewer)
		skinViewer->setSkeleton(sk);
	Fl::check();
}


void AutoRigViewer::voxelComplete(SrModel& voxelModel)
{
	if (modelViewer)
	{
		modelViewer->setModel(voxelModel);
		modelViewer->redraw();
	}
}

AutoRigViewer::~AutoRigViewer()
{
	
}

void AutoRigViewer::draw()
{
	if (skinViewer)
		skinViewer->redraw();

	if (modelViewer)
		modelViewer->redraw();
	Fl_Double_Window::draw();
}


void AutoRigViewer::updateAutoRigViewer()
{
	redraw();
}

void AutoRigViewer::setCharacterName(const std::string& name)
{
	_characterName = name;
	SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(name);
	if (pawn)
	{
		DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
		if (meshInstance)
		{
			if (meshInstance->getDeformableMesh()->getNumMeshes() > 0)
			{
				this->modelViewer->setModel(meshInstance->getDeformableMesh()->getStaticModel(0));
			}
		}
		
	}
	updateAutoRigViewer();
}

void AutoRigViewer::applyAutoRig( int riggingType /*= 0*/ )
{
	if (_characterName == "")
		return;

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	SBAutoRigManager& autoRigManager = SBAutoRigManager::singleton();
	autoRigManager.setAutoRigCallBack(this);

	SmartBody::SBPawn* sbPawn = scene->getPawn(_characterName);	
	DeformableMeshInstance* meshInstance = sbPawn->dStaticMeshInstance_p;
	if (!sbPawn || !meshInstance || meshInstance->getDeformableMesh() == NULL)
	{

		LOG("AutoRigging Fail : No pawn is selected, or the selected pawn does not contain 3D mesh for rigging.");
		return;
	}
	DeformableMesh* mesh = meshInstance->getDeformableMesh();	

	std::string modelName = mesh->getName();//(const char*) model.name;
	std::string filebasename = boost::filesystem::basename(modelName);
	std::string fileextension = boost::filesystem::extension(modelName);
	std::string skelName = filebasename+".sk";
	std::string deformMeshName = filebasename+"AutoRig.dae"; 
	LOG("Start Build Auto Rigging");
	bool autoRigSuccess = autoRigManager.buildAutoRiggingFromPawnMesh(_characterName, riggingType, skelName, deformMeshName);
	LOG("Auto Rigging Done");
#if 0

	SrModel& model = mesh->dMeshStatic_p[0]->shape();		
	SrModel scaleModel = SrModel(model);
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	bool autoRigSuccess = false;
	SrMat worldRotation = sbPawn->get_world_offset().get_rotation(); 
	if (!assetManager->getDeformableMesh(deformMeshName))
	{			
		//model.scale(meshInstance->getMeshScale()); // resize the vertices
		float meshScale = meshInstance->getMeshScale();
		for (int i=0;i<scaleModel.V.size();i++)
			scaleModel.V[i] *= meshScale;
		for (int i=0;i<scaleModel.V.size();i++)
			scaleModel.V[i] = scaleModel.V[i]*worldRotation;

		if (riggingType == 0)
			autoRigSuccess = autoRigManager.buildAutoRiggingVoxels(scaleModel,skelName,deformMeshName);
		//autoRigSuccess = autoRigManager.buildAutoRiggingVoxelsWithVoxelSkinWeights(scaleModel,skelName,deformMeshName);
		else if (riggingType == 1)
			autoRigSuccess = autoRigManager.buildAutoRiggingVoxelsWithVoxelSkinWeights(scaleModel,skelName,deformMeshName);
		else if (riggingType == 2)
			autoRigSuccess = autoRigManager.buildAutoRigging(scaleModel, skelName, deformMeshName);		
	}
	else
	{
		LOG("Deformable mesh %s already exists. Skip auto-rigging and create the character directly.");
	}
#endif

	if (!autoRigSuccess && riggingType == 2)		
	{
		std::string errorMsg = "AutoRigging Fail : The input mesh must be a single component and water tight mesh. Try to enable 'voxelRigging'.";
		LOG(errorMsg.c_str());
		fl_alert(errorMsg.c_str());
		return;
	}		

	std::string charName = sbPawn->getName()+"autoRig";

	SmartBody::SBJointMapManager* jointMapManager = scene->getJointMapManager();
	SmartBody::SBJointMap* jointMap = jointMapManager->getJointMap(skelName);
	if (!jointMap)
	{
		jointMap = jointMapManager->createJointMap(skelName);
		jointMap->guessMapping(scene->getSkeleton(skelName), false);
	}

	SmartBody::SBSkeleton* skel = scene->createSkeleton(skelName);

	SmartBody::SBCharacter* character = scene->createCharacter(charName, "");
	character->setSkeleton(skel);
	character->createStandardControllers();
	character->setStringAttribute("deformableMesh",deformMeshName);	

	SrVec dest = sbPawn->getPosition();
	float yOffset = -skel->getBoundingBox().a.y;
	dest.y = yOffset;		
	character->setPosition(SrVec(dest.x,dest.y,dest.z));
	character->setStringAttribute("displayType","GPUmesh");


	// setup behavior set
	SmartBody::SBBehaviorSetManager* manager = scene->getBehaviorSetManager();
	if (manager->getNumBehaviorSets() == 0)
	{
		// look for the behavior set directory under the media path
		scene->addAssetPath("script", "behaviorsets");
		scene->runScript("default-behavior-sets.py");

		if (manager->getNumBehaviorSets() == 0)
		{
			LOG("Can not find any behavior sets under path %s/behaviorsets.", scene->getMediaPath().c_str());
		}
		else
		{
			LOG("Found %d behavior sets under path %s/behaviorsets", manager->getNumBehaviorSets(), scene->getMediaPath().c_str());
		}
	}
#define TEST_ROCKETBOX 1
#if TEST_ROCKETBOX
	scene->addAssetPath("script", "scripts");
	scene->run("scene.run('characterUnitTest.py')");

	character->createActionAttribute("_1testHead", true, "TestHead", 300, false, false, false, "Test Head");
	character->createActionAttribute("_2testGaze", true, "TestHead", 300, false, false, false, "Test Head");
	character->createActionAttribute("_3testGesture", true, "TestHead", 300, false, false, false, "Test Head");
	character->createActionAttribute("_4testReach", true, "TestHead", 300, false, false, false, "Test Head");
	character->createActionAttribute("_5testLocomotion", true, "TestHead", 300, false, false, false, "Test Head");
#endif	
	//updateCharacterList();	
	scene->removePawn(_characterName);	

	
	if (retargetStepWindow)
	{
		retargetStepWindow->refreshAll();		
		retargetStepWindow->setApplyType(true);

		retargetStepWindow->setCharacterName(charName);
		retargetStepWindow->setJointMapName(skelName);	
	}
	
}

void AutoRigViewer::ApplyAutoRigCB( Fl_Widget* widget, void* data )
{
	AutoRigViewer* viewer = (AutoRigViewer*) data;
	int riggingType = viewer->_choiceVoxelRigging->value();
	viewer->applyAutoRig(riggingType);
}


void ModelViewer::updateFancyLights()
{
	SrLight light;		

	light.directional = true;
	light.diffuse = SrColor( 0.4f, 0.4f, 0.4f );
	light.specular = SrColor( 0.5f, 0.5f, 0.5f );
	light.position = SrVec( 100.0, 100.0, 500.0 );
	//	light.constant_attenuation = 1.0f/cam.scale;
	light.constant_attenuation = 1.0f;
	lights.push_back(light);

	light.directional = true;
	light.diffuse = SrColor( 0.9f, 0.5f, 0.5f );
	light.specular = SrColor( 0.5f, 0.5f, 0.5f );
	light.position = SrVec( 900.0, -150.0, 150.0 );
	//	light.constant_attenuation = 1.0f/cam.scale;
	light.constant_attenuation = 1.0f;
	lights.push_back(light);

	SrLight light2 = light;
	light2.directional = true;
	light2.diffuse = SrColor( 0.5f, 0.9f, 0.9f );
	light.specular = SrColor( 0.5f, 0.5f, 0.5f );
	light2.position = SrVec( -900.0, 100.0, 100.0 );
	//	light2.constant_attenuation = 1.0f;
	//	light2.linear_attenuation = 2.0f;
	lights.push_back(light2);
}

ModelViewer::ModelViewer( int x, int y, int w, int h, char* name ) : MouseViewer(x,y,w,h,name)
{
	_model = NULL;
	updateFancyLights();
}

ModelViewer::~ModelViewer()
{
	if (_model)
		delete _model;
}


void ModelViewer::setModel( SrModel& model )
{
	if (_model)
		delete _model;
	_model = new SrSnModel();
	_model->shape(model);
	_model->render_mode(srRenderModeFlat);
	focusOnModel();
}

void ModelViewer::draw()
{
	

	make_current();

	if (!visible()) 
		return;
	if (!valid()) 
	{
		init_opengl();
		valid(1);
	}

	preRender();

	glDisable(GL_BLEND);
	if (_model) 
	{
		SrGlRenderFuncs::render_model(_model);
	}
}

void ModelViewer::focusOnModel()
{
	//sceneBox = skeleton->getBoundingBox();	
	if (!_model) return;
	SrBox modelBox;
	_model->shape().get_bounding_box(modelBox);

	cam.view_all(modelBox, cam.getFov());	
	float scale = modelBox.getSize().norm();
	float znear = 0.01f*scale;
	float zfar = 100.0f*scale;
	cam.setNearPlane(znear);
	cam.setFarPlane(zfar);
}

/************************************************************************/
/* Skin Viewer                                                          */
/************************************************************************/
SkinViewer::SkinViewer(int x, int y, int w, int h, char* name) : MouseViewer(x,y,w,h,name)
{
	skeleton = NULL;
	skeletonScene = NULL;
	mesh = NULL;
	updateLights();
}
SkinViewer::~SkinViewer()
{

}
void SkinViewer::setSkeleton(SmartBody::SBSkeleton* sk)
{
	skeleton = sk;
	if (!skeleton) return;	
	if (skeletonScene)
	{
		delete skeletonScene;
		skeletonScene = NULL;
	}
	skeletonScene = new SkScene();
	skeletonScene->ref();
	skeletonScene->init(skeleton);
	skeletonScene->set_visibility(true,false,false,false);
	focusOnSkeleton();
	redraw();
}
void SkinViewer::setDeformableMesh(DeformableMesh* defMesh)
{
	if (defMesh)
		LOG("SkinViewer::setDeformableMesh = %s", defMesh->getName().c_str());
	else
		LOG("SkinViewer::setDeformableMesh to NULL.");
	mesh = defMesh;
	redraw();
}

void SkinViewer::drawSkinWeight()
{

	// Original code

	if (!mesh)
	{
		//LOG("Render SkinWeight ERR: no deformable mesh found!");
		return; // no deformable mesh
	}
	//SrGlRenderFuncs::render_model(mesh->dMeshStatic_p[0]);
	// draw skin weight visualization
	std::vector<SrVec>& V = mesh->posBuf;
	std::vector<SrVec3i>& F = mesh->triBuf;
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glBegin ( GL_TRIANGLES ); // some cards do require begin/end for each triangle!
	for (int k=0; k<F.size(); k++ )
	{	
		int f = k;
		SrVec c1,c2,c3;
		c1 = mesh->skinColorBuf[F[f][0]];
		c2 = mesh->skinColorBuf[F[f][1]];
		c3 = mesh->skinColorBuf[F[f][2]];
		glColor3f(c1[0],c1[1],c1[2]);  glVertex ( V[F[f][0]] );
		glColor3f(c2[0],c2[1],c2[2]);  glVertex ( V[F[f][1]] );
		glColor3f(c3[0],c3[1],c3[2]);  glVertex ( V[F[f][2]] );		   
	}
	glEnd ();
	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
	return;
#if 0 // don't know why this is not working.
	std::vector<SbmSubMesh*>& subMeshList = mesh->subMeshList;
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, (GLfloat*)&mesh->posBuf[0]);  
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 0, (GLfloat*)&mesh->normalBuf[0]);

	bool showSkinWeight = true;
	if (showSkinWeight)
	{
		//glDepthMask(GL_FALSE);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3,GL_FLOAT, 0,  (GLfloat*)&mesh->skinColorBuf[0]);		
		//glColorPointer(3,GL_FLOAT, 0,  (GLfloat*)&mesh->meshColorBuf[0]);
		glDisable(GL_LIGHTING);
	}


	for (unsigned int i=0;i<subMeshList.size();i++)
	{	
		SbmSubMesh* subMesh = subMeshList[i];
		glMaterial(subMesh->material);		
		
#if GLES_RENDER
		glDrawElements(GL_TRIANGLES, subMesh->triBuf.size()*3, GL_UNSIGNED_SHORT, &subMesh->triBuf[0]);
#else
		glDrawElements(GL_TRIANGLES, subMesh->triBuf.size()*3, GL_UNSIGNED_INT, &subMesh->triBuf[0]);
#endif
		glBindTexture(GL_TEXTURE_2D,0);
	}	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);
#endif
}
void SkinViewer::draw()
{
	make_current();

 	if (!visible()) 
 		return;
	if (!valid()) 
	{
		init_opengl();
		valid(1);
	}

	preRender();

	if (mesh)
	{
		drawSkinWeight();
	}

	if (skeletonScene)
	{
		renderFunction.apply(skeletonScene);	
	}
}


void SkinViewer::focusOnSkeleton()
{
	SrBox sceneBox;
	if (!skeleton) return;
	sceneBox = skeleton->getBoundingBox();	
	cam.view_all(sceneBox, cam.getFov());	
	float scale = skeleton->getCurrentHeight();
	float znear = 0.01f*scale;
	float zfar = 100.0f*scale;
	cam.setNearPlane(0.001);
	cam.setFarPlane(1000.f);
}