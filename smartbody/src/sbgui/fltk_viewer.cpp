/*
 *  sr_viewer.cpp - part of SBM: SmartBody Module
 *  Copyright (C) 2008  University of Southern California
 *
 *  SBM is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SBM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SBM.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 */

#include "FL/Fl_Slider.H"  // before vhcl.h because of LOG enum which conflicts with vhcl::Log
#include "vhcl.h"
#include "external/glew/glew.h"
#include "external/SOIL/SOIL.h"
//#include <FL/enumerations.H>
#if !defined (__ANDROID__) && !defined(SBM_IPHONE) // disable shader support
#include "sbm/GPU/SbmShader.h"
#include "sbm/GPU/SbmTexture.h"
#endif
# include <sbm/GPU/SbmDeformableMeshGPU.h>
# include "fltk_viewer.h"
# include <FL/Fl.H>
# include <FL/gl.h>
# include <FL/glut.H>
//# include <fltk/visual.h>
//# include <fltk/compat/FL/Fl_Menu_Item.H>
# include <FL/fl_draw.H>
# include <FL/Fl_Color_Chooser.H>
# include <FL/Fl_File_Chooser.H>
# include <FL/Fl_Browser.H>
//# include <fltk/ToggleItem.h>
# include <sr/sr_box.h>
# include <sr/sr_sphere.h>
# include <sr/sr_cylinder.h>
# include <sr/sr_quat.h>
# include <sr/sr_line.h>
# include <sr/sr_plane.h>
# include <sr/sr_event.h>
# include <sr/sr_string.h>

# include <sr/sr_gl.h>
# include <sr/sr_camera.h>
# include <sr/sr_trackball.h>
# include <sr/sr_lines.h>
# include <sr/sr_color.h>
# include <sr/sr_points.h>
# include <sr/sr_euler.h>

# include <sr/sr_sn_matrix.h>
# include <sr/sr_sn.h>
# include <sr/sr_sn_group.h>

# include <sr/sr_sa.h>
# include <sr/sr_sa_event.h>
# include <sr/sr_gl_render_funcs.h>
# include <controllers/me_ct_eyelid.h>
# include <controllers/me_ct_data_driven_reach.hpp>
# include <controllers/MeCtBodyReachState.h>
# include <controllers/me_ct_example_body_reach.hpp>
# include <controllers/me_ct_constraint.hpp>
# include <controllers/me_ct_param_animation.h>
# include <controllers/me_ct_scheduler2.h>
# include <controllers/me_controller_tree_root.hpp>

# include <sb/SBJointMap.h>
# include <sb/SBColObject.h>
# include <sb/PABlend.h>
# include <sb/SBScene.h>
# include <sb/SBSkeleton.h>
# include <sb/SBCharacter.h>
# include <sb/SBSteerManager.h>
# include <sb/SBSteerAgent.h>
# include <sb/SBAnimationStateManager.h>
# include <sb/SBCollisionManager.h>
# include <sb/SBJointMapManager.h>
# include <sb/SBBehaviorSetManager.h>
# include <sb/SBSimulationManager.h>
# include <sb/SBAssetManager.h>
# include <sb/SBBmlProcessor.h>
# include <sb/SBNavigationMesh.h>

#include <boost/version.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <sbm/Heightfield.h>
#include <bml/bml_processor.hpp>

#include "jointmapviewer/JointMapViewer.h"
#include "jointmapviewer/RetargetStepWindow.h"

#include <sbm/PPRAISteeringAgent.h>
#include "SBGUIManager.h"
#include <autorig/SBAutoRigManager.h>
#include <sbm/sbm_deformable_mesh.h>
#include "FLTKListener.h"
#include "RootWindow.h"
#include "SBInterfaceListener.h"

#include <sbm/GPU/SbmBlendFace.h>
#include <sbm/GPU/SbmTexture.h>

#include <sr/jpge.h>

/*
#define USE_CEGUI 1
#if USE_CEGUI
#include <CEGUI.h>
#include "RendererModules/OpenGL/CEGUIOpenGLRenderer.h"
#endif
*/
//#include <sbm/SbmShader.h>

//#include "Heightfield.h"

////# define SR_USE_TRACE1  // basic fltk events
////# define SR_USE_TRACE2  // more fltk events
////# define SR_USE_TRACE3  // sr translated events
////# define SR_USE_TRACE4  // view_all
////# define SR_USE_TRACE5  // timeout
//# include <sr/sr_trace.h>

const int SHADOW_MAP_RES = 2048;

std::string Std_VS =
"varying vec4 vPos;\n\
varying vec3 normal;\n\
void main()\n\
{\n\
	vPos = gl_TextureMatrix[7]* gl_ModelViewMatrix * gl_Vertex;\n\
	gl_Position = ftransform();\n\
	normal = normalize(gl_NormalMatrix * gl_Normal);\n\
	gl_FrontColor = gl_FrontMaterial.diffuse*gl_LightSource[0].diffuse *vec4(max(dot(normal, normalize(gl_LightSource[0].position.xyz)), 0.0));\n\
	//gl_FrontColor = color;\n\
	gl_TexCoord[0] = gl_MultiTexCoord0;\n\
}";
std::string Std_FS = 
"uniform sampler2D tex;\n\
uniform sampler2D diffuseTex;\n\
uniform int useShadowMap;\n\
varying vec4 vPos;\n\
float shadowCoef()\n\
{\n\
	int index = 0;\n\
	vec4 shadow_coord = vPos/vPos.w;\n\
	shadow_coord.z += 0.000005;\n\
	float shadow_d = texture2D(tex, shadow_coord.st).x;\n\
	float diff = 1.0;\n\
	diff = shadow_d - shadow_coord.z;\n\
	return clamp( diff*850.0 + 1.0, 0.0, 1.0);\n\
}\n\
void main()\n\
{\n\
	const float shadow_ambient = 1.0;\n\
	float shadow_coef = 1.0;\n\
	if (useShadowMap == 1) \n\
		shadow_coef = shadowCoef();\n\
	vec4 texColor = texture2D(diffuseTex,gl_TexCoord[0].st);\n\
	if (length(texColor.rgb) < 0.01) texColor = vec4(1.0,1.0,1.0,1.0); \n\
	vec4 color = vec4(gl_Color.rgb*texColor.rgb, texColor.a);\n\
        gl_FragColor = vec4(color.rgb*shadow_coef*shadow_ambient,color.a);//color.a);//gl_Color*shadow_ambient * shadow_coef;\n\
}";


std::string Shadow_FS = 
"void main (void)\n\
{\n\
gl_FragColor = gl_Color;\n\
}";

//=============================== srSaSetShapesChanged ===========================================

class srSaSetShapesChanged : public SrSa
 { public :
    virtual bool shape_apply ( SrSnShapeBase* s ) { s->changed(true); return true; }
 };

//================================= popup menu ===================================================

static void menucb ( Fl_Widget* o, void* v ) 
 {
	 Fl_Widget* widget = o->parent();
	
	 std::vector<Fl_Menu_Item>* pmenu = (std::vector<Fl_Menu_Item>*) v;
	 FltkViewer* viewer = NULL;
	 while (!viewer && widget && widget->parent() != NULL)
	 {
		widget = widget->parent();
		viewer = dynamic_cast<FltkViewer*>(widget);
	 }
	 if (viewer)
		 viewer->menu_cmd((FltkViewer::MenuCmd)(uintptr_t)v,o->label());
 }

# define MCB     ((Fl_Callback*)menucb)
# define CMD(c)  ((void*)FltkViewer::c)
const int NUM_GAZE_TYPES = 4;
const int NUM_REACH_TYPES = 2;

static char gaze_on_target_menu_name[] = {"&gaze"};
static char gaze_type_name[NUM_GAZE_TYPES][40] = {"&create EYE gaze","&create EYE NECK gaze","&create EYE CHEST gaze","&create EYE BACK gaze" }; 
static std::vector<Fl_Menu_Item> gaze_submenus[NUM_GAZE_TYPES];

Fl_Menu_Item GazeMenuTable[] = 
{
	{ gaze_type_name[0],   0, MCB, 0 },			
    { gaze_type_name[1],   0, MCB, 0 },
    { gaze_type_name[2],   0, MCB, 0 },
	{ gaze_type_name[3],   0, MCB, 0 },
	{ "&remove all gazes",   0, MCB, CMD(CmdRemoveAllGazeTarget) },
	{ 0 }
};

static char body_reach_menu_name[] = {"&reach"};
Fl_Menu_Item BodyReachMenuTable[] = 
{		
	{ "&show pose examples", 0, MCB, CMD(CmdReachShowExamples), 0 },
	{ "&no pose examples", 0, MCB, CMD(CmdReachNoExamples), 0 },	
	{ 0 }
};

Fl_Menu_Item MenuTable[] =
 { 
   { "&view all",   0, MCB, CMD(CmdViewAll) },
   { "&background", 0, MCB, CMD(CmdBackground) }, // FL_FL_MENU_DIVIDER

   { "&draw style", 0, 0, 0, FL_SUBMENU },
         { "&as is",   0, MCB, CMD(CmdAsIs),    FL_MENU_RADIO },
         { "d&efault", 0, MCB, CMD(CmdDefault), FL_MENU_RADIO },
         { "&smooth",  0, MCB, CMD(CmdSmooth),  FL_MENU_RADIO },
         { "&flat",    0, MCB, CMD(CmdFlat),    FL_MENU_RADIO },
         { "&lines",   0, MCB, CMD(CmdLines),   FL_MENU_RADIO },
         { "&points",  0, MCB, CMD(CmdPoints),  FL_MENU_RADIO },
         { 0 },
	{ "&shadows", 0, 0, 0, FL_SUBMENU },
         { "&no shadows",   0, MCB, CMD(CmdNoShadows),    FL_MENU_RADIO },
         { "&shadows",  0, MCB, CMD(CmdShadows),  FL_MENU_RADIO },
         { 0 },
    { "&characters", 0, 0, 0, FL_SUBMENU },
         { "&geometry", 0, MCB, CMD(CmdCharacterShowGeometry), FL_MENU_RADIO },
         { "&collision geometry", 0, MCB, CMD(CmdCharacterShowCollisionGeometry),   FL_MENU_RADIO },
         { "&deformable geometry", 0, MCB, CMD(CmdCharacterShowDeformableGeometry),   FL_MENU_RADIO },
		 { "&gpu deformable geometry", 0, MCB, CMD(CmdCharacterShowDeformableGeometryGPU),   FL_MENU_RADIO },
         { "&bones",   0, MCB, CMD(CmdCharacterShowBones),   FL_MENU_RADIO },
         { "&axis",   0, MCB, CMD(CmdCharacterShowAxis),   FL_MENU_RADIO },
         { 0 },
    { "p&references", 0, 0, 0, FL_SUBMENU },
		 { "&axis",         0, MCB, CMD(CmdAxis),        FL_MENU_TOGGLE },
		 { "b&ounding box", 0, MCB, CMD(CmdBoundingBox), FL_MENU_TOGGLE },
		 { "&statistics",   0, MCB, CMD(CmdStatistics),  FL_MENU_TOGGLE },
		 { 0 },
	{ "&pawns", 0, 0, 0, FL_SUBMENU },
		 { "&create pawn", 0, MCB, CMD(CmdCreatePawn), FL_MENU_DIVIDER},		 
         { "&no pawns shown", 0, MCB, CMD(CmdNoPawns), FL_MENU_RADIO },
         { "&show pawns as spheres", 0, MCB, CMD(CmdPawnShowAsSpheres),   FL_MENU_RADIO },        
         { 0 },
    { "&constraint", 0, 0, 0, FL_SUBMENU },
	     { "&use IK constraint", 0, MCB, CMD(CmdConstraintToggleIK), FL_MENU_TOGGLE},	
		 //{ "&use balance", 0, MCB, CMD(CmdConstraintToggleBalance), FL_MENU_TOGGLE},		 
		 { "&use reference joints", 0, MCB, CMD(CmdConstraintToggleReferencePose), FL_MENU_TOGGLE },		     
		 { 0 },    
    { gaze_on_target_menu_name, 0, 0, GazeMenuTable, FL_SUBMENU_POINTER }, 	
	{ body_reach_menu_name, 0, 0, BodyReachMenuTable, FL_SUBMENU_POINTER },        
    { "&terrain", 0, 0, 0, FL_SUBMENU },
         { "&no terrain",   0, MCB, CMD(CmdNoTerrain),    FL_MENU_RADIO },
         { "&terrain wireframe",  0, MCB, CMD(CmdTerrainWireframe),  FL_MENU_RADIO },
         { "&terrain",  0, MCB, CMD(CmdTerrain),  FL_MENU_RADIO },
         { 0 },
	{ "&eye beams", 0, 0, 0, FL_SUBMENU },
         { "&no eye beams",   0, MCB, CMD(CmdNoEyeBeams),    FL_MENU_RADIO },
         { "&eye beams",  0, MCB, CMD(CmdEyeBeams),  FL_MENU_RADIO },
         { 0 },
	{ "&eye lids", 0, 0, 0, FL_SUBMENU },
         { "&no eye lids",   0, MCB, CMD(CmdNoEyeLids),    FL_MENU_RADIO },
         { "&eye lids",  0, MCB, CMD(CmdEyeLids),  FL_MENU_RADIO },
         { 0 },
	{ "&dynamics", 0, 0, 0, FL_SUBMENU },
         { "&no dynamics",   0, MCB, CMD(CmdNoDynamics),    FL_MENU_RADIO },
         { "&show COM",  0, MCB, CMD(CmdShowCOM),  FL_MENU_RADIO },
         { "&show COM and support polygon",  0, MCB, CMD(CmdShowCOMSupportPolygon),  FL_MENU_RADIO },
		 { "&show masses",   0, MCB, CMD(CmdShowMasses),  FL_MENU_TOGGLE },
		 { "&show bounding volume",   0, MCB, CMD(CmdShowBoundingVolume),  FL_MENU_TOGGLE },
         { 0 },
	{ "&locomotion", 0, 0, 0, FL_SUBMENU },
         { "&enable locomotion",   0, MCB, CMD(CmdEnableLocomotion),    FL_MENU_TOGGLE },
         //{ "&show all",  0, MCB, CMD(CmdShowLocomotionAll),  FL_MENU_TOGGLE },
         { "&show velocity",  0, MCB, CMD(CmdShowVelocity),  FL_MENU_TOGGLE },
		 { "&show orientation",   0, MCB, CMD(CmdShowOrientation),  FL_MENU_TOGGLE },
		 { "&show selection",   0, MCB, CMD(CmdShowSelection),  FL_MENU_TOGGLE },
		 { "&show kinematic footprints",   0, MCB, CMD(CmdShowKinematicFootprints),  FL_MENU_TOGGLE },
		 { "&show locomotion footprints",   0, MCB, CMD(CmdShowLocomotionFootprints),  FL_MENU_TOGGLE },
		 { "&show trajectory", 0, MCB, CMD(CmdShowTrajectory), FL_MENU_TOGGLE },
		 { "&interactive",   0, MCB, CMD(CmdInteractiveLocomotion),  FL_MENU_TOGGLE },
         { 0 },
   { 0 }
 };

