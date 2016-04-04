/*
 *  ParamAnimStateEditor.cpp - part of SmartBody-lib's Test Suite
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

#include "vhcl.h"
#include "ParamAnimStateEditor.h"
#include <FL/Fl_File_Chooser.H>
#include <sb/SBScene.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBMotion.h>

#include "ParamAnimBlock.h"
#include "ParamAnimStateCreatorWidget.h"
#include "ParamAnimParameterEditor.h"
#include "ParameterVisualization.h"
#include "Parameter3DVisualization.h"
#include "ParamAnimEditorWidget.h"
#include "ParamAnimAutoMarkingEditor.h"
#include "RootWindow.h"

PABlendEditor::PABlendEditor(int x, int y, int w, int h, PanimationWindow* window) : Fl_Group(x, y, w, h), paWindow(window)
{
	lastNameIndex = 0;
	isPlaying = false;

	this->label("Blend Editor");
	this->begin();

		int csx = xDis + x;
		int csy = yDis + y;
		int workspaceWidth = w - 2 * xDis;
		int workspaceHeight = h - csy;
		stateSelectionGroup = new Fl_Group(csx, csy, workspaceWidth, workspaceHeight / 3, "");
		int stateSelectionGroupW = w - 2 * xDis;
		int stateSelectionGroupH = h /2 - 3 * yDis;
		stateSelectionGroup->begin();
			stateList = new Fl_Choice(csx + 50, csy + 5, 150, 20, "Blend");
//			stateList->when(FL_WHEN_ENTER_KEY);
			stateList->callback(changeStateList, this);
			choiceStateType = new Fl_Choice(csx + 250, csy + 5, 80, 20, "Type");
			choiceStateType->add("---");
			choiceStateType->add("0D");
			choiceStateType->add("1D");
			choiceStateType->add("2D");
			choiceStateType->add("3D");
			choiceStateType->value(0);
			choiceStateType->deactivate();
			createStateButton = new Fl_Button(csx + 400, csy + 5, 80, 20, "Create");
			createStateButton->callback(editStateMotions, this);
			
			stateAnimationList = new Fl_Multi_Browser(csx + 50, csy + 40, 290, 100, "Blend Motions");
			stateAnimationList->align(FL_ALIGN_TOP);
			stateAnimationList->when(FL_WHEN_CHANGED);
			stateAnimationList->callback(selectStateAnimations, this);
			shapeAdd = new Fl_Button(csx + 360, csy + 60, 60, 20, "+");
			shapeAdd->callback(addShape, this);
			shapeAdd->deactivate();
			shapeRemove = new Fl_Button(csx + 360, csy + 100, 60, 20, "-");
			shapeRemove->callback(removeShape, this);
			shapeRemove->deactivate();
			shapeList = new Fl_Multi_Browser(csx + 440, csy + 40, 290, 100,  "Parameterization");
			shapeList->align(FL_ALIGN_TOP);
			shapeList->when(FL_WHEN_CHANGED);
			shapeList->callback(selectShape, this);

			inputParameterX = new Fl_Float_Input(csx + 80, csy + 150, 60, 20,  "Parameters");
			inputParameterY = new Fl_Float_Input(csx + 150, csy + 150, 60, 20, "");
			inputParameterZ = new Fl_Float_Input(csx + 220, csy + 150, 60, 20, "");
			inputParameterX->callback(updateParameters, this);
			inputParameterY->callback(updateParameters, this);
			inputParameterZ->callback(updateParameters, this);
			inputParameterX->deactivate();
			inputParameterY->deactivate();
			inputParameterZ->deactivate();

			buttonEditParameter = new Fl_Button(csx + 300, csy + 150, 60, 20, "Edit");
			buttonEditParameter->deactivate();
			buttonEditParameter->callback(editParameterCb, this);

			buttonPlay = new Fl_Button(csx + 380, csy + 150, 30, 20, "@>");
			buttonPlay->callback(playmotion, this);
			buttonPlay->deactivate();
			sliderScrub = new Fl_Value_Slider(csx + 420, csy + 150, 300, 20, "");
			sliderScrub->type(FL_HORIZONTAL);
			sliderScrub->range(0, 1);
			sliderScrub->callback(scrub, this);
			sliderScrub->deactivate();

		stateSelectionGroup->end();
		stateSelectionGroup->box(FL_BORDER_BOX);

		int esx = xDis + x;
		int esy = h / 3  + y;
		Fl_Group* buttonGroup = new Fl_Group(esx, esy, w - 2 * xDis, 4 * yDis);
		buttonGroup->begin();
			addMark = new Fl_Button(xDis + esx, esy, 50, 2 * yDis, "+");
			addMark->callback(addStateTimeMark, this);
			removeMark = new Fl_Button(xDis + 50 + esx, esy, 50, 2 * yDis, "-");
			removeMark->callback(removeStateTimeMark, this);
			snapMark = new Fl_Button(xDis + 100 + esx, esy, 100, 2 * yDis, "SnapScrub");
			snapMark->callback(snapTimeMark, this);
			snapSliderMark = new Fl_Button(xDis + 200 + esx, esy, 100, 2 * yDis, "SnapSlider");
			snapSliderMark->callback(snapSliderTimeMark, this);
			snapStartMark = new Fl_Button(xDis + 300 + esx, esy, 50, 2 * yDis, "@|< Snap");
			snapStartMark->callback(snapStartTimeMark, this);
			snapEndMark = new Fl_Button(xDis + 350 + esx, esy, 50, 2 * yDis, "Snap @>|");
			snapEndMark->callback(snapEndTimeMark, this);
			buttonAutoFootSetpsEditor = new Fl_Button(xDis + 500 + esx, esy, 100, 2 * yDis, "AutoFootsteps");
			buttonAutoFootSetpsEditor->callback(addFootStepMark, this);
			buttonAutoFootSetpsEditor->deactivate();
			buttonUndoAutoFootSteps = new Fl_Button(xDis + 600 + esx, esy, 100, 2 * yDis, "UndoFootsteps");
			buttonUndoAutoFootSteps->callback(undoFootStepMark, this);
			buttonUndoAutoFootSteps->deactivate();
			buttonAlignLeft = new Fl_Button(xDis + 500 + esx, 2 * yDis + esy, 50, 2 * yDis, "@|< Align");
			buttonAlignLeft->callback(alignLeft, this);
			buttonAlignLeft->deactivate();
			buttonAlignRight = new Fl_Button(xDis + 550 + esx, 2 * yDis + esy, 50, 2 * yDis, "Align @>|");
			buttonAlignRight->callback(alignRight, this);
			buttonAlignRight->deactivate();
			buttonAlignRecover = new Fl_Button(xDis + 600 + esx, 2 * yDis + esy, 100, 2 * yDis, "RestoreAlign");
			buttonAlignRecover->callback(alignRecover, this);
			buttonAlignRecover->deactivate();

			buttonSave = new Fl_Button(xDis + esx, 2 * yDis + esy, 100, 2 * yDis, "Save");
			buttonSave->callback(save, this);
			minTimeInput = new Fl_Float_Input(xDis + 200 + esx, 2 * yDis + esy, 60, 2 * yDis, "Show Times");
			minTimeInput->callback(updateMinTime, this);
			maxTimeInput = new Fl_Float_Input(xDis + 260 + esx, 2 * yDis + esy, 60, 2 * yDis, "");
			maxTimeInput->callback(updateMaxTime, this);
		buttonGroup->end();

		editStateTimeMarkGroup = new Fl_Scroll(csx, esy + 3 * yDis + 20, workspaceWidth, workspaceHeight / 3 - 5 * yDis - 10);
		editStateTimeMarkGroup->type(Fl_Scroll::VERTICAL_ALWAYS);
		editStateTimeMarkGroup->begin();
			stateTimeMarkWidget = new ParamAnimEditorWidget(this, csx + 10, 5 * yDis + esy, workspaceWidth - 10, workspaceHeight / 3 - 7 * yDis, (char*) "");
		editStateTimeMarkGroup->end();
		editStateTimeMarkGroup->resizable(stateTimeMarkWidget);
		editStateTimeMarkGroup->box(FL_BORDER_BOX);
		this->resizable(editStateTimeMarkGroup);

		int vgx = xDis + x;
		int vgy = 2 * h / 3  + yDis + y;
		visualizationGroup = new Fl_Group(vgx, vgy, w - 2 * xDis, h / 3 - 3 * yDis);
		visualizationGroup->box(FL_BORDER_BOX);
		visualizationGroup->begin();
		visualizationGroup->end();
		
	this->end();

	stateEditorNleModel = new nle::NonLinearEditorModel();
	stateTimeMarkWidget->setModel(stateEditorNleModel);
	stateEditorNleModel->addModelListener(stateTimeMarkWidget);
	stateTimeMarkWidget->setup();

	creator = NULL;
	parameterEditor = NULL;
	autoFootStepsEditor = NULL;
	lastSelectedMotion = "";
	triangleVisualization = NULL;
	tetraVisualization = NULL;
	blendData = NULL;

	loadStates();
	changeStateList(stateList, this);
}

PABlendEditor::~PABlendEditor()
{
}

void PABlendEditor::loadStates()
{
	stateList->clear();
	stateList->add("---");
	// states may have names that conflict with FLTK's parsing, such as a '@'
	std::vector<std::string> blendNames = SmartBody::SBScene::getScene()->getBlendManager()->getBlendNames();
	for (std::vector<std::string>::iterator iter = blendNames.begin();
		 iter != blendNames.end();
		 iter++)
	{
		stateList->add((*iter).c_str());
	}
	
	stateList->value(0);
}

void PABlendEditor::updateStateTimeMarkEditor(Fl_Widget* widget, void* data, bool toAdd)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	if (editor->stateAnimationList->size() == 0)
		editor->stateEditorNleModel->removeAllTracks();

	if (toAdd)
	{
		for (int i = 0; i < editor->stateAnimationList->size(); i++)
		{
			std::string motionName = editor->stateAnimationList->text(i+1);//text(i+1);
			nle::Track* track = editor->stateEditorNleModel->getTrack(motionName);
			if (!track && toAdd)
			{
				ParamAnimTrack* newTrack = new ParamAnimTrack();
				newTrack->setName(motionName.c_str());
				SmartBody::SBMotion * motion = SmartBody::SBScene::getScene()->getMotion(motionName);
				ParamAnimBlock* block = new ParamAnimBlock();
				block->setName(motionName.c_str());
				block->setStartTime(0);
				block->setEndTime(motion->duration());
				editor->stateEditorNleModel->addTrack(newTrack);
				newTrack->addBlock(block);		
				editor->stateEditorNleModel->update();
				editor->stateTimeMarkWidget->setViewableTimeEnd(editor->stateEditorNleModel->getEndTime());
			}
		}
	}
	else
	{
		std::string motionName;
		for (int i = 0; i < editor->stateEditorNleModel->getNumTracks(); i++)
		{
			bool del = true;
			for (int j = 0; j < editor->stateAnimationList->size(); j++)
			{
				std::string motionName = editor->stateAnimationList->text(j + 1);
				if (editor->stateEditorNleModel->getTrack(i)->getName() == motionName)
				{
					del = false;
					break;
				}
			}
			if (del)
			{
				editor->stateEditorNleModel->removeTrack(editor->stateEditorNleModel->getTrack(i)->getName());
				i--;
			}
		}
	}
	editor->paWindow->redraw();
}

void PABlendEditor::editStateMotions(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	bool isCreateMode = true;
	std::string stateName = "";
	if (std::string("Edit Blend") == editor->createStateButton->label())
	{
		isCreateMode = false;
		stateName = editor->stateList->menu()[editor->stateList->value()].label();
	}
	if (!editor->creator)
	{
		editor->creator = new PABlendCreator(editor, editor->paWindow->x() + 50, editor->paWindow->y() + 50, 800, 600);
	}
	editor->creator->setInfo(isCreateMode, stateName);
	
	editor->creator->show();
}

void PABlendEditor::changeStateList(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;
	int curValue = editor->stateList->value();
	if (curValue < 0)
		return;

	editor->loadStates();
	editor->stateList->value(curValue);
	editor->stateEditorNleModel->removeAllTracks();
	std::string currentStateName = editor->stateList->menu()[editor->stateList->value()].label();
	PABlend* currentState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(currentStateName);
	
	if (currentState)
	{
		if (editor->blendData)
			delete editor->blendData;
		editor->blendData = NULL;

		editor->shapeList->clear();

		editor->createStateButton->label("Edit Blend");
		SmartBody::SBMotionBlendBase* blendBase = dynamic_cast<SmartBody::SBMotionBlendBase*>(currentState);
		// determine the state type
		SmartBody::SBAnimationBlend0D* state0d = dynamic_cast<SmartBody::SBAnimationBlend0D*>(currentState);
		if (state0d)
		{
			editor->choiceStateType->value(1);
			editor->shapeList->activate();
			editor->shapeList->label("Triangles");
			if (editor->triangleVisualization != NULL)
			{
				editor->visualizationGroup->remove(editor->triangleVisualization);
				delete editor->triangleVisualization;
				editor->triangleVisualization = NULL;
			}			
			if (editor->tetraVisualization != NULL)
			{
				editor->tetraVisualization->remove(editor->triangleVisualization);
				delete editor->tetraVisualization;
				editor->tetraVisualization = NULL;
			}
			std::vector<double> weights;
			editor->blendData = new PABlendData(currentState, weights);
			editor->triangleVisualization = new ParameterVisualization(false, editor->visualizationGroup->x(), editor->visualizationGroup->y(), editor->visualizationGroup->w(), editor->visualizationGroup->h(), "triangle", editor->blendData, NULL);
			editor->visualizationGroup->add(editor->triangleVisualization);
			editor->triangleVisualization->show();
			editor->triangleVisualization->redraw();
		}
		SmartBody::SBAnimationBlend1D* state1d = dynamic_cast<SmartBody::SBAnimationBlend1D*>(currentState);
		if (state1d)
		{
			editor->choiceStateType->value(2);
			editor->shapeList->activate();
			editor->shapeList->label("Triangles");
			if (editor->triangleVisualization != NULL)
			{
				editor->visualizationGroup->remove(editor->triangleVisualization);
				delete editor->triangleVisualization;
				editor->triangleVisualization = NULL;
			}
			if (editor->tetraVisualization != NULL)
			{
				editor->tetraVisualization->remove(editor->triangleVisualization);
				delete editor->tetraVisualization;
				editor->tetraVisualization = NULL;
			}
			std::vector<double> weights;
			editor->blendData = new PABlendData(currentState, weights);
			editor->triangleVisualization = new ParameterVisualization(false, editor->visualizationGroup->x(), editor->visualizationGroup->y(), editor->visualizationGroup->w(), editor->visualizationGroup->h(), "triangle", editor->blendData, NULL);
			editor->visualizationGroup->add(editor->triangleVisualization);
			editor->triangleVisualization->show();
			editor->triangleVisualization->redraw();
		}
		SmartBody::SBAnimationBlend2D* state2d = dynamic_cast<SmartBody::SBAnimationBlend2D*>(currentState);
		if (state2d)
		{
			editor->choiceStateType->value(3);
			editor->shapeList->activate();
			editor->shapeList->label("Triangles");
			if (editor->triangleVisualization != NULL)
			{
				editor->visualizationGroup->remove(editor->triangleVisualization);
				delete editor->triangleVisualization;
				editor->triangleVisualization = NULL;
			}
			if (editor->tetraVisualization != NULL)
			{
				editor->tetraVisualization->remove(editor->triangleVisualization);
				delete editor->tetraVisualization;
				editor->tetraVisualization = NULL;
			}
			std::vector<double> weights;
			editor->blendData = new PABlendData(currentState, weights);
			editor->triangleVisualization = new ParameterVisualization(false, editor->visualizationGroup->x(), editor->visualizationGroup->y(), editor->visualizationGroup->w(), editor->visualizationGroup->h(), "triangle", editor->blendData, NULL);
			editor->visualizationGroup->add(editor->triangleVisualization);
			editor->triangleVisualization->show();
			editor->triangleVisualization->redraw();
		}
		SmartBody::SBAnimationBlend3D* state3d = dynamic_cast<SmartBody::SBAnimationBlend3D*>(currentState);
		
		bool use3DVisualization = state3d || (blendBase && blendBase->getParameterDimension() == 3);
		if (use3DVisualization)
		{
			editor->choiceStateType->value(4);
			editor->shapeList->activate();
			editor->shapeList->label("Tetrahedrons");
			if (editor->triangleVisualization != NULL)
			{
				editor->visualizationGroup->remove(editor->triangleVisualization);
				delete editor->triangleVisualization;
				editor->triangleVisualization = NULL;
			}
			if (editor->tetraVisualization != NULL)
			{
				editor->tetraVisualization->remove(editor->triangleVisualization);
				delete editor->tetraVisualization;
				editor->tetraVisualization = NULL;
			}
			std::vector<double> weights;
			editor->blendData = new PABlendData(currentState, weights);
			editor->tetraVisualization = new Parameter3DVisualization(editor->visualizationGroup->x(), editor->visualizationGroup->y(), editor->visualizationGroup->w(), editor->visualizationGroup->h(), "triangle", editor->blendData, NULL);
			editor->visualizationGroup->add(editor->tetraVisualization);
			editor->tetraVisualization->show();
			editor->tetraVisualization->redraw();
		}

		editor->inputParameterX->deactivate();
		editor->inputParameterY->deactivate();
		editor->inputParameterZ->deactivate();

		editor->buttonPlay->deactivate();
		editor->isPlaying = false;
		editor->sliderScrub->deactivate();
		editor->shapeAdd->deactivate();
		editor->shapeRemove->deactivate();


		for (int i = 0; i < currentState->getNumMotions(); i++)
		{
			SkMotion* motion = currentState->motions[i];
			std::string motionName = motion->getName();
			ParamAnimTrack* track = new ParamAnimTrack();
			track->setName(motionName.c_str());
			ParamAnimBlock* block = new ParamAnimBlock();
			block->setName(motionName.c_str());
			block->setStartTime(0);
			block->setEndTime(motion->duration());

			editor->stateEditorNleModel->addTrack(track);
			track->addBlock(block);
		}
		editor->updateCorrespondenceMarks(currentState);

		// add the motions to the motion list
		editor->stateAnimationList->clear();
		for (int i = 0; i < currentState->getNumMotions(); i++)
		{
			SkMotion* motion = currentState->motions[i];
			editor->stateAnimationList->add(motion->getName().c_str());
		}

		// add the shapes to the shape list accordingly
		if (state2d) //triangles
		{
			std::vector<TriangleInfo>& triangles = state2d->getTriangles();
			for (size_t i = 0; i < triangles.size(); i++)
			{
				std::string triString = triangles[i].motion1 + "|" + triangles[i].motion2 + "|" + triangles[i].motion3;
				editor->shapeList->add(triString.c_str());
			}
		}
		if (state3d) //tetrahedrons
		{
			std::vector<TetrahedronInfo>& tetrahedrons = state3d->getTetrahedrons();
			for (size_t i = 0; i < tetrahedrons.size(); i++)
			{
				std::string tetraString = tetrahedrons[i].motion1 + "|" + tetrahedrons[i].motion2 + "|" + tetrahedrons[i].motion3 + "|" + tetrahedrons[i].motion4;
				editor->shapeList->add(tetraString.c_str());
			}
		}
		editor->buttonPlay->activate();
	}
	else
	{
		editor->createStateButton->label("Create Blend");
		editor->choiceStateType->value(0);

		editor->buttonPlay->deactivate();
		editor->isPlaying = false;
		editor->sliderScrub->deactivate();

		editor->stateAnimationList->clear();
		editor->shapeList->clear();
		editor->shapeList->label("");
		editor->inputParameterX->value("");
		editor->inputParameterX->deactivate();
		editor->inputParameterY->value("");
		editor->inputParameterY->deactivate();
		editor->inputParameterZ->value("");
		editor->inputParameterZ->deactivate();


		// remove any visualizations
		if (editor->triangleVisualization != NULL)
		{
			editor->visualizationGroup->remove(editor->triangleVisualization);
			delete editor->triangleVisualization;
			editor->triangleVisualization = NULL;
		}
		if (editor->tetraVisualization != NULL)
		{
			editor->tetraVisualization->remove(editor->triangleVisualization);
			delete editor->tetraVisualization;
			editor->tetraVisualization = NULL;
		}
	}

	editor->isPlaying = true;
	editor->playmotion(editor->buttonPlay, editor);
	editor->refreshAlign();
	editor->stateTimeMarkWidget->setup();
	editor->editStateTimeMarkGroup->scroll_to(0, 0);
	editor->paWindow->redraw();
}

void PABlendEditor::addFootStepMark(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;
	if (!editor->autoFootStepsEditor)
	{
		editor->autoFootStepsEditor = new PAAutoFootStepsEditor(editor, editor->paWindow->x() + 50, editor->paWindow->y() + 50, 500, 600);
	}
	editor->autoFootStepsEditor->show();	
	editor->autoFootStepsEditor->refreshSelectedMotions();
}


void PABlendEditor::undoFootStepMark(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;	

	PABlend* currentState = editor->getCurrentState();

	currentState->keys.clear();
	currentState->keys.resize(currentState->getNumMotions());
	for (int i = 0; i < currentState->getNumMotions(); i++)
	{
		currentState->keys[i].resize(editor->previousKeys[i].size());
		currentState->keys[i] = editor->previousKeys[i];
	}

	editor->refresh();
	editor->buttonUndoAutoFootSteps->deactivate();
}

void PABlendEditor::updateMaxTime(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;
	editor->stateTimeMarkWidget->setViewableTimeEnd(atof(editor->maxTimeInput->value()));
	editor->stateTimeMarkWidget->setup();
	editor->paWindow->redraw();
}

void PABlendEditor::updateMinTime(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor* ) data;
	editor->stateTimeMarkWidget->setViewableTimeStart(atof(editor->minTimeInput->value()));
	editor->stateTimeMarkWidget->setup();
	editor->paWindow->redraw();
}



void PABlendEditor::addStateTimeMark(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	// determine where to add the time marks
	std::string stateName = editor->stateList->text();

	SmartBody::SBAnimationBlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	if (!state)
		return;
	std::vector<double> localTimes = editor->stateTimeMarkWidget->getLocalTimes();
	if (localTimes.size() != state->getNumMotions())
	{
		localTimes.resize(state->getNumMotions());
		for (int x = 0; x < state->getNumMotions(); x++)
		{
			localTimes[x] = 0.0;
		}
	}

	SmartBody::SBAnimationBlend0D* state0D = dynamic_cast<SmartBody::SBAnimationBlend0D*>(state);
	SmartBody::SBAnimationBlend1D* state1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(state);
	SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(state);
	SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(state);

	std::vector<std::string> motionNames;
	for (int x = 0; x < state->getNumMotions(); x++)
	{
		motionNames.push_back(state->getMotion(x));
	}

	state->addCorrespondencePoints(motionNames, localTimes);

	editor->updateCorrespondenceMarks(state);
	
//	editor->paWindow->addTimeMark(editor->stateEditorNleModel);
	editor->stateTimeMarkWidget->setup();
	editor->selectStateAnimations(editor->stateAnimationList, editor);
	editor->scrub(editor->sliderScrub, editor);
	editor->paWindow->redraw();
}

void PABlendEditor::removeStateTimeMark(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	// determine where to add the time marks
	std::string stateName = editor->stateList->text();

	SmartBody::SBAnimationBlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	if (!state)
		return;

	// which correspondence point has been selected?
	int keyIndex = -1;
	CorrespondenceMark* attachedMark = NULL;
	for (int t = 0; t < editor->stateEditorNleModel->getNumTracks(); t++)
	{
		nle::Track* track = editor->stateEditorNleModel->getTrack(t);
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			for (int m = 0; m < block->getNumMarks(); m++)
			{
				nle::Mark* mark = block->getMark(m);
				if (mark->isSelected())
				{
					keyIndex = m;
					break;
				}
			}
		}
	}
	if (keyIndex > -1 && keyIndex < state->getNumKeys())
	{
		state->removeCorrespondencePoints(keyIndex);
		editor->updateCorrespondenceMarks(state);

		editor->stateTimeMarkWidget->setup();
		editor->selectStateAnimations(editor->stateAnimationList, editor);
		editor->scrub(editor->sliderScrub, editor);
		editor->paWindow->redraw();
	}

	
}

void PABlendEditor::snapTimeMark(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	// determine where to add the time marks
	std::string stateName = editor->stateList->text();

	SmartBody::SBAnimationBlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	if (!state)
		return;

	// which correspondence point has been selected?
	int keyIndex = -1;
	CorrespondenceMark* attachedMark = NULL;
	int motionIndex = -1;
	for (int t = 0; t < editor->stateEditorNleModel->getNumTracks(); t++)
	{
		nle::Track* track = editor->stateEditorNleModel->getTrack(t);
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			for (int m = 0; m < block->getNumMarks(); m++)
			{
				nle::Mark* mark = block->getMark(m);
				if (mark->isSelected())
				{
					keyIndex = m;
					motionIndex = t;
					break;
				}
			}
		}
	}
	if (keyIndex > -1 && keyIndex < state->getNumKeys())
	{
		// get the local times
		std::vector<double> localTimes = editor->stateTimeMarkWidget->getLocalTimes();
		state->setCorrespondencePoints(motionIndex, keyIndex, localTimes[motionIndex]);
		editor->updateCorrespondenceMarks(state);

		editor->stateTimeMarkWidget->setup();
		editor->selectStateAnimations(editor->stateAnimationList, editor);
		// reselect the equivalent mark
		editor->stateTimeMarkWidget->getModel()->getTrack(motionIndex)->getBlock(0)->getMark(keyIndex)->setSelected(true);

		editor->sliderScrub->value(localTimes[motionIndex]);
		editor->scrub(editor->sliderScrub, editor);
		editor->paWindow->redraw();
	}
}

void PABlendEditor::snapStartTimeMark(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	std::string stateName = editor->stateList->text();

	SmartBody::SBAnimationBlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	if (!state)
		return;

	// which correspondence point has been selected?
	int keyIndex = -1;
	CorrespondenceMark* attachedMark = NULL;
	int motionIndex = -1;
	for (int t = 0; t < editor->stateEditorNleModel->getNumTracks(); t++)
	{
		nle::Track* track = editor->stateEditorNleModel->getTrack(t);
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			for (int m = 0; m < block->getNumMarks(); m++)
			{
				nle::Mark* mark = block->getMark(m);
				if (mark->isSelected())
				{
					keyIndex = m;
					motionIndex = t;
					break;
				}
			}
		}
	}
	if (keyIndex > -1 && keyIndex < state->getNumKeys())
	{
		// get the local times
		std::vector<double> localTimes = editor->stateTimeMarkWidget->getLocalTimes();
		state->setCorrespondencePoints(motionIndex, keyIndex, 0.0);
		editor->updateCorrespondenceMarks(state);

		editor->stateTimeMarkWidget->setup();
		editor->selectStateAnimations(editor->stateAnimationList, editor);
		// reselect the equivalent mark
		editor->stateTimeMarkWidget->getModel()->getTrack(motionIndex)->getBlock(0)->getMark(keyIndex)->setSelected(true);

		editor->sliderScrub->value(0.0);
		editor->scrub(editor->sliderScrub, editor);
		editor->paWindow->redraw();
	}
}

void PABlendEditor::snapEndTimeMark(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	std::string stateName = editor->stateList->text();

	SmartBody::SBAnimationBlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	if (!state)
		return;

	// which correspondence point has been selected?
	int keyIndex = -1;
	CorrespondenceMark* attachedMark = NULL;
	int motionIndex = -1;
	for (int t = 0; t < editor->stateEditorNleModel->getNumTracks(); t++)
	{
		nle::Track* track = editor->stateEditorNleModel->getTrack(t);
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			for (int m = 0; m < block->getNumMarks(); m++)
			{
				nle::Mark* mark = block->getMark(m);
				if (mark->isSelected())
				{
					keyIndex = m;
					motionIndex = t;
					break;
				}
			}
		}
	}
	if (keyIndex > -1 && keyIndex < state->getNumKeys())
	{
		// get the local times
		std::vector<double> localTimes = editor->stateTimeMarkWidget->getLocalTimes();
		// get the end time of the motion
		double endTime = SmartBody::SBScene::getScene()->getMotion(state->getMotion(motionIndex))->duration();
		state->setCorrespondencePoints(motionIndex, keyIndex, endTime);
		editor->updateCorrespondenceMarks(state);

		editor->stateTimeMarkWidget->setup();
		editor->selectStateAnimations(editor->stateAnimationList, editor);
		// reselect the equivalent mark
		editor->stateTimeMarkWidget->getModel()->getTrack(motionIndex)->getBlock(0)->getMark(keyIndex)->setSelected(true);

		editor->sliderScrub->value(endTime);
		editor->scrub(editor->sliderScrub, editor);
		editor->paWindow->redraw();
	}
}

void PABlendEditor::snapSliderTimeMark(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	std::string stateName = editor->stateList->text();

	SmartBody::SBAnimationBlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	if (!state)
		return;

	// which correspondence point has been selected?
	int keyIndex = -1;
	CorrespondenceMark* attachedMark = NULL;
	int motionIndex = -1;
	for (int t = 0; t < editor->stateEditorNleModel->getNumTracks(); t++)
	{
		nle::Track* track = editor->stateEditorNleModel->getTrack(t);
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			for (int m = 0; m < block->getNumMarks(); m++)
			{
				nle::Mark* mark = block->getMark(m);
				if (mark->isSelected())
				{
					keyIndex = m;
					motionIndex = t;
					break;
				}
			}
		}
	}
	if (keyIndex > -1 && keyIndex < state->getNumKeys())
	{
		// get the slider time
		double sliderTime = editor->sliderScrub->value();
		state->setCorrespondencePoints(motionIndex, keyIndex, sliderTime);
		editor->updateCorrespondenceMarks(state);

		editor->stateTimeMarkWidget->setup();
		editor->selectStateAnimations(editor->stateAnimationList, editor);
		// reselect the equivalent mark
		editor->stateTimeMarkWidget->getModel()->getTrack(motionIndex)->getBlock(0)->getMark(keyIndex)->setSelected(true);

		editor->sliderScrub->value(sliderTime);
		editor->scrub(editor->sliderScrub, editor);
		editor->paWindow->redraw();
	}
}

void PABlendEditor::alignLeft(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;
	
	const std::vector<std::string>& selectedMotions = editor->getSelectedMotions();
	for (size_t i = 0; i < selectedMotions.size(); i++)
	{
		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(selectedMotions[i]);
		if (motion)
			motion->alignToBegin(1);
	}
	editor->refreshAlign();
}

void PABlendEditor::alignRight(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	const std::vector<std::string>& selectedMotions = editor->getSelectedMotions();
	for (size_t i = 0; i < selectedMotions.size(); i++)
	{
		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(selectedMotions[i]);
		if (motion)
			motion->alignToEnd(1);
	}
	editor->refreshAlign();
}

void PABlendEditor::alignRecover(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	const std::vector<std::string>& selectedMotions = editor->getSelectedMotions();
	for (size_t i = 0; i < selectedMotions.size(); i++)
	{
		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(selectedMotions[i]);
		if (motion)
			motion->recoverAlign();
	}
	editor->refreshAlign();
}

void PABlendEditor::refreshAlign()
{
	PABlend* currentState = getCurrentState();
	if (!currentState)
		return;

	std::vector<double> alignTimes;
	for (int i = 0 ;i < currentState->getNumMotions(); i++)
	{
		SmartBody::SBMotion* motion = dynamic_cast<SmartBody::SBMotion*> (currentState->motions[i]);
		if (motion)
		{
			int alignId = motion->getAlignIndex();
			if (alignId < 0)
				alignId = motion->getNumFrames() + alignId;
			double alignTime = alignId * motion->getFrameRate();
			alignTimes.push_back(alignTime);
		}
		else
		{
			alignTimes.push_back(0);
		}
	}
	stateTimeMarkWidget->setAlignTimes(alignTimes);
	stateTimeMarkWidget->redraw();
}

void PABlendEditor::updateCorrespondenceMarks(PABlend* state)
{
	for (int i = 0; i < stateEditorNleModel->getNumTracks(); i++)
	{
		nle::Track* track = stateEditorNleModel->getTrack(i);
		nle::Block* block = track->getBlock(0);
		block->removeAllMarks();
	}

	for (int i = 0; i < state->getNumMotions(); i++)
	{
		nle::Track* track = stateEditorNleModel->getTrack(i);
		nle::Block* block = track->getBlock(0);

		for (int j = 0; j < state->getNumKeys(); j++)
		{
			CorrespondenceMark* mark = new CorrespondenceMark();
			mark->setStartTime(state->keys[i][j]);
			mark->setEndTime(mark->getStartTime());
			mark->setColor(FL_RED);
			char buff[256];
			sprintf(buff, "%6.2f", mark->getStartTime());
			mark->setName(buff);
			mark->setShowName(true);
			block->addMark(mark);
			mark->setSelected(false);
		}
	}

	for (int i = 0; i < stateEditorNleModel->getNumTracks() - 1; i++)
	{
		nle::Track* track1 = stateEditorNleModel->getTrack(i);
		nle::Track* track2 = stateEditorNleModel->getTrack(i + 1);
		nle::Block* block1 = track1->getBlock(0);
		nle::Block* block2 = track2->getBlock(0);
		if (block1->getNumMarks() != block2->getNumMarks())
			continue;

		for (int j = 0; j < block1->getNumMarks(); j++)
		{
			CorrespondenceMark* mark1 = dynamic_cast<CorrespondenceMark*> (block1->getMark(j));
			CorrespondenceMark* mark2 = dynamic_cast<CorrespondenceMark*> (block2->getMark(j));
			mark1->attach(mark2);
			mark2->attach(mark1);
		}
	}
}

void PABlendEditor::refresh()
{
	int origStateValue = stateList->value();
	stateEditorNleModel->removeAllTracks();
	stateTimeMarkWidget->setup();
	stateAnimationList->clear();
	shapeList->clear();
	loadStates();
	stateList->value(origStateValue);
	changeStateList(stateList, this);
	selectStateAnimations(stateAnimationList, this);
	isPlaying = true;
	playmotion(this->buttonPlay, this);
}

std::vector<std::string> PABlendEditor::getSelectedMotions()
{
	std::vector<std::string> selectedMotions;

	for (int i = 0; i < stateAnimationList->size(); i++)
	{
		if (stateAnimationList->selected(i+1))
		{
			selectedMotions.push_back(stateAnimationList->text(i + 1));
		}
	}
	return selectedMotions;
}

PABlend* PABlendEditor::getCurrentState()
{
	std::string stateName = stateList->text();
	SmartBody::SBAnimationBlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	return state;
}


void PABlendEditor::save(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	std::string mediaPath = SmartBody::SBScene::getScene()->getMediaPath();
	std::string stateFileName = BaseWindow::chooseFile("Blend File:", "Python\t*.py\n", mediaPath);
	if (stateFileName == "")
		return;

	std::string stateName = editor->stateList->text();

	SmartBody::SBAnimationBlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	SmartBody::SBAnimationBlend0D* state0D = dynamic_cast<SmartBody::SBAnimationBlend0D*>(state);
	SmartBody::SBAnimationBlend1D* state1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(state);
	SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(state);
	SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(state);

	std::string stateNameVariable = "blend" + stateName;
	std::stringstream strstr;
	strstr << "# blend " << stateName << "\n";
	strstr << "# autogenerated by SmartBody\n";
	strstr << "\n";
	strstr << "blendManager = scene.getBlendManager()\n";
	strstr << "\n";
	strstr << "# align motions first if needed\n";
	for (int i = 0; i < state->getNumMotions(); i++)
	{
		SmartBody::SBMotion* motion = dynamic_cast<SmartBody::SBMotion*>(state->motions[i]);
		if (!motion)
			continue;
		int alignIndex = motion->getAlignIndex();
		if (alignIndex > 0)
		{
			strstr << "alignmotion = scene.getMotion(\"" << state->getMotionName(i) << "\")\n";
			strstr << "alignmotion.alignToBegin(" << alignIndex << ")\n";
		}
		else if (alignIndex < 0)
		{
			strstr << "alignmotion = scene.getMotion(\"" << state->getMotionName(i) << "\")\n";
			strstr << "alignmotion.alignToEnd(" << -alignIndex << ")\n";
		}
	}

	if (state0D || state1D || state2D || state3D)
	{
		strstr << "\n";
		// add the motions
		if (state0D)
		{
			strstr << stateNameVariable << " = blendManager.createBlend0D(\"" << stateName << "\")\n";		
		}
		if (state1D)
		{
			strstr << stateNameVariable << " = blendManager.createBlend1D(\"" << stateName << "\")\n";
		}
		else if (state2D)
		{
			strstr << stateNameVariable << " = blendManager.createBlend2D(\"" << stateName << "\")\n";
		}
		else if (state3D)
		{
			strstr << stateNameVariable << " = blendManager.createBlend3D(\"" << stateName << "\")\n";
		}

		strstr << "\n";
		strstr << "motions = StringVec()\n";
		for (int x = 0; x < state->getNumMotions(); x++)
		{
			strstr << "motions.append(\"" << state->getMotion(x) << "\")\n";
		}
		// add the parameters
		strstr << "\n";
		if (state1D || state2D || state3D)
		{
			strstr << "paramsX = DoubleVec()\n";
		}
		if (state2D || state3D)
		{
			strstr << "paramsY = DoubleVec()\n";
		}
		if (state3D)
		{
			strstr << "paramsZ = DoubleVec()\n";
		}
		
		for (int x = 0; x < state->getNumMotions(); x++)
		{
			double p1, p2, p3;
			if (state1D)
			{
				state->getParameter(state->getMotion(x), p1);
				strstr << "paramsX.append(" << p1 << ") # " << state->getMotion(x) << " X\n";
			}
			else if (state2D)
			{
				state->getParameter(state->getMotion(x), p1, p2);
				strstr << "paramsX.append(" << p1 << ") # " << state->getMotion(x) << " X\n";
				strstr << "paramsY.append(" << p2 << ") # " << state->getMotion(x) << " Y\n";
			}
			else if (state3D)
			{
				state->getParameter(state->getMotion(x), p1, p2, p3);
				strstr << "paramsX.append(" << p1 << ") # " << state->getMotion(x) << " X\n";
				strstr << "paramsY.append(" << p2 << ") # " << state->getMotion(x) << " Y\n";
				strstr << "paramsZ.append(" << p3 << ") # " << state->getMotion(x) << " Z\n";
			}
			
		}
		strstr << "for i in range(0, len(motions)):\n";
		if (state0D)
		{
			strstr << "\t" << stateNameVariable << ".addMotion(motions[i])\n";
		}
		else if (state1D)
		{
			strstr << "\t" << stateNameVariable << ".addMotion(motions[i], paramsX[i])\n";
		}
		else if (state2D)
		{
			strstr << "\t" << stateNameVariable << ".addMotion(motions[i], paramsX[i], paramsY[i])\n";
		}
		else if (state3D)
		{
			strstr << "\t" << stateNameVariable << ".addMotion(motions[i], paramsX[i], paramsY[i], paramsZ[i])\n";
		}
		// add the correspondence points
		strstr << "\n";
		for (int c = 0; c < state->getNumKeys(); c++)
		{
			strstr << "points" << c << " = DoubleVec()\n";
			for (int m = 0; m < state->getNumMotions(); m++)
			{
				strstr << "points" << c << ".append(" << state->keys[m][c] << ") # " << state->getMotion(m) << " " << c << "\n";
			}
			strstr << stateNameVariable << ".addCorrespondencePoints(motions, points" << c << ")\n";
		}

	}
	if (state2D)
	{
		// create the triangles
		strstr << "\n";
		std::vector<TriangleInfo>& triangleInfo = state->getTriangles();
		for (size_t t = 0; t < triangleInfo.size(); t++)
		{
			strstr << stateNameVariable << ".addTriangle(\"" << triangleInfo[t].motion1 << "\", \"" <<  triangleInfo[t].motion2 << "\", \"" <<  triangleInfo[t].motion3 << "\")\n"; 
		}
	}
	if (state3D)
	{
		// create the tetrahedrons
		strstr << "\n";
		std::vector<TetrahedronInfo>& tetrahedronInfo = state->getTetrahedrons();
		for (size_t t = 0; t < tetrahedronInfo.size(); t++)
		{
			strstr << stateNameVariable << ".addTetrahedron(\"" << tetrahedronInfo[t].motion1 << "\", \"" <<  tetrahedronInfo[t].motion2 << "\", \"" <<  tetrahedronInfo[t].motion3 << "\", \"" <<  tetrahedronInfo[t].motion4 << "\")\n"; 
		}
	}

	// save to the file
	std::ofstream stateFile(stateFileName.c_str());
	if (stateFile.is_open() != true)
	{
		fl_alert("Problem writing to file %s, blend was not saved.", stateFileName.c_str());
		return;
	}
	stateFile << strstr.str();
	stateFile.close();

	/*
	// output to the window
	Fl_Text_Display* display = editor->paWindow->textDisplay;	
	display->insert(strstr.str().c_str());
	display->insert("\n");
	display->redraw();
	display->show_insert_position();
	*/
}

