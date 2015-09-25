#include "irrlichtsmartbodylistener.h"
#include <sb/SBSceneListener.h>
#include <IAnimatedMesh.h>
#include <vhcl.h>
#include <sstream>
#include <iostream>

IrrlichtSmartBodyListener::IrrlichtSmartBodyListener(irr::scene::ISceneManager* sceneMgr, std::map<std::string, int>* characterMap)
{
	mSceneMgr = sceneMgr;
	map = characterMap;
	id = 1;
}

IrrlichtSmartBodyListener::~IrrlichtSmartBodyListener()
{
}

void IrrlichtSmartBodyListener::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
	std::stringstream strstr;
	strstr << "../irrlicht-1.8.1/media/" << objectClass<< ".mesh";
	std::cout << "Getting mesh " << strstr.str() << std::endl;

	irr::scene::ISkinnedMesh* skinnedMesh = (irr::scene::ISkinnedMesh*)mSceneMgr->getMesh(strstr.str().c_str());

	if (!skinnedMesh)
	{
		LOG("Cannot find mesh named '%s", objectClass.c_str());
		return;
	}

	irr::scene::IAnimatedMeshSceneNode* node = mSceneMgr->addAnimatedMeshSceneNode( skinnedMesh, NULL, id );
	
	//must set to allow manual joint control
	node->setJointMode(irr::scene::EJUOR_CONTROL);

	irr::scene::IAnimatedMesh* animMesh  = node->getMesh();
	irr::scene::E_ANIMATED_MESH_TYPE type = animMesh->getMeshType();

	irr::core::array<irr::scene::ISkinnedMesh::SJoint*> jointssss  = skinnedMesh->getAllJoints();


	for(irr::u32 i = 1; i < jointssss.size(); i++)
	{
		//Clear all joint animation keys - model takes less space on disk
		jointssss[i]->PositionKeys.clear();
		jointssss[i]->RotationKeys.clear();
		jointssss[i]->ScaleKeys.clear();
	}



	(*map)[name] = id;
	id++;

	//set texture
	std::string textures[9];
	textures[0] = "../irrlicht-1.8.1/media/sinbad_body.tga";
	textures[1] = "../irrlicht-1.8.1/media/sinbad_body.tga";
	textures[2] = "../irrlicht-1.8.1/media/sinbad_clothes.tga";
	textures[3] = "../irrlicht-1.8.1/media/sinbad_body.tga";
	textures[4] = "../irrlicht-1.8.1/media/sinbad_sword.tga";
	textures[5] = "../irrlicht-1.8.1/media/sinbad_clothes.tga";
	textures[6] = "../irrlicht-1.8.1/media/sinbad_clothes.tga";
	textures[7] = "../irrlicht-1.8.1/media/sinbad_clothes.tga";
	textures[8] = "../irrlicht-1.8.1/media/irrlicht2_dn.jpg";
	node->getMaterial(8).setTexture(0,mSceneMgr->getVideoDriver()->getTexture("../irrlicht-1.8.1/media/irrlicht2_dn.jpg"));
	for (int t = 0; t < 8; t++)
	{
		std::cout << "Attempting to retrieve " << textures[t] << std::endl;
		node->getMaterial(t).setTexture(0,mSceneMgr->getVideoDriver()->getTexture(textures[t].c_str()));
	}

	node->setPosition(irr::core::vector3df(0,-80,0));


	node->addShadowVolumeSceneNode();
	
	//mSceneMgr->setShadowColor(irr::video::SColor(150,0,0,0));

	node->setScale(irr::core::vector3df(10,10,10));
	node->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS , true);

}

void IrrlichtSmartBodyListener::OnCharacterDelete( const std::string & name )
{
}

void IrrlichtSmartBodyListener::OnCharacterChanged( const std::string& name )
{

}

void IrrlichtSmartBodyListener::OnLogMessage( const std::string & message )
{
#ifdef WIN32
	LOG(message.c_str());
#endif
}


