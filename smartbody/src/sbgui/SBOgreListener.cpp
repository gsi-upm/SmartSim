#include "SBOgreListener.h"
#include <sb/SBCharacter.h>
#include <sb/SBPawn.h>
#include <sb/SBSkeleton.h>
#include <sb/SBScene.h>
#include <sb/SBAttribute.h>
#include <sbm/sbm_deformable_mesh.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>

#ifdef INTMAX_C 
#undef INTMAX_C
#endif
#ifdef UINTMAX_C
#undef UINTMAX_C
#endif

#include "EmbeddedOgre.h"
#include "OgreFrameListener.h"


using namespace Ogre;
using namespace SmartBody;

OgreListener::OgreListener(EmbeddedOgre* ogre)
{
	ogreInterface = ogre;
}

OgreListener::~OgreListener(void)
{
}

void OgreListener::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
	FLTKListener::OnCharacterCreate(name, objectClass);

	SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(name);
	if (!pawn)
		return;
	// created a ogre entity only when the character is changed and valid
	bool isPawn = false;
	
	SBCharacter* sbChar = SBScene::getScene()->getCharacter(name);

	//if (!sbChar) return; // no smartbody character exist ?
	if (!sbChar) 
	{
		isPawn = true;
	}
// 	else
// 	{
// 		if (sbChar->dMeshInstance_p && sbChar->dMeshInstance_p->isStaticMesh())
// 		{
// 			isPawn = true;
// 		}
// 	}

	//std::string logMsg = "Character " + name ;
	//LogManager::getSingleton().logMessage(logMsg.c_str());

	Entity * ent = NULL;
	if (ogreInterface->getSceneManager()->hasEntity(name))
	{
		LOG("ALREADY FOUND ENTITY NAMED %s", name.c_str());
		return;
	}


	SmartBody::SBAttribute* attr = pawn->getAttribute("mesh");
	if (attr)
		attr->registerObserver(this);

	attr = pawn->getAttribute("deformableMesh");
	if (attr)
		attr->registerObserver(this);
	
	attr = pawn->getAttribute("meshScale");
	if (attr)
		attr->registerObserver(this);

	attr = pawn->getAttribute("meshPivot");
	if (attr)
		attr->registerObserver(this);

	attr = pawn->getAttribute("meshRotate");
	if (attr)
		attr->registerObserver(this);

	attr = pawn->getAttribute("meshTranslate");
	if (attr)
		attr->registerObserver(this);

	attr = pawn->getAttribute("deformableMeshScale");
	if (attr)
		attr->registerObserver(this);
	
	attr = pawn->getAttribute("displayType");
	if (attr)
		attr->registerObserver(this);

	try
	{
		//Create character from characterType
		//ent = ogreInterface->getSceneManager()->createEntity(name, name + ".mesh" );
		//LOG("create ogre chracter = %s",name.c_str());		
		if (isPawn)
			ent = ogreInterface->createOgrePawn(pawn);
		else
			ent = ogreInterface->createOgreCharacter(sbChar);
	}
	catch( Ogre::ItemIdentityException& )
	{
		;
	}
	catch( Ogre::Exception& e )
	{
		if (sbChar)
			LOG("Can not create character %s ...",sbChar->getName().c_str());
		LOG("Exception %s",e.getDescription().c_str());
		if( e.getNumber() == Ogre::Exception::ERR_FILE_NOT_FOUND ) 
		{
			//Default to existing Brad character			
			ent = ogreInterface->getSceneManager()->createEntity(name, "Brad.mesh" );
		}
	}

	if (ent == NULL)
	{
		if (objectClass != "")
		{
			if (isPawn)
				LOG("Can not create pawn %s, no mesh exists with type %s",pawn->getName().c_str(), objectClass.c_str());
			else
				LOG("Can not create character %s, no mesh or skeleton exists with type %s",sbChar->getName().c_str(), objectClass.c_str());
		}
		return;
	}



	// Add entity to the scene node	
	SrVec meshScale = pawn->getVec3Attribute("meshScale");
	SceneNode * mSceneNode = ogreInterface->getSceneManager()->getRootSceneNode()->createChildSceneNode(name);
	// scale the node
	SmartBody::SBAttribute* scaleAttr = pawn->getAttribute("meshScale");
	SmartBody:Vec3Attribute* vec3Attr = dynamic_cast<SmartBody::Vec3Attribute*>(scaleAttr);
	if (vec3Attr)
		mSceneNode->setScale(vec3Attr->getValue().x, vec3Attr->getValue().y, vec3Attr->getValue().z);

	mSceneNode->attachObject(ent);
	mSceneNode->setVisible(ogreInterface->getCharacterVisiblility());
	mSceneNode->setScale(meshScale[0], meshScale[1], meshScale[2]);
	
	

	//ent->setDisplaySkeleton(true);
	std::map<std::string, Ogre::Vector3> intialBonePositions;
	OgreFrameListener* frameListener = dynamic_cast<OgreFrameListener*>(ogreInterface->getOgreFrameListener());
	if (frameListener)
	{
		if (isPawn)
		{
			// insert into pawn list
			frameListener->m_pawnList.push_back(name);
			mSceneNode->setVisible(true);
		}
		else
		{
			// insert into character list
			Ogre::Skeleton* skel = ent->getSkeleton();	
			if (!skel) return; // this should not happen at all ?
			frameListener->m_characterList.push_back(name);
			// get intial bone position for every character
			if (skel)
			{
				for (int i = 0; i < skel->getNumBones(); i++)
				{
					Bone* bone = skel->getBone(i);
					intialBonePositions.insert(std::make_pair(bone->getName(), bone->getPosition()));
					frameListener->m_validJointNames.insert(bone->getName());

				}
				frameListener->m_initialBonePositions.insert(std::make_pair(name, intialBonePositions));
			}			
		}		
	}

}

