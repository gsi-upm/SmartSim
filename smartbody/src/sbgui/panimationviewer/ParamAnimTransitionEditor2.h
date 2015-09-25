/*
 *  ParamAnimStateEditor.h - part of SmartBody-lib's Test Suite
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

#ifndef _PARAM_ANIM_TRANSITION_EDITOR2_H_
#define _PARAM_ANIM_TRANSITION_EDITOR2_H_

#include <FL/Fl_Slider.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Check_Button.H>
#include "PanimationWindow.h"

class PATransitionCreator;
class PanimationWindow;
class ParamAnimEditorWidget;
namespace SmartBody {
	class SBAnimationTransition;
}

class PATransitionEditor2 : public Fl_Group
{
	public:
		PATransitionEditor2(int x, int y, int w, int h, PanimationWindow* window);
		~PATransitionEditor2();
	
		void loadStates();
		static void changeStateEditorMode(Fl_Widget* widget, void* data);
		static void updateTransitionTimeMarkEditor(Fl_Widget* widget, void* data, bool toAdd);
		static void editStateMotions(Fl_Widget* widget, void* data);
		static void changeState1List(Fl_Widget* widget, void* data);
		static void changeState2List(Fl_Widget* widget, void* data);
		static void addTransitionTimeMark(Fl_Widget* widget, void* data);
		static void removeTransitionTimeMark(Fl_Widget* widget, void* data);
		static void snapTimeMark(Fl_Widget* widget, void* data);
		static void snapStartTimeMark(Fl_Widget* widget, void* data);
		static void snapEndTimeMark(Fl_Widget* widget, void* data);
		static void updateTransitionTimeMark(Fl_Widget* widget, void* data);
		static void updateMaxTime(Fl_Widget* widget, void* data);
		static void updateMinTime(Fl_Widget* widget, void* data);
		static void save(Fl_Widget* widget, void* data);
		static void selectState1Animations(Fl_Widget* widget, void* data);
		static void selectState2Animations(Fl_Widget* widget, void* data);
		static void scrub(Fl_Widget* widget, void* data);
		static void playmotion(Fl_Widget* widget, void* data);

		void updateIntervalMarks(SmartBody::SBAnimationTransition* transition);
		void refresh();

	public:
		PanimationWindow* paWindow;
		Fl_Check_Button*	stateEditorMode;
		Fl_Group*		stateSelectionGroup;
		Fl_Button*		createTransitionButton;
		Fl_Input*		newStateName;
		Fl_Choice*		state1List;
		Fl_Browser*		state1AnimationList;
		Fl_Choice*		state2List;
		Fl_Browser*		state2AnimationList;
		Fl_Scroll*		editTransitionTimeMarkGroup;
		Fl_Button*		addMark;
		Fl_Button*		removeMark;
		Fl_Button*		snapMark;
		Fl_Button*		snapStartMark;
		Fl_Button*		snapEndMark;
		Fl_Button*		updateMark;
		Fl_Button*		buttonSave;
		Fl_Float_Input*		minTimeInput;
		Fl_Float_Input*		maxTimeInput;
		Fl_Float_Input* inputParameterX;
		Fl_Float_Input* inputParameterY;
		Fl_Float_Input* inputParameterZ;
		Fl_Value_Slider* sliderScrub;
		Fl_Button* buttonPlay;
		Fl_Choice*		choiceAutoParameter;
		ParamAnimEditorWidget* transitionTimeMarkWidget;
		nle::NonLinearEditorModel* transitionEditorNleModel;
		PATransitionCreator* creator;
		std::string lastSelectedMotion;
		double precisionCompensate;
		SmartBody::SBAnimationTransition* curTransition;

		int lastNameIndex;
		bool isPlaying;
};

#endif
