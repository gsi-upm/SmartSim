#include "ExampleFrameListener.h"
#if !defined(WIN32)
#include <X11/Xlib.h>
#endif
#define SHOW_OGRE_DEBUG_OVERLAY 0

using namespace Ogre;
void ExampleFrameListener::updateStats( void )
{
#if SHOW_OGRE_DEBUG_OVERLAY
	static String currFps = "Current FPS: ";
	static String avgFps = "Average FPS: ";
	static String bestFps = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris = "Triangle Count: ";
	static String batches = "Batch Count: ";

	// update stats when necessary
	try {
		OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
		OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
		OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
		OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

		const RenderTarget::FrameStats& stats = mWindow->getStatistics();
		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
			+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
		guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
			+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

		OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

		OverlayElement* guiBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
		guiBatches->setCaption(batches + StringConverter::toString(stats.batchCount));

		OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		guiDbg->setCaption(mDebugText);
	}
	catch(...) { /* ignore */ }
#endif
}

ExampleFrameListener::ExampleFrameListener( RenderWindow* win, Camera* cam, bool bufferedKeys /*= false*/, bool bufferedMouse /*= false*/, bool bufferedJoy /*= false */ ) :
mCamera(cam), mTranslateVector(Vector3::ZERO), mCurrentSpeed(0), mWindow(win), mStatsOn(false), mNumScreenShots(0),
mMoveScale(0.0f), mRotScale(0.0f), mTimeUntilNextToggle(0), mFiltering(TFO_BILINEAR),
mAniso(1), mSceneDetailIndex(0), mMoveSpeed(500), mRotateSpeed(36), mDebugOverlay(0),
mInputManager(0), mMouse(0), mKeyboard(0), mJoy(0)
{	
	LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	unsigned int windowAttr = 15;
	size_t windowHnd = 10;
	std::ostringstream windowHndStr;	
	win->getCustomAttribute("WINDOW", &windowAttr);
	
	windowHnd = windowAttr;
	printf("custom attribute = %lu, windowHnd = %lu\n",windowAttr,windowHnd);

	/*while (getParentWindowHandle(windowHnd)) // loop until we get top level window
	{
		windowHnd = getParentWindowHandle(windowHnd);
	}
	*/
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	pl.insert(std::make_pair(std::string("w32_keyboard"),std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"),std::string("DISCL_BACKGROUND")));
	pl.insert(std::make_pair(std::string("w32_mouse"),std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_mouse"),std::string("DISCL_BACKGROUND")));

	//LogManager::getSingletonPtr()->logMessage( "*** Initializing OIS step1 ***" );
#if 0
	//LogManager::getSingletonPtr()->logMessage( "*** Initializing OIS step2 ***" );
	mInputManager = OIS::InputManager::createInputSystem( windowHnd );
	//LogManager::getSingletonPtr()->logMessage( "*** Initializing OIS step3 ***" );


	//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
	try {
		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, bufferedMouse ));
		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, bufferedKeys ));		
	}
	catch (OIS::Exception& e)
	{
		//LogManager::getSingletonPtr()->logMessage("Exception when createInputObject, description = " + e);	 
		printf("exception = %s\n", e.eText);
	}
#endif	
#if 0
	try {
		mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject( OIS::OISJoyStick, bufferedJoy ));
	}
	catch(...) {
		mJoy = 0;
	}
#endif

	//Set initial mouse clipping size
	//LogManager::getSingletonPtr()->logMessage( "*** Initializing OIS step4 ***" );
	windowResized(mWindow);
	//LogManager::getSingletonPtr()->logMessage( "*** Initializing OIS step5 ***" );
#if SHOW_OGRE_DEBUG_OVERLAY
	mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	Ogre::OverlayContainer* logo = mDebugOverlay->getChild("Core/LogoPanel");
	logo->hide();	
	mDebugOverlay->hide();
	showDebugOverlay(false);
#endif
	//LogManager::getSingletonPtr()->logMessage( "*** Initializing OIS step6 ***" );

	//Register as a Window listener
	WindowEventUtilities::addWindowEventListener(mWindow, this);
	LogManager::getSingletonPtr()->logMessage( "*** Finish Initialize OIS ***" );
}

size_t ExampleFrameListener::getParentWindowHandle(size_t winHandle)
{
#if defined(WIN32)
	return (size_t)GetParent((HWND)winHandle);
#else
	Window rootReturn;
        Window parentReturn;
        Window *childrenReturn;
	unsigned int nchildrenReturn;	
	XQueryTree(XOpenDisplay(NULL),winHandle,&rootReturn,&parentReturn,&childrenReturn,&nchildrenReturn);
	printf("window handle  =%d, parent handle = %d\n",winHandle,parentReturn);
  	return parentReturn;
#endif
}

