#include "ogresmartbody.h"
#include "ogresmartbodylistener.h" 

#include <sb/SBPython.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>

//-------------------------------------------------------------------------------------
OgreSmartBody::OgreSmartBody(void)
    : mRoot(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mWindow(0),
    mSceneMgr(0),
    mCamera(0),
	m_pScene(NULL)
{
}
//-------------------------------------------------------------------------------------
OgreSmartBody::~OgreSmartBody(void)
{
    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

Ogre::SceneManager* OgreSmartBody::getSceneManager()
{
	return mSceneMgr;
}
 
 
bool OgreSmartBody::go(void)
{
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
	mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif
 
    // construct Ogre::Root
    mRoot = new Ogre::Root(mPluginsCfg);
 
    // setup resources
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);
 
    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
 
    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
    // configure
    // Show the configuration dialog and initialise the system
    if(!(mRoot->restoreConfig() || mRoot->showConfigDialog()))
    {
        return false;
    }
 
    mWindow = mRoot->initialise(true, "OgreSmartBody Render Window");
 
    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    // initialise all resource groups
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
 
    // Create the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager("DefaultSceneManager");
 
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");
 
    // Position it at 500 in Z direction
    mCamera->setPosition(Ogre::Vector3(0,10,60));
    // Look back along -Z
    mCamera->lookAt(Ogre::Vector3(0,0,-300));
    mCamera->setNearClipDistance(5);
 
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
 
    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	// create a floor for better visualization
		//adding plane entity to the scene
	Ogre::Plane plane;
	plane.normal = Ogre::Vector3::UNIT_Y;
	plane.d = 0;
	Ogre::MeshManager::getSingleton().createPlane( "Myplane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500, 1500, 20, 20, true, 1, 60, 60, Ogre::Vector3::UNIT_Z );
	Ogre::Entity * pPlaneEnt = mSceneMgr->createEntity( "plane", "Myplane" );
	//pPlaneEnt->setMaterialName( "Examples/Rockwall" );
	pPlaneEnt->setMaterialName( "Rockwall" );
	pPlaneEnt->setCastShadows( false );
	mSceneMgr->getRootSceneNode()->createChildSceneNode("plane_node", Ogre::Vector3( 0, 0, 0 ) )->attachObject( pPlaneEnt );
	mSceneMgr->getSceneNode("plane_node")->setVisible(true);

	// shadows
	mSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_TEXTURE_MODULATIVE );
	mSceneMgr->setShadowTextureSize( 4048 );
	mSceneMgr->setShadowColour( Ogre::ColourValue( 0.3f, 0.3f, 0.3f ) );
  
    // Set ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
 
    // Create a light
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    l->setPosition(20,80,50);
 
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
 
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	
    mInputManager = OIS::InputManager::createInputSystem( pl );
 
    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, false ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, false ));
 
    //Set initial mouse clipping size
    windowResized(mWindow);
 
    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
 
    mRoot->addFrameListener(this);

	// smartbody
	// the root path to SmartBody: change this to your own path
	std::string smartbodyRoot = "..";
	// set the following to the location of the Python libraries. 
	// if you downloaded SmartBody, it will be in core/smartbody/Python26/Lib
#ifdef WIN32
	initPython(smartbodyRoot + "/Python27/lib");
#else
	initPython("/usr/lib/python2.7");
#endif
	m_pScene = SmartBody::SBScene::getScene();

	m_pScene->startFileLogging("smartbody.log");
	OgreSmartBodyListener* listener = new OgreSmartBodyListener(this);
	m_pScene->addSceneListener(listener);
	m_pScene->start();

	// sets the media path, or root of all the data to be used
	// other paths will be relative to the media path
	m_pScene->setMediaPath(smartbodyRoot + "/data");
	m_pScene->addAssetPath("script", ".");
	// the file 'OgreSmartBody.py' needs to be placed in the media path directory
	m_pScene->runScript("ogresmartbody.py");

	mStartTime = Ogre::Root::getSingleton().getTimer()->getMilliseconds() / 1000.0f;
	mRoot->startRendering();
 
    return true;
}
 
//Adjust mouse clipping area
void OgreSmartBody::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
 
    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}
 
//Unattach OIS before window shutdown (very important under Linux)
void OgreSmartBody::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );
 
            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}
 
bool OgreSmartBody::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;
 
    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();
 
    if(mKeyboard->isKeyDown(OIS::KC_ESCAPE))
        return false;

	// smartbody
	if (!m_pScene)
		return true;

	SmartBody::SBSimulationManager* sim = m_pScene->getSimulationManager();
	sim->setTime((Ogre::Root::getSingleton().getTimer()->getMilliseconds() / 1000.0f) - mStartTime);
	m_pScene->update();

	int numCharacters = m_pScene->getNumCharacters();
	if (numCharacters == 0)
		return true;

	const std::vector<std::string>& characterNames = m_pScene->getCharacterNames();

	for (size_t n = 0; n < characterNames.size(); n++)
	{
		SmartBody::SBCharacter* character = m_pScene->getCharacter(characterNames[n]);
		if (!this->getSceneManager()->hasEntity(characterNames[n]))
			continue;

		Ogre::Entity* entity = this->getSceneManager()->getEntity(characterNames[n]);
		Ogre::Skeleton* meshSkel = entity->getSkeleton();
		Ogre::Node* node = entity->getParentNode();

		SrVec pos = character->getPosition();
		SrQuat ori = character->getOrientation();
		//std::cout << ori.w << ori.x << " " << ori.y << " " << ori.z << std::endl;
		node->setPosition(pos.x, pos.y, pos.z);
		node->setOrientation(ori.w, ori.x, ori.y, ori.z);
	
		// Update joints
		SmartBody::SBSkeleton* sbSkel = character->getSkeleton();
			
		int numJoints = sbSkel->getNumJoints();
		for (int j = 0; j < numJoints; j++)
		{
			SmartBody::SBJoint* joint = sbSkel->getJoint(j);
	
			try
			{
				SrQuat orientation = joint->quat()->value();
	
				Ogre::Vector3 posDelta(joint->getPosition().x, joint->getPosition().y, joint->getPosition().z);
				Ogre::Quaternion quatDelta(orientation.w, orientation.x, orientation.y, orientation.z);
				Ogre::Bone* bone = meshSkel->getBone(joint->getName());
				if (!bone)
					continue;
				bone->setPosition(bone->getInitialPosition() + posDelta);
				bone->setOrientation(quatDelta);
			}
			catch (Ogre::ItemIdentityException& ex)
			{
				// Should not happen as we filtered using m_mValidBones
			}
		}
	}
	

 
    return true;
}


//================================
 
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        OgreSmartBody app;
 
        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
 
        return 0;
    }
 
#ifdef __cplusplus
}
#endif