void PABlendEditor::selectStateAnimations(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	std::vector<std::string> selectedMotions;
	std::vector<bool> boolSelectedMotions;
	for (int i = 0; i < editor->stateAnimationList->size(); i++)
	{
		if (editor->stateAnimationList->selected(i+1))
		{
			boolSelectedMotions.push_back(true);
			selectedMotions.push_back(editor->stateAnimationList->text(i + 1));
		}
		else
			boolSelectedMotions.push_back(false);
	}

	const char* stateText = editor->stateList->text();
	if (!stateText)
		return;

	PABlend* currentState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateText);
	
	if (!currentState)
		return;

	SmartBody::SBAnimationBlend0D* state0D = dynamic_cast<SmartBody::SBAnimationBlend0D*>(currentState);
	SmartBody::SBAnimationBlend1D* state1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(currentState);
	SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(currentState);
	SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(currentState);

	bool needsRedraw = false;
	int numTracks = editor->stateTimeMarkWidget->getModel()->getNumTracks();
	for (int t = 0; t < numTracks; t++)
	{
		nle::Track* track = editor->stateTimeMarkWidget->getModel()->getTrack(t);
		if (track->isSelected())
		{
			needsRedraw = true;
			track->setSelected(false);
		}
	}
	for (std::vector<std::string>::iterator iter = selectedMotions.begin();
		 iter != selectedMotions.end();
		 iter++)
	{
		nle::Track* track = editor->stateTimeMarkWidget->getModel()->getTrack(*iter);
		if (!track->isSelected())
		{
			needsRedraw = true;
			track->setSelected(true);
		}		
	}

	if (selectedMotions.size() == 1)
	{
		editor->buttonPlay->activate();
		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(selectedMotions[0]);
		if (motion)
		{
			int index = currentState->getMotionId(motion->getName());
			editor->sliderScrub->range(0, motion->duration());
			if (editor->lastSelectedMotion != "")
			{
				int lastMotionIndex = currentState->getMotionId(editor->lastSelectedMotion);
				double curTime = editor->sliderScrub->value();
				double localTime = currentState-> getLocalTime(curTime, lastMotionIndex);
				std::vector<double> weights(currentState->getNumMotions());
				for (size_t x = 0; x < weights.size(); x++)
				{
					weights[x] = 0.;
				}
				if (lastMotionIndex > -1)
				{
					weights[lastMotionIndex] = 1.;
				}
				PABlendData blendData(currentState, weights);
				blendData.timeManager->updateWeights();
				std::vector<double> times(blendData.state->getNumMotions());
				blendData.timeManager->getParallelTimes(localTime, times);
				editor->stateTimeMarkWidget->setLocalTimes(times);

				
				if (index > -1)
				{
					double newScrubTime = currentState->getMotionTime(times[index], index);
					editor->sliderScrub->value(newScrubTime);
					editor->updateMotionPlayer(newScrubTime);
					//scrub(editor->sliderScrub, editor);
				}
			}
			if (editor->isPlaying)
			{
				editor->sliderScrub->activate();
				editor->buttonPlay->label("@square");
				editor->stateTimeMarkWidget->setShowScrubLine(true);
			}
			else
			{
				editor->sliderScrub->deactivate();
				editor->buttonPlay->label("@>");
				editor->stateTimeMarkWidget->setShowScrubLine(false);
			}
			// if a correspondence point was selected on the last motion track, select the 
			// equivalent point on the new motion track
			int selectedIndex;
			nle::Mark* mark = editor->stateTimeMarkWidget->getSelectedCorrespondancePointIndex(selectedIndex);
			if (mark)
			{
				mark->setSelected(false);
				nle::Track* nextTrack = editor->stateEditorNleModel->getTrack(index);
				nle::Block* nextBlock = nextTrack->getBlock(0);
				nle::Mark* nextMark = nextBlock->getMark(selectedIndex);
				nextMark->setSelected(true);
				editor->stateTimeMarkWidget->setup();
				editor->stateTimeMarkWidget->redraw();
			}
			editor->lastSelectedMotion = motion->getName();
		}
		
		char buff[32];
		if (state0D)
		{
			editor->inputParameterX->deactivate();
			editor->inputParameterY->deactivate();
			editor->inputParameterZ->deactivate();
		}
		else if (state1D)
		{
			double x;
			state1D->getParameter(selectedMotions[0], x); 
			sprintf(buff, "%.2f", x);
			editor->inputParameterX->value(buff);

			editor->inputParameterX->activate();
			editor->inputParameterY->deactivate();
			editor->inputParameterZ->deactivate();
		}
		else if (state2D)
		{
			double x, y;
			state2D->getParameter(selectedMotions[0], x, y); 
			sprintf(buff, "%.2f", x);
			editor->inputParameterX->value(buff);
			sprintf(buff, "%.2f", y);
			editor->inputParameterY->value(buff);

			editor->inputParameterX->activate();
			editor->inputParameterY->activate();
			editor->inputParameterZ->deactivate();
		}
		else if (state3D)
		{
			double x, y, z;
			state3D->getParameter(selectedMotions[0], x, y, z); 
			sprintf(buff, "%.2f", x);
			editor->inputParameterX->value(buff);
			sprintf(buff, "%.2f", y);
			editor->inputParameterY->value(buff);
			sprintf(buff, "%.2f", z);
			editor->inputParameterZ->value(buff);

			editor->inputParameterX->activate();
			editor->inputParameterY->activate();
			editor->inputParameterZ->activate();
		}
	}
	else
	{
		editor->buttonPlay->deactivate();
		editor->sliderScrub->deactivate();
		
		editor->inputParameterX->value(0);
		editor->inputParameterY->value(0);
		editor->inputParameterZ->value(0);
		editor->inputParameterX->deactivate();
		editor->inputParameterY->deactivate();
		editor->inputParameterZ->deactivate();
	}

	if (selectedMotions.size() > 0 && !state0D)
	{
		editor->buttonEditParameter->activate();
		editor->buttonAutoFootSetpsEditor->activate();
		editor->buttonAlignLeft->activate();
		editor->buttonAlignRight->activate();
		editor->buttonAlignRecover->activate();
	}
	else
	{
		editor->buttonEditParameter->deactivate();
		editor->buttonAutoFootSetpsEditor->deactivate();
		editor->buttonAlignLeft->deactivate();
		editor->buttonAlignRight->deactivate();
		editor->buttonAlignRecover->deactivate();
	}

	if ((selectedMotions.size() == 3 && state2D) ||
		(selectedMotions.size() == 4 && state3D))
	
	{
		editor->shapeAdd->activate();
	}
	else
	{
		editor->shapeAdd->deactivate();
	}
	if (editor->triangleVisualization)
	{
		editor->triangleVisualization->setSelectedParameters(boolSelectedMotions);
	}
	if (editor->tetraVisualization)
	{
		editor->tetraVisualization->setSelectedParameters(boolSelectedMotions);
	}

	if (needsRedraw)
	{
		editor->stateTimeMarkWidget->redraw();
	}
}

