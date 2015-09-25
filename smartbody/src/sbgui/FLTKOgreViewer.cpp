#include <sb/SBScene.h>
#include "FLTKListener.h"

#ifdef INTMAX_C 
#undef INTMAX_C
#endif
#ifdef UINTMAX_C
#undef UINTMAX_C
#endif
#include "external/glew/glew.h"

#include "EmbeddedOgre.h"
#include "FLTKOgreViewer.h"
#include "SBOgreListener.h"
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sbm/GPU/SbmTexture.h>
#include <FL/x.H>
#include <sr/sr_gl.h>
#include <sbm/Heightfield.h>
#include "SBGUIManager.h"
#include <sb/SBAttribute.h>

FLTKOgreWindow::FLTKOgreWindow( int x, int y, int w, int h, const char *label/*=0 */ ) : FltkViewer(x,y,w,h,label)//Fl_Gl_Window(x,y,w,h,label), SrViewer(x, y, w, h)
{
	ogreInterface = NULL;
}

FLTKOgreWindow::~FLTKOgreWindow()
{

}

void FLTKOgreWindow::show_viewer()
{
	show();
	if (!ogreInterface)
	{
		initOgreWindow();
		make_current();		
		SBGUIManager::singleton().init();			
		SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
		fltkListener = new OgreListener(ogreInterface);
		sbScene->addSceneListener(fltkListener);
	}
}

void FLTKOgreWindow::hide_viewer()
{

}

void FLTKOgreWindow::initOgreWindow()
{
	//fl_open_display();
	//GLChoiceType* glChoice = GLChoiceType::find( CAP_DEPTH_BUFFER | CAP_DOUBLE_BUFFER, NULL );
	//Fl_X::make_xid( this, glChoice->vis, glChoice->colormap );	
    
	void* flHwnd = (void*)fl_xid(this); // get hwnd	
	void* flParentHwnd = NULL;
	if (parent())
	{
		Fl_Window* parentWindow = dynamic_cast<Fl_Window*>(parent());
		if (!parentWindow)
			parentWindow = dynamic_cast<Fl_Window*>(parent()->parent());
		flParentHwnd = (void*)fl_xid(dynamic_cast<Fl_Window*>(parentWindow));
	}
	make_current();
    unsigned long fltkGLContext = (unsigned long)this->context();
	printf("ogreWindow, GLContext = %d\n",fltkGLContext);
    
	ogreInterface = new EmbeddedOgre();
	ogreInterface->createOgreWindow(flHwnd, flParentHwnd, fltkGLContext, w(), h(), "OgreWindow");	     
	updateOptions();    
	//fl_set_gl_context(this,ogreInterface->getGLContext());	
}

EmbeddedOgre* FLTKOgreWindow::getOgreInterface()
{
	return ogreInterface;
}

void FLTKOgreWindow::draw()
{	
	//fl_set_gl_context(this,ogreInterface->getGLContext());
	make_current();
	
	if (ogreInterface)
		static int counter = 0;
    //if (1)
	{
		
 		//glPushAttrib( GL_ALL_ATTRIB_BITS );
       	glPushClientAttrib( GL_CLIENT_ALL_ATTRIB_BITS );
 		glMatrixMode( GL_COLOR );
		glPushMatrix();
		glMatrixMode( GL_TEXTURE );
		glPushMatrix();
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();		
		updateOgreCamera();
		ogreInterface->updateOgreLights();
		ogreInterface->updateOgreCharacterRenderMode(false);
		ogreInterface->update(); // do Ogre rendering for deformable characters
		// pop everything!
		glMatrixMode( GL_COLOR );
		glPopMatrix();
		glMatrixMode( GL_TEXTURE );
		glPopMatrix();
		glMatrixMode( GL_PROJECTION );
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();
 		glPopClientAttrib();        
		
 		//glPopAttrib(); 		
		glViewport ( 0, 0, w(), h() );
		glClearColor ( _data->bcolor );
		
		//glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		fltkRender2(); // let old fltk viewer render rest of stuffs
		
		
		//FltkViewer::draw();
		
		//SBGUIManager::singleton().update();
 		//ogreInterface->finishRender();
	}
	//FltkViewer::draw();
	//Ogre::WindowEventUtilities::messagePump();

	
}


