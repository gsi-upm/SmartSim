# ifndef FLTK_VIEWER_H
# define FLTK_VIEWER_H

//#define USE_GLEW 1
//#include <sbm/GPU/SbmShader.h>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Menu_Item.H>
# include <FL/Fl_Gl_Window.H>
# include <sr/sr_viewer.h>
#include <FL/Fl_Input.H>
# include <sr/sr_color.h>
# include <sr/sr_light.h>
# include <sr/sr_timer.h>
# include <sr/sr_sa_gl_render.h>
# include <sr/sr_sa_bbox.h>
# include <FL/Fl_Menu.H>
#include "retargetviewer/RetargetViewer.h"
#include "jointmapviewer/RetargetStepWindow.h"
#include <sb/SBMotionBlendBase.h>
#include <sb/SBEvent.h>
#include "ObjectManipulationHandle.h"
#include <SBSelectionManager.h>
//#include <CEGUI.h>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

class SBGeomObject;
class SrQuat;
class SrVec;
class SrEvent;
class SrCamera;
class SrSn;
class SrViewer;
class SrLight;
class GestureData;
class FltkViewerData;
class LocomotionData;
class PALocomotionData;
class SbmCharacter;
class MeCtDataDrivenReach;
class MeCtConstraint;
class MeCtExampleBodyReach;
class JointMapViewer;
class RetargetStepWindow;
class PALocomotionData;
class FLTKListener;
class BaseWindow;


struct Point3D
{
	SrVec point;
	SrColor color;
	int size;
};

struct Line3D
{
	std::vector<SrVec> points;
	SrColor color;
	int width;
};

/*! \class SrViewer sr_viewer.h
    \brief A fltk-opengl viewer

    SrViewer implements a viewer to render objects derived from SrSn.
    The viewer has currently a planar and a tridimensional examiner mode.
    In ModePlanar, only transformation on the XY plane are accepted.
    In all modes, mouse interaction is done together with Ctrl and Shift modifiers.
    A popup menu appears with a right button click or ctrl+shift+m. */