void OgreListener::OnCharacterDelete( const std::string & name )
{
	SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(name);
	if (!pawn)
		return;

	SmartBody::SBCharacter* sbChar = dynamic_cast<SmartBody::SBCharacter*>(pawn);

	bool isPawn = true;
	if (sbChar)
		isPawn = false;
	// since the charcter/pawn exists, remove it from the scene
	FLTKListener::OnCharacterDelete(name);

	SceneManager* sceneManager = ogreInterface->getSceneManager();
	if (!sceneManager->hasSceneNode(name)) return;

	SceneNode* rootSceneNode = ogreInterface->getSceneManager()->getRootSceneNode();
	if (!rootSceneNode) return;

	SceneNode * node = (SceneNode *)rootSceneNode->getChild(name);
	if (!node) return;

	node->detachAllObjects();	
	ogreInterface->getSceneManager()->destroyEntity(name);
	ogreInterface->getSceneManager()->getRootSceneNode()->removeAndDestroyChild(name);

	OgreFrameListener* frameListener = dynamic_cast<OgreFrameListener*>(ogreInterface->getOgreFrameListener());
	if (frameListener)
	{
		// delete from character list
		std::vector<std::string>& objList = isPawn ? frameListener->m_pawnList : frameListener->m_characterList;
		int eraseId = -1;
		for (unsigned int i = 0; i < objList.size(); i++)
		{
			if (objList[i] == name)
			{
				eraseId = i;
				break;
			}
		}
		if (eraseId >= 0)
		{
			if (isPawn)
				objList.erase(frameListener->m_pawnList.begin() + eraseId);
			else
				objList.erase(frameListener->m_characterList.begin() + eraseId);
		}

		if (!isPawn)
		{
			// delete from initial bone position map
			std::map<std::string, std::map<std::string, Ogre::Vector3> >::iterator iter = frameListener->m_initialBonePositions.find(name);
			if (iter != frameListener->m_initialBonePositions.end())
				frameListener->m_initialBonePositions.erase(iter);
		}	
	}

	
}

void OgreListener::OnCharacterUpdate( const std::string & name )
{
	SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(name);
	
	OgreListener::OnCharacterDelete(name);
	OgreListener::OnCharacterCreate(name, "");
}

