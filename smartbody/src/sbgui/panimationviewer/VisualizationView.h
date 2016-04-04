/*
 *  ParamAnimRunTimeEditor.h - part of SmartBody-lib's Test Suite
 *  Copyright (C) 2009  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Yuyu Xu, USC
 */

#ifndef _VISUALIZATION_VIEW_H_
#define _VISUALIZATION_VIEW_H_

#include <FL/Fl_Slider.H>
#include <vhcl.h>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Group.H>
#include <sr/sr_gl.h>
#include <sr/sr_light.h>
#include <sr/sr_camera.h>
#include <sr/sr_event.h>
#include "PanimationWindow.h"
#include <sb/SBAnimationState.h>

class ErrorVisualization;
class VisualizationView : public Fl_Group
{
public:
	VisualizationView(int x, int y, int w, int h, PanimationWindow* window);
	~VisualizationView();
public:
	Fl_Output*				currentCycleState;
	Fl_Choice*              visShapeChoice;
	Fl_Choice*              currentViz;
	Fl_Button*              buildVizButton;

	Fl_Button*              plotMotionButton;
	Fl_Choice*				plotJointChoice; // joint list
	Fl_Button*				plotJointButton; // refresh button
	Fl_Button*              plotJointTrajButton;
	Fl_Check_Button*		plotRandomColorCheckbox;
	Fl_Button*              clearMotionButton;
	Fl_Button*              plotVectorFlowButton;
	Fl_Button*              clearVectorFlowButton;

	std::string				prevCycleState;
	PanimationWindow*		paWindow;
	Fl_Group*				parameterGroup;		
	ErrorVisualization*     errorViz;

public:
	SmartBody::SBAnimationBlend* getCurrentBlend(void);

	void update();		
	void buildVisualization();
	static void buildViz(Fl_Widget* widget, void* data);
	static void updateVizType(Fl_Widget* widget, void* data);

	void plotMotion(bool randomColor);
	static void plotMotion(Fl_Widget* widget, void* data);
	void plotJointTraj(const std::string& jntName, bool randomColor);
	static void plotJointTraj(Fl_Widget* widget, void* data);
	void refreshJointList();
	static void refreshJointList(Fl_Widget* widget, void* data);
	void clearMotion();
	static void clearMotion(Fl_Widget* widget, void* data);

	void plotVectorFlow();
	static void plotVectorFlow(Fl_Widget* widget, void* data);
	void clearVectorFlow();
	static void clearVectorFlow(Fl_Widget* widget, void* data);

};
#endif