void FLTKOgreWindow::updateOgreCamera()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	if (!ogreInterface)
		return;
	Ogre::Camera* ogreCam = ogreInterface->getCamera();
	if (!ogreCam)
		return;
	//SrCamera& cam = *FltkViewer::_data->camera;
	SrCamera& cam = *scene->getActiveCamera();
	// override the ogre camera with fltk camera
	SrMat viewMat;
	cam.get_view_mat(viewMat);	
	SrQuat q = SrQuat(viewMat);
	SrVec  p = cam.getEye();
	ogreCam->setOrientation(Ogre::Quaternion(q.w,q.x,q.y,q.z).Inverse());
	ogreCam->setPosition(Ogre::Vector3(p.x,p.y,p.z));			
	//ogreCam->setOrientation()	
	ogreCam->setFarClipDistance(cam.getFarPlane());
	ogreCam->setNearClipDistance(cam.getNearPlane());
	//cam.zfar = ogreCam->getFarClipDistance();
	//cam.znear = ogreCam->getNearClipDistance();
	ogreCam->setFOVy(Ogre::Radian(cam.getFov()));

//
//	// OLD VERSION for visibility test: Requires OgreCam->isVisible, new version is implemented in scene->checkVisibility();
//
//	// Checks visibility 
//	const std::vector<std::string>& pawns = scene->getPawnNames();
//	for (	std::vector<std::string>::const_iterator pawnIter = pawns.begin();
//			pawnIter != pawns.end();
//			pawnIter++)
//	{
//		SmartBody::SBPawn* pawn		= scene->getPawn((*pawnIter));
//
//		SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
//		if (!camera) {
//			SrBox pawn_bb				= pawn->getBoundingBox();
//			Ogre::Vector3 point_min		= Ogre::Vector3(pawn_bb.a.x, pawn_bb.a.y, pawn_bb.a.z); 
//			Ogre::Vector3 point_max		= Ogre::Vector3(pawn_bb.b.x, pawn_bb.b.y, pawn_bb.b.z); 
//
//			if(ogreCam->isVisible(point_min) || ogreCam->isVisible(point_max))
//				LOG("Pawn %s is visible", pawn->getName().c_str());
//		}
//
//	}

}

void drawFloor(bool shadowPass)
{
	// floor is managed separately
	// this function overrides the drawFloor() in FltkViewer
}