void PABlendEditor::addShape(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;
	const char* stateText = editor->stateList->text();
	PABlend* currentState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateText);
	
	if (!currentState)
		return;

	std::vector<std::string> selectedMotions;
	for (int i = 0; i < editor->stateAnimationList->size(); i++)
	{
		if (editor->stateAnimationList->selected(i+1))
		{
			selectedMotions.push_back(editor->stateAnimationList->text(i + 1));
		}
	}

	SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(currentState);
	SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(currentState);
	if (state2D && selectedMotions.size() == 3)
	{
		state2D->addTriangle(selectedMotions[0], selectedMotions[1], selectedMotions[2]);
	}
	if (state3D && selectedMotions.size() == 4)
	{
		state3D->addTetrahedron(selectedMotions[0], selectedMotions[1], selectedMotions[2], selectedMotions[3]);
	}
	editor->refresh();
}

void PABlendEditor::removeShape(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;
	const char* stateText = editor->stateList->text();
	PABlend* currentState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateText);
	
	if (!currentState)
		return;

	std::vector<std::string> selectedShapes;
	for (int i = 0; i < editor->shapeList->size(); i++)
	{
		if (editor->shapeList->selected(i + 1))
		{
			selectedShapes.push_back(editor->shapeList->text(i + 1));
			editor->shapeList->remove(i + 1);
			i--;
		}
	}

	for (size_t i = 0; i < selectedShapes.size(); i++)
	{
		// remove from state
		std::vector<std::string> motions;
		vhcl::Tokenize(selectedShapes[i], motions, "|");		
		SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(currentState);
		SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(currentState);
		if (state2D && motions.size() == 3)
		{
			state2D->removeTriangle(motions[0], motions[1], motions[2]);
		}
		if (state3D && motions.size() == 4)
		{
			state3D->removeTetrahedron(motions[0], motions[1], motions[2], motions[3]);
		}	
	}
	editor->refresh();
}

