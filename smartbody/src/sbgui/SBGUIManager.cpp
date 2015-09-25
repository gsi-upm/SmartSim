#if !defined(__FLASHPLAYER__)
#include "external/glew/glew.h"
#endif

#ifdef WIN32
#if _MSC_VER == 1500
#define USE_CEGUI 0
#else
#define USE_CEGUI 1
#endif
#else
//#define USE_CEGUI
#endif
#if USE_CEGUI
#include "vhcl.h"
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GLRenderer.h>
#include <FL/Fl.H>
#include <sb/SBScene.h>

#include <sr/sr_gl_render_funcs.h>
#include <sr/jpge.h>

#endif

#include "SBGUIManager.h"

SBGUIManager* SBGUIManager::_singleton = NULL;
SBGUIManager::SBGUIManager()
{
	initialized = false;
}

SBGUIManager::~SBGUIManager()
{

}


void SBGUIManager::update()
{
#if USE_CEGUI
	if (!initialized)
		return;
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_TEXTURE_2D);
	CEGUI::System::getSingleton().renderAllGUIContexts();
	glEnable(GL_DEPTH_TEST);
#endif
}

#if USE_CEGUI	
CEGUI::Key::Scan mapFlKeyToCEGUI(int flKey)
{
	CEGUI::Key::Scan ceguiKey = CEGUI::Key::Unknown;
	switch(flKey)
	{			
	case 32:
		ceguiKey = CEGUI::Key::Space;
		break;
	case FL_BackSpace:
		ceguiKey = CEGUI::Key::Backspace;
		//LOG("flKey = %d, CEGUI Backspace", flKey);
		break;
	case FL_Delete:
		ceguiKey = CEGUI::Key::Delete;
		//LOG("flKey = %d, CEGUI Delete", flKey);
		break;
	case FL_Caps_Lock:
		ceguiKey = CEGUI::Key::Capital;
		break;

	case FL_Left:
		ceguiKey = CEGUI::Key::ArrowLeft;
		//LOG("flKey = %d, CEGUI Left", flKey);
		break;
	case FL_Right:
		ceguiKey = CEGUI::Key::ArrowRight;
		//LOG("flKey = %d, CEGUI Right", flKey);
		break;
	case FL_Up:
		ceguiKey = CEGUI::Key::ArrowUp;
		//LOG("flKey = %d, CEGUI Up", flKey);
		break;
	case FL_Down:
		ceguiKey = CEGUI::Key::ArrowDown;
		//LOG("flKey = %d, CEGUI Down", flKey);
		break;
	}
	//LOG("cegui key = %d",ceguiKey);
	return ceguiKey;
}
#endif 

int SBGUIManager::handleEvent(int eventID)
{	
	if (!initialized) return 0; 
	int ret = 0;
#if USE_CEGUI	

	CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
	CEGUI::MouseButton ceguiButtons[3] = {CEGUI::LeftButton, CEGUI::RightButton, CEGUI::MiddleButton };

	int button = Fl::event_button();
	if (eventID == FL_PUSH) // mouse button push
	{
		//LOG("Button %d is pushed", button);
		context.injectMouseButtonDown(ceguiButtons[button-1]);				
	}
	else if (eventID == FL_RELEASE) // mouse button release
	{
		//LOG("Button %d is relased", button);
		context.injectMouseButtonUp(ceguiButtons[button-1]);		
	}
	else if (eventID == FL_MOVE || eventID == FL_DRAG) // mouse move
	{	
		//if (eventID == FL_MOVE) LOG("Mouse is moved to %d %d", Fl::event_x(), Fl::event_y());
		//else if (eventID == FL_DRAG) LOG("Mouse is dragged to %d %d", Fl::event_x(), Fl::event_y());
		context.injectMousePosition((float)Fl::event_x(), (float)Fl::event_y());
	}	

	int key = Fl::event_key();	
	if (eventID == FL_SHORTCUT || key == 32)
	{
		//LOG("event FL_SHORTCUT");
		context.injectKeyDown(mapFlKeyToCEGUI(key));	
		if (Fl::event_length() == 1)
			context.injectChar(*Fl::event_text());
		ret = 1;
	}
	else if (eventID == FL_KEYUP)
	{
		context.injectKeyUp(mapFlKeyToCEGUI(key));
		//if (Fl::event_length() == 1)
		//	CEGUI::System::getSingleton().injectChar(*Fl::event_text());		
	}	
#endif
	return ret;
}

void SBGUIManager::resetGUI()
{
#if USE_CEGUI

	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* oldRootWin = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
	if (oldRootWin)
	{
		winMgr.destroyWindow(oldRootWin);
	}	
	CEGUI::Window *sheet = winMgr.createWindow("DefaultWindow", "SBGUI");
	sheet->setUsingAutoRenderingSurface(false);
	sheet->setClippedByParent(false);
	//sheet->setMinSize(CEGUI::UVector2(CEGUI::UDim(0.0, 1920), CEGUI::UDim(0.0, 1200)));
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow( sheet );
#endif
}