void FLTKOgreWindow::fltkRender()
{

	//FltkViewer::draw();
    //return;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	glEnable(GL_DEPTH_TEST);
// 	if (_objManipulator.hasPicking())
// 	{
// 		SrVec2 pick_loc = _objManipulator.getPickLoc();
// 		_objManipulator.picking(pick_loc.x,pick_loc.y, scene->getActiveCamera());	   
// 	}  

	glViewport ( 0, 0, w(), h() );
	SrLight &light = _data->light;
	SrCamera &cam  = *scene->getActiveCamera();
	SrMat mat ( SrMat::NotInitialized );
    
    
    //glClearColor ( _data->bcolor );
    //glClearColor(1.f,0.f,0.f,1.f);
    //glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//----- Set Projection ----------------------------------------------
	cam.setAspectRatio((float)w()/(float)h());

	glMatrixMode ( GL_PROJECTION );
	glLoadMatrix ( cam.get_perspective_mat(mat) );

	//----- Set Visualisation -------------------------------------------
	glMatrixMode ( GL_MODELVIEW );
	glLoadMatrix ( cam.get_view_mat(mat) );

	glScalef ( cam.getScale(), cam.getScale(), cam.getScale() );

	updateLights();
	glEnable ( GL_LIGHTING );
	for (size_t x = 0; x < _lights.size(); x++)
	{
		glLight ( x, _lights[x] );		
	}	
	static GLfloat mat_emissin[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_ambient[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_diffuse[] = { 1.0,  1.0,    1.0,    1.0 };
	static GLfloat mat_speclar[] = { 0.0,  0.0,    0.0,    1.0 };
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, mat_emissin );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_speclar );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
	glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_NORMALIZE );

	if (_data->terrainMode == FltkViewer::ModeTerrain)
	{
		Heightfield* h = SmartBody::SBScene::getScene()->getHeightfield();		
		if (h)
			h->render(0);
	}
	else if (_data->terrainMode == FltkViewer::ModeTerrainWireframe)
	{
		Heightfield* h = SmartBody::SBScene::getScene()->getHeightfield();		
		if (h)
			h->render(1);
	}		

	glDisable( GL_COLOR_MATERIAL );
	//drawAllGeometries();		
	if( SmartBody::SBScene::getScene()->getRootGroup() )	{		
		_data->render_action.apply ( SmartBody::SBScene::getScene()->getRootGroup() );
	} 

	drawPawns();
	// draw the grid
	//   if (gridList == -1)
	//	   initGridList();
	drawGrid();
	glDisable(GL_BLEND);
	drawSteeringInfo();
	drawEyeBeams();
	drawEyeLids();
	drawDynamics();
	drawLocomotion();
	if (_data->showcollisiongeometry)
		drawCharacterPhysicsObjs();
	drawInteractiveLocomotion();

	//_posControl.Draw();
	_objManipulator.draw(cam);
	// feng : debugging draw for reach controller
	drawReach();

	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
	SBGUIManager::singleton().update();
}

void FLTKOgreWindow::render()
{
	redraw(); 
}

void FLTKOgreWindow::resize( int x, int y, int w, int h )
{
	make_current();
	Fl_Gl_Window::resize(x,y,w,h);
	if (!ogreInterface)
		return;
	Ogre::RenderWindow* win = ogreInterface->getRenderWindow();
	if (win) 
	{
		win->resize(w,h);		
		win->windowMovedOrResized();
	}
	SBGUIManager::singleton().resize(w,h);
	redraw();
}

void FLTKOgreWindow::menu_cmd( MenuCmd c, const char* label )
{
	// override the GPU deformable model to let Ogre3D do the rendering
	if (c == CmdCharacterShowDeformableGeometryGPU || c == CmdCharacterShowDeformableGeometry)
	{
		SbmDeformableMeshGPU::disableRendering = true;		
		_data->charactermode = ModeShowDeformableGeometryGPU;				
		_data->showgeometry = false;
		_data->showcollisiongeometry = false;
		_data->showdeformablegeometry = false;
		_data->showbones = false;
		_data->showaxis = false;	
		_data->showSkinWeight = false;
		applyToCharacters();
		ogreInterface->setCharacterVisibility(true);
		FltkViewer::menu_cmd(c,label);
	}	
	else
	{
		if (c ==  CmdCharacterShowGeometry || c == 	CmdCharacterShowCollisionGeometry 
			|| c == CmdCharacterShowBones || c == CmdCharacterShowAxis || c == CmdCharacterShowSkinWeight)
		{
			SbmDeformableMeshGPU::disableRendering = false;
			ogreInterface->setCharacterVisibility(false);

			std::cout << "Test\n";
		}
		FltkViewer::menu_cmd(c,label);
	}
	//FltkViewer::menu_cmd(c,label);
}

void FLTKOgreWindow::resetViewer()
{	
	ogreInterface->resetOgreScene();
	FltkViewer::resetViewer();
	updateOptions();
}

int printOglError(char *file, int line)
{
	/*
    GLenum glErr;
    int    retCode = 0;
#if 0 // disable for now
    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n",
			     file, line, gluErrorString(glErr));
        retCode = 1;
    }
#endif

    return retCode;
	*/
	return 0;
}