void PABlendEditor::selectShape(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;
	std::vector<std::string> selectedShapes;
	std::vector<bool> highlightShapes;
	for (int i = 0; i < editor->shapeList->size(); i++)
	{
		highlightShapes.push_back(false);
	}
	for (int i = 0; i < editor->shapeList->size(); i++)
	{
		if (editor->shapeList->selected(i + 1))
		{
			selectedShapes.push_back(editor->shapeList->text(i + 1));
			highlightShapes[i] = true;
		}
	}

	if (selectedShapes.size() > 0)
	{
		editor->shapeRemove->activate();
	}
	else
	{
		editor->shapeRemove->deactivate();
	}

	if (editor->triangleVisualization)
		editor->triangleVisualization->setSelectedTriangles(highlightShapes);
	if (editor->tetraVisualization)
		editor->tetraVisualization->setSelectedTetrahedrons(highlightShapes);
}

void PABlendEditor::updateParameters(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;
	const std::vector<std::string>& selectedMotions = editor->getSelectedMotions();
	PABlend* currentState = editor->getCurrentState();
	if (!currentState)
		return;


	SmartBody::SBAnimationBlend0D* state0D = dynamic_cast<SmartBody::SBAnimationBlend0D*>(currentState);
	SmartBody::SBAnimationBlend1D* state1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(currentState);
	SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(currentState);
	SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(currentState);
	
	if (state0D)
	{
		editor->inputParameterX->deactivate();
		editor->inputParameterY->deactivate();
		editor->inputParameterZ->deactivate();
	}
	else if (state1D)
	{
		state1D->setParameter(selectedMotions[0], (float) atof(editor->inputParameterX->value())); 
	}
	else if (state2D)
	{
		state2D->setParameter(selectedMotions[0], (float) atof(editor->inputParameterX->value()), (float) atof(editor->inputParameterY->value())); 
	}
	else if (state3D)
	{
		state3D->setParameter(selectedMotions[0], (float) atof(editor->inputParameterX->value()), (float) atof(editor->inputParameterY->value()), (float) atof(editor->inputParameterZ->value())); 
	}
}

