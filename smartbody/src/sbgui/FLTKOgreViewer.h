# ifndef FLTK_OGRE_VIEWER_H
# define FLTK_OGRE_VIEWER_H

//#define USE_GLEW 1
#include <FL/Fl_Gl_Window.H>
#include <sr/sr_viewer.h>
#include <sbm/GenericViewer.h>
#include "fltk_viewer.h"

class EmbeddedOgre;

class FLTKOgreWindow : public FltkViewer//public SrViewer, public Fl_Gl_Window
{
public:
	FLTKOgreWindow ( int x, int y, int w, int h, const char *label=0 );
	virtual ~FLTKOgreWindow();
	void draw();
	void updateOgreCamera();
	void render ();
	void drawFloor(bool shadowPass);
	virtual void show_viewer();
	virtual void hide_viewer();	
	virtual void resize(int x, int y, int w, int h);
	virtual void menu_cmd ( MenuCmd c, const char* label );
	virtual void resetViewer();

	virtual void notify(SmartBody::SBSubject* subject);

	void updateOptions();
	virtual std::string rendererType() { return "ogre3D"; }
	
	static void checkVisibility();

	void initOgreWindow();
	EmbeddedOgre* getOgreInterface();	
	// handle fltk mouse events ?		
protected:
	
	void fltkRender();
	void fltkRender2();
	// ogre related stuff
	EmbeddedOgre* ogreInterface;	
};

#if 0
class OgreViewerFactory : public SrViewerFactory
{
public:
	OgreViewerFactory();
	//void setFltkViewer(FltkViewer* viewer);

	virtual SrViewer* create(int x, int y, int w, int h);
	virtual void remove(SrViewer* viewer);
private:
	static SrViewer* s_viewer;
};
#endif

# endif // FLTK_VIEWER_H
