/*
 *  ParamAnimTransitionEditor.h - part of SmartBody-lib's Test Suite
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

#ifndef _PARAM_ANIM_TRANSITION_EDITOR_H_
#define _PARAM_ANIM_TRANSITION_EDITOR_H_

#include <FL/Fl_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Browser.H>

class ParamAnimEditorWidget;
class PanimationWindow;
namespace nle { class NonLinearEditorModel; }


class PATransitionEditor : public Fl_Group
{
	public:
		PATransitionEditor(int x, int y, int w, int h, PanimationWindow* window);
		~PATransitionEditor();

		void loadStates();
		void loadTransitions();
		static void changeTransitionEditorMode(Fl_Widget* widget, void* data);
		static void changeStateList1(Fl_Widget* widget, void* data);
		static void changeStateList2(Fl_Widget* widget, void* data);
		static void changeAnimForTransition(Fl_Widget* widget, void* data);
		static void addTransitionTimeMark(Fl_Widget* widget, void* data);
		static void removeTransitionTimeMark(Fl_Widget* widget, void* data);
		static void updateTransitionTimeMark(Fl_Widget* widget, void* data);
		static void createNewTransition(Fl_Widget* widget, void* data);
		static void changeTransitionList(Fl_Widget* widget, void* data);

	public:
		PanimationWindow* paWindow;
		Fl_Check_Button*	transitionEditorMode;
		Fl_Group*		createTransitionGroup;
		Fl_Button*		createTransitionButton;
		Fl_Choice*		stateList1;
		Fl_Choice*		stateList2;
		Fl_Browser*		animForTransition1;
		Fl_Browser*		animForTransition2;
		Fl_Group*		editTransitionTimeMarkGroup;
		Fl_Choice*		transitionList;
		Fl_Button*		addMark1;
		Fl_Button*		removeMark1;
		Fl_Button*		updateMark1;
		ParamAnimEditorWidget* transitionTimeMarkWidget;
		nle::NonLinearEditorModel* transitionEditorNleModel;
};

#endif