void PABlendEditor::editParameterCb(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;
	if (!editor->parameterEditor)
	{
		editor->parameterEditor = new PAParameterEditor(editor, editor->paWindow->x() + 50, editor->paWindow->y() + 50, 300, 300);
	}
	editor->parameterEditor->show();	
}

void PABlendEditor::scrub(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;
	const std::vector<std::string>& selectedMotions = editor->getSelectedMotions();

	if (selectedMotions.size() == 1)
	{
		std::string currentStateName = editor->stateList->menu()[editor->stateList->value()].label();

		PABlend* currentState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(currentStateName);
	
		int lastMotionIndex = currentState->getMotionId(editor->lastSelectedMotion);
		double curTime = editor->sliderScrub->value();
		double localTime = currentState->getLocalTime(curTime, lastMotionIndex);
		std::vector<double> weights(currentState->getNumMotions());
		for (size_t x = 0; x < weights.size(); x++)
		{
			weights[x] = 0.;
		}
		if (lastMotionIndex > -1)
		{
			weights[lastMotionIndex] = 1.;
		}
		PABlendData blendData(currentState, weights);
		blendData.timeManager->updateWeights();
		std::vector<double> times(blendData.state->getNumMotions());
		blendData.timeManager->getParallelTimes(localTime, times);
		editor->stateTimeMarkWidget->setLocalTimes(times);

		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(selectedMotions[0]);
		double time = editor->sliderScrub->value();
		double delta = motion->duration() / double(motion->frames() - 1);
		int frameNumber = int(time / delta);
		std::string charName = editor->paWindow->characterList->menu()[editor->paWindow->characterList->value()].label();
		std::stringstream command;
		command << "motionplayer " << charName << " " << selectedMotions[0] << " " << frameNumber;
		SmartBody::SBScene::getScene()->command(command.str());
		editor->stateTimeMarkWidget->redraw();
	}
	
}


