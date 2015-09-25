#include "vhcl.h"
#ifndef SB_NO_VHMSG
#include "vhmsg-tt.h"
#endif
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sbm/sbm_deformable_mesh.h>
#include "OgreFrameListener.h"
#include "EmbeddedOgre.h"


using namespace Ogre;

OgreFrameListener::OgreFrameListener(RenderWindow * win, Camera * cam, const std::string & debugText, SceneManager * mgr, EmbeddedOgre* ogreInterface) 
								   : ExampleFrameListener( win, cam )
	{
	embeddedOgre = ogreInterface;
	mDebugText = debugText;
	mSceneMgr = mgr;	
	mQuit = false;
	m_ogreMouseEnabled = true;
	// turn off mouse look by default
	//SetOgreMouse( true );
}


void OgreFrameListener::windowFocusChange( RenderWindow * rw )
{
	rw->setActive( true );
}

bool OgreFrameListener::processUnbufferedKeyInput(const FrameEvent& evt)
{
    return false;
#if 0
	if(mKeyboard->isKeyDown(OIS::KC_A))
		mTranslateVector.x = -mMoveScale;	// Move camera left

	if(mKeyboard->isKeyDown(OIS::KC_D))
		mTranslateVector.x = mMoveScale;	// Move camera RIGHT

	if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) )
		mTranslateVector.z = -mMoveScale;	// Move camera forward

	if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) )
		mTranslateVector.z = mMoveScale;	// Move camera backward

	if(mKeyboard->isKeyDown(OIS::KC_PGUP) || mKeyboard->isKeyDown(OIS::KC_Q) )
		mTranslateVector.y = mMoveScale;	// Move camera up

	if(mKeyboard->isKeyDown(OIS::KC_PGDOWN) || mKeyboard->isKeyDown(OIS::KC_E) )
		mTranslateVector.y = -mMoveScale;	// Move camera down

	if(mKeyboard->isKeyDown(OIS::KC_RIGHT))
		mCamera->yaw(-mRotScale);

	if(mKeyboard->isKeyDown(OIS::KC_LEFT))
		mCamera->yaw(mRotScale);

	if( mKeyboard->isKeyDown(OIS::KC_ESCAPE))
		return false;
#endif

   	if( mKeyboard->isKeyDown(OIS::KC_Z) && mTimeUntilNextToggle <= 0 )
	{
		mStatsOn = !mStatsOn;
		showDebugOverlay(mStatsOn);
		mTimeUntilNextToggle = 1;
	}

	if( mKeyboard->isKeyDown(OIS::KC_M) && mTimeUntilNextToggle <= 0 )
	{
		switch(mFiltering)
		{
		case TFO_BILINEAR:
			mFiltering = TFO_TRILINEAR;
			mAniso = 1;
			break;
		case TFO_TRILINEAR:
			mFiltering = TFO_ANISOTROPIC;
			mAniso = 8;
			break;
		case TFO_ANISOTROPIC:
			mFiltering = TFO_BILINEAR;
			mAniso = 1;
			break;
		default: break;
		}
		MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
		MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);

		showDebugOverlay(mStatsOn);
		mTimeUntilNextToggle = 1;
	}

	if(mKeyboard->isKeyDown(OIS::KC_SYSRQ) && mTimeUntilNextToggle <= 0)
	{
		std::ostringstream ss;
		ss << "screenshot_" << ++mNumScreenShots << ".png";
		mWindow->writeContentsToFile(ss.str());
		mTimeUntilNextToggle = 0.5;
		mDebugText = "Saved: " + ss.str();
	}

	if(mKeyboard->isKeyDown(OIS::KC_N) && mTimeUntilNextToggle <=0)
	{
		mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
		switch(mSceneDetailIndex) {
			case 0 : mCamera->setPolygonMode(PM_SOLID); break;
			case 1 : mCamera->setPolygonMode(PM_WIREFRAME); break;
			case 2 : mCamera->setPolygonMode(PM_POINTS); break;
		}
		mTimeUntilNextToggle = 0.5;
	}

	static bool displayCameraDetails = false;
	if(mKeyboard->isKeyDown(OIS::KC_P) && mTimeUntilNextToggle <= 0)
	{
		displayCameraDetails = !displayCameraDetails;
		mTimeUntilNextToggle = 0.5;
		if (!displayCameraDetails)
			mDebugText = "";
	}

	// display appropriate scenes
	if(mKeyboard->isKeyDown(OIS::KC_1))
	{
		mSceneMgr->getSceneNode("world_scene_ft")->setVisible(false);
		mSceneMgr->getSceneNode("world_scene_cm")->setVisible(false);
		mSceneMgr->getSceneNode("plane_node")->setVisible(false); 
	    mSceneMgr->getSceneNode("world_scene_vh")->setVisible(true);
	}
	if(mKeyboard->isKeyDown(OIS::KC_2))
	{
		mSceneMgr->getSceneNode("world_scene_ft")->setVisible(false);
		mSceneMgr->getSceneNode("world_scene_cm")->setVisible(true);
		mSceneMgr->getSceneNode("plane_node")->setVisible(false);
		mSceneMgr->getSceneNode("world_scene_vh")->setVisible(false);
	}
	if(mKeyboard->isKeyDown(OIS::KC_3))
	{
		mSceneMgr->getSceneNode("world_scene_ft")->setVisible(true);
		mSceneMgr->getSceneNode("world_scene_cm")->setVisible(false);
		mSceneMgr->getSceneNode("plane_node")->setVisible(false);
		mSceneMgr->getSceneNode("world_scene_vh")->setVisible(false);
	}
	if(mKeyboard->isKeyDown(OIS::KC_4))
	{
		mSceneMgr->getSceneNode("world_scene_ft")->setVisible(false);
		mSceneMgr->getSceneNode("world_scene_cm")->setVisible(false);
		mSceneMgr->getSceneNode("plane_node")->setVisible(true);
		mSceneMgr->getSceneNode("world_scene_vh")->setVisible(false);
	}


	if ( mKeyboard->isKeyDown( OIS::KC_J ) )
	{
		SetOgreMouse( !m_ogreMouseEnabled );
	}


	// Print camera details
	if(displayCameraDetails)
		mDebugText = "P: " + StringConverter::toString(mCamera->getDerivedPosition()) +
					 " " + "O: " + StringConverter::toString(mCamera->getDerivedOrientation());

	// Return true to continue rendering
	return true;
}


