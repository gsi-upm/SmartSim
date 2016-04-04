/*
-----------------------------------------------------------------------------
Filename:    OgreSmartBody.h
-----------------------------------------------------------------------------
 
This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#ifndef __OgreSmartBody_h_
#define __OgreSmartBody_h_
 
#include <OGRE/Ogre.h>
#include <OIS/OIS.h>
// smartbody
#ifndef NDEBUG
#define NDEBUG
#endif

#include <sb/SBScene.h>

class OgreSmartBody : public Ogre::WindowEventListener, public Ogre::FrameListener
{
public:
    OgreSmartBody(void);
    virtual ~OgreSmartBody(void);
    bool go(void);

	Ogre::SceneManager* getSceneManager();
 
protected:
    // Ogre::WindowEventListener
    virtual void windowResized(Ogre::RenderWindow* rw);
    virtual void windowClosed(Ogre::RenderWindow* rw);
 
    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	
 
private:
    Ogre::Root* mRoot;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;
    Ogre::RenderWindow* mWindow;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;
 
    // OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;
	double mStartTime;

	// smartbody
	SmartBody::SBScene* m_pScene;
};
 
#endif // #ifndef __OgreSmartBody_h_

