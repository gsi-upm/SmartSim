/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------
Filename:    ExampleFrameListener.h
Description: Defines an example frame listener which responds to frame events.
This frame listener just moves a specified camera around based on
keyboard and mouse movements.
Mouse:    Freelook
W or Up:  Forward
S or Down:Backward
A:        Step left
D:        Step right
             PgUp:     Move upwards
             PgDown:   Move downwards
             F:        Toggle frame rate stats on/off
			 R:        Render mode
             T:        Cycle texture filtering
                       Bilinear, Trilinear, Anisotropic(8)
             P:        Toggle on/off display of camera position / orientation
-----------------------------------------------------------------------------
*/

#ifndef __ExampleFrameListener_H__
#define __ExampleFrameListener_H__

#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif
#include "OGRE/Ogre.h"
#include "OGRE/OgreStringConverter.h"
#include "OGRE/OgreException.h"
#if OGRE_VERSION_MAJOR >= 1 && OGRE_VERSION_MINOR >= 9
#include "OGRE/Overlay/OgreOverlay.h"
#else
#include "OGRE/OgreOverlay.h"
#endif

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

//using namespace Ogre;

class ExampleFrameListener: public Ogre::FrameListener, public Ogre::WindowEventListener
{	
protected:
	virtual void updateStats(void);

public:
	// Constructor takes a RenderWindow because it uses that to determine input context
	ExampleFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam, bool bufferedKeys = false, bool bufferedMouse = false,
			     bool bufferedJoy = false );

	//Adjust mouse clipping area
	virtual void windowResized(Ogre::RenderWindow* rw)
	{
		unsigned int width, height, depth;
		int left, top;
		rw->getMetrics(width, height, depth, left, top);

		//const OIS::MouseState &ms = mMouse->getMouseState();
		//ms.width = width;
		//ms.height = height;
	}

	//Unattach OIS before window shutdown (very important under Linux)
	virtual void windowClosed(Ogre::RenderWindow* rw);

	virtual ~ExampleFrameListener();

	virtual bool processUnbufferedKeyInput(const Ogre::FrameEvent& evt);

	virtual bool processUnbufferedMouseInput(const Ogre::FrameEvent& evt);

	virtual void moveCamera();

	virtual void showDebugOverlay(bool show);

	// Override frameRenderingQueued event to process that (don't care about frameEnded)
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	bool frameEnded(const Ogre::FrameEvent& evt)
	{
		updateStats();
		return true;
	}
protected:
	size_t getParentWindowHandle(size_t winHandle);


protected:
	Ogre::Camera* mCamera;

	Ogre::Vector3 mTranslateVector;
	Ogre::Real mCurrentSpeed;
	Ogre::RenderWindow* mWindow;
	bool mStatsOn;

	std::string mDebugText;

	unsigned int mNumScreenShots;
	float mMoveScale;
	float mSpeedLimit;
	Ogre::Degree mRotScale;
	// just to stop toggles flipping too fast
	Ogre::Real mTimeUntilNextToggle ;
	Ogre::Radian mRotX, mRotY;
	Ogre::TextureFilterOptions mFiltering;
	int mAniso;

	int mSceneDetailIndex ;
	Ogre::Real mMoveSpeed;
	Ogre::Degree mRotateSpeed;
	Ogre::Overlay* mDebugOverlay;

	//OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse*    mMouse;
	OIS::Keyboard* mKeyboard;
	OIS::JoyStick* mJoy;
};

#endif