bool OgreFrameListener::processUnbufferedMouseInput( const FrameEvent & evt )
{
#if 0
	if ( m_ogreMouseEnabled )
	{
		//return ExampleFrameListener::processUnbufferedMouseInput( evt );

		const OIS::MouseState &ms = mMouse->getMouseState();		
		if( ms.buttonDown( OIS::MB_Middle ) && mKeyboard->isKeyDown(OIS::KC_LMENU) )
		{
			mTranslateVector.x += ms.X.rel * mMoveSpeed *evt.timeSinceLastFrame;
			mTranslateVector.y -= ms.Y.rel * mMoveSpeed *evt.timeSinceLastFrame;
		}
		else if ( ms.buttonDown( OIS::MB_Left)  && mKeyboard->isKeyDown(OIS::KC_LMENU))
		{
			mRotX = Degree(-ms.X.rel * 0.13);
			mRotY = Degree(-ms.Y.rel * 0.13);
		}
		//else if ( ms.buttonDown( OIS::MB_Right) && mKeyboard->)
	}
#endif

	return true;
}


void OgreFrameListener::ogreBlendShape( Ogre::Entity* sbEntity, DeformableMeshInstance* meshInstance )
{
	DeformableMesh* defMesh = meshInstance->getDeformableMesh();
	if (!defMesh) return;
	if (defMesh->blendShapeMap.size() == 0) return;  // no blend shape

	Ogre::MeshPtr meshPtr = sbEntity->getMesh();

	std::map<std::string, std::vector<SrSnModel*> >::iterator mIter;
	mIter = defMesh->blendShapeMap.begin();
	SrSnModel* writeToBaseModel = NULL;
	int vtxBaseIdx = 0;
	for (size_t i = 0; i < defMesh->dMeshStatic_p.size(); ++i)
	{		
		if (strcmp(defMesh->dMeshStatic_p[i]->shape().name, mIter->first.c_str()) == 0)
		{
			writeToBaseModel = defMesh->dMeshStatic_p[i];
			break;
		}
		else
		{
			// skip vertices for this sub mesh
			vtxBaseIdx += defMesh->dMeshStatic_p[i]->shape().V.size();
		}
	}
	
	if (!writeToBaseModel) return;

	// perform face blendshape
	meshInstance->blendShapes();

	SrModel& baseModel = writeToBaseModel->shape();
	const Ogre::VertexElement* VertexEle_POS = meshPtr->sharedVertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
	Ogre::HardwareVertexBufferSharedPtr VertexBufPOS = meshPtr->sharedVertexData->vertexBufferBinding->getBuffer( VertexEle_POS->getSource() );
	unsigned char* VertexPtrPOS = static_cast<unsigned char*>( VertexBufPOS->lock( Ogre::HardwareBuffer::HBL_NORMAL ) );
	int VertSizePOS=VertexBufPOS->getVertexSize();
	float * pElementPOS=NULL;
	std::map<int,std::vector<int> >& vtxNewVtxIdxMap = defMesh->vtxNewVtxIdxMap;

	for (int i=0;i<baseModel.V.size();i++)
	{
		int iVtx = vtxBaseIdx+i;
		SrVec basePos = baseModel.V[i];
		unsigned char* curVertexPtrPos = VertexPtrPOS + iVtx*VertSizePOS;

		VertexEle_POS->baseVertexPointerToElement( curVertexPtrPos, &pElementPOS );	
		pElementPOS[0] = basePos[0];
		pElementPOS[1] = basePos[1];
		pElementPOS[2] = basePos[2];
		if (vtxNewVtxIdxMap.find(iVtx) != vtxNewVtxIdxMap.end())
		{
			std::vector<int>& idxMap = vtxNewVtxIdxMap[iVtx];
			// copy related vtx components 
			for (unsigned int k=0;k<idxMap.size();k++)
			{
				curVertexPtrPos = VertexPtrPOS + idxMap[k]*VertSizePOS;
				VertexEle_POS->baseVertexPointerToElement( curVertexPtrPos, &pElementPOS );
				pElementPOS[0] = basePos[0];
				pElementPOS[1] = basePos[1];
				pElementPOS[2] = basePos[2];
				//_deformPosBuf[idxMap[k]] = finalVec;
			}
		}			
	}

	//UNLOCK BUFFER
	if(VertexBufPOS->isLocked()) 
		VertexBufPOS->unlock();
}