void FLTKOgreWindow::fltkRender2()
{
	printOglError("fltkRender2()", 1);
	

	if ( !visible() ) return;

	SrCamera* cam = SmartBody::SBScene::getScene()->getActiveCamera();
	SbmShaderManager& ssm = SbmShaderManager::singleton();
	SbmTextureManager& texm = SbmTextureManager::singleton();


	bool hasShaderSupport = false;
	if (!context_valid())
	{			
		hasShaderSupport = ssm.initGLExtension();
		// no shadow map in Ogre view
		//if (hasShaderSupport)
			//initShadowMap();		
	}

	glPushAttrib(GL_ENABLE_BIT);
	
	if ( !valid() ) 
	{
		// window resize				
		init_opengl ( w(), h() ); // valid() is turned on by fltk after draw() returns
		//hasShaderSupport = SbmShaderManager::initGLExtension();	   
		SBGUIManager::singleton().resize(w(),h());
	} 	
	//make_current();
	//wglMakeCurrent(fl_GetDC(fl_xid(this)),(HGLRC)context());
	//LOG("viewer GL context = %d, current context = %d",context(), wglGetCurrentContext());	
	
	bool hasOpenGL = ssm.initOpenGL();   
	// init OpenGL extension
	if (hasOpenGL)
	{
		hasShaderSupport = ssm.initGLExtension();		
	}   
	// update the shader map  
	if (hasShaderSupport)
	{
		ssm.buildShaders();
		texm.updateTexture();
	}	

// 	if (_objManipulator.hasPicking())
// 	{
// 		SrVec2 pick_loc = _objManipulator.getPickLoc();
// 		_objManipulator.picking(pick_loc.x,pick_loc.y, cam);	   
// 	}  


	//glViewport ( 0, 0, w(), h() );
	SrLight &light = _data->light;

	SrMat mat ( SrMat::NotInitialized );


	//----- Clear Background --------------------------------------------
	//glClearColor ( _data->bcolor );
	//glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// if there is no active camera, then only show the blank screen
	if (!cam)
		return;

	//----- Set Projection ----------------------------------------------
	cam->setAspectRatio((float)w()/(float)h());

	glMatrixMode ( GL_PROJECTION );
	glLoadMatrix ( cam->get_perspective_mat(mat) );

	//----- Set Visualisation -------------------------------------------
	glMatrixMode ( GL_MODELVIEW );
	glLoadMatrix ( cam->get_view_mat(mat) );

	glScalef ( cam->getScale(), cam->getScale(), cam->getScale() );

	updateLights();
	glEnable ( GL_LIGHTING );
	for (size_t x = 0; x < _lights.size(); x++)
	{
		glLight ( x, _lights[x] );		
	}

	static GLfloat mat_emissin[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_ambient[] = { 0.0,  0.0,    0.0,    1.0 };
	static GLfloat mat_diffuse[] = { 1.0,  1.0,    1.0,    1.0 };
	static GLfloat mat_speclar[] = { 0.0,  0.0,    0.0,    1.0 };
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, mat_emissin );
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_speclar );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
	glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_NORMALIZE );
	//----- Render user scene -------------------------------------------
	glDisable( GL_COLOR_MATERIAL );
	//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	//if (_data->shadowmode == ModeShadows && hasShaderSupport)
	//	makeShadowMap();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);

	
	
	if (hasShaderSupport)
	{
		glBindBuffer( GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	printOglError("fltkRender2()", 3);

	// real surface geometries
	//drawAllGeometries();	
	//if (_data->showSkinWeight)
	if (1)
	{
		drawDeformableModels();
		//drawAllGeometries();
	}
	

	if( SmartBody::SBScene::getScene()->getRootGroup() )	{		
		_data->render_action.apply ( SmartBody::SBScene::getScene()->getRootGroup() );
	} 

	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	drawPawns();
	// draw the grid
	//   if (gridList == -1)
	//	   initGridList();	
	drawNavigationMesh();

	drawGrid();
	drawSteeringInfo();
	drawCollisionInfo();
	drawEyeBeams();
	drawGazeJointLimits();
	drawEyeLids();
	drawDynamics();
	drawLocomotion();
	drawGestures();
	drawJointLabels();

	drawMotionVectorFlow();
	drawPlotMotion();
	drawDynamicVisuals();
	


	//drawKinematicFootprints(0);


	static GLfloat terrainMatDiffuse[] = { 0.8f,  0.8f,    0.5f,    1.f };
	static GLfloat terrainMatSpecular[] = { 0.f,  0.f,    0.f,    1.f }; 
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, terrainMatDiffuse );	
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, terrainMatSpecular );
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
	glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );

	if (_data->terrainMode == FltkViewer::ModeTerrain)
	{
		Heightfield* h = SmartBody::SBScene::getScene()->getHeightfield();		
		if (h)
			h->render(0);
	}
	else if (_data->terrainMode == FltkViewer::ModeTerrainWireframe)
	{
		Heightfield* h = SmartBody::SBScene::getScene()->getHeightfield();
		if (h)
			h->render(1);
	}



	if (_data->showcollisiongeometry)
		drawCharacterPhysicsObjs();
	if (_data->showBoundingVolume)
		drawCharacterBoundingVolumes();

	drawInteractiveLocomotion();	
	//_posControl.Draw();
	_objManipulator.draw(*cam);
	// feng : debugging draw for reach controller
	drawReach();

	_data->fcounter.stop();