int printOglError2(char *file, int line)
{
	/*
    GLenum glErr;
    int    retCode = 0;
#if 0
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

void FltkViewer::create_popup_menus()
{
	update_submenus();   
	begin();
	_data->menubut = new Fl_Menu_Button(0,0,50,50);
	_data->menubut->type(Fl_Menu_Button::POPUP23);
	_data->menubut->menu(MenuTable);   
	_data->menubut->textsize(12);	
	end();
}

Fl_Menu_Item FltkViewer::createMenuItem( const char* itemName, Fl_Callback* funcCallback, void* userData, int flag )
{
	Fl_Menu_Item menuItem = { itemName, 0, funcCallback, userData, flag};
	return menuItem;
}

void FltkViewer::initializePopUpMenus()
{
	Fl_Menu_Item temp = {0};

	selectionPopUpMenu.clear();
	selectionPopUpMenu.push_back(createMenuItem("Frame Selected Object", ((Fl_Callback*)BaseWindow::CameraFrameObjectCB), baseWin, 0));
	selectionPopUpMenu.push_back(createMenuItem("Face Camera", ((Fl_Callback*)BaseWindow::FaceCameraCB), baseWin, 0));
	selectionPopUpMenu.push_back(createMenuItem("Track Character", ((Fl_Callback*)BaseWindow::TrackCharacterCB), baseWin, 0));
	selectionPopUpMenu.push_back(createMenuItem("Rotate Around Character", ((Fl_Callback*)BaseWindow::RotateSelectedCB), baseWin, FL_MENU_DIVIDER));
	selectionPopUpMenu.push_back(createMenuItem("Delete Selected Object", ((Fl_Callback*)BaseWindow::DeleteSelectionCB), baseWin, 0));	
	selectionPopUpMenu.push_back(temp);


	nonSelectionPopUpMenu.clear();
	nonSelectionPopUpMenu.push_back(createMenuItem("Create Character...", ((Fl_Callback*)BaseWindow::CreateCharacterCB), baseWin, 0));
	nonSelectionPopUpMenu.push_back(createMenuItem("Create Pawn...", ((Fl_Callback*)BaseWindow::CreatePawnCB), baseWin, 0));
	nonSelectionPopUpMenu.push_back(createMenuItem("Create Light...", ((Fl_Callback*)BaseWindow::CreateLightCB), baseWin, 0));
	nonSelectionPopUpMenu.push_back(createMenuItem("Create Camera...", ((Fl_Callback*)BaseWindow::CreateCameraCB), baseWin, FL_MENU_DIVIDER));
	nonSelectionPopUpMenu.push_back(createMenuItem("Camera Reset", ((Fl_Callback*)BaseWindow::CameraResetCB), baseWin, 0));
	nonSelectionPopUpMenu.push_back(createMenuItem("Camera Frame All", ((Fl_Callback*)BaseWindow::CameraFrameCB), baseWin, 0));	
	nonSelectionPopUpMenu.push_back(temp);
}

void FltkViewer::createRightClickMenu( bool isSelected, int x, int y )
{
	initializePopUpMenus();
	Fl_Menu_Item* popUp = NULL;
	if (isSelected)
		popUp = &selectionPopUpMenu[0];
	else
		popUp = &nonSelectionPopUpMenu[0];
	const Fl_Menu_Item *m = popUp->popup(x, y, 0, 0, 0);
	if ( m ) m->do_callback(0, m->user_data());

}

static void get_pawn_submenus(void* user_data, std::vector<Fl_Menu_Item>& menu_list)
{
	std::vector<SbmPawn*> pawn_list;
	ObjectManipulationHandle::get_pawn_list(pawn_list);
	for (unsigned int i=0;i<pawn_list.size();i++)
	{
		SbmPawn* pawn = pawn_list[i];
		//printf("pawn name = %s\n",pawn->name);
		Fl_Menu_Item temp_pawn = { pawn->getName().c_str(), 0, MCB, user_data } ;
		menu_list.push_back(temp_pawn);		
	}

	Fl_Menu_Item temp = {0};
	menu_list.push_back(temp);
}


void FltkViewer::update_submenus()
{
	for (int i=0;i<NUM_GAZE_TYPES;i++)
	{
		Fl_Menu_Item& gaze_menu = GazeMenuTable[i];	
		gaze_menu.flags |= FL_SUBMENU_POINTER;
		std::vector<Fl_Menu_Item>& menu_list = gaze_submenus[i];
		menu_list = std::vector<Fl_Menu_Item>();
		int iCmd = FltkViewer::CmdGazeOnTargetType1+i;
		Fl_Menu_Item select_pawn = { "selected pawn",   0, MCB,((void*)iCmd)  };
		menu_list.push_back(select_pawn);			
		get_pawn_submenus(select_pawn.user_data(),menu_list);
		std::vector<Fl_Menu_Item>* pmenu = &menu_list;
		gaze_menu.user_data((void*)pmenu);				
	}
}

# undef CMD
# undef MCB

// need to set/get data to be able to share the same popup menu with many instances of viewers

static void set_menu_data ( FltkViewer::RenderMode r, FltkViewer::CharacterMode c,
                            bool axis, bool bbox, bool stat)
 {
   # define SET(i,b)  if(b) MenuTable[i].set(); else MenuTable[i].clear();
#ifdef WIN32
   # define SETO(i)   MenuTable[i].setonly();
#else
   # define SETO(i)   
#endif
   # define CMD(i)    ((uintptr_t)(MenuTable[i].user_data_))

   uintptr_t i=0;
   while ( CMD(i)!=FltkViewer::CmdAsIs ) i++;          SETO (  i+(uintptr_t)r );      
   while ( CMD(i)!=FltkViewer::CmdCharacterShowGeometry ) i++; SETO (  i+(uintptr_t)c );
   while ( CMD(i)!=FltkViewer::CmdAxis ) i++;          SET  ( i, axis );
   while ( CMD(i)!=FltkViewer::CmdBoundingBox ) i++;   SET  ( i, bbox );
   while ( CMD(i)!=FltkViewer::CmdStatistics ) i++;    SET  ( i, stat );

# undef CMD
   # undef SETO
   # undef SET
 }


//================================= Internal Structures =================================


//===================================== FltkViewer =================================

// Called when the small "cross" button to close the window is pressed
static void _callback_func ( Fl_Widget* win, void* pt )
 {
   //LOG("DBG callback_func!\n");
   FltkViewer* v = (FltkViewer*)pt;
   v->close_requested ();
 }

FltkViewer::FltkViewer ( int x, int y, int w, int h, const char *label )
//         : SrViewer(x, y, w, h) , Fl_Gl_Window ( x, y, w, h, label )
         : Fl_Gl_Window ( x, y, w, h, label ), SelectionListener()
 {
	 Fl::gl_visual( FL_RGB | FL_DOUBLE | FL_DEPTH | FL_MULTISAMPLE);//| FL_ALPHA );

   callback ( _callback_func, this );

   resizable(this);

   _data = new FltkViewerData();
   _data->setupData();
   
   _gestureData = new GestureData();

   
   
   _data->light.init();


   user_data ( (void*)(this) ); // to be retrieved by the menu callback

   create_popup_menus();   

   gridColor[0] = 0.7f;
   gridColor[1] = 0.7f;
   gridColor[2] = 0.7f;
   gridColor[3] = .5f;
   gridHighlightColor[0] = .6f;
   gridHighlightColor[1] = .6f;
   gridHighlightColor[2] = .6f;
   gridSize = 400.0;
   gridStep = 20.0;
//   gridSize = 400.0;
//   gridStep = 50.0;
   gridList = -1;
   _arrowTime = 0.0f;
   _transformMode = ObjectManipulationHandle::CONTROL_SELECTION;

   init_foot_print();
   _lastSelectedCharacter = "";   
   _retargetStepWindow = NULL;
   fltkListener = NULL;
   // register gesture event handler
	GestureVisualizationHandler* gv = new GestureVisualizationHandler();
	gv->setGestureData(_gestureData);
	SmartBody::SBEventManager* manager = SmartBody::SBScene::getScene()->getEventManager();
	manager->addEventHandler("bmlstatus", gv);

	mode( FL_RGB | FL_DOUBLE | FL_DEPTH | FL_MULTISAMPLE);
	//CEGUI::OpenGLRenderer* myRenderer = 
	//	new CEGUI::OpenGLRenderer( 0 );
	
	//make_current();	

	registerUIControls();
}




FltkViewer::~FltkViewer ()
 {
   delete _data->scenebox;
   delete _data->sceneaxis;
   delete _data;
}

void FltkViewer::registerUIControls()
{
	// add UI controls to the scene
	SmartBody::SBAttribute* attribute = NULL;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	attribute = scene->createBoolAttribute("GUI.ShowCameras",false,true,"GUI",10,false,false,false,"Show/hide the cameras.");
	attribute->registerObserver(this);
	attribute = scene->createBoolAttribute("GUI.ShowLights",false,true,"GUI",10,false,false,false,"Show/hide the lights.");
	attribute->registerObserver(this);
	attribute = scene->createBoolAttribute("GUI.ShowGrid",true,true,"GUI",10,false,false,false,"Show/hide the grid.");
	attribute->registerObserver(this);
	attribute = scene->createBoolAttribute("GUI.ShowFloor",true,true,"GUI",10,false,false,false,"Show/hide the floor.");
	attribute->registerObserver(this);
	attribute = scene->createVec3Attribute("GUI.FloorColor",0.6, 0.6, 0.6,true,"GUI",10,false,false,false,"Floor color.");
	attribute->registerObserver(this);
	attribute = scene->createVec3Attribute("GUI.BackgroundColor",0.63, 0.63, 0.63,true,"GUI",10,false,false,false,"Background color.");
	attribute->registerObserver(this);
}

void FltkViewer::draw_message ( const char* s )
 {
   if ( _data->message != s )
      redraw();
   _data->message = s;
 }

void FltkViewer::show_menu ()
 { 
	create_popup_menus();
	set_menu_data (_data->rendermode, _data->charactermode, _data->displayaxis,
                   _data->boundingbox, _data->statistics);
	const Fl_Menu_Item *m = _data->menubut->popup();	
	
	if ( m ) 
	{		
		//m->do_callback(_data->menubut,m->user_data());     
		// instead of doing the callback function, call the menu command directly.
		menu_cmd((FltkViewer::MenuCmd)(uintptr_t)m->user_data(),m->label());
	}
	//MenuTable->popup();	
 }


void FltkViewer::OnSelect(const std::string& value)
{
	SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(SmartBody::SBScene::getScene()->getObjectFromString(value));
	if (!pawn)
	{
		// deselect a pawn if it is already selected.
		SbmPawn* selectedPawn = _objManipulator.get_selected_pawn();
		if (selectedPawn)
		{
			_objManipulator.set_selected_pawn(NULL);
		}
		_objManipulator.removeActiveControl();
		return;
	}

	PawnControl* tempControl = this->_objManipulator.getPawnControl(this->_transformMode);
	this->_objManipulator.active_control = tempControl;
	this->_objManipulator.set_selected_pawn(pawn);

}

void FltkViewer::applyToCharacters()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& characterNames = scene->getCharacterNames();
	for (std::vector<std::string> ::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = scene->getCharacter(*iter);
		// set the visibility parameters of the scene
		//character->scene_p->set_visibility(_data->showbones,_data->showgeometry, _data->showcollisiongeometry, _data->showaxis);

		// feng : never show the collision mesh, instead we will show the bounding volumes as capsules
		if (character->scene_p && character->dMeshInstance_p)
		{
//			character->scene_p->set_visibility(_data->showbones,_data->showgeometry, false, _data->showaxis);
	//		character->dMeshInstance_p->setVisibility(_data->showdeformablegeometry);
		}
	}
}

void FltkViewer::menu_cmd ( MenuCmd s, const char* label  )
 {
	 bool applyToCharacter = false; 	 
	 MeCtExampleBodyReach* bodyReachCt = getCurrentCharacterBodyReachController();
	 MeCtConstraint* constraintCt = getCurrentCharacterConstraintController();

   switch ( s )
    { 
      case CmdViewAll : view_all (); break;

	  case CmdBackground:
		  {
			  SrColor c = background(); 
			  fl_color_chooser("Set background color:", c.r, c.g, c.b);
			  background(c);
			   updateOptions();
		  } break;

	  case CmdFloorColor:
		  {
			  SrColor c = _data->floorColor;
			  fl_color_chooser("Set floor color:", c.r, c.g, c.b);
			  _data->floorColor = c;
			  updateOptions();
		  } break;

      case CmdAsIs   : _data->rendermode = ModeAsIs;
					   _data->render_action.restore_render_mode ( SmartBody::SBScene::getScene()->getRootGroup());
                       break;
      case CmdDefault : _data->rendermode = ModeDefault;
                       _data->render_action.override_render_mode ( SmartBody::SBScene::getScene()->getRootGroup(), srRenderModeDefault );
                       break;
      case CmdSmooth : _data->rendermode = ModeSmooth;
                       _data->render_action.override_render_mode ( SmartBody::SBScene::getScene()->getRootGroup(), srRenderModeSmooth );
                       break;
      case CmdFlat   : _data->rendermode = ModeFlat;
                       _data->render_action.override_render_mode ( SmartBody::SBScene::getScene()->getRootGroup(), srRenderModeFlat );
                       break;
      case CmdLines  : _data->rendermode = ModeLines;
                       _data->render_action.override_render_mode ( SmartBody::SBScene::getScene()->getRootGroup(), srRenderModeLines );
                       break;
      case CmdPoints : _data->rendermode = ModePoints;
                       _data->render_action.override_render_mode ( SmartBody::SBScene::getScene()->getRootGroup(), srRenderModePoints );
                       break;

      case CmdAxis : SR_SWAPB(_data->displayaxis); 
                     if ( _data->displayaxis ) update_axis();
                     break;
	  case CmdShadows  : _data->shadowmode = ModeShadows;             
                       break;
      case CmdNoShadows : _data->shadowmode = ModeNoShadows;
                       break;
	  case CmdGrid: 
					   _data->gridMode = ModeShowGrid;
					   break;
	  case CmdNoGrid: 
					   _data->gridMode = ModeNoGrid;
					   break;
	  case CmdNoTerrain  : _data->terrainMode = ModeNoTerrain;             
                       break;
      case CmdTerrainWireframe : _data->terrainMode = ModeTerrainWireframe;
                       break;
      case CmdTerrain : _data->terrainMode = ModeTerrain;
                       break;
	  case CmdNoNavigationMesh  : _data->navigationMeshMode = ModeNoNavigationMesh;             
		  break;
	  case CmdNavigationMesh : _data->navigationMeshMode = ModeNavigationMesh;
		  break;
	  case CmdRawMesh : _data->navigationMeshMode = ModeRawMesh;
		  break;

	  case CmdNoEyeBeams  : _data->eyeBeamMode = ModeNoEyeBeams;             
                       break;
      case CmdEyeBeams: _data->eyeBeamMode = ModeEyeBeams;
                       break;
	  case CmdNoGazeLimit  : _data->gazeLimitMode = ModeNoGazeLimit;             
		  break;
	  case CmdGazeLimit: _data->gazeLimitMode = ModeGazeLimit;
		  break;

	  case CmdNoEyeLids  : _data->eyeLidMode = ModeNoEyeLids;             
                       break;
	  case CmdEyeLids: _data->eyeLidMode = ModeEyeLids;
                       break;
	  case CmdNoDynamics  : _data->dynamicsMode = ModeNoDynamics;             
                       break;
      case CmdShowCOM:		_data->dynamicsMode = ModeShowCOM;
                       break;
	  case CmdShowCOMSupportPolygon: _data->dynamicsMode = ModeShowCOMSupportPolygon;
                       break;
	  case CmdShowMasses: _data->showmasses =  !_data->showmasses;
                       break;
	  case CmdShowBoundingVolume: _data->showBoundingVolume =  !_data->showBoundingVolume;
					   break;
	  case CmdEnableLocomotion  : _data->locomotionenabled = !_data->locomotionenabled;             
                       break;
	  case CmdShowLocomotionAll  : _data->showlocomotionall = !_data->showlocomotionall;
						if(_data->showlocomotionall)
						{
							_data->showvelocity = true;
							_data->showorientation = true;
							_data->showselection = true;

						}
						else
						{
							_data->showvelocity = false;
							_data->showorientation = false;
							_data->showselection = false;
						}
                       break;
      case CmdShowVelocity  : _data->showvelocity = !_data->showvelocity;
						if(!_data->showvelocity) _data->showlocomotionall = false;
                       break;
	  case CmdShowOrientation  : _data->showorientation = !_data->showorientation;
						if(!_data->showorientation) _data->showlocomotionall = false;
                       break;
	  case CmdShowSelection  : _data->showselection = !_data->showselection;
						if(!_data->showselection) _data->showlocomotionall = false;
                       break;
	  case CmdShowKinematicFootprints  : _data->showkinematicfootprints = !_data->showkinematicfootprints;
						if(!_data->showkinematicfootprints) _data->showlocomotionall = false;
                       break;
	  case CmdShowLocomotionFootprints  : _data->showlocofootprints = !_data->showlocofootprints;
						if(!_data->showlocofootprints) _data->showlocomotionall = false;
                       break;
	  case CmdShowTrajectory : _data->showtrajectory = !_data->showtrajectory;
						if (!_data->showtrajectory) _data->showtrajectory = false;
					   break;
	  case CmdShowGesture : _data->showgesture = !_data->showgesture;
						if (!_data->showgesture) 
						{
							_gestureData->reset();
							_gestureData->toggleFeedback(false);
						}
						else
							_gestureData->toggleFeedback(true);
					   break;
		case CmdShowJoints: _data->showJointLabels = !_data->showJointLabels;
					   break;
	  case CmdInteractiveLocomotion  : _data->interactiveLocomotion = !_data->interactiveLocomotion;
                       break;
      case CmdBoundingBox : SR_SWAPB(_data->boundingbox); 
                            if ( _data->boundingbox ) update_bbox();
                            break;

      case CmdStatistics : SR_SWAPB(_data->statistics); break;
	  case CmdCharacterShowGeometry:
		  _data->charactermode = ModeShowGeometry;		  
		  _data->showgeometry = true;
		  _data->showcollisiongeometry = false;
		  _data->showdeformablegeometry = false;
		  _data->showbones = false;
		  _data->showaxis = false;
		  _data->showSkinWeight = false;
		  applyToCharacter = true;
		  break;
	  case CmdCharacterShowCollisionGeometry: 
		  _data->charactermode = ModeShowCollisionGeometry;		
		  _data->showgeometry = false;
		  _data->showcollisiongeometry = true;
		  _data->showdeformablegeometry = false;
		  _data->showbones = false;
		  _data->showaxis = false;
		  _data->showSkinWeight = false;
		  applyToCharacter = true;
		  break;
	  case CmdCharacterShowDeformableGeometry: 
		  _data->charactermode = ModeShowDeformableGeometry;		
		  SbmDeformableMeshGPU::useGPUDeformableMesh = false;
		  _data->showgeometry = false;
		  _data->showcollisiongeometry = false;
		  _data->showdeformablegeometry = true;
		  _data->showSkinWeight = false;
		  _data->showbones = false;
		  _data->showaxis = false;
		  applyToCharacter = true;
		  break;
	  case CmdCharacterShowSkinWeight: 
// 		  _data->charactermode = ModeShowDeformableGeometry;		
 		  SbmDeformableMeshGPU::useGPUDeformableMesh = false;
 		  _data->showgeometry = false;
 		  _data->showcollisiongeometry = false;
 		  _data->showdeformablegeometry = true;
 		  _data->showSkinWeight = true;
 		  _data->showbones = false;
 		  _data->showaxis = false;
		  applyToCharacter = true;
		  break;

	  case CmdCharacterShowDeformableGeometryGPU: 
		  _data->charactermode = ModeShowDeformableGeometryGPU;		
		  SbmDeformableMeshGPU::useGPUDeformableMesh = true;
		  _data->showgeometry = false;
		  _data->showcollisiongeometry = false;
		  _data->showdeformablegeometry = true;
		  _data->showbones = false;
		  _data->showaxis = false;
		  _data->showSkinWeight = false;
		  applyToCharacter = true;
		  break;
	  case CmdCharacterShowBones: 
		  _data->charactermode = ModeShowBones;		
		  _data->showgeometry = false;
		  _data->showcollisiongeometry = false;
		  _data->showdeformablegeometry = false;
		  _data->showSkinWeight = false;
		  _data->showbones = true;
		  _data->showaxis = false;
		  applyToCharacter = true;
		  break;
	  case CmdCharacterShowAxis: 
		  _data->charactermode = ModeShowAxis;		
		  _data->showgeometry = false;
		  _data->showcollisiongeometry = false;
		  _data->showdeformablegeometry = false;
		  _data->showSkinWeight = false;
		  _data->showbones = false;
		  _data->showaxis = true;
		  applyToCharacter = true;
		  break;
	  case CmdCreatePawn : 
						create_pawn();						
						break;
	  case CmdNoPawns : _data->pawnmode = ModeNoPawns;
                       break;
	  case CmdPawnShowAsSpheres  : _data->pawnmode = ModePawnShowAsSpheres;             
                       break;
	  case CmdGazeOnTargetType1:	
	  case CmdGazeOnTargetType2:
	  case CmdGazeOnTargetType3:
	  case CmdGazeOnTargetType4:
					   set_gaze_target(s-CmdGazeOnTargetType1,label);
					   break;
	  case CmdRemoveAllGazeTarget:
		               set_gaze_target(-1,NULL);
					   break;	  
	  case CmdReachShowExamples:
		  _data->reachRenderMode = ModeShowExamples;
		  break;
	  case CmdReachNoExamples:
		  _data->reachRenderMode = ModeNoExamples;
		  break;
	  case CmdNoSteer: 
		   _data->steerMode = ModeNoSteer;
		   break;
	   case CmdSteerAll: 
		   _data->steerMode = ModeSteerAll;
		   break;
	   case CmdSteerCharactersGoalsOnly:
			_data->steerMode = ModeSteerCharactersGoalsOnly;
		 break;
	   case CmdCollisionShow:
			_data->collisionMode = ModeCollisionShow;
		break;
	   case CmdCollisionHide:
			_data->collisionMode = ModeCollisionHide;
		break;
	  case CmdConstraintToggleIK:
		  if (constraintCt)
		  {
			  //bodyReachCt->useDataDriven = !reachCt->useDataDriven;
			  //bodyReachCt->useBalance = !bodyReachCt->useBalance;
			  if (constraintCt->useIKConstraint)
			  {
				  //bodyReachCt->useIKConstraint = false;
				  constraintCt->setFadeOut(2.0);
			  }
			  else
			  {
				  constraintCt->useIKConstraint = true;
				  constraintCt->setFadeIn(2.0);
			  }
		  }
		  break;
	  default:
		  break;
	}
	
	if (applyToCharacter)
	{
		applyToCharacters();						
	}
	render ();
 }



bool FltkViewer::menu_cmd_activated ( MenuCmd c )
 {
   switch ( c )
    { case CmdAsIs     : return _data->rendermode==ModeAsIs? true:false;
      case CmdDefault  : return _data->rendermode==ModeDefault? true:false;
      case CmdSmooth   : return _data->rendermode==ModeSmooth? true:false;
      case CmdFlat     : return _data->rendermode==ModeFlat? true:false;
      case CmdLines    : return _data->rendermode==ModeLines? true:false;
      case CmdPoints   : return _data->rendermode==ModePoints? true:false;
      case CmdShadows   : return _data->shadowmode==ModeShadows? true:false;
      case CmdNoShadows   : return _data->shadowmode==ModeNoShadows? true:false;
	  case CmdTerrain   : return _data->terrainMode==ModeTerrain? true:false;
      case CmdTerrainWireframe   : return _data->terrainMode==ModeTerrainWireframe? true:false;
	  case CmdNoTerrain   : return _data->terrainMode==ModeNoTerrain? true:false;
	  case CmdNoEyeBeams  : return _data->eyeBeamMode==ModeNoEyeBeams? true:false;
      case CmdEyeBeams   : return _data->eyeBeamMode==ModeEyeBeams? true:false;
	  case CmdNoEyeLids  : return _data->eyeLidMode==ModeNoEyeLids? true:false;
      case CmdEyeLids  : return _data->eyeLidMode==ModeEyeLids? true:false;
	  case CmdNoDynamics   : return _data->dynamicsMode==ModeNoDynamics? true:false;
      case CmdShowCOM   : return _data->dynamicsMode==ModeShowCOM? true:false;
	  case CmdShowCOMSupportPolygon   : return _data->dynamicsMode==ModeShowCOMSupportPolygon? true:false;
	  case CmdShowMasses : return _data->showmasses? true:false;
	  case CmdShowBoundingVolume : return _data->showBoundingVolume? true:false;
	  case CmdEnableLocomotion : return _data->locomotionenabled? true:false;
	  case CmdShowLocomotionAll : return _data->showlocomotionall? true:false;
	  case CmdShowVelocity : return _data->showvelocity? true:false;
	  case CmdShowOrientation : return _data->showorientation? true:false;
	  case CmdShowSelection : return _data->showselection? true:false;
	  case CmdShowKinematicFootprints : return _data->showkinematicfootprints? true:false;
	  case CmdShowTrajectory : return _data->showtrajectory ? true:false;
	  case CmdShowGesture : return _data->showgesture ? true:false;
	  case CmdShowLocomotionFootprints : return _data->showlocofootprints? true:false;
      case CmdAxis        : return _data->displayaxis? true:false;
      case CmdBoundingBox : return _data->boundingbox? true:false;
      case CmdStatistics  : return _data->statistics? true:false;
	  case CmdCharacterShowGeometry : return _data->showgeometry? true:false;
	  case CmdCharacterShowCollisionGeometry : return _data->showcollisiongeometry? true:false;
	  case CmdCharacterShowDeformableGeometry : return _data->showdeformablegeometry? true:false;
	  case CmdCharacterShowBones : return _data->showbones? true:false;
	  case CmdCharacterShowAxis : return _data->showaxis? true:false;
      default : return false;
    }
 }

void FltkViewer::update_bbox ()
 {
   _data->bbox_action.apply ( SmartBody::SBScene::getScene()->getRootGroup() );
   _data->scenebox->shape().init();
   _data->scenebox->shape().push_box ( _data->bbox_action.get(), true );
 }

void FltkViewer::update_axis ()
 {
   _data->bbox_action.apply ( SmartBody::SBScene::getScene()->getRootGroup() );
   SrBox b = _data->bbox_action.get();
   float len1 = SR_MAX3(b.a.x,b.a.y,b.a.z);
   float len2 = SR_MAX3(b.b.x,b.b.y,b.b.z);
   float len = SR_MAX(len1,len2);

   _data->sceneaxis->shape().init();

   _data->sceneaxis->shape().push_axis ( SrPnt::null, len, 3, "xyz" );
 }

void FltkViewer::view_all ()
 {
	SrCamera* camera = get_camera();
   camera->setCenter(SrVec::null.x, SrVec::null.y, SrVec::null.z);
   camera->setUpVector(SrVec::j);
   camera->setEye( 0, 0, 1.0f );

   if ( SmartBody::SBScene::getScene()->getRootGroup() )
    { update_bbox ();
      SrBox box = _data->bbox_action.get();

      if ( _data->displayaxis )
       { SrBox b;
         _data->sceneaxis->get_bounding_box(b);
         box.extend ( b );
       }

      float s = box.max_size();
      // _data->light.constant_attenuation = s;
      //_data->camera.view_all ( box, SR_TORAD(60) );
      camera->setScale(1.0f / s);
      //_data->camera.center = box.center();
      //_data->camera.eye = _data->camera.center;
      //_data->camera.eye.z = s.z;
    }

   render ();
 }

void FltkViewer::render () 
 { 
   redraw(); 
 } 

bool FltkViewer::iconized () 
 { 
   return _data->iconized;
 }

float FltkViewer::fps () 
 { 
   return (float)_data->fcounter.mps(); 
 }

sruint FltkViewer::curframe () 
 { 
   return (sruint) _data->fcounter.measurements(); 
 }

SrColor FltkViewer::background ()
 {
   return _data->bcolor;
 }

void FltkViewer::background ( SrColor c )
 {
   _data->bcolor = c;
 }

SrCamera* FltkViewer::get_camera()
{
	return SmartBody::SBScene::getScene()->getActiveCamera();
}

void FltkViewer::set_camera ( const SrCamera* cam )
 {
   SmartBody::SBScene::getScene()->setActiveCamera(const_cast<SrCamera*>(cam));

 //  if ( _data->viewmode==ModeExaminer )
//    { _data->trackball.init();
  //    _data->trackball.rotation.set ( cam.eye-cam.center, SrVec::k );
   // }

//   if ( _data->root )
  //  { update_bbox ();
    //  SrBox box = _data->bbox_action.get();
      //float s = box.max_size();
    //  _data->light.constant_attenuation = s;
   // }
   //else _data->light.constant_attenuation = 1.0f; // the default value
 }

static void gl_draw_string ( const char* s, float x, float y )
 {
   glMatrixMode ( GL_PROJECTION );
   glLoadIdentity ();
   glMatrixMode ( GL_MODELVIEW );
   glLoadIdentity ();
   glDisable ( GL_LIGHTING );
   glColor ( SrColor::red );
#ifdef __APPLE__
   Fl::set_font(FL_HELVETICA, 12 ); // from fltk
#else
   Fl::set_font(FL_TIMES, 12 ); // from fltk
#endif
   fl_draw(s, (int) x, (int) y );      // from fltk
 }

//-- Render  ------------------------------------------------------------------


void FltkViewer::initShadowMap()
{    

	// currently getting an error when activating shadow map, not sure why AS 12/2/14
	//return;

	// init basic shader for rendering 
	SbmShaderManager::singleton().addShader("Basic",Std_VS.c_str(),Std_FS.c_str(),false);
	SbmShaderManager::singleton().addShader("BasicShadow","",Shadow_FS.c_str(),false);
	// init shadow map and frame buffer 
	
	int depth_size = SHADOW_MAP_RES;
	//glGenTextures(1, &_data->shadowMapID);
	//LOG("Shadow map ID = %d\n",_data->shadowMapID);	
	glGenFramebuffersEXT(1, &_data->depthFB);	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _data->depthFB);
	glDrawBuffer(GL_FRONT_AND_BACK);
	

	glGenTextures(1, &_data->shadowMapID);
	glBindTexture(GL_TEXTURE_2D, _data->shadowMapID);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, depth_size, depth_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, depth_size, depth_size, 0, GL_RGBA, GL_FLOAT, NULL);	
	glBindTexture(GL_TEXTURE_2D,0);


	glGenTextures(1, &_data->depthMapID);
	glBindTexture(GL_TEXTURE_2D, _data->depthMapID);	
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, depth_size, depth_size, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);	
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    //glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, depth_size, depth_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D,0);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, _data->depthMapID,0);
	//glBindTexture(GL_TEXTURE_2D, _data->shadowMapID);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,GL_TEXTURE_2D, _data->shadowMapID, 0);

	GLenum FBOstatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(FBOstatus != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("GL_FRAMEBUFFER_COMPLETE_EXT failed, CANNOT use FBO\n");

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	/*
	glGenRenderbuffersEXT(1, &_data->rboID);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _data->rboID);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
		depth_size, depth_size);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	*/
}

void FltkViewer::init_opengl ( int w, int h )
 {   
   //sr_out<<"INIT"<<srnl;
   glViewport ( 0, 0, w, h );
   glEnable ( GL_DEPTH_TEST );
   glEnable ( GL_LIGHT0 ); 
   glEnable ( GL_LIGHTING );

   //glEnable ( GL_BLEND ); // for transparency
   //glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

   glCullFace ( GL_BACK );
   glDepthFunc ( GL_LEQUAL );
   glFrontFace ( GL_CCW );

   //glEnable ( GL_POLYGON_SMOOTH );

   //glEnable ( GL_LINE_SMOOTH );
   //glHint ( GL_LINE_SMOOTH_HINT, GL_NICEST );

   glEnable ( GL_POINT_SMOOTH );
   glPointSize ( 2.0 );

   glShadeModel ( GL_SMOOTH );

   SrCamera* cam = get_camera();
   //cam.zfar = 1000000;
   float scale = 1.f/SmartBody::SBScene::getScene()->getScale();
   
   // camera near = 0.1 m, camera far plane is 100 m
   cam->setNearPlane(0.1f*scale);
   cam->setFarPlane(10000.f*scale);


   // init shader
   //initShadowMap();
   //SbmShaderManager::singleton().addShader("Basic","",Std_FS.c_str(),false);


   updateLights();
   
 }

void FltkViewer::close_requested ()
 {
   exit ( 0 );
 }

//# include <sr/sr_sphere.h>
//static SrSnSphere* SPH=0;
//   if ( !SPH ) SPH = new SrSnSphere;
//   SPH->shape().center = light.position;
//   SPH->shape().radius = 0.6f;
//   SPH->color ( SrColor::red );
//   _data->render_action.apply ( SPH );

/////////////////////////////////////////////////////////////////////////////////////

void MakeShadowMatrix( GLfloat points[3][3], GLfloat light[4], GLfloat matrix[4][4] )	{
	GLfloat plane[ 2 ][ 3 ];
    GLfloat coeff[4];
    GLfloat dot;

    // Find the plane equation coefficients
    // Find the first three coefficients the same way we find a normal.
//    calcNormal(points,planeCoeff);

	plane[ 0 ][ 0 ] = points[ 1 ][ 0 ] - points[ 0 ][ 0 ];
	plane[ 0 ][ 1 ] = points[ 1 ][ 1 ] - points[ 0 ][ 1 ];
	plane[ 0 ][ 2 ] = points[ 1 ][ 2 ] - points[ 0 ][ 2 ];
	plane[ 1 ][ 0 ] = points[ 2 ][ 0 ] - points[ 1 ][ 0 ];
	plane[ 1 ][ 1 ] = points[ 2 ][ 1 ] - points[ 1 ][ 1 ];
	plane[ 1 ][ 2 ] = points[ 2 ][ 2 ] - points[ 1 ][ 2 ];
	
	coeff[ 0 ] = plane[ 0 ][ 1 ] * plane[ 1 ][ 2 ] - plane[ 0 ][ 2 ] * plane[ 1 ][ 1 ];
	coeff[ 1 ] = plane[ 0 ][ 2 ] * plane[ 1 ][ 0 ] - plane[ 0 ][ 0 ] * plane[ 1 ][ 2 ];
	coeff[ 2 ] = plane[ 0 ][ 0 ] * plane[ 1 ][ 1 ] - plane[ 0 ][ 1 ] * plane[ 1 ][ 0 ];

    // Find the last coefficient by back substitutions
    coeff[3] = -( ( coeff[ 0 ] * points[ 2 ][ 0 ] ) + ( coeff[ 1 ] * points[ 2 ][ 1 ] ) + ( coeff[ 2 ] * points[ 2 ][ 2 ] ) );

    // Dot product of plane and light position
    dot = coeff[0] * light[0] + coeff[1] * light[1] + coeff[2] * light[2] + coeff[3] * light[3];

    matrix[0][0] = dot - light[0] * coeff[0];
    matrix[1][0] = 0.0f - light[0] * coeff[1];
    matrix[2][0] = 0.0f - light[0] * coeff[2];
    matrix[3][0] = 0.0f - light[0] * coeff[3];

    matrix[0][1] = 0.0f - light[1] * coeff[0];
    matrix[1][1] = dot - light[1] * coeff[1];
    matrix[2][1] = 0.0f - light[1] * coeff[2];
    matrix[3][1] = 0.0f - light[1] * coeff[3];

    matrix[0][2] = 0.0f - light[2] * coeff[0];
    matrix[1][2] = 0.0f - light[2] * coeff[1];
    matrix[2][2] = dot - light[2] * coeff[2];
    matrix[3][2] = 0.0f - light[2] * coeff[3];

    matrix[0][3] = 0.0f - light[3] * coeff[0];
    matrix[1][3] = 0.0f - light[3] * coeff[1];
    matrix[2][3] = 0.0f - light[3] * coeff[2];
    matrix[3][3] = dot - light[3] * coeff[3];
}

/////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////