bool OgreFrameListener::frameStarted( const FrameEvent & evt )
{
	//return true;
	if (mQuit)
	{
		return false;
	}

	if ( ExampleFrameListener::frameStarted( evt ) == false )
		return false;	
	
	char temp[ 1024 ];
	sprintf( temp, "%5.2f %5.2f %5.2f", mCamera->getPosition().x, mCamera->getPosition().y, mCamera->getPosition().z );
	
	{		
		SceneNode* sceneNode = mSceneMgr->getRootSceneNode();
		if (!sceneNode)
			return false;

		int numVisibleCharacters = 0;	
		// for pawn, only update the world offset
		for ( size_t i = 0; i< m_pawnList.size(); i++ )
		{
			std::string& name = m_pawnList[i];
			SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(name);
			if (!mSceneMgr->hasEntity(name))
				continue;
			if (!pawn)
				continue;

			Node* node = sceneNode->getChild(name);

			if (!node)
				continue;
			// set character positions& rotations
			SrMat wMat = pawn->get_world_offset();
			SrQuat wRot = SrQuat(wMat);
			SrVec wPos = wMat.get_translation();
			node->setPosition(wPos.x, wPos.y, wPos.z);
			node->setOrientation(Quaternion(wRot.w, wRot.x, wRot.y, wRot.z));
		}
		// set character position and rotation
		for ( size_t i = 0; i < m_characterList.size(); i++ )
		{
			std::string& name = m_characterList[i];
			SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(name);
			if (!mSceneMgr->hasEntity(name))
				continue;
			if (!character)
				continue;

			SceneNode* node = dynamic_cast<SceneNode*>(sceneNode->getChild(name));

			if (!node)
				continue;
			
			Ogre::Entity* nodeEntity = dynamic_cast<Entity*>(node->getAttachedObject(0));		
			if (nodeEntity && character->dMeshInstance_p)
			{
				ogreBlendShape(nodeEntity, character->dMeshInstance_p);				
			}
			
			// set character positions& rotations
			SrMat wMat = character->get_world_offset();
			SrQuat wRot = SrQuat(wMat);
			SrVec wPos = wMat.get_translation();
			node->setPosition(wPos.x, wPos.y, wPos.z);
			node->setOrientation(Quaternion(wRot.w, wRot.x, wRot.y, wRot.z));
			
			// set bone positions& rotations				
			SceneNode* n = (SceneNode*) sceneNode->getChild(name);
			Entity * ent = (Entity*)n->getAttachedObject(name);
			if (mCamera->isVisible(n->_getWorldAABB()))
			{
				numVisibleCharacters++;	
				if (embeddedOgre->getCharacterVisiblility())
					n->setVisible(true);
			}
			else
			{				
				n->setVisible(false);
				continue;
			}
			if ( ent == NULL )
				continue;
			Ogre::Skeleton* skel = ent->getSkeleton();				
			if (!skel) continue;

			std::map<std::string, Ogre::Vector3>& intialBonePositionMap = m_initialBonePositions[name];			
			SmartBody::SBSkeleton* sbSkel = character->getSkeleton();
			for (int jId = 0; jId < sbSkel->getNumJoints(); jId++)
			{
				SmartBody::SBJoint* joint = sbSkel->getJoint(jId);
				const std::string& jointName = joint->getName();
				if (jointName == "")
					continue;

				std::set<std::string>::iterator iter = m_validJointNames.find(jointName);
				if (iter == m_validJointNames.end())
				{
					continue;
				}				

				//try
				{
					Ogre::Bone* bone = skel->getBone(jointName);
					if (bone)
					{						
						if (!bone->isManuallyControlled())
							bone->setManuallyControlled(true);
						Ogre::Vector3& vec = intialBonePositionMap[jointName];
						float x = joint->pos()->value(0)+ vec.x;
						float y = joint->pos()->value(1) + vec.y;
						float z = joint->pos()->value(2) + vec.z;
 						bone->setPosition(x, y, z);
 						const SrQuat& q = joint->quat()->value();
 						bone->setOrientation(q.w, q.x, q.y, q.z);
					}
				}
				//catch (ItemIdentityException&)
				//{
					//printf("Could not find bone name %s", jointName.c_str());
				//}
			}			
		}
		//LOG("Num of visible characters = %d",numVisibleCharacters);
	}
	

	return true;
}


