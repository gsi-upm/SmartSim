#ifndef _PARAMETER3DVISUALIZATION_H_
#define _PARAMETER3DVISUALIZATION_H_

#include <FL/Fl_Group.H>
#include <FL/Fl_Gl_Window.H>
#include <sb/PABlend.h>
#include <sr/sr_camera.h>
#include <sr/sr_event.h>

class PABlendData;
class ParameterGroup;

class VisualizationBase : public Fl_Gl_Window
{
public:
	VisualizationBase(int x, int y, int w, int h, char* name);
	~VisualizationBase();

	virtual void draw();
	virtual int handle(int event);
	virtual void resize(int x, int y, int w, int h);
	void init_opengl();
	void translate_event(SrEvent& e, SrEvent::EventType t, int w, int h, VisualizationBase* viewer);
	void mouse_event(SrEvent& e);

	// user data		
	void drawGrid();		

public:
	SrCamera cam;
	SrEvent e;
	float gridSize;
	float gridStep;
	float floorHeight;
	int lastMouseX;
	int lastMouseY;
protected:
	PABlendData* blendData;	
	float viewScale;
};


class Parameter3DVisualization : public VisualizationBase
{
	public:
		Parameter3DVisualization(int x, int y, int w, int h, char* name, PABlendData* s, ParameterGroup* window);
		~Parameter3DVisualization();

		virtual void draw();			

		// user data
		void setSelectedTetrahedrons(std::vector<bool>& selected);
		void setSelectedParameters(std::vector<bool>& selected);
		void drawTetrahedrons();		
		void drawParameter();
		SrVec determineScale(float& largestScale);

	public:		
		std::vector<bool> selectedTetrahedrons;
		std::vector<bool> selectedParameters;		

	protected:		
		ParameterGroup* paramGroup;
		std::vector<SrVec> tet;		
};

#endif