void SBGUIManager::init()
{
	if (initialized) return; // don't init twice	
#if USE_CEGUI
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();	
#ifdef _DEBUG
	scene->run("from PyCEGUI_d import *");
	scene->run("from PyCEGUIOpenGLRenderer_d import *");
#else
	scene->run("from PyCEGUI import *");
	scene->run("from PyCEGUIOpenGLRenderer import *");
#endif

	guiRenderer = &CEGUI::OpenGLRenderer::bootstrapSystem(CEGUI::OpenGLRenderer::TTT_NONE);
	//LOG("After bootstrap system");	

	// initialize all resources
	CEGUI::DefaultResourceProvider* rp = dynamic_cast<CEGUI::DefaultResourceProvider*>
		(CEGUI::System::getSingleton().getResourceProvider());
	std::string mediaPath = scene->getMediaPath();

	LOG("setResourceGroupDirectory");
	rp->setResourceGroupDirectory("schemes", mediaPath+"/cegui/datafiles-0.8.2/schemes/");
	rp->setResourceGroupDirectory("imagesets", mediaPath+"/cegui/datafiles-0.8.2/imagesets/");
	rp->setResourceGroupDirectory("fonts", mediaPath+"/cegui/datafiles-0.8.2/fonts/");
	rp->setResourceGroupDirectory("layouts", mediaPath+"/cegui/datafiles-0.8.2/layouts/");
	rp->setResourceGroupDirectory("looknfeels", mediaPath+"/cegui/datafiles-0.8.2/looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts", mediaPath+"/cegui/datafiles-0.8.2/lua_scripts/");

	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	
	// create a default window 
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Scheme& ogreScheme = CEGUI::SchemeManager::getSingleton().createFromFile( "OgreTray.scheme" );	
	CEGUI::Scheme& taharezScheme = CEGUI::SchemeManager::getSingleton().createFromFile( "TaharezLook.scheme" );	
	CEGUI::Scheme& windowScheme = CEGUI::SchemeManager::getSingleton().createFromFile( "WindowsLook.scheme" );	
	CEGUI::Scheme& alfiskoScheme = CEGUI::SchemeManager::getSingleton().createFromFile( "AlfiskoSkin.scheme" );	
	CEGUI::Scheme& vanillaScheme = CEGUI::SchemeManager::getSingleton().createFromFile( "VanillaSkin.scheme" );
	
	
	//CEGUI::Imageset& imageSet = CEGUI::ImagesetManager::getSingleton().get("OgreTrayImages");
	//imageSet.setAutoScalingEnabled(false);
	//CEGUI::Font& font = CEGUI::FontManager::getSingleton().get("DejaVuSans-10");
	//font.setAutoScaled(false);
	CEGUI::FontManager& fontManager = CEGUI::FontManager::getSingleton();		
	CEGUI::FontManager::FontIterator pi = fontManager.getIterator();
	while ( !pi.isAtEnd () )   {
		CEGUI::Font* curFont = dynamic_cast<CEGUI::Font*>(pi.getCurrentValue());
		if ( curFont ) 
		{
			//LOG("Font name = %s",curFont->getName().c_str());
			curFont->setAutoScaled(CEGUI::ASM_Disabled);
			curFont->setNativeResolution(CEGUI::Sizef(800,600));
		}
		pi++;
	}
	//CEGUI::Font& font = CEGUI::FontManager.getSingleton().get("")
	//CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

	resetGUI();

		
	initialized = true;
	
	// create other widgets on screen
	
	//Create the quit button
// 	CEGUI::Window *sheet = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
//  	CEGUI::Window *quit = winMgr.createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
//  	//quit->setText("Quit");
//  	quit->setSize(CEGUI::USize(CEGUI::UDim(0.0, 200), CEGUI::UDim(0.0, 60)));
// 	quit->setAlpha(0.3);
//  	sheet->addChild(quit);
		
	/*
	CEGUI::Window *button = winMgr.createWindow("OgreTray/StaticText", "CEGUIDemo/Button2");
	button->setText("Button2 Text");	
	button->setProperty("TextColours","FFFF0000");
	button->setProperty("BackgroundEnabled", "false"); 
	button->setProperty("FrameEnabled", "false"); 		
	button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.10, 0.0)));
	button->setSize(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.10, 0)));
	sheet->addChildWindow(button);
	*/
	//quit->subscribeEvent(CEGUI::PushButton::EventClicked, &testCEGUIButtonPush);
	//LOG("Finish create CEGUI");	
#endif
}



void SBGUIManager::resize( int w, int h )
{
#if USE_CEGUI
	if (initialized)
	{
		CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Sizef((float)w,(float)h));
		CEGUI::FontManager& fontManager = CEGUI::FontManager::getSingleton();		
		fontManager.notifyDisplaySizeChanged(CEGUI::Sizef((float)w,(float)h));
		CEGUI::Window* sheet = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
		sheet->setClippedByParent(false);
		sheet->setUsingAutoRenderingSurface(false);		
	}
#endif
}