void FltkViewer::updateLights()
{
	
	// get any pawns called 'light#' 
	// if none exist, use the standard lights
	_lights.clear();
	int numLightsInScene = 0;
	const std::vector<std::string>& pawnNames =  SmartBody::SBScene::getScene()->getPawnNames();
	for (std::vector<std::string>::const_iterator iter = pawnNames.begin();
		 iter != pawnNames.end();
	      iter++)
	{
		SmartBody::SBPawn* sbpawn = SmartBody::SBScene::getScene()->getPawn(*iter);
		const std::string& name = sbpawn->getName();
		if (name.find("light") == 0)
		{
			numLightsInScene++;
			SmartBody::BoolAttribute* enabledAttr = dynamic_cast<SmartBody::BoolAttribute*>(sbpawn->getAttribute("enabled"));
			if (enabledAttr && !enabledAttr->getValue())
			{
				continue;
			}
			SrLight light;

			light.position = sbpawn->getPosition();

			SrQuat orientation = sbpawn->getOrientation();
			SmartBody::BoolAttribute* directionalAttr = dynamic_cast<SmartBody::BoolAttribute*>(sbpawn->getAttribute("lightIsDirectional"));
			if (directionalAttr)
			{
				light.directional = directionalAttr->getValue();
			}
			else
			{
				light.directional = true;
			}
			if (light.directional)
			{
				light.position = -SrVec(0, 1, 0) * orientation;
			}
			
			SmartBody::Vec3Attribute* diffuseColorAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightDiffuseColor"));
			if (diffuseColorAttr)
			{
				const SrVec& color = diffuseColorAttr->getValue();
				light.diffuse = SrColor( color.x, color.y, color.z );
			}
			else
			{
				light.diffuse = SrColor( 1.0f, 0.95f, 0.8f );
			}
			SmartBody::Vec3Attribute* ambientColorAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightAmbientColor"));
			if (ambientColorAttr)
			{
				const SrVec& color = ambientColorAttr->getValue();
				light.ambient = SrColor( color.x, color.y, color.z );
			}
			else
			{
				light.ambient = SrColor( 0.0f, 0.0f, 0.0f );
			}
			SmartBody::Vec3Attribute* specularColorAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightSpecularColor"));
			if (specularColorAttr)
			{
				const SrVec& color = specularColorAttr->getValue();
				light.specular = SrColor( color.x, color.y, color.z );
			}
			else
			{
				light.specular = SrColor( 0.0f, 0.0f, 0.0f );
			}
			SmartBody::DoubleAttribute* spotExponentAttr = dynamic_cast<SmartBody::DoubleAttribute*>(sbpawn->getAttribute("lightSpotExponent"));
			if (spotExponentAttr)
			{
				light.spot_exponent = (float) spotExponentAttr->getValue();
			}
			else
			{
				light.spot_exponent = 0.0f;
			}
			SmartBody::Vec3Attribute* spotDirectionAttr = dynamic_cast<SmartBody::Vec3Attribute*>(sbpawn->getAttribute("lightSpotDirection"));
			if (spotDirectionAttr)
			{
				const SrVec& direction = spotDirectionAttr->getValue();
				light.spot_direction = direction;
				// override the explicit direction with orientation
				light.spot_direction = SrVec(0, 1, 0) * orientation;
			}
			else
			{
				light.spot_direction = SrVec( 0.0f, 0.0f, -1.0f );
			}
			SmartBody::DoubleAttribute* spotCutOffAttr = dynamic_cast<SmartBody::DoubleAttribute*>(sbpawn->getAttribute("lightSpotCutoff"));
			if (spotExponentAttr)
			{
				if (light.directional)
					light.spot_cutoff = 180.0f;
				else
					light.spot_cutoff = (float) spotCutOffAttr->getValue();

			}
			else
			{
				light.spot_cutoff = 180.0f;
			}
			SmartBody::DoubleAttribute* constantAttentuationAttr = dynamic_cast<SmartBody::DoubleAttribute*>(sbpawn->getAttribute("lightConstantAttenuation"));
			if (constantAttentuationAttr)
			{
				light.constant_attenuation = (float) constantAttentuationAttr->getValue();
			}
			else
			{
				light.constant_attenuation = 1.0f;
			}
			SmartBody::DoubleAttribute* linearAttentuationAttr = dynamic_cast<SmartBody::DoubleAttribute*>(sbpawn->getAttribute("lightLinearAttenuation"));
			if (linearAttentuationAttr)
			{
				light.linear_attenuation = (float) linearAttentuationAttr->getValue();
			}
			else
			{
				light.linear_attenuation = 0.0f;
			}
			SmartBody::DoubleAttribute* quadraticAttentuationAttr = dynamic_cast<SmartBody::DoubleAttribute*>(sbpawn->getAttribute("lightQuadraticAttenuation"));
			if (quadraticAttentuationAttr)
			{
				light.quadratic_attenuation = (float) quadraticAttentuationAttr->getValue();
			}
			else
			{
				light.quadratic_attenuation = 0.0f;
			}
			
			_lights.push_back(light);
		}
	}
	//LOG("light size = %d\n",_lights.size());
	
	if (_lights.size() == 0 && numLightsInScene == 0)
	{
		SrLight light;		
		light.directional = true;
		light.diffuse = SrColor( 1.0f, 1.0f, 1.0f );
		SrMat mat;
		sr_euler_mat_xyz (mat, SR_TORAD(0), SR_TORAD(0), SR_TORAD(135	));
		SrQuat orientation(mat);
		SrVec up(0,1,0);
		SrVec lightDirection = -up * orientation;
		light.position = SrVec( lightDirection.x, lightDirection.y, lightDirection.z);
	//	light.constant_attenuation = 1.0f/cam.scale;
		light.constant_attenuation = 1.0f;
		_lights.push_back(light);

		SrLight light2 = light;
		light2.directional = true;
		light2.diffuse = SrColor( 0.8f, 0.8f, 0.8f );
		sr_euler_mat_xyz (mat, SR_TORAD(0), SR_TORAD(0), SR_TORAD(-135));
		SrQuat orientation2(mat);
		lightDirection = -up * orientation2;
		light2.position = SrVec( lightDirection.x, lightDirection.y, lightDirection.z);
	//	light2.constant_attenuation = 1.0f;
	//	light2.linear_attenuation = 2.0f;
		_lights.push_back(light2);
	}
	
}

void cameraInverse(float* dst, float* src)
{
	dst[0] = src[0];
	dst[1] = src[4];
	dst[2] = src[8];
	dst[3] = 0.0f;
	dst[4] = src[1];
	dst[5] = src[5];
	dst[6]  = src[9];
	dst[7] = 0.0f;
	dst[8] = src[2];
	dst[9] = src[6];
	dst[10] = src[10];
	dst[11] = 0.0f;
	dst[12] = -(src[12] * src[0]) - (src[13] * src[1]) - (src[14] * src[2]);
	dst[13] = -(src[12] * src[4]) - (src[13] * src[5]) - (src[14] * src[6]);
	dst[14] = -(src[12] * src[8]) - (src[13] * src[9]) - (src[14] * src[10]);
	dst[15] = 1.0f;
}

void FltkViewer::drawFloor(bool shadowPass)
{
	if (_data->showFloor)
	{
		static GLfloat mat_emissin[] = { 0.f,  0.f,    0.f,    1.f };
		static GLfloat mat_ambient[] = { 0.f,  0.f,    0.f,    1.f };
		static GLfloat mat_diffuse[] = { 0.5f,  0.5f,    0.5f,    1.f };
		static GLfloat mat_speclar[] = { 0.f,  0.f,    0.f,    1.f }; 
		glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, mat_emissin );
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient );

		_data->floorColor.get(mat_diffuse);
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse );	
		glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_speclar );
		glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0 );
		glColorMaterial( GL_FRONT_AND_BACK, GL_DIFFUSE );
		glEnable(GL_LIGHTING);
		float floorSize = 1200;
		float planeY = -0.0f;
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glNormal3f(0,1,0);
		glVertex3f(-floorSize,planeY,floorSize);	
		glTexCoord2f(0,1);
		glNormal3f(0,1,0);
		glVertex3f(floorSize,planeY,floorSize);
		glTexCoord2f(1,1);
		glNormal3f(0,1,0);
		glVertex3f(floorSize,planeY,-floorSize);
		glTexCoord2f(1,0);
		glNormal3f(0,1,0);
		glVertex3f(-floorSize,planeY,-floorSize);	
		glEnd();
	}	
}


// This is call when NOT using the Ogre3D rendering
void FltkViewer::drawAllGeometries(bool shadowPass)
{
	printOglError2("drawAllGeometries()", 1);

	// update deformable mesh
    bool hasGPUSupport = SbmShaderManager::getShaderSupport() != SbmShaderManager::NO_GPU_SUPPORT;
	
	SrMat shadowTexMatrix;
	
	if (_data->shadowmode == ModeShadows && !shadowPass && hasGPUSupport) // update the texture transform matrix
	{		
		float cam_inverse_modelview[16];
		const float bias[16] = {	0.5f, 0.0f, 0.0f, 0.0f, 
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f	};		
		//glGetFloatv(GL_MODELVIEW_MATRIX, cam_modelview);
		SrMat viewMat;
		viewMat = get_camera()->get_view_mat(viewMat);
		cameraInverse(cam_inverse_modelview, viewMat.pt(0));		
		// since gluLookAt gives us an orthogonal matrix, we speed up the inverse computation
		//cameraInverse(cam_inverse_modelview, cam_modelview);		
		glActiveTexture(GL_TEXTURE7);
		glMatrixMode(GL_TEXTURE);		
		glLoadMatrixf(bias);
		glMultMatrixf(_data->shadowCPM);
		// multiply the light's (bias*crop*proj*modelview) by the inverse camera modelview
		// so that we can transform a pixel as seen from the camera
		glMultMatrixf(cam_inverse_modelview);	
		glCullFace(GL_BACK);
		SbmDeformableMeshGPU::shadowMapID = _data->depthMapID;
	}
	else
	{
		SbmDeformableMeshGPU::shadowMapID = -1;
	}
	
	bool updateSim = SmartBody::SBScene::getScene()->getSimulationManager()->updateTimer();
	SbmDeformableMeshGPU::useShadowPass = shadowPass;

	printOglError2("drawAllGeometries()", 2);

	drawDeformableModels();
	
	
	_data->fcounter.start();
	if ( _data->displayaxis ) _data->render_action.apply ( _data->sceneaxis );
	if ( _data->boundingbox ) _data->render_action.apply ( _data->scenebox );

	/*
    if (hasGPUSupport)// && _data->shadowmode == ModeShadows)
    {
        std::string shaderName = _data->shadowmode == ModeShadows && !shadowPass ? "Basic" : "BasicShadow";
	    SbmShaderProgram* basicShader = SbmShaderManager::singleton().getShader(shaderName);
	    GLuint program = basicShader->getShaderProgram();
	   
		if (_data->shadowmode == ModeShadows && !shadowPass)
			glUseProgram(program);		
	    
		GLuint useShadowMapLoc = glGetUniformLocation(program,"useShadowMap");

		
	    if (_data->shadowmode == ModeShadows && !shadowPass) // attach the texture
	    {		
    		cerr << "HERE PASS\n";
			printOglError2("drawAllGeometries()", 3);
		    glActiveTexture(GL_TEXTURE7);
		    glBindTexture(GL_TEXTURE_2D, _data->depthMapID);
		    //glMatrixMode(GL_TEXTURE);
		    //glLoadMatrixf(shadowTexMatrix.pt(0));
		    glCullFace(GL_BACK);
		    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		    glUniform1i(glGetUniformLocation(program, "tex"), 7); 	
			glUniform1i(glGetUniformLocation(program, "diffuseTex"), 0); 
		    glMatrixMode(GL_MODELVIEW);			
		    glUniform1i(useShadowMapLoc,1);	
			printOglError2("drawAllGeometries()", 4);
	    }
	    else
	    {
			printOglError2("drawAllGeometries()", 30);
			glUniform1i(glGetUniformLocation(program, "diffuseTex"), 0); 
			printOglError2("drawAllGeometries()", 31);
		    glUniform1i(useShadowMapLoc,0);		
			printOglError2("drawAllGeometries()", 32);
	    }
    }
	*/
	printOglError2("drawAllGeometries()", 5);
	if( SmartBody::SBScene::getScene()->getRootGroup() )	{		
		_data->render_action.apply ( SmartBody::SBScene::getScene()->getRootGroup() );
	}	

	printOglError2("drawAllGeometries()", 6);
	
	drawFloor(shadowPass);

	
	
	glDisable(GL_LIGHTING);
	if (_data->shadowmode == ModeShadows && !shadowPass)
			glUseProgram(0);	
}


   
void FltkViewer::draw() 
{	
	printOglError2("draw()", 1);

	if ( !visible() ) return;
	
	SrCamera* cam = SmartBody::SBScene::getScene()->getActiveCamera();
	SbmShaderManager& ssm = SbmShaderManager::singleton();
	SbmTextureManager& texm = SbmTextureManager::singleton();

	
	bool hasShaderSupport = false;
	if (!context_valid())
	{			
		hasShaderSupport = ssm.initGLExtension();
        if (hasShaderSupport)
		    initShadowMap();		
	}

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
   
   if (_objManipulator.hasPicking())
   {
		SrVec2 pick_loc = _objManipulator.getPickLoc();
		_objManipulator.picking(pick_loc.x,pick_loc.y, cam);	   
   }  

   
   glViewport ( 0, 0, w(), h() );
   SrLight &light = _data->light;
  
   SrMat mat ( SrMat::NotInitialized );


   //----- Clear Background --------------------------------------------
   glClearColor ( _data->bcolor );
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

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
	int maxLight = -1;
	for (size_t x = 0; x < _lights.size(); x++)
	{
		glLight ( x, _lights[x] );	
		maxLight++;
	}

	if (maxLight < 0)
	{
		glDisable(GL_LIGHT0);
	}
	if (maxLight < 1)
	{
		glDisable(GL_LIGHT1);
	}
	if (maxLight < 2)
	{
		glDisable(GL_LIGHT2);
	}
	if (maxLight < 3)
	{
		glDisable(GL_LIGHT3);
	}
	if (maxLight < 4)
	{
		glDisable(GL_LIGHT4);
	}
	if (maxLight < 5)
	{
		glDisable(GL_LIGHT5);
	}

	if (maxLight > 0)
	{
		glEnable(GL_LIGHT0);
	}
	if (maxLight > 1)
	{
		glEnable(GL_LIGHT1);
	}
	if (maxLight > 2)
	{
		glEnable(GL_LIGHT2);
	}
	if (maxLight > 3)
	{
		glEnable(GL_LIGHT3);
	}
	if (maxLight > 4)
	{
		glEnable(GL_LIGHT4);
	}
	if (maxLight > 5)
	{
		glEnable(GL_LIGHT5);
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

	/* // draw skeleton
	const std::vector<std::string> names = SmartBody::SBScene::getScene()->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = names.begin();
		 iter != names.end();
		 iter++)
	{
		glBegin(GL_LINES);     
		SmartBody::SBCharacter* cur =  SmartBody::SBScene::getScene()->getCharacter(*iter);
		SmartBody::SBSkeleton* skeleton = cur->getSkeleton();
		int numJoints = skeleton->getNumJoints();
		for (int j = 0; j < numJoints; j++)
		{
			SmartBody::SBJoint* joint = skeleton->getJoint(j);
			SmartBody::SBJoint* parent = joint->getParent();
			if (parent)
			{
				const SrMat& gmat = joint->gmat();
				glVertex3f(gmat[12], gmat[13], gmat[14]);  
				const SrMat& pgmat = parent->gmat();
				glVertex3f(pgmat[12], pgmat[13], pgmat[14]);  
			}
			
		}
		glEnd();
		
	}
*/
	printOglError2("draw()", 2);
	// real surface geometries
	drawAllGeometries();	

	printOglError2("draw()", 3);

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
	


	//if (_data->showcollisiongeometry)
		drawCharacterPhysicsObjs();
	if (_data->showBoundingVolume)
		drawCharacterBoundingVolumes();

	drawDynamicVisuals();

	drawInteractiveLocomotion();	
	//_posControl.Draw();
	_objManipulator.draw(*cam);
	// feng : debugging draw for reach controller
	drawReach();

	_data->fcounter.stop();

    if ( !_data->message.empty() )
    {
        gl_draw_string ( _data->message.c_str(), -1, -1 );
    }
    else if ( _data->statistics )
    {
        _data->message = vhcl::Format( "FPS:%5.2f frame(%2.0f):%4.1fms render:%4.1fms", 
                  _data->fcounter.mps(),
                  _data->fcounter.measurements(),
                  _data->fcounter.loopdt()*1000.0,
                  _data->fcounter.meandt()*1000.0 );
        gl_draw_string ( _data->message.c_str(), -1.0f, -1.0f );
        _data->message = "";
    }

   if (_retargetStepWindow)
   {
	   _retargetStepWindow->redraw();
   }


   // draw UI
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   SBGUIManager::singleton().update();

   //----- Fltk will then flush and swap buffers -----------------------------
   
	
	if(getData()->saveSnapshot)
	{
		snapshot(w(), h(), static_cast<int>(_data->fcounter.measurements()));
	}

	if(getData()->saveSnapshot_tga)
	{
		snapshot_tga(w(), h(), static_cast<int>(_data->fcounter.measurements()));
	}
		
 }


std::string FltkViewer::ZeroPadNumber(int num)
{
	std::stringstream ss;
	
	// the number is converted to string with the help of stringstream
	ss << num; 
	std::string ret;
	ss >> ret;
	
	// Append zero chars
	int str_length = ret.length();
	for (int i = 0; i < 5 - str_length; i++)
		ret = "0" + ret;
	return ret;
}

 void FltkViewer::snapshot(int width, int height, int frame )
 {
	int channels = 3;

	GLubyte *image = (GLubyte *) malloc(width * height * sizeof(GLubyte) * channels);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);

	std::string path = getData()->snapshotPath + "/frame." + ZeroPadNumber(frame) + ".jpg";
	
	jpge::compress_image_to_jpeg_file(path.c_str(),  width, height, 3, image);

	free(image);
 }


void FltkViewer::snapshot_tga(int width, int height, int frame)
{
	int channels = 3;

	GLubyte *image = (GLubyte *) malloc(width * height * sizeof(GLubyte) * channels);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);

	std::string path = getData()->snapshotPath + "/frame." + ZeroPadNumber(frame) + ".tga";

	int save_result = SOIL_save_image
	(
		path.c_str(),
		SOIL_SAVE_TYPE_TGA,
		width, height, channels,
		image
	);

	free(image);
}

// Fl::event_x/y() variates from (0,0) to (w(),h())
// transformed coords in SrEvent are in "normalized device coordinates" [-1,-1]x[1,1]
static void translate_event ( SrEvent& e, SrEvent::EventType t, int w, int h, FltkViewer* viewer )
 {
	 if (!viewer->get_camera())
		 return;

   e.init_lmouse ();

   e.type = t;

   // put coordinates inside [-1,1] with (0,0) in the middle :
   e.mouse.x  = ((float)Fl::event_x())*2.0f / ((float)w) - 1.0f;
   e.mouse.y  = ((float)Fl::event_y())*2.0f / ((float)h) - 1.0f;
   e.mouse.y *= -1.0f;
   e.width = w;
   e.height = h;
   e.mouseCoord.x = (float)Fl::event_x();
   e.mouseCoord.y = (float)Fl::event_y();  

   if ( t==SrEvent::EventPush)
   {
	   e.button = Fl::event_button();
	   e.origUp = viewer->get_camera()->getUpVector();
	   e.origEye = viewer->get_camera()->getEye();
	   e.origCenter = viewer->get_camera()->getCenter();
	   e.origMouse.x = e.mouseCoord.x;
	   e.origMouse.y = e.mouseCoord.y;
   }
//    else if ( t==SrEvent::EventDrag)
//    {
// 	   e.origMouse.x = e.mouseCoord.x;
// 	   e.origMouse.y = e.mouseCoord.y;	   
//    }
   else if (t==SrEvent::EventRelease )
   {
	   e.button = Fl::event_button();
	   e.origMouse.x = -1;
	   e.origMouse.y = -1;
   }




   if ( Fl::event_state(FL_BUTTON1) ) e.button1 = 1;
   if ( Fl::event_state(FL_BUTTON2) ) e.button2 = 1;
   if ( Fl::event_state(FL_BUTTON3) ) e.button3 = 1;

#ifdef LINUX_BUILD
   if ( Fl::event_state(FL_CTRL)   ) e.alt = 1;
   e.ctrl = 0;
#else
   if ( Fl::event_state(FL_ALT)   ) e.alt = 1;
   if ( Fl::event_state(FL_CTRL)  ) e.ctrl = 1;
#endif

   if ( Fl::event_state(FL_SHIFT) ) e.shift = 1;
   
   e.key = Fl::event_key();

 }



void FltkViewer::translate_keyboard_state()
{
}

void FltkViewer::processDragAndDrop( std::string dndMsg, float x, float y )
{
	static int characterCount = 0;
	static int pawnCount = 0;
	std::vector<std::string> toks;
	vhcl::Tokenize(dndMsg,toks,":");
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SrVec p1;
	SrVec p2;
	scene->getActiveCamera()->get_ray(x,y, p1, p2);
	SrPlane ground(SrVec(0,0,0), SrVec(0, 1, 0));
	SrVec dest = ground.intersect(p1, p2);
	if (toks[0] == "SKELETON")
	{
		//dest.y = 102;		
		std::string skelName = toks[1].c_str();
		SmartBody::SBSkeleton* skel = scene->getSkeleton(skelName);
		if (skel)
		{
			SmartBody::SBScene* scene = SmartBody::SBScene::getScene();		
			SmartBody::SBCharacter* character = NULL;
			while (!character)
			{
				std::stringstream strstr;
				strstr << "char" << characterCount;
				character = scene->createCharacter(strstr.str(), "");
				characterCount++;
			}
			SmartBody::SBSkeleton* skeleton = scene->createSkeleton(toks[1]);
			if (!skeleton)
			{
				LOG("Could not find skeleton named %s, character will not be created.", toks[1].c_str());
				return;
			}
			character->setSkeleton(skeleton);
			character->createStandardControllers();

			float yOffset = -skel->getBoundingBox().a.y;
			dest.y = yOffset;

			SrVec worldOffset(dest.x, dest.y, dest.z);
			character->setPosition(worldOffset);
			character->setStringAttribute("displayType", "bones");
		}
		else
		{
			LOG("Error : Drag and drop,  skeleton %s not found",skelName.c_str());
		}		
	}
#if 0 // don't think this will be useful anymore
	else if (toks[0] == "PAWN")
	{
		dest.y = 10;
		sprintf(cmdStr,"pawn defaultPawn%d init",pawnCount);
		SmartBody::SBScene::getScene()->command(cmdStr);
		sprintf(cmdStr,"set pawn defaultPawn%d world_offset x %f y %f z %f",pawnCount,dest.x,dest.y,dest.z);
		SmartBody::SBScene::getScene()->command(cmdStr);
		pawnCount++;
	}	
#endif
	else // drag a file from explorer
	{	
		boost::filesystem::path dndPath(dndMsg);
		std::string fullPathName = dndMsg;
		std::string filebasename = boost::filesystem::basename(dndMsg);
		std::string fileextension = boost::filesystem::extension(dndMsg);					
		std::string fullPath = dndPath.parent_path().string();

		if(fileextension == ".camera") // camera config file, load camera
		{
			std::string f = fullPathName;
			FILE * pFile = fopen (f.c_str(), "r");
			if (pFile!=0)
			{
				SrInput file_in (pFile);
				SrCamera* camera = SmartBody::SBScene::getScene()->createCamera("cameraDefault");
				file_in >> *(camera);
				fclose (pFile);
				SmartBody::SBScene::getScene()->setActiveCamera(camera);
			}
			else
				LOG("WARNING: can not load cam file!");
			return;
		}

		// process mesh or skeleton
		//std::cout << "path name = " << fullPath << " base name = " << filebasename << "  extension = " << fileextension << std::endl;
		// first, load the drag-in-assets
		SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
		//std::cout << "FULLPATHNAME = " << fullPathName << std::endl;
		int pos = fullPathName.find("file://");
		if (pos != std::string::npos)
		{
			fullPathName = fullPathName.substr(pos + 7);
		}
std::cout << "LOADING [" << fullPathName << "]" << std::endl;
		assetManager->loadAsset(fullPathName);	

#if 0 // the code is replaced by the new asset loading mechanism, which provides cleaner handling. So there is no need to copy the files to retarget folders. 
		if ( boost::iequals(fileextension,".skm") || boost::iequals(fileextension,".bvh") || boost::iequals(fileextension,".amc") ) // a motion extension
		{
			SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
			assetManager->loadAsset(fullPathName);
			return;
		}		
		

		bool hasMesh = false;
		bool hasSkeleton = false;
		// copy the file over
		std::string mediaPath = SmartBody::SBScene::getScene()->getMediaPath();
		std::string retargetDir = mediaPath + "/" + "retarget/";
		std::string meshBaseDir = "retarget/mesh/";
		std::string meshDir = mediaPath + "/" + meshBaseDir ;		
		std::string skeletonDir = mediaPath + "/" + "retarget/skeletons/";

		SmartBody::SBAssetManager* assetManager = scene->getAssetManager();

		std::vector<std::string> meshPaths = assetManager->getAssetPaths("mesh");
		bool hasMeshDir = false;
		for (unsigned int i=0;i<meshPaths.size();i++)
		{
			if (meshPaths[i] == meshBaseDir)
				hasMeshDir = true;
		}
		if (!hasMeshDir)
		{
			assetManager->addAssetPath("mesh",meshBaseDir);
		}
		

		// create the folder if they do not exist
		if (!boost::filesystem::exists(retargetDir))
			boost::filesystem::create_directory(retargetDir);
		if (!boost::filesystem::exists(skeletonDir))
			boost::filesystem::create_directory(skeletonDir);
		if (!boost::filesystem::exists(meshDir))
			boost::filesystem::create_directory(meshDir);		
		boost::filesystem::create_directory(meshDir+meshName);

		
		boost::filesystem::directory_iterator dirIter(dndPath.parent_path());
		boost::filesystem::directory_iterator endIter;
		cout << " dirIter initial : " << dirIter->path().string() << "\n";
		for ( ;
			  dirIter != endIter;
			  ++dirIter )
		{
			cout << " dirIter : " << dirIter->path().string() << "\n";
			if ( boost::filesystem::is_regular_file( *dirIter ) )
			{
				std::string filename = dirIter->path().string();
				cout << filename << " : " << boost::filesystem::file_size( dirIter->path() ) << "\n";
				std::string basename = boost::filesystem::basename(filename);
				std::string extname = boost::filesystem::extension(filename);
				if ( boost::iequals(extname,".jpg") || boost::iequals(extname,".bmp") || boost::iequals(extname,".png") 
					|| boost::iequals(extname,".dds") || boost::iequals(extname,".tga"))
				{
					// copy over the file if it is an image file
					std::string targetImgFile = meshDir+meshName+"/"+basename+extname;
					if (!boost::filesystem::exists(targetImgFile))
						boost::filesystem::copy_file(filename,targetImgFile, fs::copy_option::none);
				}
			}
		}
		
		SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
		
		// load the new skeleton

		boost::filesystem::path tempPath(retargetDir);
#if (BOOST_VERSION > 104400)
		boost::filesystem::path completePath = boost::filesystem::absolute( tempPath );	
#else
		boost::filesystem::path completePath = boost::filesystem::complete( tempPath );	
#endif

		bool doAutoRigging = false;		
		if (boost::iequals(fileextension,".obj")) // a mesh model file, create a pawn and attach the model to the pawn
		{
			std::stringstream strstr;
			strstr << "defaultPawn";
			strstr << pawnCount;
			pawnCount++;
			SmartBody::SBPawn* pawn = scene->createPawn(strstr.str());
			//SrModel model; 
			//model.import_obj(fullPathName.c_str());
			pawn.setStringAttribute("mesh", fullPathName);

// 			skelName = filebasename+".sk"; // reset back to .sk extension
// 			SBAutoRigManager& autoRigManager = SBAutoRigManager::singleton();
// 			SrModel model; 
// 			model.import_obj(fullPathName.c_str());
// 			autoRigManager.buildAutoRigging(model,skelName, meshName);
// 			doAutoRigging = true;
			return;
		}
		else // load the mesh skeleton
		{
			// copy the mesh/skeleton file to retarget directory
			std::string targetSkelFile = skeletonDir+filebasename+fileextension;
			std::string targetMeshFile = meshDir+meshName+"/"+filebasename+fileextension;
			if (!boost::filesystem::exists(targetSkelFile))
				boost::filesystem::copy_file(fullPathName,targetSkelFile, fs::copy_option::none);	
			if (!boost::filesystem::exists(targetMeshFile))
				boost::filesystem::copy_file(fullPathName,targetMeshFile, fs::copy_option::none);
			scene->loadAsset(targetSkelFile);
		}
#endif

		std::string skelName = filebasename+fileextension;
		std::string meshName = filebasename+fileextension;
		// check the filename extension
		SmartBody::SBSkeleton* dndSkel = assetManager->getSkeleton(skelName);
		DeformableMesh* dndMesh = assetManager->getDeformableMesh(meshName);

		if (!dndMesh && !dndSkel) // the file doesn't contain skeleton or mesh. Just return after loading the assets.
		{
			return; 
		}

		if (dndMesh && !dndSkel) // the file contains mesh, but there is no skeleton
		{
			std::stringstream strstr;
			strstr << "defaultPawn";
			strstr << pawnCount;
			pawnCount++;
			SmartBody::SBPawn* pawn = scene->createPawn(strstr.str());			
			pawn->setStringAttribute("mesh",dndMesh->getName());	
			pawn->setDoubleAttribute("rotY",180.0);
			pawn->setDoubleAttribute("rotZ",-90.0);
			pawn->dStaticMeshInstance_p->setVisibility(2);
			return;
		}
		
		// otherwise the file contatins skeleton

		// create the joint mapping before creating the skeleton for the character
		std::string jointMapName = skelName + "-autoMap";
		SmartBody::SBJointMapManager* jointMapManager = scene->getJointMapManager();
		SmartBody::SBJointMap* jointMap = jointMapManager->getJointMap(jointMapName);
		if (!jointMap)
		{
			jointMap = jointMapManager->createJointMap(jointMapName);
			jointMap->guessMapping(assetManager->getSkeleton(skelName), false);
		}

 		SmartBody::SBSkeleton* skel = assetManager->createSkeleton(skelName);

		std::stringstream strstr;
		strstr << "defaultChar";
		strstr << characterCount;
		characterCount++;
		std::string charName = strstr.str();

		SmartBody::SBCharacter* character = scene->createCharacter(charName, "");
		character->setSkeleton(skel);
		character->createStandardControllers();
		if (dndMesh) // set the deformable mesh if the file contatins it
			character->setStringAttribute("deformableMesh",meshName);

 		float yOffset = -skel->getBoundingBox().a.y;
 		dest.y = yOffset;		
		character->setPosition(SrVec(dest.x,dest.y,dest.z));
		if (dndMesh)
			character->setStringAttribute("displayType","GPUmesh");
		else
			character->setStringAttribute("displayType","bones");
		

		// load the behavior sets if they have not yet been loaded
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

#define TEST_ROCKETBOX 0
#if TEST_ROCKETBOX
		scene->addAssetPath("script", "scripts");
		scene->run("scene.run('characterUnitTest.py')");
		
		character->createActionAttribute("_1testHead", true, "TestHead", 300, false, false, false, "Test Head");
		character->createActionAttribute("_2testGaze", true, "TestHead", 300, false, false, false, "Test Head");
		character->createActionAttribute("_3testGesture", true, "TestHead", 300, false, false, false, "Test Head");
		character->createActionAttribute("_4testReach", true, "TestHead", 300, false, false, false, "Test Head");
		character->createActionAttribute("_5testLocomotion", true, "TestHead", 300, false, false, false, "Test Head");
#endif

		if (!_retargetStepWindow)
			_retargetStepWindow = new RetargetStepWindow(this->x(), this->y(), 1024, 500, "Retarget Viewer");
		_retargetStepWindow->refreshAll();
		_retargetStepWindow->setApplyType(true);
		_retargetStepWindow->show();			
		_retargetStepWindow->setCharacterName(charName);		
		_retargetStepWindow->setJointMapName(jointMapName);		
	}
}


