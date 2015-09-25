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

#ifndef _PARAM_ANIM_STATE_EDITOR_H_
#define _PARAM_ANIM_STATE_EDITOR_H_

#include <FL/Fl_Slider.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Check_Button.H>
#include "PanimationWindow.h"

class PABlendCreator;
class PanimationWindow;
class ParameterVisualization;
class Parameter3DVisualization;
class ParamAnimEditorWidget;
class PAParameterEditor;
class PAAutoFootStepsEditor;

class PABlendEditor : public Fl_Group
{
	public:
		PABlendEditor(int x, int y, int w, int h, PanimationWindow* window);
		~PABlendEditor();
	
		void loadStates();
		static void changeStateEditorMode(Fl_Widget* widget, void* data);
		static void updateStateTimeMarkEditor(Fl_Widget* widget, void* data, bool toAdd);
		static void editStateMotions(Fl_Widget* widget, void* data);
		static void changeStateList(Fl_Widget* widget, void* data);
		static void addStateTimeMark(Fl_Widget* widget, void* data);
		static void addFootStepMark(Fl_Widget* widget, void* data);
		static void undoFootStepMark(Fl_Widget* widget, void* data);
		static void removeStateTimeMark(Fl_Widget* widget, void* data);
		static void snapTimeMark(Fl_Widget* widget, void* data);
		static void snapStartTimeMark(Fl_Widget* widget, void* data);
		static void snapEndTimeMark(Fl_Widget* widget, void* data);
		static void snapSliderTimeMark(Fl_Widget* widget, void* data);
		static void alignLeft(Fl_Widget* widget, void* data);
		static void alignRight(Fl_Widget* widget, void* data);
		static void alignRecover(Fl_Widget* widget, void* data);
		static void updateStateTimeMark(Fl_Widget* widget, void* data);
		static void updateMaxTime(Fl_Widget* widget, void* data);
		static void updateMinTime(Fl_Widget* widget, void* data);
		static void save(Fl_Widget* widget, void* data);
		static void selectStateAnimations(Fl_Widget* widget, void* data);
		static void addShape(Fl_Widget* widget, void* data);
		static void removeShape(Fl_Widget* widget, void* data);
		static void selectShape(Fl_Widget* widget, void* data);
		static void updateParameters(Fl_Widget* widget, void* data);
		static void editParameterCb(Fl_Widget* widget, void* data);
		static void scrub(Fl_Widget* widget, void* data);
		static void playmotion(Fl_Widget* widget, void* data);
		void refreshAlign();
		void updateMotionPlayer(double t);
		void updateCorrespondenceMarks(PABlend* state);
		void refresh();

		// helper functions
		std::vector<std::string> getSelectedMotions();
		PABlend* getCurrentState();

	public:
		PanimationWindow* paWindow;
		Fl_Check_Button*	stateEditorMode;
		Fl_Group*		stateSelectionGroup;
		Fl_Button*		createStateButton;
		Fl_Input*		newStateName;
		Fl_Multi_Browser*		stateAnimationList;
		Fl_Multi_Browser*		shapeList;
		Fl_Button*		shapeAdd;
		Fl_Button*		shapeRemove;
		Fl_Scroll*		editStateTimeMarkGroup;
		Fl_Choice*		stateList;
		Fl_Choice*		choiceStateType;
		Fl_Button*		addMark;
		Fl_Button*		removeMark;
		Fl_Button*		snapMark;
		Fl_Button*		snapStartMark;
		Fl_Button*		snapEndMark;
		Fl_Button*		snapSliderMark;
		Fl_Button*		updateMark;
		Fl_Button*		buttonSave;
		Fl_Button*		buttonAutoFootSetpsEditor;
		Fl_Button*		buttonUndoAutoFootSteps;
		Fl_Button*		buttonAlignLeft;
		Fl_Button*		buttonAlignRight;
		Fl_Button*		buttonAlignRecover;
		Fl_Float_Input*		minTimeInput;
		Fl_Float_Input*		maxTimeInput;
		Fl_Float_Input* inputParameterX;
		Fl_Float_Input* inputParameterY;
		Fl_Float_Input* inputParameterZ;
		Fl_Button*		buttonEditParameter;
		Fl_Value_Slider* sliderScrub;
		Fl_Button* buttonPlay;
		Fl_Choice*		choiceAutoParameter;
		ParamAnimEditorWidget* stateTimeMarkWidget;
		nle::NonLinearEditorModel* stateEditorNleModel;
		PABlendCreator* creator;
		PAParameterEditor* parameterEditor;
		PAAutoFootStepsEditor* autoFootStepsEditor;
		std::string lastSelectedMotion;

		Fl_Group*				visualizationGroup;
		ParameterVisualization* triangleVisualization;
		Parameter3DVisualization* tetraVisualization;
		PABlendData*	blendData;

		int lastNameIndex;
		bool isPlaying;

		std::vector<std::vector<double> > previousKeys;
};

#endif