void OgreListener::notify(SmartBody::SBSubject* subject)
{
	// notify the FLTKListener first
	FLTKListener::notify(subject);


	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		const std::string& name = attribute->getName();		
		
		SmartBody::SBScene* scene = dynamic_cast<SmartBody::SBScene*>(attribute->getObject());
		if (scene)
		{
			if (name == "floorMaterial")
			{
				SmartBody::StringAttribute* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
				const std::string& value = strAttribute->getValue();
				ogreInterface->updateOgreFloorMaterial(value);
			}
		}


		SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(attribute->getObject());
		
		if (name == "deformableMesh" || name == "mesh" || name == "meshScale" || name == "meshPivot")
		{
			OnCharacterUpdate(pawn->getName());
		}		
		else if (name == "displayType")
		{
			SmartBody::StringAttribute* strAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
			Entity * ent = NULL;
			std::string entName = pawn->getName();
			if (ogreInterface->getSceneManager()->hasEntity(entName))
			{
				ent = ogreInterface->getSceneManager()->getEntity(entName);
			}
			if (strAttribute)
			{
				const std::string& value = strAttribute->getValue();
				if (value == "bones")
				{
					if (pawn->scene_p)
						pawn->scene_p->set_visibility(1,0,0,0);
					if (pawn->dMeshInstance_p)
						pawn->dMeshInstance_p->setVisibility(0);
					if (ent)
						ent->setVisible(false);
				}
				else if (value == "visgeo")
				{
					if (pawn->scene_p)
						pawn->scene_p->set_visibility(0,1,0,0);
					if (pawn->dMeshInstance_p)
						pawn->dMeshInstance_p->setVisibility(0);
					if (ent)
						ent->setVisible(false);
					
				}
				else if (value == "colgeo")
				{
					if (pawn->scene_p)
						pawn->scene_p->set_visibility(0,0,1,0);
					if (pawn->dMeshInstance_p)
						pawn->dMeshInstance_p->setVisibility(0);
					if (ent)
						ent->setVisible(false);
					
				}
				else if (value == "axis")
				{
					if (pawn->scene_p)
						pawn->scene_p->set_visibility(0,0,0,1);
					if (pawn->dMeshInstance_p)
						pawn->dMeshInstance_p->setVisibility(0);
					if (ent)
						ent->setVisible(false);
					
				}
				else if (value == "skinWeight")
				{
					if (pawn->scene_p)
						pawn->scene_p->set_visibility(1,0,0,0);
					if (pawn->dMeshInstance_p)
						pawn->dMeshInstance_p->setVisibility(2);
					if (ent)
						ent->setVisible(false);
				}
				else if (value == "mesh")
				{
					if (pawn->scene_p)
						pawn->scene_p->set_visibility(0,0,0,0);
					if (pawn->dMeshInstance_p)
						pawn->dMeshInstance_p->setVisibility(0);
					if (ent)
						ent->setVisible(true);	
					SbmDeformableMeshGPU::useGPUDeformableMesh = false;
				}
				else if (value == "GPUmesh")
				{
					if (pawn->scene_p)
						pawn->scene_p->set_visibility(0,0,0,0);
					if (pawn->dMeshInstance_p)
						pawn->dMeshInstance_p->setVisibility(0);
					if (ent)
						ent->setVisible(true);
					SbmDeformableMeshGPU::useGPUDeformableMesh = false;
				}
			}
		}
	}	
}

void OgreListener::OnPawnCreate( const std::string & name )
{
	OgreListener::OnCharacterCreate(name, "");

	if (name.compare(0, 5, "light") == 0)
	{
		ogreInterface->resetOgreLights();
	}
}

void OgreListener::OnPawnDelete( const std::string & name )
{
	OgreListener::OnCharacterDelete(name);
	if (name.compare(0, 5, "light") == 0)
	{
		ogreInterface->resetOgreLights();
	}
}

void OgreListener::OnViseme( const std::string & name, const std::string & visemeName, const float weight, const float blendTime )
{
	FLTKListener::OnViseme(name, visemeName, weight, blendTime);
}

void OgreListener::OnChannel( const std::string & name, const std::string & channelName, const float value )
{
	FLTKListener::OnChannel(name,channelName, value);
}

void OgreListener::OnSimulationStart()
{
	FLTKListener::OnSimulationStart();

	SmartBody::SBAttribute* attribute = SmartBody::SBScene::getScene()->getAttribute("floorMaterial");
	if (attribute)
		attribute->registerObserver(this);
}
