#ifndef _VisemeRTCurveViewer_H_
#define _VisemeRTCurveViewer_H_

#include <FL/Fl_Gl_Window.H>
#include <sr/sr_event.h>
#include <sr/sr_camera.h>
#include <sr/sr_vec2.h>
#ifdef WIN32
#include "glfont2.h"
using namespace glfont;
#endif

class RunTimeCurveData;
class VisemeRTCurveViewer : public Fl_Gl_Window
{
public:
	VisemeRTCurveViewer(int x, int y, int w, int h, char* name, RunTimeCurveData* data);
	~VisemeRTCurveViewer();

	virtual void draw();
	virtual int handle(int event);
	virtual void resize(int x, int y, int w, int h);
	void initOpenGL();
	void initFont();
	void resetCamera();
	void drawLabel();
	void drawPhonemeNames();
	void drawPhonemeLines();
	void drawAxisNGrid();
	void drawCurves();
	void setZoomRes(float val);
	float getZoomRes();

	// utility function
	SrVec2 getStringSize(const char* str);
	float getMaxX();

public:
#ifdef WIN32
	GLFont label;				// label
#endif
	float xSpan;				// default xSpan of the camera window
	float xShift;				// shift of camera center on x axis
	float maxGridX;				// what's the max grid 
	float zoom;					// what's the current zoom for camera
	float zoomResolution;		// what's the zoom resolution when you scroll the mouse wheel
	SrVec2 prevMouseCoord;		// your previous mouse coordination
	bool mouseCoordInit;		// mouse click status
	std::map<int, SrVec> colorTable;

	RunTimeCurveData* _data;
};

#endif