int FltkViewer::handle ( int event ) 
 {
   # define POPUP_MENU(e) e.ctrl && e.button3
   SrEvent &e = _data->event;
   e.type = SrEvent::EventNone;   
   translate_keyboard_state();  

   // find any interface listeners

   std::vector<SBInterfaceListener*> interfaceListeners = SBInterfaceManager::getInterfaceManager()->getInterfaceListeners();

   int ret = SBGUIManager::singleton().handleEvent(event);  
   std::string dndText;
   static float dndX,dndY;     
   switch ( event )
   {   
	   case FL_DND_RELEASE:
//		   LOG("DND Release");
	       ret = 1;
	       break;
	   case FL_DND_ENTER:          // return(1) for these events to 'accept' dnd
//		   LOG("DND Enter");
		   Fl::belowmouse(this); // send the leave events first
		   Fl::focus(this);
		   handle(FL_FOCUS);		
		   ret = 1;
		   break;
	   case FL_DND_DRAG:
//		   LOG("DND Drag");
		   translate_event ( e, SrEvent::EventPush, w(), h(), this );
		   dndX = e.mouse.x;
		   dndY = e.mouse.y;
		   ret = 1;
		   break;

	   case FL_DND_LEAVE:
//		   LOG("DND Leave");
		   ret = 1;
		   break;	  
	   case FL_PASTE:              // handle actual drop (paste) operation		   
		   label(Fl::event_text());
		   //fprintf(stderr, "PASTE: %s\n", Fl::event_text());
		   std::cout << "PASTE: n" << Fl::event_text() << std::endl;
		   dndText = Fl::event_text();
		   // remove any trailing newlines
		   if (dndText.size() > 0)
		   {
		   	if (dndText[dndText.size() - 1] == '\n')
				dndText = dndText.substr(0, dndText.size() - 1);
		   }	
			
		   processDragAndDrop(dndText,dndX,dndY);
		   ret = 1;
		   break;		
       case FL_PUSH:
       { 
		   bool earlyReturn = false;
		   for (size_t l = 0; l < interfaceListeners.size(); l++)
		   {
				int mouseX = Fl::event_x();
				int mouseY = Fl::event_y();
				int button = Fl::event_button();
				bool ret = interfaceListeners[l]->onMouseClick(mouseX, mouseY, button);
				if (ret)
					earlyReturn = true;
		   }
		   if (earlyReturn)
			   return true;
		   
		   //SR_TRACE1 ( "Mouse Push : but="<<Fl::event_button()<<" ("<<Fl::event_x()<<", "<<Fl::event_y()<<")" <<" Ctrl:"<<Fl::event_state(FL_CTRL) );
         translate_event ( e, SrEvent::EventPush, w(), h(), this );
//          if ( POPUP_MENU(e) ) { show_menu(); e.type=SrEvent::EventNone; }
// 		 // Mouse Button Push Handling for CEGUI
//         
// 		 if (e.button == 1) 
// 			 ceguiButton = CEGUI::LeftButton;
// 		 else if (e.button == 2)
// 			 ceguiButton = CEGUI::MiddleButton;
// 		 else if (e.button == 3)
// 			 ceguiButton = CEGUI::RightButton;
// 		 
// 		 CEGUI::System::getSingleton().injectMouseButtonDown(ceguiButton);
		 // process picking
		 //printf("Mouse Push\n");

		 //char exe_cmd[256];
		 if (e.button1 && (!e.alt) && (!e.ctrl) && !(e.shift))
		 {
			 {				 			 
				 SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
				 makeGLContext();
				 _objManipulator.picking(e.mouse.x, e.mouse.y,scene->getActiveCamera());
				 SbmPawn* selectedPawn = _objManipulator.get_selected_pawn();
				 if (selectedPawn)
				 {
					 std::string selectString = SmartBody::SBScene::getScene()->getStringFromObject(selectedPawn);
					 SBSelectionManager::getSelectionManager()->select(selectString);

					 SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*> (selectedPawn);
					 if (character)
					 {
						 _lastSelectedCharacter = character->getName();
					 }
				 }
				 else
				 {
					 SBSelectionManager::getSelectionManager()->select("");
				 }
			 }			 
		 }

		 if (e.button3 && e.ctrl)
		 {
			 LOG("Create Right Click Pop-up Menus");
			 SbmPawn* selectedPawn = _objManipulator.get_selected_pawn();
			 bool hasSelection = (selectedPawn != NULL)? true : false;
			 createRightClickMenu(hasSelection,Fl::event_x(), Fl::event_y());
		 }
		 else if (SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->isInitialized() && e.button3 && !e.alt)
		 {
		
			 SbmPawn* selectedPawn = _objManipulator.get_selected_pawn();
			 SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*>(selectedPawn);
			 if (!character)
				 return ret;
			 SmartBody::SBSteerAgent* steerAgent = SmartBody::SBScene::getScene()->getSteerManager()->getSteerAgent(character->getName());
			 if (steerAgent)
			 {
				 PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
				ppraiAgent->setTargetAgent(NULL);
				SrVec p1;
				SrVec p2;
				SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
				scene->getActiveCamera()->get_ray(e.mouse.x, e.mouse.y, p1, p2);
				bool intersectGround = true;
				SrVec dest, src;				
				if (scene->getNavigationMesh())				
				{
					std::vector<SrVec> pathList;
					dest = scene->getNavigationMesh()->queryMeshPointByRayCast(p1,p2);
					if (dest.x != 0.f || dest.y != 0.f || dest.z != 0.f)
					{
						intersectGround = false;
						src = ppraiAgent->getCharacter()->getPosition();
						pathList = scene->getNavigationMesh()->findPath(src,dest);
						std::stringstream command;
						command << "steer move " << character->getName() << " normal ";
						for (unsigned int k=0; k < pathList.size(); k++)
						{
							SrVec& pt = pathList[k];
							std::string xstr, ystr, zstr;
							xstr = boost::lexical_cast<std::string>(pt.x);
							ystr = boost::lexical_cast<std::string>(pt.y);
							zstr = boost::lexical_cast<std::string>(pt.z);
							command <<  xstr << " " << ystr << " " << zstr << " ";	
							
						}
						//command <<  xstr << " 0 " << zstr << " ";
						SmartBody::SBScene::getScene()->command((char*)command.str().c_str());
					}							
				}

				if (intersectGround)
				{
					SrPlane ground(SrVec(0,0,0), SrVec(0, 1, 0));
					dest = ground.intersect(p1, p2);
					dest.y = character->getHeight() / 100.0f;
					std::stringstream command;
					command << "steer move " << character->getName() << " normal " << dest.x << " " << dest.y << " " << dest.z;
					SmartBody::SBScene::getScene()->command((char*)command.str().c_str());
				}			
			
				
				
			 }
		 }
       } 
	   ret = 1; // in order to receive FL_DRAG event
	   break;

      case FL_RELEASE:
		  {
		  bool earlyReturn = false;
		  for (size_t l = 0; l < interfaceListeners.size(); l++)
		   {
				int mouseX = Fl::event_x();
				int mouseY = Fl::event_y();
				int button = Fl::event_button();
				bool ret = interfaceListeners[l]->onMouseRelease(mouseX, mouseY, button);
				if (ret)
					earlyReturn = true;
		   }
		  if (earlyReturn)
			  return true;
		  }

        //SR_TRACE1 ( "Mouse Release : ("<<Fl::event_x()<<", "<<Fl::event_y()<<") buts: "
         //            <<(Fl::event_state(FL_BUTTON1)?1:0)<<" "<<(Fl::event_state(FL_BUTTON2)?1:0) );
        //translate_event ( e, SrEvent::EventRelease, w(), h(), this);		
// 		if (e.button == 1) 
// 			ceguiButton = CEGUI::LeftButton;
// 		else if (e.button == 2)
// 			ceguiButton = CEGUI::MiddleButton;
// 		else if (e.button == 3)
// 			ceguiButton = CEGUI::RightButton;
// 
// 		//LOG("Mouse Release %d",e.button);
// 		CEGUI::System::getSingleton().injectMouseButtonUp(ceguiButton);
		// process picking
		//if (!e.button1)	
		//printf("Mouse Release\n");
		//LOG("Mouse release");
        break;

      case FL_MOVE:
		  {
			bool earlyReturn = false;
		   for (size_t l = 0; l < interfaceListeners.size(); l++)
		   {
				int mouseX = Fl::event_x();
				int mouseY = Fl::event_y();
			   bool ret = interfaceListeners[l]->onMouseMove(e.mouse.x, e.mouse.y);
			   if (ret)
				   earlyReturn = true;
		   }
		   if (earlyReturn)
			   return true;
		  }
        //SR_TRACE2 ( "Move buts: "<<(Fl::event_state(FL_BUTTON1)?1:0)<<" "<<(Fl::event_state(FL_BUTTON2)?1:0) );
		//LOG("Move mouse cursor to %f %f",e.mouseCoord.x, e.mouseCoord.y);
		//translate_event ( e, SrEvent::EventNone, w(), h(), this );
		//CEGUI::System::getSingleton().injectMousePosition(e.mouseCoord.x, e.mouseCoord.y);
        if ( !Fl::event_state(FL_BUTTON1) && !Fl::event_state(FL_BUTTON2) ) break;
        // otherwise, this is a drag: enter in the drag case.
        // not sure if this is a hack or a feature.
      case FL_DRAG:
		  {
		  bool earlyReturn = false;
		   for (size_t l = 0; l < interfaceListeners.size(); l++)
		   {
				int mouseX = Fl::event_x();
				int mouseY = Fl::event_y();
			   bool ret = interfaceListeners[l]->onMouseDrag(e.mouse.x, e.mouse.y);
			   if (ret)
				   earlyReturn = true;
		   }
		   if (earlyReturn)
			   return true;
        //SR_TRACE2 ( "Mouse Drag : ("<<Fl::event_x()<<", "<<Fl::event_y()<<") buts: "
        //             <<(Fl::event_state(FL_BUTTON1)?1:0)<<" "<<(Fl::event_state(FL_BUTTON2)?1:0) );
        translate_event ( e, SrEvent::EventDrag, w(), h(), this );		
		  }
        break;

      case FL_SHORTCUT: // not sure the relationship between a shortcut and keyboard event...
        //SR_TRACE1 ( "Shortcut : "<< Fl::event_key() <<" "<<fltk::event_text() );
        //translate_event ( e, SrEvent::Keyboard, w(), h() );
        //break;

	  case FL_KEYDOWN:
		  {
		 //LOG("Receiving FL_KEYDOWN");
         e.type = SrEvent::EventKeyboard;
         e.key = Fl::event_key();

		 bool earlyReturn = false;
		for (size_t l = 0; l < interfaceListeners.size(); l++)
		{	
				bool ret = interfaceListeners[l]->onKeyboardPress(e.key);
				if (ret)
					earlyReturn = true;
		}
		if (earlyReturn)
			return true;


		  switch (Fl::event_key())
		  {
			case 'w': // translate mode
				_transformMode = ObjectManipulationHandle::CONTROL_POS;
				{
					PawnControl* posControl = _objManipulator.getPawnControl(ObjectManipulationHandle::CONTROL_POS);
					PawnControl* rotControl = _objManipulator.getPawnControl(ObjectManipulationHandle::CONTROL_ROT);
					PawnControl* selControl = _objManipulator.getPawnControl(ObjectManipulationHandle::CONTROL_SELECTION);
					_transformMode = ObjectManipulationHandle::CONTROL_POS;					
					
					SbmPawn* newAttachPawn = NULL;
					if (rotControl->get_attach_pawn()) newAttachPawn = rotControl->get_attach_pawn();
					if (selControl->get_attach_pawn()) newAttachPawn = selControl->get_attach_pawn();
					if (newAttachPawn)
					{
						posControl->attach_pawn(newAttachPawn);
						rotControl->detach_pawn();
						selControl->detach_pawn();
						 _objManipulator.active_control = posControl;
					}					
				}
				return ret;
			case 'e': // rotate mode
 				_transformMode = ObjectManipulationHandle::CONTROL_ROT;
				{
					PawnControl* posControl = _objManipulator.getPawnControl(ObjectManipulationHandle::CONTROL_POS);
					PawnControl* rotControl = _objManipulator.getPawnControl(ObjectManipulationHandle::CONTROL_ROT);
					PawnControl* selControl = _objManipulator.getPawnControl(ObjectManipulationHandle::CONTROL_SELECTION);
					_transformMode = ObjectManipulationHandle::CONTROL_ROT;

					SbmPawn* newAttachPawn = NULL;
					if (posControl->get_attach_pawn()) newAttachPawn = posControl->get_attach_pawn();
					if (selControl->get_attach_pawn()) newAttachPawn = selControl->get_attach_pawn();
					if (newAttachPawn)
					{
						rotControl->attach_pawn(newAttachPawn);
						posControl->detach_pawn();
						selControl->detach_pawn();
						_objManipulator.active_control = rotControl;
					}	
				}
				return ret;

			case 'q': // rotate mode
				_transformMode = ObjectManipulationHandle::CONTROL_SELECTION;
				{
					PawnControl* posControl = _objManipulator.getPawnControl(ObjectManipulationHandle::CONTROL_POS);
					PawnControl* rotControl = _objManipulator.getPawnControl(ObjectManipulationHandle::CONTROL_ROT);
					PawnControl* selControl = _objManipulator.getPawnControl(ObjectManipulationHandle::CONTROL_SELECTION);
					_transformMode = ObjectManipulationHandle::CONTROL_SELECTION;

					SbmPawn* newAttachPawn = NULL;
					if (posControl->get_attach_pawn()) newAttachPawn = posControl->get_attach_pawn();
					if (rotControl->get_attach_pawn()) newAttachPawn = rotControl->get_attach_pawn();
					if (newAttachPawn)
					{
						selControl->attach_pawn(newAttachPawn);
						posControl->detach_pawn();
						rotControl->detach_pawn();
						_objManipulator.active_control = selControl;
					}	
				}
				return ret;
			//case 't': // scale mode - not yet supported
				//_transformMode = 2;
				//return ret;
			case 'f': // frame selected object
				{
				SrBox sceneBox;
				SrCamera* camera = SmartBody::SBScene::getScene()->getActiveCamera();
				if (!camera)
					return ret;
				
				SbmPawn* selectedPawn = _objManipulator.get_selected_pawn();
				if (selectedPawn)
				{
					SrBox box = selectedPawn->getSkeleton()->getBoundingBox();
					if (box.volume() < .0001)
					{
						double val = 1.0 / SmartBody::SBScene::getScene()->getScale() * .5;
						box.grows((float) val, (float) val, (float) val);
					} 
					sceneBox.extend(box);
				}
				else
				{
					const std::vector<std::string>& pawnNames =  SmartBody::SBScene::getScene()->getPawnNames();
					for (std::vector<std::string>::const_iterator iter = pawnNames.begin();
						 iter != pawnNames.end();
					      iter++)
					{
						SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(*iter);
						bool visible = pawn->getBoolAttribute("visible");
						if (!visible)
							continue;
						SrBox box = pawn->getSkeleton()->getBoundingBox();
						sceneBox.extend(box);
					}
				}
				
				if (sceneBox.volume() < .0001)
				{
					double val = 1.0 / SmartBody::SBScene::getScene()->getScale() * .5;
					sceneBox.grows((float) val, (float) val, (float) val);
					sceneBox.extend(sceneBox);
				} 
				
				camera->view_all(sceneBox, camera->getFov());	
				float scale = 1.f/SmartBody::SBScene::getScene()->getScale();
				float znear = 0.01f*scale;
				float zfar = 100.0f*scale;
				camera->setNearPlane(znear);
				camera->setFarPlane(zfar);
				}
				break;
			case FL_Delete: // delete selected object
				ret = deleteSelectedObject(ret);
				break;
		  }
		  }
	  case FL_KEYUP:
		  {
			e.type = SrEvent::EventKeyboard;
			e.key = Fl::event_key();
			bool earlyReturn = false;
			 int lastKey = Fl::event_key(e.key);

			 if (lastKey != e.key)
			 {
				for (size_t l = 0; l < interfaceListeners.size(); l++)
				{
					bool ret = interfaceListeners[l]->onKeyboardRelease(e.key);
					if (ret)
						earlyReturn = true;
				}
			 }
			 if (earlyReturn)
				 return true;
		  }
		  break;
      case FL_HIDE: // Called when the window is iconized
        { //SR_TRACE1 ( "Hide" );
          _data->iconized = true;
          // the opengl lists need to be re-created when the window appears again, so
          // we mark already here all shapes as changed:
          _data->scenebox->changed(true);
          _data->sceneaxis->changed(true);
          srSaSetShapesChanged sa;
          sa.apply ( SmartBody::SBScene::getScene()->getRootGroup() );
        } break;

      case FL_SHOW: // Called when the window is de-iconized or when show() is called
        //SR_TRACE1 ( "Show" );
        _data->iconized = false;
        show ();
        break;	 
      // Other events :
      case FL_ENTER:          
		  //SR_TRACE2 ( "Enter" );    
		  ret = 1;
		  break;
      case FL_LEAVE:          
		  //SR_TRACE2 ( "Leave" ); 
		  ret = 1;
		  break;
      case FL_FOCUS:          
		  //SR_TRACE2 ( "Focus" );         
		  break;
      case FL_UNFOCUS:        
		  //SR_TRACE2 ( "Unfocus" );       
		  break;
     // case FL_CLOSE:          
		  //SR_TRACE2 ( "Close");          
	//	  break;
      case FL_ACTIVATE:       
		  //SR_TRACE2 ( "Activate");       
		  break;
      case FL_DEACTIVATE:     
		  //SR_TRACE2 ( "Deactivate");     
		  break;
	  //case FL_PASTE:          
		  //SR_TRACE2 ( "Paste");          
		 // break;
 //     case FL_SELECTIONCLEAR: 
		  //SR_TRACE2 ( "SelectionClear"); 
	//	  break;
    }

   //SR_TRACE3 ( e );

	

   if ( e.type == SrEvent::EventNone && ret == 0 ) return 0; // not an interesting event

   if ( event==FL_PUSH || event==FL_DRAG )
    { 
		SrCamera* camera = SmartBody::SBScene::getScene()->getActiveCamera();
		SrPlane plane ( camera->getCenter(), SrVec::k );
      camera->get_ray ( e.mouse.x, e.mouse.y, e.ray.p1, e.ray.p2 );
      camera->get_ray ( e.lmouse.x, e.lmouse.y, e.lray.p1, e.lray.p2 );
      e.mousep = plane.intersect ( e.ray.p1, e.ray.p2 );
      e.lmousep = plane.intersect ( e.lray.p1, e.lray.p2 );
	  if ( event==FL_PUSH  ) // update picking precision
       { // define a and b with 1 pixel difference:
         SrPnt2 a ( ((float)w())/2.0f, ((float)h())/2.0f ); // ( float(Fl::event_x()), float(Fl::event_y()) );
         SrPnt2 b (a+SrVec2::one);// ( float(Fl::event_x()+1), float(Fl::event_y()+1) );
         // put coordinates inside [-1,1] with (0,0) in the middle :
         a.x  = a.x*2.0f / float(w()) - 1.0f;
         a.y  = a.y*2.0f / float(h()) - 1.0f; a.y *= -1.0f;
         b.x  = b.x*2.0f / float(w()) - 1.0f;
         b.y  = b.y*2.0f / float(h()) - 1.0f; b.y *= -1.0f;
         //sr_out << "a,b: " << a << srspc << b <<srnl;
         SrLine aray, bray;
         camera->get_ray ( a.x, a.y, aray.p1, aray.p2 );
         camera->get_ray ( b.x, b.y, bray.p1, bray.p2 );
         SrPnt pa = plane.intersect ( aray.p1, aray.p2 );
         SrPnt pb = plane.intersect ( bray.p1, bray.p2 );
         //sr_out << "pa,pb: " << pa << srspc << pb <<srnl;
         e.pixel_size = (SR_DIST(pa.x,pb.x)+SR_DIST(pa.y,pb.y))/2.0f;
		 interactivePoint = e.lmousep;
         //sr_out << "pixel_size: " << e.pixel_size <<srnl;
       }
    }  
   int ret2 = handle_event ( e );    

   if (ret == 1)  // a drag and drop event
   {	
	   //LOG("ret == 1");
	   return ret;
   }
   else
   {
	   return ret2;
   }
}

//== handle sr event =======================================================

int FltkViewer::handle_event ( const SrEvent &e )
 {
   int res=0;   

   //if ( (e.alt && e.mouse_event() || e.type == SrEvent::EventKeyboard) && !e.ctrl )
   if (e.alt && e.mouse_event() && !e.ctrl )
    { 
        res = handle_examiner_manipulation ( e );

      if ( res ) return res;
    }
   
   if (e.mouse_event() )
   {
	   res = handle_object_manipulation ( e );
	   if ( res ) return res;
   }

   if (e.mouse_event() )
   {
	   res = handle_object_manipulation ( e );
	   if ( res ) return res;
   }

   if ( e.mouse_event() ) return handle_scene_event ( e );


   return res; // this point should not be reached
 }

//== Object Manipulation event =======================================================

int FltkViewer::handle_object_manipulation( const SrEvent& e)
{
	if (e.type==SrEvent::EventPush)
	 {
		 if (e.button1)
		 {
			 //_objManipulator.picking(e.mouse.x,e.mouse.y, _data->camera);
			 //_objManipulator.hasPicking(true);
			SrVec2 mouseVec(e.mouse.x, e.mouse.y);
			 _objManipulator.setPicking(mouseVec);
			 if (this->_transformMode == ObjectManipulationHandle::CONTROL_POS)
			 {
				 _objManipulator.setPickingType(ObjectManipulationHandle::CONTROL_POS);
					
			 }
			 else if (this->_transformMode == ObjectManipulationHandle::CONTROL_ROT)
			 {
				 _objManipulator.setPickingType(ObjectManipulationHandle::CONTROL_ROT);
			 }
			 else if (this->_transformMode == ObjectManipulationHandle::CONTROL_SELECTION)
			 {
				 _objManipulator.setPickingType(ObjectManipulationHandle::CONTROL_SELECTION);
			 }
		 }
		 if (e.button3 && e.shift)
		 {
			SrVec2 pickVec(e.mouse.x, e.mouse.y);
			_objManipulator.setPicking(pickVec);
			_objManipulator.picking(e.mouse.x, e.mouse.y, SmartBody::SBScene::getScene()->getActiveCamera());
			SbmPawn* selectedPawn = _objManipulator.get_selected_pawn();
			if (selectedPawn)
			{
				SbmCharacter* selectedCharacter = dynamic_cast<SbmCharacter*> (selectedPawn);
				if (selectedCharacter)
				{
					SmartBody::SBSteerAgent* steerAgent = SmartBody::SBScene::getScene()->getSteerManager()->getSteerAgent(selectedPawn->getName());
					if (steerAgent)
					{
						PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
						ppraiAgent->setTargetAgent(selectedCharacter);
					}
					
				}
			}
		 }
		return 1;
	 }
	else if (e.type==SrEvent::EventDrag)
	{
		if (e.button1)// && _posControl.dragging)
		{			
			_objManipulator.drag(*(SmartBody::SBScene::getScene()->getActiveCamera()),e.lmouse.x,e.lmouse.y,e.mouse.x,e.mouse.y);			
		}
	}
	else if (e.type==SrEvent::EventRelease)
	{
		if (e.button == 1)
		{			
			//_posControl.dragging = false;
		}
	}
	return 0;
}