// 	if ( !_data->message.empty() )
// 	{
// 		gl_draw_string ( _data->message.c_str(), -1, -1 );
// 	}
// 	else if ( _data->statistics )
// 	{
// 		_data->message = vhcl::Format( "FPS:%5.2f frame(%2.0f):%4.1fms render:%4.1fms", 
// 			_data->fcounter.mps(),
// 			_data->fcounter.measurements(),
// 			_data->fcounter.loopdt()*1000.0,
// 			_data->fcounter.meandt()*1000.0 );
// 		gl_draw_string ( _data->message.c_str(), -1.0f, -1.0f );
// 		_data->message = "";
// 	}

	if (_retargetStepWindow)
	{
		_retargetStepWindow->redraw();
	}

	// draw UI
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_LIGHTING);
	if (hasShaderSupport)
	{
		glBindBuffer( GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}
		
	glBindTexture(GL_TEXTURE_2D,0);
	
	SBGUIManager::singleton().update();

	glPopAttrib();
	
}

void FLTKOgreWindow::notify(SmartBody::SBSubject* subject)
{
	FltkViewer::notify(subject);

	SmartBody::SBAttribute* attr = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attr)
	{
		if (attr->getObject() == SmartBody::SBScene::getScene())
		{
			SmartBody::BoolAttribute* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attr);
			if (boolAttribute)
			{
				if (boolAttribute->getName() == "scale")
				{
					try {
						Ogre::Entity * pPlaneEnt = ogreInterface->getSceneManager()->getEntity( "plane" );
						Ogre::MeshPtr planeMesh = pPlaneEnt->getMesh();
						// modify the scale of the plane mesh. Can this be done?
						// ...
						// ...
					} catch ( Ogre::Exception&) {
					}

				}
			}
		}
	}
}

void FLTKOgreWindow::updateOptions()
{
	std::string floorMaterial = SmartBody::SBScene::getScene()->getStringAttribute("floorMaterial");
	ogreInterface->updateOgreFloor(_data->floorColor, floorMaterial, _data->showFloor);
	ogreInterface->updateOgreEnvironment(background(), _data->shadowmode);
}


#if 0
// Ogre viewer factory
SrViewer* OgreViewerFactory::s_viewer = NULL;
OgreViewerFactory::OgreViewerFactory()
{
	s_viewer = NULL;
}
SrViewer* OgreViewerFactory::create(int x, int y, int w, int h)
{
	if (!s_viewer)
		s_viewer = new FLTKOgreWindow(x, y, w, h, "SmartBodyOgre");
	return s_viewer;
}
void OgreViewerFactory::remove(SrViewer* viewer)
{
	if (viewer && (viewer == s_viewer))
	{
		viewer->hide_viewer();
	}
}



#endif