void PABlendEditor::updateMotionPlayer(double t)
{
	const std::vector<std::string>& selectedMotions = getSelectedMotions();

	if (selectedMotions.size() == 1)
	{
		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(selectedMotions[0]);
		if (!motion)
			return;

		int frameIndex = int(t * motion->getFrameRate());
		if (!paWindow->characterList->menu())
			return;

		std::string charName = paWindow->characterList->menu()[paWindow->characterList->value()].label();
		std::stringstream command;
		command << "motionplayer " << charName << " " << selectedMotions[0] << " " << frameIndex;
		SmartBody::SBScene::getScene()->command(command.str());
	}
}

void PABlendEditor::playmotion(Fl_Widget* widget, void* data)
{
	PABlendEditor* editor = (PABlendEditor*) data;

	PABlendEditor::selectStateAnimations(widget, data);

	if (editor->paWindow->characterList->size() == 0)
		return;

	std::string charName = editor->paWindow->characterList->menu()[editor->paWindow->characterList->value()].label();

	std::stringstream command;
	if (editor->isPlaying == false)
	{
		editor->isPlaying = true;
		editor->buttonPlay->label("@square");
		editor->sliderScrub->activate();
		command << "motionplayer " << charName << " on";
		editor->stateTimeMarkWidget->setShowScrubLine(true);
	}
	else
	{
		editor->isPlaying = false;
		editor->buttonPlay->label("@>");
		editor->sliderScrub->deactivate();
		command << "motionplayer " << charName << " off";
		editor->stateTimeMarkWidget->setShowScrubLine(false);
	}
	SmartBody::SBScene::getScene()->command(command.str());
}