std::string FltkViewer::create_pawn()
{
	static int numPawn = 0;
	std::string pawnName = "pawn" + boost::lexical_cast<std::string>(numPawn);
	
	const char* pawn_name = fl_input("Input Pawn Name",pawnName.c_str());
	if (!pawn_name) // no name is input
		return "";

	char cmd_pawn[256];
	sprintf(cmd_pawn,"scene.createPawn(\"%s\")", pawn_name);
	SmartBody::SBScene::getScene()->run(cmd_pawn);
	numPawn++;

	return pawnName;
}


void FltkViewer::set_reach_target( int itype, const char* targetname )
{
	char exe_cmd[256];
	SbmCharacter* actor = NULL;
	int counter = 0;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& characterNames = scene->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = scene->getCharacter((*iter));
		//if (counter == _locoData->char_index)
		{
			actor = character;
			break;
		}
		counter++;
	}	

	SbmPawn* pawn = _objManipulator.get_selected_pawn();
	static char reach_type[NUM_REACH_TYPES][20] = { "right", "left" };	
	if (actor)
	{
		char pawn_name[30];
		if (strcmp(targetname,"selected pawn")==0)
		{
			if (pawn)
				strcpy(pawn_name,pawn->getName().c_str());
			else
			{
				// handle user error : call set target command without selecting a pawn target.
			}
		}
		else
			strcpy(pawn_name,targetname);

		sprintf(exe_cmd,"bml char %s <sbm:reach target=\"%s\" reach-arm=\"%s\"/>",actor->getName().c_str(),pawn_name,reach_type[itype]);
		scene->commandAt(1.0, exe_cmd); // delay execution for one second to avoid popping
	}
}

void FltkViewer::set_gaze_target(int itype, const char* label)
{
	SbmPawn* pawn = this->getObjectManipulationHandle().get_selected_pawn();

	SbmCharacter* actor = this->getCurrentCharacter();
	if (!actor)
		return;

	if (itype == -1)
	{
		std::stringstream strstr;
		strstr << "char " << actor->getName() << "gazefade out 0";
		SmartBody::SBScene::getScene()->command((char*) strstr.str().c_str());
		return;
	}
		
	static char gaze_type[NUM_GAZE_TYPES][20] = { "EYES", "EYES NECK", "EYES CHEST", "EYES BACK" };
	//if (actor)
	//	printf("current char %s ", actor->name);

	if (actor)
	{
		char pawn_name[30];
		if (strcmp(label,"selected pawn")==0)
		{
			if (pawn)
				strcpy(pawn_name, pawn->getName().c_str());
			else
			{
				// handle user error : call set target command without selecting a pawn target.
			}
		}
		else
			strcpy(pawn_name,label);

		std::stringstream strstr;
		strstr << "bml char " << actor->getName() << " <gaze target=\"" << pawn_name << "\" sbm:joint-range=\"" << gaze_type[itype] << "\"/>";
		SmartBody::SBScene::getScene()->command((char*) strstr.str().c_str());
	}
}

//== Examiner ==============================================================

# define ROTATING2(e)    (e.alt && e.button1)
# define ROTATING(e)   (e.alt && e.shift && e.button1)
//# define ZOOMING(e)   (e.alt && e.button3)
# define ZOOMING(e)     (e.shift && e.alt && e.button3)
# define DOLLYING(e)     (e.alt && e.button3)
# define TRANSLATING(e) (e.alt && e.button2)

int FltkViewer::handle_examiner_manipulation ( const SrEvent &e )
 {
    SrCamera* camera = SmartBody::SBScene::getScene()->getActiveCamera();
    switch (getData()->cameraMode)
    {
    case Default:
       handle_default_camera_manipulation(e, camera);
       break;

    case FreeLook:
       handle_freelook_camera_manipulation(e, camera);
       break;

    case FollowRenderer:

       break;
    }
	
  
   return 1;
 }

int FltkViewer::handle_default_camera_manipulation ( const SrEvent &e, SrCamera* camera )
{
   if ( e.type==SrEvent::EventDrag )
    { 
      float dx = e.mousedx() * camera->getAspectRatio();
      float dy = e.mousedy() / camera->getAspectRatio();

		if ( ROTATING(e) )
       { 
#if 1
		   float deltaX = -(e.mouseCoord.x - e.origMouse.x) / e.width;
		   float deltaY = -(e.mouseCoord.y -  e.origMouse.y) / e.height;
		   SrVec origUp = e.origUp;
		   SrVec origCenter = e.origCenter;
		   SrVec origCamera = e.origEye;
#else

		   float deltaX = -(e.mouse.x - e.lmouse.x) ;
		   float deltaY = (e.mouse.y -  e.lmouse.y) ;
		   SrVec origUp = _data->camera.up;
		   SrVec origCenter = _data->camera.center;
		   SrVec origCamera = _data->camera.eye;
#endif
		   if (deltaX == 0.0 && deltaY == 0.0)
			   return 1;

		   SrVec forward =origCenter - origCamera; 		   
		   SrQuat q = SrQuat(origUp, vhcl::DEG_TO_RAD()*deltaX*150.f);			   
		   forward = forward*q;
		   SrVec tmp = get_camera()->getEye() + forward;
		   camera->setCenter(tmp.x, tmp.y, tmp.z);

		   SrVec cameraRight = cross(forward,origUp);
		   cameraRight.normalize();		   
		   q = SrQuat(cameraRight, vhcl::DEG_TO_RAD()*deltaY*150.f);	
		   camera->setUpVector(origUp*q);
		   forward = forward*q;
		   SrVec tmpCenter = camera->getEye() + forward;
		   camera->setCenter(tmpCenter.x, tmpCenter.y, tmpCenter.z);		  
		   redraw();
       }
      else if ( ROTATING2(e) )
       { 
 		float deltaX = -(e.mouseCoord.x - e.origMouse.x) / e.width;
		float deltaY = -(e.mouseCoord.y -  e.origMouse.y) / e.height;
		if (deltaX == 0.0 && deltaY == 0.0)
			return 1;

		SrVec origUp = e.origUp;
		SrVec origCenter = e.origCenter;
		SrVec origCamera = e.origEye;

		SrVec dirX = origUp;
		SrVec  dirY;
		dirY.cross(origUp, (origCenter - origCamera));
		dirY /= dirY.len();

		SrVec cameraPoint = rotatePoint(origCamera, origCenter, dirX, -deltaX * float(M_PI));
		cameraPoint = rotatePoint(cameraPoint, origCenter, dirY, deltaY * float(M_PI));

		camera->setEye(cameraPoint.x, cameraPoint.y, cameraPoint.z);
		redraw();
	  }
	  else if ( TRANSLATING(e) )
	  { camera->apply_translation_from_mouse_motion ( e.lmouse.x, e.lmouse.y, e.mouse.x, e.mouse.y );
	  redraw();
	  }	  
	  else if ( ZOOMING(e) )
	  {
		  float tmpFov = camera->getFov() + (-dx+dy);//40.0f;
		  camera->setFov(SR_BOUND ( tmpFov, 0.001f, srpi ));
		  redraw();
	  }
	  else if ( DOLLYING(e) )
	  { 
		  float amount = dx-dy;
		  SrVec cameraPos(camera->getEye());
		  SrVec targetPos(camera->getCenter());
		  SrVec diff = targetPos - cameraPos;
		  float distance = diff.len();
		  diff.normalize();

		  if (amount >= distance)
			  amount = distance - .000001f;

		  SrVec diffVector = diff;
		  SrVec adjustment = diffVector * distance * amount;
		  cameraPos += adjustment;
		  SrVec oldEyePos = camera->getEye();
		  camera->setEye(cameraPos.x, cameraPos.y, cameraPos.z);
//		  SrVec cameraDiff = camera->getEye() - oldEyePos;
//		  SrVec tmpCenter = camera->getCenter();
//		  tmpCenter += cameraDiff;
//		  camera->setCenter(tmpCenter.x, tmpCenter.y, tmpCenter.z);
		  redraw();
	  }
    }   
   else if ( e.type==SrEvent::EventRelease )
    { 
    }

   return 1;
}

int FltkViewer::handle_freelook_camera_manipulation ( const SrEvent &e, SrCamera* camera )
{
   // mouse rotation
   bool needRedraw = false;
   if ( e.type==SrEvent::EventDrag && e.button3)
   { 
		float deltaX = -(e.mouseCoord.x - e.origMouse.x) / e.width;
		float deltaY = -(e.mouseCoord.y -  e.origMouse.y) / e.height;
		SrVec origUp = e.origUp;
		SrVec origCenter = e.origCenter;
		SrVec origCamera = e.origEye;
		if (deltaX == 0.0 && deltaY == 0.0)
			return 1;

		SrVec forward =origCenter - origCamera; 		   
		SrQuat q = SrQuat(origUp, vhcl::DEG_TO_RAD()*deltaX*150.f);			   
		forward = forward*q;
		SrVec tmp = get_camera()->getEye() + forward;
		camera->setCenter(tmp.x, tmp.y, tmp.z);

		SrVec cameraRight = cross(forward,origUp);
		cameraRight.normalize();		   
		q = SrQuat(cameraRight, vhcl::DEG_TO_RAD()*deltaY*150.f);	
		camera->setUpVector(origUp*q);
		forward = forward*q;
		SrVec tmpCenter = camera->getEye() + forward;
		camera->setCenter(tmpCenter.x, tmpCenter.y, tmpCenter.z);		  

      needRedraw = true;
		//redraw();
   }

   const float MovementSpeed = 0.1f;
   if (e.type == SrEvent::EventKeyboard) // keyboard handling
   {
      // forward/back
      if (e.key == 'w')
      { 
         translate_camera(camera, camera->getForwardVector() * MovementSpeed);
         needRedraw = true;
      }
      else if (e.key == 's')
      { 
         translate_camera(camera, camera->getForwardVector() * -MovementSpeed);
         needRedraw = true;
      }

      // left/right
      if (e.key == 'a')
      { 
            translate_camera(camera, camera->getRightVector() * MovementSpeed);
            needRedraw = true;
      }
      else if (e.key == 'd')
      { 
         translate_camera(camera, camera->getRightVector() * -MovementSpeed);
         needRedraw = true;
      }

      // up/down
      if (e.key == 'e')
      { 
         translate_camera(camera, camera->getUpVector() * MovementSpeed);
         needRedraw = true;
      }
      else if (e.key == 'q')
      { 
         translate_camera(camera, camera->getUpVector() * -MovementSpeed);
         needRedraw = true;
      }

      if (needRedraw)
      {
         redraw();
      } 
   }

   return 1;
}

void FltkViewer::translate_camera( SrCamera* camera, const SrVec& displacement)
{
   // need to move both the eye (camera pos) and the camera's look at position (center)
   SrVec oldEye = camera->getEye();
   SrVec newEye = camera->getEye() + displacement;
   camera->setEye(newEye.x, newEye.y, newEye.z);
   SrVec posDiff = newEye - oldEye;
   SrVec tempCenter = camera->getCenter() + posDiff;
   camera->setCenter(tempCenter.x, tempCenter.y, tempCenter.z);
}

int FltkViewer::handle_followrenderer_camera_manipulation ( const SrEvent &e, SrCamera* camera  )
{
   return 1;
}
//== Apply Scene action ==========================================================

int FltkViewer::handle_scene_event ( const SrEvent& e )
 {
   SrSaEvent ea(e);
   ea.apply ( SmartBody::SBScene::getScene()->getRootGroup() );
   int used = ea.result();
   if ( used ) render();
   return used;
 }

//== Keyboard ==============================================================

void FltkViewer::label_viewer(const char* str)
{
	label(str);
}

void FltkViewer::show_viewer()
{	
	show();
	make_current();
	SBGUIManager::singleton().init();	
	if (!fltkListener)
	{
		fltkListener = new FLTKListener();
		SmartBody::SBScene::getScene()->addSceneListener(fltkListener);
	}
}

void FltkViewer::hide_viewer()
{
	if (this->shown())
		this->hide();
}

void FltkViewer::initGridList()
{
	glDeleteLists(gridList, 1);
	gridList = glGenLists(1);
	if ( gridList == GL_INVALID_VALUE || gridList == GL_INVALID_OPERATION)
		return;
	glNewList(gridList, GL_COMPILE);
	drawGrid();
	glEndList();
}

void FltkViewer::drawGrid()
{
	if (_data->gridMode == ModeNoGrid)
		return;
	if( gridList != -1 )	{
		glCallList( gridList );
		return;
	}

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	float sceneScale = scene->getScale();
	GLfloat gridHeight = 0.0f + 0.001f/scene->getScale();

	glPushAttrib(GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
	bool colorChanged = false;
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);

	glColor4f(gridColor[0], gridColor[1], gridColor[2], gridColor[3]);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glLineWidth(3.f);
//	glLineStipple(1, 0xAAAA);
	glBegin(GL_LINES);
	
	float adjustedGridStep = gridStep;
	float adjustGridSize = gridSize * .01f / sceneScale;
	if (sceneScale > 0.f)
	{
		adjustedGridStep *= .01f / sceneScale;
	}

	for (float x = -adjustGridSize; x <= adjustGridSize + .001; x += adjustedGridStep)
	{
		if (x == 0.0) {
			colorChanged = true;
			glColor4f(gridHighlightColor[0], gridHighlightColor[1], gridHighlightColor[2], 1.0f);
		}
		glVertex3f(x, gridHeight, -adjustGridSize);
		glVertex3f(x, gridHeight, adjustGridSize);
		
		if (colorChanged) {
			colorChanged = false;
			glColor4f(gridColor[0], gridColor[1], gridColor[2], gridColor[3]);
		}

	}
	for (float x = -adjustGridSize; x <= adjustGridSize + .001; x += adjustedGridStep)
	{
		if (x == 0) {
			colorChanged = true;
			glColor4f(gridHighlightColor[0], gridHighlightColor[1], gridHighlightColor[2], 1.0f );
		}
		glVertex3f(-adjustGridSize, gridHeight, x);
		glVertex3f(adjustGridSize, gridHeight, x);
		if (colorChanged) {
			colorChanged = false;
			glColor4f(gridColor[0], gridColor[1], gridColor[2], gridColor[3]);
		}
	}

	glEnd();
	glDisable(GL_BLEND);
//	glDisable(GL_LINE_STIPPLE);

	glPopAttrib();
}

void FltkViewer::drawDynamicVisuals()
{
	if (_points3D.size() == 0 &&
		_lines3D.size() == 0)
		return;

	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_PROGRAM_POINT_SIZE);
	glDisable(GL_LIGHTING);
	
	glBegin(GL_POINTS);
	for (std::map<std::string, Point3D>::iterator iter = _points3D.begin();
		 iter != _points3D.end();
		 iter++)
	{
		glPointSize((*iter).second.size);
		glColor3f((*iter).second.color.r, (*iter).second.color.g, (*iter).second.color.b);
		glVertex3f((*iter).second.point.x, (*iter).second.point.y, (*iter).second.point.z);
	}
	glEnd();

	for (std::map<std::string, Line3D>::iterator iter = _lines3D.begin();
		 iter != _lines3D.end();
		 iter++)
	{
		glColor3f((*iter).second.color.r, (*iter).second.color.g, (*iter).second.color.b);
		glLineWidth((*iter).second.width);
		glBegin(GL_LINES);
		for (size_t p = 0; p < (*iter).second.points.size(); p++)
			glVertex3f((*iter).second.points[p].x, (*iter).second.points[p].y, (*iter).second.points[p].z);
		glEnd();
	}


	glPopAttrib();
}



void FltkViewer::drawJointLimitCone( SmartBody::SBJoint* joint, float coneSize, float pitchUpLimit, float pitchDownLimit, float headLimit )
{
	SrMat gmat = joint->gmat();
	SrVec center = SrVec(0,0,0);
	SrVec pitchUpVec = joint->localGlobalAxis(2)*SrQuat(joint->localGlobalAxis(0),(float)sr_torad(pitchUpLimit));
	SrVec pitchDownVec = joint->localGlobalAxis(2)*SrQuat(joint->localGlobalAxis(0),(float)sr_torad(pitchDownLimit));
	SrVec pitchLeftVec = joint->localGlobalAxis(2)*SrQuat(joint->localGlobalAxis(1),(float)sr_torad(headLimit));
	SrVec pitchRightVec = joint->localGlobalAxis(2)*SrQuat(joint->localGlobalAxis(1),(float)sr_torad(-headLimit));
	pitchUpVec *= coneSize;	pitchDownVec *= coneSize; pitchLeftVec *= coneSize; pitchRightVec *= coneSize;
	
	glPushMatrix();
	glMultMatrixf((const float*) gmat);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	glVertex3fv(&center[0]);
	glVertex3fv(&pitchRightVec[0]);
	glVertex3fv(&pitchUpVec[0]);
	glVertex3fv(&center[0]);
	glVertex3fv(&pitchDownVec[0]);	
	glVertex3fv(&pitchRightVec[0]);
	glVertex3fv(&center[0]);
	glVertex3fv(&pitchLeftVec[0]);	
	glVertex3fv(&pitchDownVec[0]);	
	glVertex3fv(&center[0]);
	glVertex3fv(&pitchUpVec[0]);		
	glVertex3fv(&pitchLeftVec[0]);		
	glEnd();
	glPopMatrix();

}

void FltkViewer::drawGazeJointLimits()
{	
	if (_data->gazeLimitMode == ModeNoGazeLimit)
		return;

	glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
	std::string jointNames[] = {"eyeball_left","eyeball_right", "spine4", "spine3", "spine1"};
	std::string gazeKeys[] = {"Eyes","Eyes", "Neck", "Chest", "Back"};
	std::string gazeLimitNames[] = {"gaze.limitPitchUp", "gaze.limitPitchDown", "gaze.limitHeading" };
	 
	glDisable(GL_CULL_FACE);
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& characterNames = scene->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = scene->getCharacter((*iter));
		character->getSkeleton()->invalidate_global_matrices();
		character->getSkeleton()->update_global_matrices();
		float coneSize = character->getHeight()*0.1f;

		for (unsigned int i=0;i<5;i++)
		{
			SmartBody::SBJoint* joint = character->getSkeleton()->getJointByName(jointNames[i]);
			if (!joint)
				continue;
			float gazeLimits[3];
			for (unsigned k=0;k<3;k++)
			{
				gazeLimits[k] = (float)character->getDoubleAttribute(gazeLimitNames[k]+gazeKeys[i]);
			}
			drawJointLimitCone(joint,coneSize,gazeLimits[0],gazeLimits[1],gazeLimits[2]);
		}		
	}
	glEnable(GL_CULL_FACE);
	glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
}


void FltkViewer::drawEyeBeams()
{
	if (_data->eyeBeamMode == ModeNoEyeBeams)
		return;
	//drawGazeJointLimits();
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& characterNames = scene->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = scene->getCharacter((*iter));
		character->getSkeleton()->invalidate_global_matrices();
		character->getSkeleton()->update_global_matrices();
		SkJoint* eyeRight = character->getSkeleton()->search_joint("eyeball_right");
		float eyebeamLength = 100 * character->getHeight() / 175.0f;
		if (eyeRight)
		{
			SrMat gmat = eyeRight->gmat();
			SrVec localAxis = eyeRight->localGlobalAxis(2)*eyebeamLength;
			glPushMatrix();
			glMultMatrixf((const float*) gmat);
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(0, 0, 0);
			glVertex3f(localAxis[0],localAxis[1],localAxis[2]);
			glEnd();
			glPopMatrix();
		}
		SkJoint* eyeLeft = character->getSkeleton()->search_joint("eyeball_left");
		if (eyeLeft)
		{
			SrMat gmat = eyeLeft->gmat();
			SrVec localAxis = eyeLeft->localGlobalAxis(2)*eyebeamLength;
			glPushMatrix();
			glMultMatrixf((const float*) gmat);
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(0, 0, 0);
			//glVertex3f(0, 0, eyebeamLength);
			glVertex3f(localAxis[0],localAxis[1],localAxis[2]);
			glEnd();
			glPopMatrix();
		}
	}

}

void FltkViewer::drawEyeLids()
{
	if (_data->eyeLidMode == ModeNoEyeLids)
		return;

	glPushAttrib(GL_LIGHTING_BIT | GL_POINT_BIT);
	glDisable(GL_LIGHTING);
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& characterNames = scene->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = scene->getCharacter((*iter));
		MeControllerTreeRoot* controllerTree = character->ct_tree_p;
		int numControllers = controllerTree->count_controllers();
	
		MeCtEyeLid* eyelidCt = NULL;
		for (int c = 0; c < numControllers; c++)
		{
			MeController* controller = controllerTree->controller(c);
			eyelidCt = dynamic_cast<MeCtEyeLid*>(controller);
			if (eyelidCt)
				break;
		}
		if (!eyelidCt)
		{
			continue;
		}

		character->getSkeleton()->update_global_matrices();
		
		float upperHi;
		float upperLo;
		eyelidCt->get_upper_lid_range(upperLo, upperHi);

		float lowerHi;
		float lowerLo;
		eyelidCt->get_lower_lid_range(lowerLo, lowerHi);

		SkJoint* eyeLidUpperRight = character->getSkeleton()->search_joint("upper_eyelid_right");
		SkJoint* eyeLidUpperLeft = character->getSkeleton()->search_joint("upper_eyelid_left");
		SkJoint* eyeLidLowerRight = character->getSkeleton()->search_joint("lower_eyelid_right");
		SkJoint* eyeLidLowerLeft = character->getSkeleton()->search_joint("lower_eyelid_left");

		glPointSize(10);
		float range = character->getHeight() / 175.0f * 2.0f;
		if (eyeLidUpperRight)
		{
			const SkJoint* parent = eyeLidUpperRight->parent();
			if (parent)
			{
				SrMat gmat = parent->gmat();
				glPushMatrix();
				glMultMatrixf((const float*) gmat);
				glColor3f(1.0, 0.0, 0.0);
				SrVec offset = eyeLidUpperRight->offset();
				glBegin(GL_POINTS);
				glVertex3f(offset.x, offset.y, offset.z);
				glEnd();

				glTranslatef(offset.x, offset.y, offset.z);

				// add the up/down offsets
				glColor3f(1.0, 1.0, 0.0);
				glPushMatrix();
				glBegin(GL_LINE_LOOP);
				glVertex3f(-range, upperHi, 0);
				glVertex3f(range, upperHi, 0);
				glVertex3f(range, upperLo, 0);
				glVertex3f(-range, upperLo, 0);
				glEnd();
				glPopMatrix();

				glPopMatrix();
			}
		}
		if (eyeLidUpperLeft)
		{
			const SkJoint* parent = eyeLidUpperLeft->parent();
			if (parent)
			{
				SrMat gmat = parent->gmat();
				glPushMatrix();
				glMultMatrixf((const float*) gmat);
				glColor3f(1.0, 0.0, 0.0);
				SrVec offset = eyeLidUpperLeft->offset();
				glBegin(GL_POINTS);
				glVertex3f(offset.x, offset.y, offset.z);
				glEnd();

				glTranslatef(offset.x, offset.y, offset.z);

				// add the up/down offsets
				glColor3f(1.0, 1.0, 0.0);
				glPushMatrix();
				glBegin(GL_LINE_LOOP);
				glVertex3f(-range, upperHi, 0);
				glVertex3f(range, upperHi, 0);
				glVertex3f(range, upperLo, 0);
				glVertex3f(-range, upperLo, 0);
				glEnd();
				glPopMatrix();

				glPopMatrix();
			}
		}
		if (eyeLidLowerRight)
		{
			const SkJoint* parent = eyeLidLowerRight->parent();
			if (parent)
			{
				SrMat gmat = parent->gmat();
				glPushMatrix();
				glMultMatrixf((const float*) gmat);
				glColor3f(1.0, 0.0, 0.0);
				SrVec offset = eyeLidLowerRight->offset();
				glBegin(GL_POINTS);
				glVertex3f(offset.x, offset.y, offset.z);
				glEnd();

				glTranslatef(offset.x, offset.y, offset.z);

				// add the up/down offsets
				glColor3f(0.0, 1.0, 0.0);
				glPushMatrix();
				glBegin(GL_LINE_LOOP);
				glVertex3f(-range, lowerHi, 0);
				glVertex3f(range, lowerHi, 0);
				glVertex3f(range, lowerLo, 0);
				glVertex3f(-range, lowerLo, 0);
				glEnd();
				glPopMatrix();

				glPopMatrix();
			}
		}
		if (eyeLidLowerLeft)
		{
			const SkJoint* parent = eyeLidLowerLeft->parent();
			if (parent)
			{
				SrMat gmat = parent->gmat();
				glPushMatrix();
				glMultMatrixf((const float*) gmat);
				glColor3f(1.0, 0.0, 0.0);
				SrVec offset = eyeLidLowerLeft->offset();
				glBegin(GL_POINTS);
				glVertex3f(offset.x, offset.y, offset.z);
				glEnd();

				glTranslatef(offset.x, offset.y, offset.z);

				// add the up/down offsets
				glColor3f(0.0, 1.0, 0.0);
				glPushMatrix();
				glBegin(GL_LINE_LOOP);
				glVertex3f(-range, lowerHi, 0);
				glVertex3f(range, lowerHi, 0);
				glVertex3f(range, lowerLo, 0);
				glVertex3f(-range, lowerLo, 0);
				glEnd();
				glPopMatrix();

				glPopMatrix();
			}
		}
	}

	glPopAttrib();

}


void FltkViewer::drawCharacterBoundingVolumes()
{
	SmartBody::SBCollisionManager* colManager = SmartBody::SBScene::getScene()->getCollisionManager();
	if(!colManager->isEnable())
		return;
	bool singleChrCapsuleMode = colManager->getJointCollisionMode();

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& characterNames = scene->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = scene->getCharacter((*iter));
		if(singleChrCapsuleMode)
		{
			if (character && character->getGeomObject())
			{
				SrMat gmat = character->getGeomObject()->getGlobalTransform().gmat();
				this->drawColObject(character->getGeomObject(), gmat, .5);
			}
		}
		else
		{
			if(character)
			{
				SkSkeleton* sk = character->getSkeleton();
				float chrHeight = character->getHeight();
				const std::vector<SkJoint*>& origJnts = sk->joints();
				sk->update_global_matrices();
				for(unsigned int i=1; i<origJnts.size(); i++) // skip world_offset
				{
					SkJoint* j = origJnts[i];
					for(int k=0; k<j->num_children(); k++)
					{
						SkJoint* j_ch = j->child(k);
						const SrVec& offset = j_ch->offset();
						float offset_len = offset.norm();
						if(offset_len < 0.03*chrHeight) continue; // skip short bones
						std::string colObjName = character->getGeomObjectName() + ":" + j->jointName();
						if(k>0) colObjName = colObjName + ":" + boost::lexical_cast<std::string>(k);
						SBGeomObject* geomObj = colManager->getCollisionObject(colObjName);
						if(!geomObj) continue;
						const SrMat& gmat = j->gmat();
						this->drawColObject(geomObj, (SrMat&)gmat, .5);
					}
				}
			}
		}
	}
}