void ExampleFrameListener::windowClosed( RenderWindow* rw )
{
	//Only close for window that created OIS (the main window in these demos)
	if( rw == mWindow )
	{
		if( mInputManager )
		{
			mInputManager->destroyInputObject( mMouse );
			mInputManager->destroyInputObject( mKeyboard );
			mInputManager->destroyInputObject( mJoy );

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}

ExampleFrameListener::~ExampleFrameListener()
{
	//Remove ourself as a Window listener
	WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
}

bool ExampleFrameListener::processUnbufferedKeyInput( const FrameEvent& evt )
{
    return false;
	if(mKeyboard->isKeyDown(OIS::KC_A))
		mTranslateVector.x = -mMoveScale;	// Move camera left

	if(mKeyboard->isKeyDown(OIS::KC_D))
		mTranslateVector.x = mMoveScale;	// Move camera RIGHT

	if(mKeyboard->isKeyDown(OIS::KC_UP) || mKeyboard->isKeyDown(OIS::KC_W) )
		mTranslateVector.z = -mMoveScale;	// Move camera forward

	if(mKeyboard->isKeyDown(OIS::KC_DOWN) || mKeyboard->isKeyDown(OIS::KC_S) )
		mTranslateVector.z = mMoveScale;	// Move camera backward

	if(mKeyboard->isKeyDown(OIS::KC_PGUP))
		mTranslateVector.y = mMoveScale;	// Move camera up

	if(mKeyboard->isKeyDown(OIS::KC_PGDOWN))
		mTranslateVector.y = -mMoveScale;	// Move camera down

	if(mKeyboard->isKeyDown(OIS::KC_RIGHT))
		mCamera->yaw(-mRotScale);

	if(mKeyboard->isKeyDown(OIS::KC_LEFT))
		mCamera->yaw(mRotScale);

	if( mKeyboard->isKeyDown(OIS::KC_ESCAPE) || mKeyboard->isKeyDown(OIS::KC_Q) )
		return false;

	if( mKeyboard->isKeyDown(OIS::KC_F) && mTimeUntilNextToggle <= 0 )
	{
		mStatsOn = !mStatsOn;
		showDebugOverlay(mStatsOn);
		mTimeUntilNextToggle = 1;
	}

	if( mKeyboard->isKeyDown(OIS::KC_T) && mTimeUntilNextToggle <= 0 )
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

	if(mKeyboard->isKeyDown(OIS::KC_R) && mTimeUntilNextToggle <=0)
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

	// Print camera details
	if(displayCameraDetails)
		mDebugText = "P: " + StringConverter::toString(mCamera->getDerivedPosition()) +
		" " + "O: " + StringConverter::toString(mCamera->getDerivedOrientation());

	// Return true to continue rendering
	return true;
}

bool ExampleFrameListener::processUnbufferedMouseInput( const FrameEvent& evt )
{
    return false;
	// Rotation factors, may not be used if the second mouse button is pressed
	// 2nd mouse button - slide, otherwise rotate
	const OIS::MouseState &ms = mMouse->getMouseState();
	if( ms.buttonDown( OIS::MB_Right ) )
	{
		mTranslateVector.x += ms.X.rel * 0.13f;
		mTranslateVector.y -= ms.Y.rel * 0.13f;
	}
	else
	{
		mRotX = Degree(-ms.X.rel * 0.13f);
		mRotY = Degree(-ms.Y.rel * 0.13f);
	}

	return true;
}

void ExampleFrameListener::moveCamera()
{
	// Make all the changes to the camera
	// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW
	//(e.g. airplane)
#if 0
	mCamera->yaw(mRotX);
	mCamera->pitch(mRotY);
	mCamera->moveRelative(mTranslateVector);
#endif
}

void ExampleFrameListener::showDebugOverlay( bool show )
{
#if SHOW_OGRE_DEBUG_OVERLAY
	if (mDebugOverlay)
	{
		if (show)
			mDebugOverlay->show();
		else
			mDebugOverlay->hide();
	}
#endif
}

bool ExampleFrameListener::frameRenderingQueued( const FrameEvent& evt )
{
    return false;
	if(mWindow->isClosed())	return false;

	mSpeedLimit = mMoveScale * evt.timeSinceLastFrame;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();
	if( mJoy ) mJoy->capture();

	bool buffJ = (mJoy) ? mJoy->buffered() : true;

	Ogre::Vector3 lastMotion = mTranslateVector;

	//Check if one of the devices is not buffered
	if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ )
	{
		// one of the input modes is immediate, so setup what is needed for immediate movement
		if (mTimeUntilNextToggle >= 0)
			mTimeUntilNextToggle -= evt.timeSinceLastFrame;

		// Move about 100 units per second
		mMoveScale = mMoveSpeed * evt.timeSinceLastFrame;
		// Take about 10 seconds for full rotation
		mRotScale = mRotateSpeed * evt.timeSinceLastFrame;

		mRotX = 0;
		mRotY = 0;
		mTranslateVector = Ogre::Vector3::ZERO;

	}

	//Check to see which device is not buffered, and handle it
	if( !mKeyboard->buffered() )
		if( processUnbufferedKeyInput(evt) == false )
			return false;
	if( !mMouse->buffered() )
		if( processUnbufferedMouseInput(evt) == false )
			return false;

	// ramp up / ramp down speed
	if (mTranslateVector == Ogre::Vector3::ZERO)
	{
		// decay (one third speed)
		mCurrentSpeed -= evt.timeSinceLastFrame * 0.3f;
		mTranslateVector = lastMotion;
	}
	else
	{
		// ramp up
		mCurrentSpeed += evt.timeSinceLastFrame;

	}
	// Limit motion speed
	if (mCurrentSpeed > 1.0)
		mCurrentSpeed = 1.0;
	if (mCurrentSpeed < 0.0)
		mCurrentSpeed = 0.0;

	mTranslateVector *= mCurrentSpeed;

	if( !mMouse->buffered() || !mKeyboard->buffered() || !buffJ )
		moveCamera();

	return true;
}