void OgreFrameListener::scheduleQuit(void)
{
	mQuit = true;
}



void OgreFrameListener::SetOgreMouse( const bool enabled )
{
	m_ogreMouseEnabled = enabled;


	// There's no interface in OIS to set the coop level on individual devices at runtime.
	// There's also no interface in OIS to set the coop level on individual devices at startup,
	// so you have to tear down the entire input manager and start from scratch.
	// Hopefully there's no side-effects from this behavior

	// http://www.wreckedgames.com/forum/index.php/topic,1149.msg6170.html#msg6170


	// taken from ExampleFrameListener::windowClosed()
	LogManager::getSingletonPtr()->logMessage( "*** Destroying OIS ***" );
	if ( mInputManager )
	{
		mInputManager->destroyInputObject( mMouse );
		mInputManager->destroyInputObject( mKeyboard );
		mInputManager->destroyInputObject( mJoy );

		OIS::InputManager::destroyInputSystem( mInputManager );
		mInputManager = 0;

		mMouse = 0;
		mKeyboard = 0;
		mJoy = 0;
	}


	// taken from ExampleFrameListener::ExampleFrameListener()
	//LogManager::getSingletonPtr()->logMessage( "*** Initializing OIS step 0***" );
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute( "WINDOW", &windowHnd );

/*
	while (getParentWindowHandle(windowHnd)) // loop until we get top level window
	{
		windowHnd = getParentWindowHandle(windowHnd);
	}
*/
	windowHndStr << windowHnd;
	pl.insert( std::make_pair( std::string( "WINDOW" ), windowHndStr.str() ) );


	
// 	if ( m_ogreMouseEnabled )
// 	{
// 		//pl.insert( std::make_pair( std::string( "w32_mouse" ), std::string( "DISCL_EXCLUSIVE" ) ) );
// 		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
// 		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));		
// 	}
// 	else
// 	{
// 		pl.insert( std::make_pair( std::string( "w32_mouse" ), std::string( "DISCL_NONEXCLUSIVE" ) ) );
// 	}

	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));		
	pl.insert( std::make_pair( std::string( "w32_mouse" ), std::string( "DISCL_FOREGROUND" ) ) );
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));


	
	mInputManager = OIS::InputManager::createInputSystem( pl );
	

	//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
	mKeyboard = static_cast<OIS::Keyboard *>( mInputManager->createInputObject( OIS::OISKeyboard, false ) );
	mMouse = static_cast<OIS::Mouse *>( mInputManager->createInputObject( OIS::OISMouse, false ) );
	try
	{
		mJoy = static_cast<OIS::JoyStick *>( mInputManager->createInputObject( OIS::OISJoyStick, false ) );
	}
	catch(...)
	{
		mJoy = 0;
	}
	
}