void FltkViewer::drawCharacterPhysicsObjs()
{
	float pawnSize = 1.0;
	
	SmartBody::SBPhysicsSim* phyEngine = SmartBody::SBPhysicsSim::getPhysicsEngine();
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& characterNames = scene->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = scene->getCharacter((*iter));
		SmartBody::SBPhysicsCharacter* phyChar = phyEngine->getPhysicsCharacter(character->getName());//character->getPhysicsCharacter();				
		if (!phyChar) 
		{			
			continue;
		}


		std::map<std::string,SmartBody::SbmJointObj*>& jointPhyObjs = phyChar->getJointObjMap();
		std::map<std::string,SmartBody::SbmJointObj*>::iterator mi;
		float totalMass = 0.f;
		for ( mi  = jointPhyObjs.begin();
			  mi != jointPhyObjs.end();
			  mi++)
		{
			SmartBody::SbmJointObj* obj = mi->second;
			totalMass += obj->getMass();
		}
		if (totalMass == 0.f) totalMass = 1.f;

		for ( mi  = jointPhyObjs.begin();
			  mi != jointPhyObjs.end();
			  mi++)
		{
			SmartBody::SbmJointObj* obj = mi->second;
			SrMat gmat = obj->getGlobalTransform().gmat();
			SmartBody::SBJoint* joint = obj->getSBJoint();	
			SmartBody::SBPhysicsSim* physics = SmartBody::SBPhysicsSim::getPhysicsEngine();
#if 1
			if (physics)
			{
				SrVec jointPos = physics->getJointConstraintPos(obj->getPhyJoint());
				SrSnSphere sphere;				
				sphere.shape().center = jointPos;//SrVec(0,-cap->extent,0);
				sphere.shape().radius = character->getHeight()*0.022f;
				float axisScale = character->getHeight()*0.01f;

			

				sphere.color(SrColor(1.f,0.f,0.f));
				glEnable(GL_LIGHTING);
				sphere.render_mode(srRenderModeSmooth);
				//SrGlRenderFuncs::render_sphere(&sphere);
				glDisable(GL_LIGHTING);		

				
				glBegin(GL_LINES);
				SrVec axis = physics->getJointRotationAxis(obj->getPhyJoint(),0)*axisScale;
				//axis = SrVec(gmat.get(0),gmat.get(1),gmat.get(2))*axisScale;
				glColor3f(1.f,0.f,0.f);
				glVertex3f(jointPos[0],jointPos[1],jointPos[2]);
				glVertex3f(jointPos[0]+axis[0],jointPos[1]+axis[1],jointPos[2]+axis[2]);

				glColor3f(0.f,1.f,0.f);
				axis = physics->getJointRotationAxis(obj->getPhyJoint(),1)*axisScale;
				//axis = SrVec(gmat.get(4),gmat.get(5),gmat.get(6))*axisScale;
				glVertex3f(jointPos[0],jointPos[1],jointPos[2]);
				glVertex3f(jointPos[0]+axis[0],jointPos[1]+axis[1],jointPos[2]+axis[2]);

				glColor3f(0.f,0.f,1.f);				
				axis = physics->getJointRotationAxis(obj->getPhyJoint(),2)*axisScale;
				//axis = SrVec(gmat.get(8),gmat.get(9),gmat.get(10))*axisScale;
				glVertex3f(jointPos[0],jointPos[1],jointPos[2]);
				glVertex3f(jointPos[0]+axis[0],jointPos[1]+axis[1],jointPos[2]+axis[2]);
				glEnd();


				// draw torque
// 				glBegin(GL_LINES);
// 				glColor3f(1.f,0.f,1.f);								
// 				axis = obj->getPhyJoint()->getJointTorque()*1.f;
// 				glVertex3f(jointPos[0],jointPos[1],jointPos[2]);
// 				glVertex3f(jointPos[0]+axis[0],jointPos[1]+axis[1],jointPos[2]+axis[2]);
// 				glEnd();				
			}
#endif

			if (_data->showmasses)
			{
				glPushAttrib(GL_LIGHTING_BIT);
				glEnable(GL_LIGHTING);
				glColor3f(1.0f, 1.0f, 0.0f);
				SrSnSphere sphere;
				float height = 200.0;
				float mass = obj->getMass();
				if (mass > 0)
				{
					float proportion = mass/totalMass;
					// draw a sphere of proportionate size to entire character to show mass distribution
					SrMat gmat = obj->getGlobalTransform().gmat();					
					sphere.shape().center = SrPnt(*gmat.pt(12), *gmat.pt(13), *gmat.pt(14));
					sphere.shape().radius = proportion * character->getHeight();
					sphere.color(SrColor(0.f,1.f,1.f));
					SrGlRenderFuncs::render_sphere(&sphere);					
				}
				glPopAttrib();				
			}
			this->drawColObject(obj->getColObj(), gmat, .5);
		}		

	}
}

void FltkViewer::drawPawns()
{
	if (_data->pawnmode == ModeNoPawns)
		return;

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	// determine the size of the pawns relative to the size of the characters
	float pawnSize = 1.0;
	const std::vector<std::string>& characterNames = scene->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = scene->getCharacter((*iter));
		pawnSize = character->getHeight()/ 30.0f;
		break;
	}

	SrCamera* currentCamera = scene->getActiveCamera();

	const std::vector<std::string>& pawnNames = scene->getPawnNames();
	for (std::vector<std::string>::const_iterator iter = pawnNames.begin();
		iter != pawnNames.end();
		iter++)
	{

		SmartBody::SBPawn* pawn = scene->getPawn((*iter));
		SrCamera* camera = dynamic_cast<SrCamera*>(pawn);
		if (camera)
		{ 
			if ((scene->getNumCameras() == 1) ||
			    (camera == currentCamera)) // don't draw the current active camera
			 continue;
		}
		
			
		if (!pawn->getSkeleton()) 
			continue;
		SbmCharacter* character = dynamic_cast<SbmCharacter*>(pawn);
		if (character)
			continue;

		const bool isVisible = pawn->getBoolAttribute("visible");
		if (!isVisible)
			continue;

		SrMat gmat = pawn->get_world_offset();//pawn->get_world_offset_joint()->gmat();		
		if (pawn->getGeomObject() && dynamic_cast<SBGeomNullObject*>(pawn->getGeomObject()) == NULL)
		{
			//pawn->colObj_p->updateTransform(gmat);
			//gmat = pawn->colObj_p->worldState.gmat();
			//if (pawn->getPhysicsObject())
			//{
			//	gmat = pawn->getPhysicsObject()->getGlobalTransform().gmat();
			//}
			//SrMat gmatPhy = pawn->getPhysicsObject()->getGlobalTransform().gmat();
			if (pawn->getBoolAttribute("showCollisionShape"))
				drawColObject(pawn->getGeomObject(),gmat, 1.0f);
		}
		else
		{
			// draw default sphere
			glPushAttrib(GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT  | GL_POLYGON_BIT);
			glDisable(GL_LIGHTING);
			glPushMatrix();
			glMultMatrixf((const float*) gmat);
			SmartBody::SBAttribute* attr = pawn->getAttribute("color");
			if (attr)
			{
				SmartBody::Vec3Attribute* colorAttribute = dynamic_cast<SmartBody::Vec3Attribute*>(attr);
				if (colorAttribute)
				{
					const SrVec& color = colorAttribute->getValue();
					glColor3f(color.x, color.y, color.z);
				}
			}
			else
			{
				if (camera)
					glColor3f(1.0, 1.0, 0.0);
				else
					glColor3f(1.0, 0.0, 0.0);
			}
			glPushMatrix();
			glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 2.0f * pawnSize, 0.0f);
			glEnd();
			glPopMatrix();
			SrSnSphere sphere;
			glPushMatrix();
			sphere.shape().center = SrPnt(0, 0, 0);
			sphere.shape().radius = pawnSize;
			sphere.render_mode(srRenderModeLines);
			SrGlRenderFuncs::render_sphere(&sphere);
			//glEnd();
			glPopMatrix();
			glPopMatrix();
			glPopAttrib();
		}		
	}
}


void FltkViewer::drawPoint( float cx, float cy, float cz, float size, SrVec& color )
{
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND); 
	glColor4f(color.x, color.y, color.z, 1.0f);
	glPointSize(5.0);
	glBegin(GL_POINTS); 
	glVertex3f(cx, cy, cz);//output vertex 
	glEnd(); 
	glDisable(GL_BLEND); 
}

void FltkViewer::drawTetra( SrVec vtxPos[4], SrVec& color )
{
	// draw a wireframe tetrahedron
	static int edgeIdx[6][2] = { {0,1}, {0,2}, {0,3}, {1,2}, {1,3}, {2,3} };
	glColor4f(color.x,color.y,color.z,1.f);
	glBegin(GL_LINES);
	for (int i=0;i<6;i++)
	{
		SrVec& p1 = vtxPos[edgeIdx[i][0]];
		SrVec& p2 = vtxPos[edgeIdx[i][1]];
		glVertex3f(p1.x,p1.y,p1.z);
		glVertex3f(p2.x,p2.y,p2.z);
	}
	glEnd();	
}


void FltkViewer::drawCircle(float cx, float cy, float cz, float r, int num_segments, SrVec& color) 
{ 
	float theta = 2.0f * 3.1415926f / float(num_segments); 
	float tangetial_factor = tanf(theta);//calculate the tangential factor 

	float radial_factor = cosf(theta);//calculate the radial factor 
	
	float x = r;//we start at angle = 0 

	float z = 0; 
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND); 
	glColor4f(color.x, color.y, color.z, 0.3f);
	glBegin(GL_LINE_LOOP); 
	for(int ii = 0; ii < num_segments; ii++) 
	{ 
		glVertex3f(x + cx, cy, z + cz);//output vertex 
        
		float tx = -z; 
		float tz = x; 

		x += tx * tangetial_factor; 
		z += tz * tangetial_factor; 

		x *= radial_factor; 
		z *= radial_factor; 
	} 
	
	glEnd(); 
	glDisable(GL_BLEND); 
}
static float spin_angle = 0.0f;
void FltkViewer::drawActiveArrow(SrVec& from, SrVec& to, int num, float width, SrVec& color, bool spin)
{
	spin_angle += 0.02f;
	if(spin_angle >= 3.1415926535f*2.0f) spin_angle = 0.0f;
	SrVec di = (to - from)/(float)num;


	float speed = 40.0f;
	float acc = -80.0f;
	float latency = 0.10f;
	
	_arrowTime += 0.01666f;

	SrVec p[6];
	p[5] = to;
	SrMat mat;
	mat.rot(di, spin_angle);

	p[1].x = width/2.0f;
	p[1].z = width/2.0f;
	p[2].x = width/2.0f;
	p[2].z = -width/2.0f;
	p[3].x = -width/2.0f;
	p[3].z = -width/2.0f;
	p[4].x = -width/2.0f;
	p[4].z = width/2.0f;

	p[1] = p[1]*mat;
	p[2] = p[2]*mat;
	p[3] = p[3]*mat;
	p[4] = p[4]*mat;
	p[1] = p[5]+p[1];
	p[2] = p[5]+p[2];
	p[3] = p[5]+p[3];
	p[4] = p[5]+p[4];

	p[0] = p[5]+di;

	float t_time = 0.0f;
	float t_speed = 0.0f;
	float dis = 0.0f;
	float s_time;
	float prev_dis = 0.0f;

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND); 
	glColor4f(color.x, color.y, color.z, 0.5f);
	glBegin(GL_TRIANGLES);
	//printf("\n");
	for(int i = 0; i < num; ++i)
	{

		p[0] -= di;
		p[1] -= di;
		p[2] -= di;
		p[3] -= di;
		p[4] -= di;
		p[5] -= di;
		t_time = _arrowTime + latency*i;
		t_speed = speed+acc*t_time;
		if(abs(t_speed) > abs(speed))
		{
			t_time = (t_speed+speed)/acc;
		}
		if(i == 0) s_time = t_time;
		prev_dis = dis;
		dis = speed*t_time+0.5f*acc*t_time*t_time;
		t_speed = speed+acc*t_time;
		
		glColor4f(color.x, color.y, color.z, 0.5f*abs(t_speed)/abs(speed));

		//printf("\n%f", dis);

		p[0].y += dis-prev_dis;
		p[1].y += dis-prev_dis;
		p[2].y += dis-prev_dis;
		p[3].y += dis-prev_dis;
		p[4].y += dis-prev_dis;
		p[5].y += dis-prev_dis;

		glVertex3f(p[0].x, p[0].y, p[0].z);
		glVertex3f(p[2].x, p[2].y, p[2].z);
		glVertex3f(p[1].x, p[1].y, p[1].z);

		glVertex3f(p[0].x, p[0].y, p[0].z);
		glVertex3f(p[3].x, p[3].y, p[3].z);
		glVertex3f(p[2].x, p[2].y, p[2].z);

		glVertex3f(p[0].x, p[0].y, p[0].z);
		glVertex3f(p[4].x, p[4].y, p[4].z);
		glVertex3f(p[3].x, p[3].y, p[3].z);

		glVertex3f(p[0].x, p[0].y, p[0].z);
		glVertex3f(p[1].x, p[1].y, p[1].z);
		glVertex3f(p[4].x, p[4].y, p[4].z);

		glVertex3f(p[1].x, p[1].y, p[1].z);
		glVertex3f(p[2].x, p[2].y, p[2].z);
		glVertex3f(p[3].x, p[3].y, p[3].z);

		glVertex3f(p[1].x, p[1].y, p[1].z);
		glVertex3f(p[3].x, p[3].y, p[3].z);
		glVertex3f(p[4].x, p[4].y, p[4].z);
	}
	_arrowTime = s_time;
	glEnd();
	glDisable(GL_BLEND); 
}

/*void FltkViewer::drawActiveArrow(SrVec& from, SrVec& to, int num, float width, SrVec& color, bool spin)
{
	spin_angle += 0.02f;
	if(spin_angle >= 3.1415926535f*2.0f) spin_angle = 0.0f;
	SrVec di = (to - from)/num;


	float speed = 40.0f;
	float acc = -80.0f;
	float latency = 0.06f;
	
	time += 0.01666f;

	SrVec p[4];
	p[3] = to;
	SrMat mat;
	mat.rot(di, spin_angle);

	p[1].x = width/2.0f;
	p[2].x = -width/2.0f;

	p[1] = p[1]*mat;
	p[2] = p[2]*mat;
	p[1] = p[3]+p[1];
	p[2] = p[3]+p[2];
	p[0] = p[3]+di;

	float t_time = 0.0f;
	float t_speed = 0.0f;
	float dis = 0.0f;
	float s_time;
	float prev_dis = 0.0f;

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND); 
	glColor4f(color.x, color.y, color.z, 0.3f);
	glBegin(GL_TRIANGLES);
	//printf("\n");
	for(int i = 0; i < num; ++i)
	{
		p[0] -= di;
		p[3] -= di;
		p[1] -= di;
		p[2] -= di;
		t_time = time+latency*i;
		t_speed = speed+acc*t_time;
		if(abs(t_speed) > abs(speed))
		{
			t_time = (t_speed+speed)/acc;
		}
		if(i == 0) s_time = t_time;
		prev_dis = dis;
		dis = speed*t_time+0.5f*acc*t_time*t_time;

		//printf("\n%f", dis);

		p[0].y += dis-prev_dis;
		p[1].y += dis-prev_dis;
		p[2].y += dis-prev_dis;
		p[3].y += dis-prev_dis;

		glVertex3f(p[0].x, p[0].y, p[0].z);
		glVertex3f(p[1].x, p[1].y, p[1].z);
		glVertex3f(p[2].x, p[2].y, p[2].z);

		glVertex3f(p[0].x, p[0].y, p[0].z);
		glVertex3f(p[2].x, p[2].y, p[2].z);
		glVertex3f(p[1].x, p[1].y, p[1].z);
	}
	time = s_time;
	glEnd();
	glDisable(GL_BLEND); 
}*/

#define FOOT_PRINT_NUM 20
static SrVec footprintpos[3][FOOT_PRINT_NUM][2];
static int footprintstart = 0;
//static int footprintend = 0;
static SrVec footprintcolor[FOOT_PRINT_NUM][2];
static SrVec footprintorientation[FOOT_PRINT_NUM][2];
static SrVec footprintnormal[FOOT_PRINT_NUM][2];
static float footprinttime[FOOT_PRINT_NUM][2];
static int footprintside[FOOT_PRINT_NUM][2];
static SrVec footprint[3][12][2];
static float fadeouttime = 9.0f;
static float footprintscacle = 1.0f;
static SrVec footprintoffset;


void FltkViewer::ChangeOffGroundHeight(Fl_Widget* widget, void* data)
{
	FltkViewer* window = (FltkViewer*) data;

	const char* motionName = window->off_height_window->value();

	window->redraw();
}

void FltkViewer::init_foot_print()
{
	footprintoffset = SrVec(0.0f, 0.0f, -16.0f);
	int i = -1;
	footprint[0][++i][0].x = 8.0f;
	footprint[0][i][0].z = 4.0f;

	footprint[0][++i][0].x = 4.0f;

	footprint[0][++i][0].x = -6.0f;

	footprint[0][++i][0].x = -10.0f;
	footprint[0][i][0].z = 4.0f;

	footprint[0][++i][0].x = -9.0f;
	footprint[0][i][0].z = 17.0f;

	footprint[0][++i][0].x = 7.0f;
	footprint[0][i][0].z = 17.0f;

	i = -1;

	footprint[1][++i][0].x = 7.0f;
	footprint[1][i][0].z = 20.0f;

	footprint[1][++i][0].x = 7.0f;
	footprint[1][i][0].z = 20.0f;

	footprint[1][++i][0].x = -9.0f;
	footprint[1][i][0].z = 20.0f;

	footprint[1][++i][0].x = -22.0f;
	footprint[1][i][0].z = 55.0f;

	footprint[1][++i][0].x = 10.0f;
	footprint[1][i][0].z = 50.0f;

	footprint[1][++i][0].x = 10.0f;
	footprint[1][i][0].z = 50.0f;

	i = -1;

	footprint[2][++i][0].x = -22.0f;
	footprint[2][i][0].z = 55.0f;

	footprint[2][++i][0].x = -22.0f;
	footprint[2][i][0].z = 55.0f;

	footprint[2][++i][0].x = -12.0f;
	footprint[2][i][0].z = 69.0f;

	footprint[2][++i][0].x = -6.0f;
	footprint[2][i][0].z = 69.0f;

	footprint[2][++i][0].x = 10.0f;
	footprint[2][i][0].z = 50.0f;

	footprint[2][++i][0].x = 10.0f;
	footprint[2][i][0].z = 50.0f;
	

	footprintscacle = 0.3f;

	float d_z;
	float d_x = 2.0f;
	for(int i = 0; i < 3; ++i)
	{
		if(i == 0) d_z = 0.0f;
		else if(i == 1) d_z = -55.0f-footprintoffset.z;
		else if(i == 2) d_z = -69.0f-footprintoffset.z;
		for(int j = 0; j < 6; ++j)
		{
			footprint[i][j][0].x += d_x;
			footprint[i][j][0].z += d_z;
			footprint[i][j][0] += footprintoffset;
			footprint[i][j][0] *= footprintscacle;
			footprint[i][5-j][1] = footprint[i][j][0];
			footprint[i][5-j][1].x = -footprint[i][5-j][1].x;
		}
	}

}

void FltkViewer::drawKinematicFootprints(int index)
{
#if 0
	int i = 0;
	SrVec vertex;
	SrMat mat, tmat;
	SrVec forward(0.0f ,0.0f, 1.0f);
	SrVec up(0.0f, 1.0f, 0.0f);

	SrVec color;

	//glDisable(GL_DEPTH_TEST);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND); 

	for(int j = 0; j < FOOT_PRINT_NUM; ++j)
	{
		//footprintpos[j] = SrVec(40, 0, 40);
		if(footprinttime[j][index] > fadeouttime) continue;
		up.set(0.0f, 1.0f, 0.0f);
		mat.rot(forward, footprintorientation[j][index]);
		up = up * mat;
		tmat.rot(up, footprintnormal[j][index]);

		//if(footprintside[j] == 0) color.set(0.0f, 1.0f, 0.4f);
		//else color.set(0.0f, 0.4f, 1.0f);

		glColor4f(footprintcolor[j][index].x, footprintcolor[j][index].y, footprintcolor[j][index].z, 0.6f*(fadeouttime-footprinttime[j][index])/fadeouttime);
		
		for(int k = 0; k < 3; ++k)
		{
			if(footprintpos[k][j][index].len() == 0.0f) continue;
			glBegin(GL_POLYGON);
			for(int i = 0; i < 6; ++i)
			{
				vertex = footprint[k][i][footprintside[j][index]];
				vertex = vertex * mat;
				vertex = vertex * tmat;
				vertex += footprintpos[k][j][index];
				vertex.y += index;
				glVertex3f(vertex.x, vertex.y, vertex.z);
			}
			glEnd();
		}
		
	}
	glDisable(GL_BLEND); 

	for(int i = 0; i < FOOT_PRINT_NUM; ++i)
	{
		if( i == footprintstart) continue;
		if(footprinttime[i][index] > fadeouttime) continue;
		footprinttime[i][index] += 0.0166666f;
	}
#endif

#if 0
	SmartBody::SBCharacter* curChar = dynamic_cast<SmartBody::SBCharacter*>(getCurrentCharacter());
	Heightfield* heightField = SmartBody::SBScene::getScene()->getHeightfield();	
	if (!curChar) return;
	SrVec faceDir = curChar->getFacingDirection();
	float scale = curChar->getHeight()*0.1f;
	for (int i=0;i<2;i++)
	{
		const std::vector<SrVec>& footSteps = curChar->getFootSteps(i);
		SrVec color = i == 0 ? SrVec(1.f,0.f,0.f) : SrVec(0.f, 1.f, 0.f);
		for (unsigned int k=0;k<footSteps.size();k++)
		{
			SrVec footStep = footSteps[k] + SrVec(0,0.05f*scale,0); // add a small height offset
			SrVec adjustedFootStep = footStep + faceDir * scale;			
			if (heightField)
			{
				SrVec tnormal;
				heightField->get_elevation(footStep.x,footStep.z,(float*)tnormal);
				SrVec sideVec = cross(faceDir,tnormal);
				SrVec newDir = cross(tnormal,sideVec); newDir.normalize();
				adjustedFootStep = footStep + newDir * scale;
			}
			drawArrow(footStep, adjustedFootStep, scale*0.5f,color);			
		}
		SrSnSphere sphere;
		sphere.shape().center = curChar->curFootIKPos[i];
		sphere.shape().radius = scale*0.3f;		
		sphere.render_mode(srRenderModeLines);
		sphere.color(SrColor(color.x,color.y,color.z,1.f));
		SrGlRenderFuncs::render_sphere(&sphere);	
		SrMat mat; mat.set_translation(curChar->curFootIKPos[i]);
		std::string text = boost::lexical_cast<std::string>(curChar->flightTime[i]);
		drawText(mat,scale*0.005f,text);
	}
#endif
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	SmartBody::SBCharacter* curChar = dynamic_cast<SmartBody::SBCharacter*>(getCurrentCharacter());
	if (!curChar) return;
	SrVec faceDir = curChar->getFacingDirection();
	float scale = curChar->getHeight()*0.1f;	
	std::vector<std::string> jointConsNames = curChar->getJointConstraintNames();
	SrVec color = SrVec(1.f,0.f,0.f);
	for (unsigned int i=0;i<jointConsNames.size();i++)
	{
		SmartBody::TrajectoryRecord* traj = curChar->getJointTrajectoryConstraint(jointConsNames[i]);		
		SrSnSphere sphere;
		sphere.shape().center = traj->jointTrajGlobalPos;
		sphere.shape().radius = scale*0.3f;		
		sphere.render_mode(srRenderModeLines);
		sphere.color(SrColor(color.x,color.y,color.z,1.f));
		SrGlRenderFuncs::render_sphere(&sphere);			
	}

	//glEnable(GL_DEPTH_TEST);

	
}


void FltkViewer::newPrints(bool newprint, int index, SrVec& pos, SrVec& orientation, SrVec& normal, SrVec& color, int side, int type)
{
	if(newprint)
	{
		++footprintstart;
		if(footprintstart >= FOOT_PRINT_NUM) footprintstart = 0;

		SrVec v;
		bool j = false;
		for(int i = 0; i < FOOT_PRINT_NUM; ++i)
		{
			v = footprintpos[index][i][type];
			v.y = 0.0f;
			v = v - pos;
			if(v.len()<5.0f)
			{
				footprintstart = i;
				j = true;
				break;
			}
			/*else if(footprinttime[i]> fadeouttime) 
			{
				footprintstart = i;
			}*/
		}
		if(!j)
		{
			for(int i = 0; i < 3; ++i)
			{
				footprintpos[i][footprintstart][type].set(0.0f, 0.0f, 0.0f);
			}
		}
	}
	footprintcolor[footprintstart][type] = color;
	footprintpos[index][footprintstart][type] = pos;
	footprinttime[footprintstart][type] = 0.0f;
	footprintorientation[footprintstart][type] = orientation;
	footprintnormal[footprintstart][type] = normal;
	footprintside[footprintstart][type] = side;
}

