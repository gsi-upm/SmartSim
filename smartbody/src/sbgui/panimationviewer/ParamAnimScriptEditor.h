/*
 *  ParamAnimScriptEditor.h - part of SmartBody-lib's Test Suite
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

#ifndef _PARAM_ANIM_SCRIPT_EDITOR_H_
#define _PARAM_ANIM_SCRIPT_EDITOR_H_

#include <FL/Fl_Slider.H>
#include "PanimationWindow.h"

class PAScriptEditor : public Fl_Group
{
	public:
		PAScriptEditor(int x, int y, int w, int h, PanimationWindow* window);
		~PAScriptEditor();
		
		static void addState(Fl_Widget* widget, void* data);
		static void removeState(Fl_Widget* widget, void* data);
		static void updateStateInfo(Fl_Widget* widget, void* data);
		static void run(Fl_Widget* widget, void* data);
		static void changeCurrentStateWeight(Fl_Widget* widget, void* data);
		void initialAvailableStates();
		void updateAvailableStates(std::string currentState);
		void refresh();
		void update();

	public:
		PanimationWindow*	paWindow;
		Fl_Browser*		availableStateList;
		Fl_Browser*		currentStateList;
		Fl_Button*		addStateButton;
		Fl_Button*		removeStateButton;
		Fl_Button*		runStateList;
		Fl_Output*		currentStatePanel;
		Fl_Output*		nextStatePanel;
		Fl_Value_Slider*	currentStateWeight;

		std::map<std::string, double>	stateTimeOffset;
		std::map<std::string, bool>		stateLoopMode;
};

#endif