//class FltkViewer : public SrViewer, public Fl_Gl_Window, public SmartBody::SBObserver
class FltkViewer : public Fl_Gl_Window, public SmartBody::SBObserver, public SelectionListener
 {
   public : // enumerators

    enum RenderMode { ModeAsIs,
                      ModeDefault,
                      ModeSmooth,
                      ModeFlat,
                      ModeLines,
                      ModePoints
                    };

	enum ShadowMode { ModeNoShadows,
					  ModeShadowMap,
					  ModeShadowStencil,
					  ModeShadows
                };

	enum SteerMode {  ModeNoSteer,
					  ModeSteerCharactersGoalsOnly,
					  ModeSteerAll,
                };

	enum CollisionMode {  ModeCollisionShow,
					  ModeCollisionHide,
                };

	enum CharacterMode { ModeShowGeometry,
                         ModeShowCollisionGeometry,
						 ModeShowDeformableGeometry,
						 ModeShowDeformableGeometryGPU,
                         ModeShowBones,
                         ModeShowAxis
                    };

	enum PawnMode {	  ModeNoPawns,
                      ModePawnShowAsSpheres,
                    };

	enum ReachRenderMode { ModeShowExamples,
			               ModeNoExamples,
	};

	enum terrainMode { ModeNoTerrain,
					   ModeTerrainWireframe,
					   ModeTerrain,
                };

	enum NavigationMeshMode { ModeNoNavigationMesh,
		ModeRawMesh,
		ModeNavigationMesh,
	};

	enum EyeBeamMode { ModeNoEyeBeams,
					   ModeEyeBeams
                };
	enum GazeLimitMode { ModeNoGazeLimit,
		ModeGazeLimit
	};

	enum EyeLidMode { ModeNoEyeLids,
					   ModeEyeLids
                };
	enum JointMode { ModeShowJoints,
					 ModeNoJoints
                };
	enum DynamicsMode { ModeNoDynamics,
					    ModeShowCOM,
						ModeShowCOMSupportPolygon,
						ModeShowMasses
                };
	enum LocomotionMode { 
						ModeEnableLocomotion,
						ModeShowAll,
						ModeShowVelocity,
						ModeShowOrientation,
						ModeShowSelection,
						ModeShowKinematicFootprints,
					    ModeShowLocomotionFootprints,
						ModeInteractiveLocomotion
                };
   enum CameraMode {
                  Default,
                  FreeLook,
                  FollowRenderer
                 };
	enum GridMode { 
						ModeShowGrid,
						ModeNoGrid
                };

    enum MenuCmd { CmdViewAll,
                   CmdBackground,
				   CmdFloorColor,
                   CmdAsIs,
                   CmdDefault,
                   CmdSmooth,
                   CmdFlat,
                   CmdLines,
                   CmdPoints,                   
				   CmdNoShadows,
				   CmdShadows,	
				   CmdCharacterShowGeometry,
				   CmdCharacterShowCollisionGeometry,
				   CmdCharacterShowSkinWeight,
				   CmdCharacterShowDeformableGeometry,
				   CmdCharacterShowDeformableGeometryGPU,
				   CmdCharacterShowBones,
				   CmdCharacterShowAxis,				   					   
				   CmdAxis,	
				   CmdBoundingBox,
				   CmdStatistics,
				   CmdGrid,
				   CmdNoGrid,				   
				   CmdNoPawns,
				   CmdPawnShowAsSpheres,
				   CmdCreatePawn,
				   CmdGazeOnTargetType1,
				   CmdGazeOnTargetType2,
				   CmdGazeOnTargetType3,
				   CmdGazeOnTargetType4,
				   CmdRemoveAllGazeTarget,
				   CmdShowJoints,
				   CmdNoJoints,
				   CmdNoTerrain,
				   CmdTerrainWireframe,
				   CmdTerrain,
				   CmdNoNavigationMesh,
				   CmdRawMesh,
				   CmdNavigationMesh,
				   CmdNoEyeBeams,
				   CmdEyeBeams,
				   CmdNoGazeLimit,
				   CmdGazeLimit,
				   CmdNoEyeLids,
				   CmdEyeLids,
				   CmdNoDynamics,
				   CmdShowCOM,
				   CmdShowCOMSupportPolygon,
				   CmdShowMasses,
				   CmdShowBoundingVolume,
				   CmdEnableLocomotion,
				   CmdShowLocomotionAll,
				   CmdShowVelocity,
				   CmdShowOrientation,
				   CmdShowSelection,
				   CmdShowKinematicFootprints,
				   CmdShowLocomotionFootprints,
				   CmdInteractiveLocomotion,
				   CmdShowTrajectory,
				   CmdShowGesture,
				   CmdReachShowExamples,
				   CmdReachNoExamples,
				   CmdConstraintToggleIK,
				   CmdConstraintToggleBalance,
				   CmdConstraintToggleReferencePose,
				   CmdNoSteer,
				   CmdSteerCharactersGoalsOnly,
				   CmdSteerAll,
				   CmdCollisionShow,
				   CmdCollisionHide
                 };

  
 private:
	 Fl_Input* off_height_window;
   public : //----> public methods 

    /*! Constructor needs the size and location of the window. */
    FltkViewer ( int x, int y, int w, int h, const char *label=0 );

    /*! Destructs all internal data, and calls unref() for the root node. */
    virtual ~FltkViewer ();


    /*! Draws string in the graphics window. If s==0 it will erase current string. */
    void draw_message ( const char* s );

    /*! Shows the viewer pop up menu. */
    void show_menu ();

    /*! Activates an option available from the right button mouse menu of the viewer. */
    virtual void menu_cmd ( MenuCmd c, const char* label );

	void applyToCharacters();
    /*! Returns true if the cmd is currently activated. */
    bool menu_cmd_activated ( MenuCmd c );

    void update_bbox ();

    void update_axis ();

    /*! Sets the camera to see the whole bounding box of the scene. The camera
        center is put in the center of the bounding box, and the eye is put in
        the line passing throught the center and parallel to the z axis, in a
        sufficient distance from the center to visualize the entire bounding,
        leaving the camera with a 60 degreed fovy. The up vector is set to (0,1,0). */
    void view_all ();

    /*! Will make SrViewer to render the scene in the next fltk loop. */
    void render ();

    /*! Returns true if the window is iconized, false otherwise. */
    bool iconized ();

    void increment_model_rotation ( const SrQuat &dq );

	void translate_keyboard_state();
    float fps ();
    sruint curframe ();

    SrColor background ();
    void background ( SrColor c );

    SrCamera* get_camera();
    void set_camera (const SrCamera* cam);

	FltkViewerData* getData() { return _data; };
	ObjectManipulationHandle& getObjectManipulationHandle() { return _objManipulator; };
	std::string _lastSelectedCharacter;

	// 3d drawing interface
	void addPoint(const std::string& pointName, SrVec point, SrVec color, float size);
	void removePoint(const std::string& pointName);
	void removeAllPoints();
	void addLine(const std::string& lineName, std::vector<SrVec>& points, SrVec color, float width);
	void removeLine(const std::string& lineName);
	void removeAllLines();
	void drawDynamicVisuals();

	protected:
	std::map<std::string, Point3D> _points3D;
	std::map<std::string, Line3D> _lines3D;


   public : // virtual methods

    /*! When the window manager asks the window to close.
        The default implementation calls exit(0). */
    virtual void close_requested ();

    /*! draw() sets the viewer options and render the scene root.
        This is a derived FLTK method and should not be called directly.
        To draw the window contents, use render() instead. */
    virtual void draw ();	

    /*! Called to initialize opengl and to set the viewport to (w,h). This
        method is called also each time the window is reshaped. */
    virtual void init_opengl ( int w, int h );

	void initShadowMap();
	void makeShadowMap();
    /*! hande(int) controls the activation of the button menu, 
		and translates fltk events into the SrEvent class
        to then call handle_event(). Note that all handle methods should return
        1 when the event was understood and executed, and return 0 otherwise.
        This is a derived FLTK method. */ 
    virtual int handle ( int event );

	int deleteSelectedObject( int ret );
    /*! handle_event() will just call the other handle methods, according 
        to the viewer configuration and generated event. This is the first method
        that will be called from the fltk derived handle() method. */
    virtual int handle_event ( const SrEvent &e );

    /*! Takes mouse events to rotate, scale and translate the scene. 
        If the event is not used, it is passed to the scene by calling
        handle_scene_event(). */
    virtual int handle_examiner_manipulation ( const SrEvent &e );
    virtual int handle_default_camera_manipulation ( const SrEvent &e, SrCamera* camera );
    virtual int handle_freelook_camera_manipulation ( const SrEvent &e, SrCamera* camera  );
    virtual void translate_camera( SrCamera* camera, const SrVec& displacement);
    virtual int handle_followrenderer_camera_manipulation ( const SrEvent &e, SrCamera* camera );

    /*! Applies an event action to the scene */
    virtual int handle_scene_event ( const SrEvent &e );

    /*! Events related to interactive object movement, rotation, etc */
	virtual int handle_object_manipulation( const SrEvent& e);

	void processDragAndDrop(std::string dndMsg, float x, float y);
	void initGridList();	
	void drawAllGeometries(bool shadowPass = false); // draw all objects with geometry ( so no debug rendering included )
	void drawFloor(bool shadowPass = false);

	void drawDeformableModels();
	void drawGrid();
	void drawEyeBeams();
	void drawEyeLids();
	void drawDynamics();
	void drawLocomotion();
	void drawGestures();
	void drawJointLabels();

	void drawText( const SrMat& mat, float textSize, std::string &text );
	void drawGazeJointLimits();
	void drawReach();
	void drawInteractiveLocomotion();
	void drawPawns();
	void drawCharacterPhysicsObjs();
	void drawCharacterBoundingVolumes();
	void drawSteeringInfo();
	void drawCollisionInfo();
	void drawColObject(SBGeomObject* colObj, SrMat& gmat, float alpha);	
	void drawMotionVectorFlow();
	void drawPlotMotion();
	void drawNavigationMesh();

	std::string ZeroPadNumber(int);
	void snapshot(int, int, int);
	void snapshot_tga(int, int, int);

	void drawTetra(SrVec vtxPos[4], SrVec& color);
	void drawArrow(SrVec& from, SrVec& to, float width, SrVec& color);
	void drawCircle(float cx, float cy, float cz, float r, int num_segments, SrVec& color);
	void drawPoint(float cx, float cy, float cz, float size, SrVec& color);
	void drawActiveArrow(SrVec& from, SrVec& to, int num, float width, SrVec& color, bool spin);
	void init_foot_print();
	//void drawLocomotionFootprints();
	void drawKinematicFootprints(int index);
	void newPrints(bool newprint, int index, SrVec& pos, SrVec& orientation, SrVec& normal, SrVec& color, int side, int type);
	static void ChangeOffGroundHeight(Fl_Widget* widget, void* data);
	void updateLights();

	std::string create_pawn();

	int gridList;
	float gridColor[4];
	float gridHighlightColor[3];
	float gridSize;
	float gridStep;

	SrVec interactivePoint;

	FLTKListener* fltkListener;	

	
	virtual void notify(SmartBody::SBSubject* subject);

	virtual void label_viewer(const char* str);
	virtual void show_viewer();
	virtual void hide_viewer();
	virtual void makeGLContext();

	virtual void resetViewer();

	virtual void OnSelect(const std::string& value);
	virtual std::string rendererType() { return "fltk"; }

    FltkViewerData* _data;
	GestureData* _gestureData;
	float _arrowTime;
	ObjectManipulationHandle _objManipulator; // a hack for testing. 

	virtual void updateOptions();

	virtual void registerUIControls();

public:
	RetargetStepWindow* _retargetStepWindow;	
	ObjectManipulationHandle::ControlType _transformMode;
	BaseWindow* baseWin;
	std::vector<Fl_Menu_Item> nonSelectionPopUpMenu;
	std::vector<Fl_Menu_Item> selectionPopUpMenu;
protected:
	
	void drawJointLimitCone(SmartBody::SBJoint* joint, float coneSize, float pitchUpLimit, float pitchDownLimit, float headLimit);
	void set_gaze_target(int itype, const char* targetname);	
	void set_reach_target(int itype, const char* targetname);	
	void update_submenus();
	void create_popup_menus();	
	void createRightClickMenu(bool isSelected, int x, int y);
	void initializePopUpMenus();
	Fl_Menu_Item createMenuItem(const char* itemName, Fl_Callback* funcCallback, void* userData, int flag);
	//bool testCEGUIButtonPush(const CEGUI::EventArgs& /*e*/);
	MeCtExampleBodyReach* getCurrentCharacterBodyReachController();
	MeCtConstraint*    getCurrentCharacterConstraintController();
	SbmCharacter*        getCurrentCharacter();
	std::vector<SrLight> _lights;

	SmartBody::SBAnimationBlend* getCurrentCharacterAnimationBlend(); // for visualization plots	

 };


 class FltkViewerData
 { public :
	FltkViewerData();
	void setupData();

   FltkViewer::RenderMode rendermode; // render mode
   FltkViewer::CharacterMode charactermode; // character mode
   FltkViewer::JointMode jointmode; // character mode
   FltkViewer::PawnMode pawnmode; // pawn mode
   FltkViewer::ShadowMode shadowmode;     // shadow mode
   FltkViewer::terrainMode terrainMode;     // terrain mode
   FltkViewer::EyeBeamMode eyeBeamMode;     // eye beam mode
   FltkViewer::GazeLimitMode gazeLimitMode;
   FltkViewer::EyeLidMode eyeLidMode;     // eye lid mode
   FltkViewer::DynamicsMode dynamicsMode;     // dynamics information mode
   FltkViewer::LocomotionMode locomotionMode;   // locomotion mode
   FltkViewer::ReachRenderMode reachRenderMode;
   FltkViewer::SteerMode steerMode;
   FltkViewer::CollisionMode collisionMode;
   FltkViewer::CameraMode cameraMode;
   FltkViewer::GridMode gridMode;
   FltkViewer::NavigationMeshMode navigationMeshMode;


   bool iconized;      // to stop processing while the window is iconized
   bool statistics;    // shows statistics or not
   bool displayaxis;   // if shows the axis or not
   bool boundingbox;   // if true will show the bbox of the whole scene
   bool scene_received_event; // to detect and send a release event to the scene graph
                              // when the alt key is released but the mouse is pushed
   bool showgeometry;
   bool showcollisiongeometry;
   bool showdeformablegeometry;
   bool showSkinWeight;
   bool showFloor;   
   bool showbones;
   bool showaxis;
   bool showmasses;
   bool showBoundingVolume;
   bool showJointLabels;
   bool showCameras;
   bool showLights;

   bool locomotionenabled;
   bool showlocomotionall;
   bool showvelocity;
   bool showorientation;
   bool showselection;
   bool showlocofootprints;
   bool showkinematicfootprints;
   bool showtrajectory;
   bool showgesture;
   bool interactiveLocomotion;

   bool	saveSnapshot;
   bool	saveSnapshot_tga;
   std::string	snapshotPath;

   std::string message;   // user msg to display in the window
   SrLight light;

   SrTimer    fcounter;   // To count frames and measure frame rate
   SrEvent    event;      // The translated event from fltk to sr format
   SrColor    bcolor;     // Background color currently used
   SrColor    floorColor;
   SrBox      bbox;       // Bounding box of the root, calculated with viewall
   SrSnLines* scenebox;  // contains the bounding box to display, and use in view_all
   SrSnLines* sceneaxis; // the current axis being displayed

   Fl_Menu_Button* menubut; // the ctrl+shift+m or button3 menu
      

   SrSaGlRender render_action;
   SrSaBBox bbox_action;

   unsigned int  shadowMapID, depthMapID, depthFB, rboID;
   float shadowCPM[16];
   
 };

class GestureVisualizationHandler : public SmartBody::SBEventHandler
{
	public:
		GestureVisualizationHandler();
		~GestureVisualizationHandler();
		void setGestureData(GestureData* data);

		virtual void executeAction(SmartBody::SBEvent* event);

	private:
		GestureData* _gestureData;
};

class GestureData
{
public:
	enum status { START, READY, STROKE, STROKE_START, STROKE_END, RELAX, END, OTHER };

public:
	GestureData();
	~GestureData() {}

	SrVec& getColor();
	SrVec& getSyncPointColor(int type);

	void reset();
	void toggleFeedback(bool val);
	struct SyncPointData
	{
		int side;
		int type;
		SrVec location;
		SrVec color;
		int bmlId;
	};
	struct GestureSection
	{
		int side;
		int length;
		SrVec color;
		std::list<SrVec> data;
	};

	int displayMaximum;
	bool pause;

	int colorIndex;
	std::vector<SrVec> colorTables;
	std::map<int, SrVec> syncPointColorMap;

	std::vector<SyncPointData> syncPoints;
	std::vector<GestureSection> gestureSections;

	int currentStatus;
	std::string currentCharacter;
};


//================================ End of File =================================================

# endif // FLTK_VIEWER_H