void FltkViewer::drawArrow(SrVec& from, SrVec& to, float width, SrVec& color)
{
	SrVec p[2];
	SrVec c[4];
	SrMat mat;
	SrVec normal;
	int seg = 5;
	
	SrVec di = from - to;
	di.normalize();
	normal = di;
	di *= width;
	mat.roty(3.15159265f/6);
	p[0] = di*mat + to;
	c[0] = (di/2.0f)*mat + to;
	mat.roty(-3.15159265f/6);
	p[1] = di*mat + to;
	c[1] = (di/2.0f)*mat + to;

	c[2] = c[0] + (from-to) - normal*di.len()/2.0f;
	c[3] = c[1] + (from-to) - normal*di.len()/2.0f;


	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND); 

	glColor4f(color.x, color.y, color.z, 1.0f);

	glBegin(GL_TRIANGLES);
	glVertex3f(to.x, to.y, to.z);
	glVertex3f(p[0].x, p[0].y, p[0].z);
	glVertex3f(p[1].x, p[1].y, p[1].z);
	glVertex3f(to.x, to.y, to.z);
	glVertex3f(p[1].x, p[1].y, p[1].z);
	glVertex3f(p[0].x, p[0].y, p[0].z);

	SrVec t[4];
	SrVec gap;
	gap = (from-to)/((float)seg*6.0f);
	t[3] = c[0]+gap;
	t[2] = c[1]+gap;
	for(int i = 0; i < seg; ++i)
	{
		glColor4f(color.x, color.y, color.z, (float)((float)(seg-i)/(float)(seg+1)));

		t[0] = t[3]+gap;
		t[1] = t[2]+gap;
		t[3] = t[0]+gap*2;
		t[2] = t[1]+gap*2;
		glVertex3f(t[0].x, t[0].y, t[0].z);
		glVertex3f(t[1].x, t[1].y, t[1].z);
		glVertex3f(t[2].x, t[2].y, t[2].z);
		glVertex3f(t[0].x, t[0].y, t[0].z);
		glVertex3f(t[2].x, t[2].y, t[2].z);
		glVertex3f(t[1].x, t[1].y, t[1].z);
	
		glVertex3f(t[0].x, t[0].y, t[0].z);
		glVertex3f(t[3].x, t[3].y, t[3].z);
		glVertex3f(t[2].x, t[2].y, t[2].z);
		glVertex3f(t[0].x, t[0].y, t[0].z);
		glVertex3f(t[2].x, t[2].y, t[2].z);
		glVertex3f(t[3].x, t[3].y, t[3].z);
	}
	glEnd();
	glDisable(GL_BLEND); 
}

void FltkViewer::drawJointLabels()
{
	if (!_data->showJointLabels)
		return;

	SbmCharacter* character = getCurrentCharacter();
	if (!character)
		return;
	
	glPushAttrib(GL_LIGHTING | GL_COLOR_BUFFER_BIT);
	
	float sceneScale = SmartBody::SBScene::getScene()->getScale();
	if (sceneScale == 0.0f)
		sceneScale = 1.0f;
	float textSize = .01f / sceneScale * .02f;
	int numJoints = character->getSkeleton()->getNumJoints();
	for (int j = 0; j < numJoints; j++)
	{
		SkJoint* joint = character->getSkeleton()->getJoint(j);
		std::string text = joint->jointName();
		const SrMat& mat = joint->gmat();
		drawText(mat, textSize, text);

	}
	glPopAttrib();	
}

void FltkViewer::drawGestures()
{
	if (_gestureData->currentCharacter == "")
		return;


	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(_gestureData->currentCharacter);
	if (!character)
		return;

	if (!_data->showgesture)
		return;
	
	if (_gestureData->gestureSections.size() < 2)
		return;

	glDisable(GL_LIGHTING);

	std::string wristLfName = "l_wrist";
	std::string wristRtName = "r_wrist";
	SmartBody::SBJoint* wristLfJoint = character->getSkeleton()->getJointByName(wristLfName);
	SmartBody::SBJoint* wristRtJoint = character->getSkeleton()->getJointByName(wristRtName);
	if (!wristLfJoint || !wristRtJoint)
		return;

	character->getSkeleton()->update_global_matrices();
	SrMat wristLfGMat = wristLfJoint->gmat();
	SrVec wristLfVec = SrVec(wristLfGMat.get(12), wristLfGMat.get(13), wristLfGMat.get(14));
	SrMat wristRtGMat = wristRtJoint->gmat();
	SrVec wristRtVec = SrVec(wristRtGMat.get(12), wristRtGMat.get(13), wristRtGMat.get(14));

	if (!_gestureData->pause)
	{
		std::list<SrVec>& leftWriteData = _gestureData->gestureSections[_gestureData->gestureSections.size() - 2].data;
		if ((int)leftWriteData.size() >= _gestureData->displayMaximum)
			leftWriteData.pop_front();
		leftWriteData.push_back(wristLfVec);
		
		std::list<SrVec>& rightWriteData = _gestureData->gestureSections[_gestureData->gestureSections.size() - 1].data;
		if ((int)rightWriteData.size() >= _gestureData->displayMaximum)
			rightWriteData.pop_front();
		rightWriteData.push_back(wristRtVec);
	}

	glBegin(GL_LINES);
	for (size_t gsId = 0; gsId < _gestureData->gestureSections.size(); ++gsId)
	{
		SrVec& sectionColor = _gestureData->gestureSections[gsId].color;
		glColor3f(sectionColor.x, sectionColor.y, sectionColor.z);
		std::list<SrVec>::iterator iter = _gestureData->gestureSections[gsId].data.begin();
		for (; iter != _gestureData->gestureSections[gsId].data.end(); iter++)
		{
			std::list<SrVec>::iterator iter1 = iter;
			iter1++;
			if (iter1 != _gestureData->gestureSections[gsId].data.end())
			{
				glVertex3f(iter->x, iter->y, iter->z);
				glVertex3f(iter1->x, iter1->y, iter1->z);
			}
		}
	}
	glEnd();

	if (_gestureData->currentStatus != GestureData::OTHER)
	{
		GestureData::SyncPointData left;
		left.side = 0;
		left.type = _gestureData->currentStatus;
		left.location = wristLfVec;
		left.color = _gestureData->getSyncPointColor(_gestureData->currentStatus);
		_gestureData->syncPoints.push_back(left);
		GestureData::SyncPointData right;
		right.side = 1;
		right.type = _gestureData->currentStatus;
		right.location = wristRtVec;
		right.color = _gestureData->getSyncPointColor(_gestureData->currentStatus);
		_gestureData->syncPoints.push_back(right);
		_gestureData->currentStatus = GestureData::OTHER;
	}

	for (size_t i = 0; i < _gestureData->syncPoints.size(); ++i)
	{
		GestureData::SyncPointData data = _gestureData->syncPoints[i];
		drawPoint(data.location.x, data.location.y, data.location.z, 20.0f, data.color);
	}

	glEnable(GL_LIGHTING);

}


static int pre_dominant = 0;
void FltkViewer::drawLocomotion()
{
	int counter = 0;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& characterNames = scene->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = scene->getCharacter((*iter));
		//if(!character->get_locomotion_ct()->is_valid()) continue;
		float x, y, z, yaw, pitch, roll;
		character->get_world_offset(x, y, z, yaw, pitch, roll);
		SrVec arrow_start(x, y, z);
		SrVec arrow_end;
		if(_data->showvelocity)
		{
			
		}
		if(_data->showselection)
		{
			/*if(counter == _locoData->char_index)
			{
				float height = character->getHeight();
				SrVec color;
				//float base_height = character->get_locomotion_ct()->translation_joint_height;
				float base_height = character->getHeight() / 2.0f;
				arrow_end = arrow_start;
				arrow_end.y += height - base_height;
				arrow_start.y += height - base_height + 30.0f * character->getHeight() / 200.0f;
				color.set(1.0f, 0.0f, 0.0f);
				drawActiveArrow(arrow_start, arrow_end, 3, 10.0f, color, false);
			}
			*/
		}
		if(_data->showkinematicfootprints)
		{
			
		}
		if(_data->showlocofootprints)
		{
		
		}
		if (_data->showtrajectory)
		{
			if (!character->param_animation_ct)
				return;
			glPushAttrib(GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
			std::string baseJointName = character->param_animation_ct->getBaseJointName();
			SkJoint* baseJ = character->getSkeleton()->search_joint(baseJointName.c_str());
			if (!baseJ) return;
			character->getSkeleton()->update_global_matrices();
			SrMat baseGM = baseJ->gmat();
			SrVec baseVec = SrVec(baseGM.get(12), baseGM.get(13), baseGM.get(14));
			if (character->trajectoryBuffer.size() >= SbmCharacter::trajectoryLength)
				character->trajectoryBuffer.pop_front();
			SrVec prevBaseVec = baseVec;
			if (character->trajectoryBuffer.size())
				prevBaseVec = character->trajectoryBuffer.back();
			//if ((baseVec-prevBaseVec).len() > character->getHeight()*0.01f)
				character->trajectoryBuffer.push_back(baseVec);
			std::list<SrVec>::iterator iter = character->trajectoryBuffer.begin();
			glColor3f(1.0f, 1.0f, 0.0f);
			glBegin(GL_LINES);
			for (; iter != character->trajectoryBuffer.end(); iter++)
			{
				std::list<SrVec>::iterator iter1 = iter;
				iter1++;
				if (iter1 != character->trajectoryBuffer.end())
				{
					//glVertex3f(iter->x, 0.5f, iter->z);
					//glVertex3f(iter1->x, 0.5f, iter1->z);
					glVertex3f(iter->x, iter->y, iter->z);
					glVertex3f(iter1->x, iter1->y, iter1->z);
				}
			}
			glEnd();

			glColor3f(1.0f, 0.0f, 1.0f);
			glBegin(GL_LINES);
			int num = int(character->trajectoryGoalList.size() / 3) - 1;
			if (num >= 2) // only show multi-goal paths
				for (int i = 0; i < num; i++)
				{
					//glVertex3f(character->trajectoryGoalList[(size_t)i * 3 + 0], 0.5f, character->trajectoryGoalList[(size_t)i * 3 + 2]);
					//glVertex3f(character->trajectoryGoalList[((size_t)i + 1) * 3 + 0], 0.5f, character->trajectoryGoalList[((size_t)i + 1) * 3 + 2]);
					glVertex3f(character->trajectoryGoalList[(size_t)i * 3 + 0],character->trajectoryGoalList[(size_t)i * 3 + 1], character->trajectoryGoalList[(size_t)i * 3 + 2]);
					glVertex3f(character->trajectoryGoalList[((size_t)i + 1) * 3 + 0], character->trajectoryGoalList[((size_t)i + 1) * 3 + 1], character->trajectoryGoalList[((size_t)i + 1) * 3 + 2]);
				}
			glEnd();

			float scale = (float)1.0/SmartBody::SBScene::getScene()->getScale(); // if it's in meters
			SmartBody::SBSteerAgent* steerAgent = SmartBody::SBScene::getScene()->getSteerManager()->getSteerAgent(character->getName());
			if (steerAgent)
			{
				PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);

				SrVec color1(0.1f, 0.3f, 1.0f);
				SrVec steerDir = ppraiAgent->curSteerPos + ppraiAgent->curSteerDir * 0.5f*scale;
				drawArrow(ppraiAgent->curSteerPos, steerDir, 0.15f*scale, color1);

				SrVec color2(0.f,1.f,0.f);
				drawCircle(ppraiAgent->nextSteerPos.x,ppraiAgent->nextSteerPos.y,ppraiAgent->nextSteerPos.z, 0.3f*scale, 72, color2);
				SrVec nextSteerPos = ppraiAgent->nextSteerPos + ppraiAgent->nextSteerDir * 0.5*scale;
				drawArrow(ppraiAgent->nextSteerPos, nextSteerPos, 0.15f*scale, color2);

				SrVec color3(1.f,0.f,0.f);
				drawCircle(ppraiAgent->nextPtOnPath.x, ppraiAgent->nextPtOnPath.y, ppraiAgent->nextPtOnPath.z, 0.3f*scale, 72, color3);											
			}
			glPopAttrib();
		}
	}
}

void FltkViewer::drawInteractiveLocomotion()
{
	if (!_data->interactiveLocomotion)
		return;

	float pawnSize = 1.0;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& characterNames = scene->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = scene->getCharacter((*iter));
		pawnSize = character->getHeight() / 8.0f;
		break;
	}

	glPushMatrix();
	glTranslatef(interactivePoint[0], 0.0, interactivePoint[2]);
	SrSnSphere sphere;
	sphere.shape().center = SrPnt(0, 0, 0);
	sphere.shape().radius = pawnSize;
	sphere.render_mode(srRenderModeLines);
	SrGlRenderFuncs::render_sphere(&sphere);
	glPopMatrix();
}

void FltkViewer::drawDynamics()
{
	if (_data->dynamicsMode == ModeNoDynamics && !_data->showmasses)
		return;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& characterNames = scene->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = scene->getCharacter((*iter));
		character->getSkeleton()->update_global_matrices();

		const std::vector<SkJoint*>& joints = character->getSkeleton()->joints();
		
		int numJoints = 0;
		float totalMass = 0;
		SrVec com(0, 0, 0);
		for (size_t j = 0; j < joints.size(); j++)
		{
			float mass = joints[j]->mass();
			if (mass > 0)
			{
				totalMass += mass;
				SrMat gmat = joints[j]->gmat();
				SrVec loc(*gmat.pt(12), *gmat.pt(13), *gmat.pt(14)); 
				com += mass * loc;
				numJoints++;
			}
		}
		if (totalMass != 0)
				com /= totalMass;

		glPushMatrix();
		glPushAttrib(GL_POINT_BIT);
		glPointSize(4.0);
		
		if (_data->dynamicsMode == ModeShowCOM ||
			_data->dynamicsMode == ModeShowCOMSupportPolygon)
		{
			
			// draw the center of mass of the character
			glColor3f(1.0, 1.0, 0.0);
			glPushMatrix();
			glTranslatef(com[0], com[1], com[2]);
			glBegin(GL_POINTS);
			glVertex3f(0.0, 0.0, 0.0);
			glEnd();
			glPopMatrix();

			
		}

		if (_data->dynamicsMode == ModeShowCOMSupportPolygon)
		{
			// draw the support polygon of the character
			// get the heel/toe points for the left and right foot
			SrVec polygon[4];
			// left heel, toe
			SkJoint* leftFoot = character->getSkeleton()->search_joint("l_ankle");
			if (leftFoot)
			{
				SrMat gmat = leftFoot->gmat();
				polygon[0].set(*gmat.pt(12), *gmat.pt(13), *gmat.pt(14)); 
			}
			SkJoint* leftToe = character->getSkeleton()->search_joint("l_toe");
			if (leftToe)
			{
				SrMat gmat  = leftToe->gmat();
				polygon[1].set(*gmat.pt(12), *gmat.pt(13), *gmat.pt(14)); 
			}			
			// right heel, toe
			SkJoint* rightFoot =character->getSkeleton()->search_joint("r_ankle");
			if (rightFoot)
			{
				SrMat gmat = rightFoot->gmat();
				polygon[3].set(*gmat.pt(12), *gmat.pt(13), *gmat.pt(14)); 
			}
			SkJoint* rightToe = character->getSkeleton()->search_joint("r_toe");
			if (rightToe)
			{
				SrMat gmat = rightToe->gmat();
				polygon[2].set(*gmat.pt(12), *gmat.pt(13), *gmat.pt(14)); 
			}			

			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_LINE_LOOP);
			for (int x = 0; x < 4; x++)
			{
				glVertex3f(polygon[x][0], polygon[x][1], polygon[x][2]);
			}
			glEnd();

			// show the center of mass projected on the ground as well
			float yLoc = 0;
			for (int i = 0; i < 4; i++)
				yLoc += polygon[i][1];
			yLoc /= 4.0;
			glColor3f(1.0, 1.0, 0.0);
			glPushMatrix();
			glTranslatef(com[0], yLoc, com[2]);
			glBegin(GL_POINTS);
			glVertex3f(0.0, 0.0, 0.0);
			glEnd();
			glPopMatrix();

		}

		// should be rendered based on physics geometry.
		/*
		if (_data->showmasses && totalMass > 0)
		{
			glPushAttrib(GL_LIGHTING_BIT);
			glEnable(GL_LIGHTING);
			glColor3f(1.0f, 1.0f, 0.0f);
			SrSnSphere sphere;
			float height = 200.0;
			for (size_t j = 0; j < joints.size(); j++)
			{
				float mass = joints[j]->mass();
				if (mass > 0)
				{
					float proportion = mass / totalMass;
					// draw a sphere of proportionate size to entire character to show mass distribution
					SrMat gmat = joints[j]->gmat();
					glPushMatrix();
					sphere.shape().center = SrPnt(*gmat.pt(12), *gmat.pt(13), *gmat.pt(14));
					sphere.shape().radius = proportion * height;
					SrGlRenderFuncs::render_sphere(&sphere);
					// draw sphere with size (proportion * height)
					glPopMatrix();
				}
			}
			
			glPopAttrib();
		}
		*/
		
		glPopAttrib();
		glPopMatrix();
	}
}


SbmCharacter* FltkViewer::getCurrentCharacter()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	 SbmPawn* selectedPawn = getObjectManipulationHandle().get_selected_pawn();
	 if (!selectedPawn)
	 {
		 if (_lastSelectedCharacter == "")
			 return NULL;
		 SbmCharacter* character = scene->getCharacter(_lastSelectedCharacter);
		 if (character)
		 {
			 return character;
		 }
		 else
		 {
			 const std::vector<std::string>& characterNames =  scene->getCharacterNames();
			 if (characterNames.size() > 0)
			 {
				 // get the first character
				 SbmCharacter* character = scene->getCharacter(characterNames[0]);
				  return character;
			 }
			 else
			 {
				 return NULL;
			 }
		 }
	 }

	 SbmCharacter* character = dynamic_cast<SbmCharacter*> (selectedPawn);
	 return character;
}

SmartBody::SBAnimationBlend* FltkViewer::getCurrentCharacterAnimationBlend()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SbmCharacter* sbmChar = getCurrentCharacter();
	SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*>(sbmChar);
	if (!character)
		return NULL;

	SmartBody::SBAnimationBlend* animBlend = NULL;

	MeCtParamAnimation* panimCt = character->param_animation_ct;
	if (panimCt)
	{
		PABlendData* pblendData = panimCt->getCurrentPABlendData();
		if (pblendData)
			animBlend = dynamic_cast<SmartBody::SBAnimationBlend*>(pblendData->state);
	}	
	return animBlend;
}

MeCtExampleBodyReach* FltkViewer::getCurrentCharacterBodyReachController()
{
	SbmCharacter* character = getCurrentCharacter();

	MeCtExampleBodyReach* reachCt = NULL;
	if ( character )
	{
		MeCtSchedulerClass* reachSched = character->reach_sched_p;
		if (!reachSched)
			return NULL;
		MeCtSchedulerClass::VecOfTrack reach_tracks = reachSched->tracks();		
		MeCtReach* tempCt = NULL;
		for (unsigned int c = 0; c < reach_tracks.size(); c++)
		{
			MeController* controller = reach_tracks[c]->animation_ct();		
			//reachCt = dynamic_cast<MeCtConstraint*>(controller);
			reachCt = dynamic_cast<MeCtExampleBodyReach*>(controller);
			//tempCt  = dynamic_cast<MeCtReach*>(controller);
			if (reachCt)
				break;
		}		
	}
	return reachCt;
}

MeCtConstraint* FltkViewer::getCurrentCharacterConstraintController()
{
	SbmCharacter* character = getCurrentCharacter();

	MeCtConstraint* reachCt = NULL;
	if ( character )
	{
		MeCtSchedulerClass* reachSched = character->constraint_sched_p;
		if (!reachSched)
			return NULL;
		MeCtSchedulerClass::VecOfTrack reach_tracks = reachSched->tracks();		
		MeCtReach* tempCt = NULL;
		for (unsigned int c = 0; c < reach_tracks.size(); c++)
		{
			MeController* controller = reach_tracks[c]->animation_ct();		
			reachCt = dynamic_cast<MeCtConstraint*>(controller);
			//tempCt  = dynamic_cast<MeCtReach*>(controller);
			if (reachCt)
				break;
		}		
	}
	return reachCt;
}

// visualize example data and other relevant information for reach controller
void FltkViewer::drawReach()
{	
	if (_data->reachRenderMode == ModeNoExamples)
		return;
	
	glPushAttrib(GL_LIGHTING_BIT | GL_POINT_BIT);
	glDisable(GL_LIGHTING);
	
	//MeCtDataDrivenReach* reachCt = getCurrentCharacterReachController();

	MeCtExampleBodyReach* reachCt = getCurrentCharacterBodyReachController();
	
	SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*>(getCurrentCharacter());
	if (!character)
		return;
	float sphereSize = character->getHeight() / 80.0f;	
	if (reachCt)
	{	
		// draw reach Example data
		/*
		const VecOfPoseExample& exampleData = reachCt->ExamplePoseData().PoseData();
		const VecOfPoseExample& resampledData = reachCt->ResampledPosedata().PoseData();
		character->skeleton_p->update_global_matrices();		
		//SkJoint* root = reachCt->getRootJoint();		
		*/
		

		MeCtReachEngine* re = reachCt->getReachEngine();
		if (!re)
			return;

		ReachStateData* rd = re->getReachData();
		if (!rd)
			return;		

		SkJoint* root = character->getSkeleton()->root();
		SrMat rootMat = rd->gmat;//root->gmat();
		//rootMat.translation(root->gmat().get(12),root->gmat().get(13),root->gmat().get(14));
		const std::vector<SrVec>& exampleData = re->examplePts;
		const std::vector<SrVec>& resampleData = re->resamplePts;		

#if 0
		SrPoints srExamplePts;	
		srExamplePts.init_with_attributes();
		for (unsigned int i=0;i<exampleData.size();i++)
		{
			//const PoseExample& exPose = exampleData[i];
			SrVec lPos = exampleData[i];//SrVec((float)exPose.poseParameter[0],(float)exPose.poseParameter[1],(float)exPose.poseParameter[2]);
			SrVec gPos = lPos*rootMat; // transform to global coordinate
			//drawCircle(gPos[0],gPos[1],gPos[2],1.0,5,SrVec(1.0,0.0,0.0));			
			//drawPoint(gPos[0],gPos[1],gPos[2],5.0,SrVec(0.0,0.0,1.0));
			glColor3f(0.0, 0.0, 1.0);
			SrSnSphere sphere;			
			sphere.shape().center = SrPnt(gPos[0], gPos[1], gPos[2]);
			sphere.shape().radius = sphereSize;
			sphere.color(SrColor(0.f,0.f,1.f));
			sphere.render_mode(srRenderModeLines);
			SrGlRenderFuncs::render_sphere(&sphere);
			//PositionControl::drawSphere(gPos,1.0f);			
		}	

	    // tetra hedron rendering, disabled for now.
// 		const VecOfSimplex& simplexList = reachCt->simplexList;
// 		SrVec tetraVtx[4];
// 		for (unsigned int i=0;i<simplexList.size();i++)
// 		{
// 			if (i != reachCt->simplexIndex )
// 				continue;
// 			const Simplex& sp = simplexList[i];
// 			for (int k=0;k<4;k++)
// 			{
// 				tetraVtx[k] = exampleData[sp.vertexIndices[k]];//*rootMat;
// 				//tetraVtx[k].z *= 3.f;
// 				tetraVtx[k] = tetraVtx[k]*rootMat;
// 			}
// 
// 			drawTetra(tetraVtx,SrVec(1,0,0));
// 		}
	
		for (unsigned int i=0;i<resampleData.size();i++)
		{			
			SrVec lPos = resampleData[i];
			SrVec gPos = lPos*rootMat; // transform to global coordinate
			//drawCircle(gPos[0],gPos[1],gPos[2],1.0,5,SrVec(1.0,0.0,0.0));			
			SrVec yaxis(0.0, 1.0, 0.0);
			drawPoint(gPos[0],gPos[1],gPos[2],1.5, yaxis);
			//PositionControl::drawSphere(gPos,1.0f);			
		}	
#endif
// 		if (reachCt->posPlanner && reachCt->posPlanner->path())
// 		{
// 			if (reachCt->posPlanner->path())
// 			{
// 				SrSnLines pathLines;			
// 				reachCt->posPlanner->draw_path(*reachCt->posPlanner->path(),pathLines.shape());
// 				pathLines.color(SrColor(1,0,0,1));
// 				SrGlRenderFuncs::render_lines(&pathLines);		
// 			}
// 
// 			SrSnBox box;
// 			box.shape() = reachCt->posPlanner->cman()->SkPosBound();
// 			//SrGlRenderFuncs::render_box(&box);
// 		}

// 		if (reachCt->blendPlanner)
// 		{
// 			if (reachCt->blendPlanner->path())
// 			{
// 				SrSnLines pathLines;			
// 				reachCt->blendPlanner->draw_path(*reachCt->blendPlanner->path(),pathLines.shape());
// 				pathLines.color(SrColor(1,0,0,1));
// 				SrGlRenderFuncs::render_lines(&pathLines);		
// 			}	
// 
// 			if (reachCt->blendPlanner->cman())
// 			{
// 				SrSnLines tree1;
// 				reachCt->blendPlanner->draw_edges(tree1.shape(),0);
// 
// 				SrSnLines tree2;
// 				reachCt->blendPlanner->draw_edges(tree2.shape(),1);
// 				SrGlRenderFuncs::render_lines(&tree1);
// 				SrGlRenderFuncs::render_lines(&tree2);
// 			}			
// 		}
// 
// 
// 
// 
// 		if (reachCt->blendPlanner && reachCt->blendPlanner->cman())
// 		{
// 			std::vector<CollisionJoint>& colJointList = reachCt->blendPlanner->cman()->colJoints;
// 			for (unsigned int i=0;i<colJointList.size();i++)
// 				drawColObject(colJointList[i].colGeo);
// 		}

		EffectorState& es = rd->effectorState;
// 		SrVec reachTraj = es.curState.tran;
		SrVec sphereColor(0,1,1);
// 		PositionControl::drawSphere(reachTraj,sphereSize,sphereColor);
		SrVec ikTraj = es.curIKTargetState.tran;		
		SrVec color(1,0,1);
		//PositionControl::drawSphere(ikTraj,sphereSize,color);
		SrVec ikTarget = es.ikTargetState.tran;
		std::string effectorJointName = rd->effectorState.effectorName;
		SmartBody::SBJoint* effectorJoint = character->getSkeleton()->getJointByName(effectorJointName);
		if (effectorJoint)
		{
			//effectorJoint->updateGmatZero();
			SrVec pos = effectorJoint->gmat().get_translation();
			SrQuat rot = SrQuat(effectorJoint->gmat().get_rotation());
			SrMat targetState = rd->effectorState.ikTargetState.gmat();
			SrQuat desireIKRot = SrQuat(targetState);
			SrQuat desireRot = SrQuat(rd->desireHandState.gmat());
			
			SrVec yaxis = SrVec(0,1,0)*rot;
			//SrVec xaxis = SrVec(1,0,0)*rot;
			//SrVec zaxis = SrVec(0,0,1)*rot;
			
			SrVec yaxis1 = SrVec(0,1,0)*desireIKRot;
			//SrVec xaxis1 = SrVec(1,0,0)*desireIKRot;
			//SrVec zaxis1 = SrVec(0,0,1)*desireIKRot;

			SrVec desireAxis = SrVec(0,1,0)*desireRot;
			float lineScale = character->getHeight() / 30.0f;	
			SrSnLines effectorUpLine;
			SrLines& line = effectorUpLine.shape();
			line.push_color(SrColor(1.f,0.f,0.f,1.f));
			line.push_line(pos, pos+yaxis1*lineScale);
			line.push_color(SrColor(0.f,1.f,0.f,1.f));
			line.push_line(pos, pos+yaxis*lineScale);
			//line.push_color(SrColor(1.f,0.f,0.f,1.f));
			//line.push_line(pos, pos+xaxis*lineScale);
			line.push_color(SrColor(1.f,0.f,1.f,1.f));
			line.push_line(pos, pos+desireAxis*lineScale);
			SrGlRenderFuncs::render_lines(&effectorUpLine);			
			
		}
// 		MeCtIKTreeScenario& ikTree = re->ikScenario;
// 
// 		for (unsigned int i=0;i<ikTree.ikValidNodes.size();i++)
// 		{
// 			MeCtIKTreeNode* node = ikTree.ikValidNodes[i];
// 			SrVec nodePos = node->getGlobalPos();
// 			SrVec color(0,1,1);
// 			PositionControl::drawSphere(nodePos,sphereSize,color);
// 		}

// 		glColor3f(1.0, 0.0, 0.0);
// 		glBegin(GL_LINES);
// 		glVertex3f(reachTraj.x,reachTraj.y,reachTraj.z);
// 		glVertex3f(ikTarget.x,ikTarget.y,ikTarget.z);
// 		glEnd();
	}

	glPopAttrib();
	
	
}

void FltkViewer::makeGLContext()
{
	make_current();
}




void FltkViewer::drawColObject( SBGeomObject* colObj, SrMat& gmat, float alpha)
{
	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_LIGHTING);
	glPushMatrix();
	//SrMat gMat = colObj->worldState.gmat();
	SrColor objColor;
	objColor = colObj->color;
	objColor.a = (srbyte) (int) (alpha * 255.0f);

	glDisable(GL_TEXTURE_2D);
	glEnable ( GL_ALPHA_TEST );
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glAlphaFunc ( GL_GREATER, 0.1f ) ;	
	SrMat gMat = gmat;
	SrMat lMat = colObj->getLocalTransform().gmat();	
	glMultMatrixf((const float*) gMat);
	glMultMatrixf((const float*) lMat);
	if (dynamic_cast<SBGeomSphere*>(colObj))
	{
		// draw sphere
		SBGeomSphere* sph = dynamic_cast<SBGeomSphere*>(colObj);
		SrSnSphere sphere;					
		sphere.shape().radius = sph->radius;
		sphere.color(objColor);
		sphere.render_mode(srRenderModeSmooth);
		SrGlRenderFuncs::render_sphere(&sphere);		
	}
	else if (dynamic_cast<SBGeomBox*>(colObj))
	{
		SBGeomBox* box = dynamic_cast<SBGeomBox*>(colObj);
		SrSnBox sbox;					
		sbox.shape().a = -box->extent;
		sbox.shape().b = box->extent;
		sbox.color(objColor);
		sbox.render_mode(srRenderModeSmooth);
		SrGlRenderFuncs::render_box(&sbox);
	}
	else if (dynamic_cast<SBGeomCapsule*>(colObj))
	{
		SBGeomCapsule* cap = dynamic_cast<SBGeomCapsule*>(colObj);
		// render two end cap
		SrSnSphere sphere;				
		sphere.shape().center = cap->endPts[0];//SrVec(0,-cap->extent,0);
		sphere.shape().radius = cap->radius;
		sphere.color(objColor);
		sphere.render_mode(srRenderModeSmooth);
		SrGlRenderFuncs::render_sphere(&sphere);

		sphere.shape().center = cap->endPts[1];//SrVec(0,cap->extent,0);
		SrGlRenderFuncs::render_sphere(&sphere);

		SrSnCylinder cyc;
		cyc.shape().a = cap->endPts[0];//SrVec(0,-cap->extent,0);
		cyc.shape().b = cap->endPts[1];//SrVec(0,cap->extent,0);
		cyc.shape().radius = cap->radius;
		cyc.color(objColor);
		cyc.render_mode(srRenderModeSmooth);
		SrGlRenderFuncs::render_cylinder(&cyc);
	}
	else if (dynamic_cast<SBGeomTriMesh*>(colObj))
	{
		SBGeomTriMesh* mesh = dynamic_cast<SBGeomTriMesh*>(colObj);
		if (mesh->geoMesh)
		{
			SrSnModel model;
			model.shape(*mesh->geoMesh);
			model.color(objColor);
			model.render_mode(srRenderModeSmooth);
			SrGlRenderFuncs::render_model(&model);
		}
	}
	glPopMatrix();	
	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST) ;
	glDisable(GL_BLEND);
	glPopAttrib();
}


void FltkViewer::drawSteeringInfo()
{
	if (_data->steerMode == ModeNoSteer)
		return;

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	if (!scene->getSteerManager()->getEngineDriver()->isInitialized() || 
		!scene->getSteerManager()->getEngineDriver()->_engine)
		return;

	glPushAttrib( GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
	glPushMatrix();
	glDisable(GL_LIGHTING);

	//glTranslatef(0.0f, 0.5f, 0.0f);
	glScalef(1.0f / scene->getScale(), 1.0f / scene->getScale(), 1.0f / scene->getScale());

	//comment out for now, have to take a look at the steering code
	const std::vector<SteerLib::AgentInterface*>& agents = scene->getSteerManager()->getEngineDriver()->_engine->getAgents();
	for (size_t x = 0; x < agents.size(); x++)
	{
		scene->getSteerManager()->getEngineDriver()->_engine->selectAgent(agents[x]);
		agents[x]->draw();
	}

	const std::set<SteerLib::ObstacleInterface*>& obstacles = scene->getSteerManager()->getEngineDriver()->_engine->getObstacles();
	for (std::set<SteerLib::ObstacleInterface*>::const_iterator iter = obstacles.begin();
		iter != obstacles.end();
		iter++)
	{
		(*iter)->draw();
	}
	
	if (_data->steerMode == ModeSteerAll)
	{
		scene->getSteerManager()->getEngineDriver()->_engine->getSpatialDatabase()->draw();
	}

	glPopMatrix();
	glPopAttrib();

}


void FltkViewer::drawCollisionInfo()
{
	if (_data->collisionMode == ModeCollisionHide)
		return;

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_LIGHTING | GL_LINE_BIT);
	//glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0, 0.0f);
	glLineWidth(5.f);
	glPushMatrix();
	std::map<std::string, SBGeomObject*>& collisionObjects = scene->getCollisionManager()->getAllCollisionObjects();
	for (std::map<std::string, SBGeomObject*>::iterator iter = collisionObjects.begin();
		 iter != collisionObjects.end();
		 iter++)
	{
		SBGeomObject* collisionObject = (*iter).second;
		SrMat mat = collisionObject->getGlobalTransform().gmat();
		drawColObject(collisionObject, mat, .5);
	}
	glPopMatrix();
	glPopAttrib();
}


// draw motion vector flow
void FltkViewer::drawMotionVectorFlow()
{

	SmartBody::SBAnimationBlend* animBlend = getCurrentCharacterAnimationBlend();
	if (!animBlend)
		return;

	glPushMatrix();
	SrMat mat = animBlend->getPlotVectorFlowTransform();
	glMultMatrixf((const float*) mat);

	std::vector<SrSnLines*>& vecflow_lines = animBlend->getVectorFlowSrSnLines();
	for(unsigned int i=0; i<vecflow_lines.size(); i++)
	{
		SrSnShapeBase* sp = dynamic_cast<SrSnShapeBase*>(vecflow_lines[i]);
		SrGlRenderFuncs::render_lines(sp);
	}
	glPopMatrix();
}

// plot motion frames
void FltkViewer::drawPlotMotion()
{
	SmartBody::SBAnimationBlend* animBlend = getCurrentCharacterAnimationBlend();
	if (!animBlend)
		return;

	glPushMatrix();
	SrMat mat = animBlend->getPlotMotionTransform();
	glMultMatrixf((const float*) mat);

	std::vector<SrSnLines*>& plotmotion_lines = animBlend->getPlotMotionSrSnLines();
	for(unsigned int i=0; i<plotmotion_lines.size(); i++)
	{
		SrSnShapeBase* sp = dynamic_cast<SrSnShapeBase*>(plotmotion_lines[i]);
		SrGlRenderFuncs::render_lines(sp);
	}
	glPopMatrix();
}


void FltkViewer::notify(SmartBody::SBSubject* subject)
{
	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		std::string attrName = attribute->getName();
		if (attrName == "GUI.ShowCameras")
		{
			SmartBody::BoolAttribute* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			bool value = boolAttribute->getValue();
			SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
			std::vector<std::string> camNames = scene->getCameraNames();
			for (unsigned int i=0;i<camNames.size();i++)
			{
				SrCamera* cam = scene->getCamera(camNames[i]);
				if (cam)
				{
					cam->setBoolAttribute("visible", value);
				}
			}
		}
		else if (attrName == "GUI.ShowLights")
		{
			SmartBody::BoolAttribute* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			bool value = boolAttribute->getValue();
			SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
			const std::vector<std::string>& pawnNames = scene->getPawnNames();
			for (unsigned int i=0;i<pawnNames.size();i++)
			{
				std::string name = pawnNames[i];
				SmartBody::SBPawn* pawn = scene->getPawn(name);
				if (name.find("light") == 0 && pawn) // is light
				{
					pawn->setBoolAttribute("visible", value);			
				}
			}
		}
		else if (attrName == "GUI.ShowGrid")
		{
			SmartBody::BoolAttribute* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			bool value = boolAttribute->getValue();
			if (getData()->gridMode != FltkViewer::ModeShowGrid)
				menu_cmd(FltkViewer::CmdGrid, NULL);
			else
				menu_cmd(FltkViewer::CmdNoGrid, NULL);
		}
		else if (attrName == "GUI.ShowFloor")
		{
			SmartBody::BoolAttribute* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
			bool value = boolAttribute->getValue();
			getData()->showFloor = !getData()->showFloor;
			updateOptions();
		}
		else if (attrName == "GUI.FloorColor")
		{
			SmartBody::Vec3Attribute* vec3Attribute = dynamic_cast<SmartBody::Vec3Attribute*>(attribute);
			SrVec val = vec3Attribute->getValue();
			_data->floorColor = SrColor(val.x, val.y, val.z);
			updateOptions();
		}
		else if (attrName == "GUI.BackgroundColor")
		{
			SmartBody::Vec3Attribute* vec3Attribute = dynamic_cast<SmartBody::Vec3Attribute*>(attribute);
			SrVec val = vec3Attribute->getValue();
			_data->bcolor = SrColor(val.x, val.y, val.z);
			updateOptions();
		}
	}

}

void FltkViewer::makeShadowMap()
{

	SrLight &light = _data->light;
	float shad_modelview[16];
	
	//	glDisable(GL_LIGHTING);
	//glDisable(GL_TEXTURE_2D);
	// since the shadow maps have only a depth channel, we don't need color computation
	// glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();	
	glLoadIdentity();
	gluPerspective(70,1,get_camera()->getNearPlane(),get_camera()->getFarPlane());
	
	SrLight& shadowLight = _lights[0]; // assume direction light
	SrVec dir = shadowLight.position;
	dir.normalize();
	float sceneScale = 1.f/SmartBody::SBScene::getScene()->getScale();
	float distance = 10*sceneScale;
	dir*= distance;
	SrVec pos = shadowLight.position;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	// the light is from position to origin
	//gluLookAt(light.position[0], light.position[1], light.position[2], 0.f, 0.f, 0.f, 0.0f, 1.0f, 0.0f);
	//gluLookAt(light.position[0], light.position[1], light.position[2], 0,0,0, -1.0f, 0.0f, 0.0f);
	//gluLookAt(0, 0, 0, -light.position[0], -light.position[1], -light.position[2], -1.0f, 0.0f, 0.0f);
	//gluLookAt(0, 600, 700, 0,0,0, 0.0f, 1.0f, 0.0f);
	gluLookAt(dir[0],dir[1],dir[2],0,0,0,0,1,0);
	//gluLookAt(pos[0],pos[1],pos[2],0,0,0,0,1,0);
	glGetFloatv(GL_MODELVIEW_MATRIX, shad_modelview);
	// redirect rendering to the depth texture
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _data->depthFB);
	// store the screen viewport
	glPushAttrib(GL_VIEWPORT_BIT);
	int depth_size = SHADOW_MAP_RES;
	// and render only to the shadowmap
	glViewport(0, 0, depth_size, depth_size);
	// offset the geometry slightly to prevent z-fighting
	// note that this introduces some light-leakage artifacts
	//glPolygonOffset( 1.0f, 4096.0f);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	// draw all faces since our terrain is not closed.
	//
	glDisable(GL_CULL_FACE);	
	//glCullFace(GL_FRONT);
	// for all shadow maps:
	// make the current depth map a rendering target
	//glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, depth_tex_ar, 0, i);
	//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,GL_TEXTURE_2D, _data->shadowMapID, 0);
	//glBindTexture(GL_TEXTURE_2D, _data->depthMapID);
	//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 
	// clear the depth texture from last time
	//glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	
	// draw the scene
	//terrain->Draw(minZ);	
	drawAllGeometries(true);
	glMatrixMode(GL_PROJECTION);
	// store the product of all shadow matries for later
	glMultMatrixf(shad_modelview);
	glGetFloatv(GL_PROJECTION_MATRIX, _data->shadowCPM);
	

	// revert to normal back face culling as used for rendering
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glPopAttrib(); 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	//SbmShaderProgram::printOglError("shadowMapError");

}

void FltkViewer::drawNavigationMesh()
{
	if (_data->navigationMeshMode == ModeNoNavigationMesh) return;

	SmartBody::SBNavigationMesh* navigationMesh = SmartBody::SBScene::getScene()->getNavigationMesh();
	if (!navigationMesh) return;

	SrModel* drawMesh = NULL;
	SrModel* naviMesh = NULL;
	SrSnModel model;
	if (_data->navigationMeshMode == ModeRawMesh)
	{
		drawMesh = navigationMesh->getRawMesh();
		
	}
	else if (_data->navigationMeshMode == ModeNavigationMesh)
	{
		naviMesh = navigationMesh->getNavigationMesh();
		drawMesh = navigationMesh->getRawMesh();
		//model.render_mode(srRenderModeLines);
		model.color(SrColor::red);
		
	}

	if (drawMesh)
	{
		glEnable(GL_LIGHTING);	
		model.shape(*drawMesh);
		//model.color(objColor);
		model.color(SrColor::gray);
		model.render_mode(srRenderModeSmooth);
		SrGlRenderFuncs::render_model(&model);
		glDisable(GL_LIGHTING);
	}
	if (naviMesh)
	{
		glEnable(GL_LIGHTING);	
		model.shape(*naviMesh);
		model.color(SrColor::red);
		model.render_mode(srRenderModeLines);
		//model.render_mode(srRenderModeSmooth);
		SrGlRenderFuncs::render_model(&model);
		glDisable(GL_LIGHTING);
	}
	
}

void FltkViewer::drawText( const SrMat& mat, float textSize, std::string &text )
{
	glPushMatrix();
	glMultMatrixf((const float*) mat);
	float modelview[16];
	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
	// undo all rotations
	// beware all scaling is lost as well 
	for( int a=0; a<3; a++ ) 
	{
		for( int b=0; b<3; b++ ) {
			if ( a==b )
				modelview[a*4+b] = 1.0;
			else
				modelview[a*4+b] = 0.0;
		}
	}
	// set the modelview with no rotations
	glLoadMatrixf(modelview);

	glColor3f(1.0f, 1.0f, 0.0f);
	glScalef(textSize, textSize, textSize);
	glutStrokeString(GLUT_STROKE_ROMAN, (const unsigned char*) text.c_str());
	glPopMatrix();
}

void FltkViewer::resetViewer()
{
	_data->setupData();

	// clear object selection

	_objManipulator.set_selected_pawn(NULL);
	if (_objManipulator.get_active_control())
		_objManipulator.get_active_control()->detach_pawn();
	_objManipulator.removeActiveControl();

	registerUIControls();
}

void FltkViewer::updateOptions()
{
}




void FltkViewer::drawDeformableModels()
{
	printOglError2("drawDeformableModels()", 1);

	const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
	for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
		pawnIter != pawns.end();
		pawnIter++)
	{
		SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn((*pawnIter));
		DeformableMeshInstance* meshInstance = pawn->getActiveMesh();
		if(meshInstance)
		{
			meshInstance->blendShapeStaticMesh();
			//LOG("drawDeformableModels(): Rendering %s", pawn->getName().c_str());
			//pawn->dMesh_p->update();
			if (!meshInstance->isStaticMesh()) // is skinned mesh
			{
#if 0
				bool useBlendShape = false;
				SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*> (pawn);
				if (character)
				{
					useBlendShape = character->getBoolAttribute("blendshape");
				}
				if (useBlendShape && meshInstance->getDeformableMesh())
				{
					//character->dMesh_p->blendShapes();
					meshInstance->getDeformableMesh()->blendShapes();
					//character->dMeshInstance_p->setDeformableMesh(character->dMesh_p);
				}
#endif
				meshInstance->update();
				//meshInstance->updateFast();
				if ( (!SbmDeformableMeshGPU::useGPUDeformableMesh && meshInstance->getVisibility() == 1) || meshInstance->getVisibility() == 2)
				{
					bool showSkinWeight = (meshInstance->getVisibility() == 2);
					SrGlRenderFuncs::renderDeformableMesh(meshInstance, showSkinWeight);
					//				if (pawn->scene_p)
					//				pawn->scene_p->set_visibility(0,1,0,0);
					//_data->render_action.apply(character->scene_p);
				}
			}
			else if (rendererType() != "ogre3D")
			{
				//meshInstance->blendShapes();
				if ( (!SbmDeformableMeshGPU::useGPUDeformableMesh && meshInstance->getVisibility() == 1) || meshInstance->getVisibility() == 2)
				{
					// simply draw the static mesh
					SrGlRenderFuncs::renderDeformableMesh(meshInstance, false);

					//SrGlRenderFuncs::renderBlendFace(meshInstance);
				}				
			}
		}
	}

	printOglError2("s",2);
}

int FltkViewer::deleteSelectedObject( int ret )
{
	SbmPawn* selectedPawn = _objManipulator.get_selected_pawn();
	//LOG("1");
	if (selectedPawn)
	{	
		int confirm = fl_choice(vhcl::Format("Are you sure you want to delete '%s'?", selectedPawn->getName().c_str()).c_str(), "No", "Yes", NULL);
		if (confirm == 0)
			return ret;
		//_objManipulator.set_selected_pawn(NULL);
		//_objManipulator.removeActiveControl();
		SBSelectionManager::getSelectionManager()->select("");
		SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*>(selectedPawn);
		if (character)
		{
			SmartBody::SBScene::getScene()->removeCharacter(character->getName());
			ret = 1;
			//LOG("2");
		}
		else
		{
			SmartBody::SBPawn* pawn = dynamic_cast<SmartBody::SBPawn*>(selectedPawn);
			SmartBody::SBScene::getScene()->removePawn(pawn->getName());
			ret = 1;
			//LOG("3");
		}		
	}
	//LOG("4");	
	//baseWin->updateObjectList();
	return ret;
}

void FltkViewer::addPoint(const std::string& pointName, SrVec point, SrVec color, float size)
{
	removePoint(pointName);
	Point3D p;
	p.point.set(point);
	p.color.r = color.x;
	p.color.g = color.y;
	p.color.b = color.z;
	p.size = size;
	_points3D.insert(std::pair<std::string, Point3D>(pointName, p));
}

void FltkViewer::removePoint(const std::string& pointName)
{
	std::map<std::string, Point3D>::iterator iter = _points3D.find(pointName);
	if (iter != _points3D.end())
	{
		_points3D.erase(pointName);
	}
}

void FltkViewer::removeAllPoints()
{
	_points3D.clear();
}

void FltkViewer::addLine(const std::string& lineName, std::vector<SrVec>& points, SrVec color, float width)
{
	removeLine(lineName);
	Line3D l;
	for (size_t i = 0; i < points.size(); i++)
	{
		l.points.push_back(points[i]);
	}
	l.color.r = color.x;
	l.color.g = color.y;
	l.color.b = color.z;
	l.width = width;
	_lines3D.insert(std::pair<std::string, Line3D>(lineName, l));
}

void FltkViewer::removeLine(const std::string& lineName)
{
	std::map<std::string, Line3D>::iterator iter = _lines3D.find(lineName);
	if (iter != _lines3D.end())
	{
		_lines3D.erase(lineName);
	}
}

void FltkViewer::removeAllLines()
{
	_lines3D.clear();
}

GestureVisualizationHandler::GestureVisualizationHandler()
{
	_gestureData = NULL;
}

GestureVisualizationHandler::~GestureVisualizationHandler()
{
	_gestureData = NULL;
}

void GestureVisualizationHandler::setGestureData(GestureData* data)
{
	_gestureData = data;
}

void GestureVisualizationHandler::executeAction(SmartBody::SBEvent* event)
{
	if (!_gestureData)
		return;

	std::vector<std::string> parameters;
	std::string param = event->getParameters();
	vhcl::Tokenize(param, parameters);
	if (parameters[0] == "syncpointprogress")
	{	
		_gestureData->currentCharacter = parameters[1];

		if (parameters[3] == "start")
		{
			GestureData::GestureSection newSectionL;
			newSectionL.side = 0;
			newSectionL.color = _gestureData->getColor();
			_gestureData->gestureSections.push_back(newSectionL);
			GestureData::GestureSection newSectionR;
			newSectionR.side = 0;
			newSectionR.color = _gestureData->getColor();
			_gestureData->gestureSections.push_back(newSectionR);

			_gestureData->currentStatus = GestureData::START;

		}
		if (parameters[3] == "end")
		{
			_gestureData->currentStatus = GestureData::END;
		}
		if (parameters[3] == "stroke_start")
			_gestureData->currentStatus = GestureData::STROKE_START;
		if (parameters[3] == "stroke")
			_gestureData->currentStatus = GestureData::STROKE;
		if (parameters[3] == "stroke_end")
			_gestureData->currentStatus = GestureData::STROKE_END;
		if (parameters[3] == "ready")
			_gestureData->currentStatus = GestureData::READY;
		if (parameters[3] == "relax")
			_gestureData->currentStatus = GestureData::RELAX;
	}
	if (parameters[0] == "blockstart")
	{
		_gestureData->pause = false;
		_gestureData->reset();
	}
	if (parameters[0] == "blockend")
	{
		_gestureData->pause = true;
	}
}

GestureData::GestureData() 
{ 
	currentStatus = OTHER; 
	currentCharacter = ""; 

	// note: the number below representing the color is for approximate, not accurate
	colorTables.push_back(SrVec(0.8f, 0.5f, 0.8f));			// violet
	colorTables.push_back(SrVec(0.7f, 0.7f, 0.7f));			// silver
	colorTables.push_back(SrVec(1.0f, 0.8f, 0.0f));			// gold
	colorTables.push_back(SrVec(0.5f, 0.5f, 0.5f));			// gray
	colorTables.push_back(SrVec(1.0f, 0.7f, 0.75f));		// pink
	colorTables.push_back(SrVec(1.0f, 0.0f, 1.0f));			// fuscia
	colorTables.push_back(SrVec(0.0f, 1.0f, 1.0f));			// aqua
	colorTables.push_back(SrVec(0.95f, 0.85f, 0.6f));		// khaki

	syncPointColorMap.insert(std::make_pair(GestureData::START, SrVec(0.0f, 0.0f, 1.0f)));				// blue
	syncPointColorMap.insert(std::make_pair(GestureData::READY, SrVec(0.0f, 0.0f, 0.0f)));				// black
	syncPointColorMap.insert(std::make_pair(GestureData::STROKE, SrVec(1.0f, 0.0f, 0.0f)));				// red
	syncPointColorMap.insert(std::make_pair(GestureData::STROKE_START, SrVec(0.0f, 0.5f, 0.0f)));		// green
	syncPointColorMap.insert(std::make_pair(GestureData::STROKE_END, SrVec(0.5f, 0.0f, 0.5f)));			// purple
	syncPointColorMap.insert(std::make_pair(GestureData::RELAX, SrVec(1.0f, 1.0f, 0.0f)));				// yellow
	syncPointColorMap.insert(std::make_pair(GestureData::END, SrVec(1.0f, 0.75, 0.0f)));				// orange

	colorIndex = 0;
	pause = false;
	displayMaximum = 5000;
}

SrVec& GestureData::getColor()
{
	SrVec& color = colorTables[colorIndex];
	colorIndex++;
	if (colorIndex >= (int)colorTables.size())
		colorIndex = 0;
	return color;
}

SrVec& GestureData::getSyncPointColor(int type)
{
	return syncPointColorMap[type];
}

void GestureData::toggleFeedback(bool val)
{
	BML::Processor* bp = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor();
	bp->set_bml_feedback(val);
}

void GestureData::reset()
{

	currentCharacter = "";
	for (size_t i = 0; i < gestureSections.size(); i++)
	{
		gestureSections[i].data.clear();
	}

	gestureSections.clear();
	syncPoints.clear();
}


FltkViewerData::FltkViewerData()
{
}

void FltkViewerData::setupData()
{
	rendermode = FltkViewer::ModeAsIs;
	charactermode = FltkViewer::ModeShowGeometry;
	pawnmode = FltkViewer::ModePawnShowAsSpheres;
	shadowmode = FltkViewer::ModeShadowMap;
	terrainMode = FltkViewer::ModeTerrain;
	navigationMeshMode = FltkViewer::ModeNoNavigationMesh;
	eyeBeamMode = FltkViewer::ModeNoEyeBeams;
	gazeLimitMode = FltkViewer::ModeNoGazeLimit;
	eyeLidMode = FltkViewer::ModeNoEyeLids;
	dynamicsMode = FltkViewer::ModeNoDynamics;
	locomotionMode = FltkViewer::ModeEnableLocomotion;
	reachRenderMode = FltkViewer::ModeNoExamples;
	steerMode = FltkViewer::ModeNoSteer;
	collisionMode = FltkViewer::ModeCollisionHide;
	gridMode = FltkViewer::ModeShowGrid;
	cameraMode = FltkViewer::Default;

	iconized    = false;
	statistics  = false;
	displayaxis = false;
	boundingbox = false;
	scene_received_event = false;
	showgeometry = false;
	showcollisiongeometry = false;
	showSkinWeight = false;
	showbones = true;
	showaxis = false;
	showmasses = false;
	showBoundingVolume = false;
	showJointLabels = false;
	showlocomotionall = false;
	showvelocity = false;
	showorientation = false;
	showselection = false;
	showlocofootprints = false;
	showkinematicfootprints = false;
	interactiveLocomotion = false;
	showtrajectory = false;
	showgesture = false;

	bcolor = SrColor(.63f, .63f, .63f);
	floorColor = SrColor(0.6f,0.6f,0.6f);
	showFloor = true;

	saveSnapshot		= false;
	saveSnapshot_tga	= false;

	scenebox = new SrSnLines;
	sceneaxis = new SrSnLines;
}

//================================ End of File =================================================
