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
#include "ParamAnimTransitionEditor2.h"
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Hold_Browser.H>
#include <sb/SBScene.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBMotion.h>

#include "ParamAnimBlock.h"
#include "ParamAnimTransitionCreatorWidget.h"
#include "ParameterVisualization.h"
#include "Parameter3DVisualization.h"
#include "ParamAnimEditorWidget.h"
#include "RootWindow.h"


PATransitionEditor2::PATransitionEditor2(int x, int y, int w, int h, PanimationWindow* window) : Fl_Group(x, y, w, h), paWindow(window)
{
	precisionCompensate = .0001;
	lastNameIndex = 0;
	isPlaying = false;
	curTransition = NULL;

	this->label("Transition Editor2");
	this->begin();

		int csx = xDis + x;
		int csy = yDis + y;
		int workspaceWidth = w - 2 * xDis;
		int workspaceHeight = h - csy;
		stateSelectionGroup = new Fl_Group(csx, csy, workspaceWidth, workspaceHeight / 3, "");
		int stateSelectionGroupW = w - 2 * xDis;
		int stateSelectionGroupH = h /2 - 3 * yDis;
		stateSelectionGroup->begin();
			state1List = new Fl_Choice(csx + 50, csy + 5, 150, 20, "State 1");
//			state1List->when(FL_WHEN_ENTER_KEY);
			state1List->callback(changeState1List, this);

			createTransitionButton = new Fl_Button(csx + 270, csy + 5,100, 20, "Create");
			createTransitionButton->callback(editStateMotions, this);
			
			state1AnimationList = new Fl_Hold_Browser(csx + 50, csy + 40, 290, 100, "State 1 Motions");
			state1AnimationList->align(FL_ALIGN_TOP);
			state1AnimationList->when(FL_WHEN_CHANGED);
			state1AnimationList->callback(selectState1Animations, this);

			state2List = new Fl_Choice(csx + 440, csy + 5, 150, 20, "State 2");
//			state1List->when(FL_WHEN_ENTER_KEY);
			state2List->callback(changeState2List, this);

			state2AnimationList = new Fl_Hold_Browser(csx + 440, csy + 40, 290, 100, "State 2 Motions");
			state2AnimationList->align(FL_ALIGN_TOP);
			state2AnimationList->when(FL_WHEN_CHANGED);
			state2AnimationList->callback(selectState2Animations, this);


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
		int esy = h / 3  + yDis + y;
		Fl_Group* buttonGroup = new Fl_Group(esx, esy, w - 2 * xDis, 3 * yDis);
		buttonGroup->begin();
			addMark = new Fl_Button(xDis + esx, yDis + esy, 50, 2 * yDis, "+");
			addMark->callback(addTransitionTimeMark, this);
			removeMark = new Fl_Button(xDis + 50 + esx, yDis + esy, 50, 2 * yDis, "-");
			removeMark->callback(removeTransitionTimeMark, this);
			snapMark = new Fl_Button(xDis + 100 + esx, yDis + esy, 50, 2 * yDis, "Snap");
			snapMark->callback(snapTimeMark, this);
			snapStartMark = new Fl_Button(xDis + 150 + esx, yDis + esy, 50, 2 * yDis, "@|< Snap");
			snapStartMark->callback(snapStartTimeMark, this);
			snapEndMark = new Fl_Button(xDis + 200 + esx, yDis + esy, 50, 2 * yDis, "Snap @>|");
			snapEndMark->callback(snapEndTimeMark, this);
			buttonSave = new Fl_Button(xDis + 300 + esx, yDis + esy, 100, 2 * yDis, "Save");
			buttonSave->callback(save, this);
			minTimeInput = new Fl_Float_Input(xDis + 580 + esx, yDis + esy, 60, 2 * yDis, "Show Times");
			minTimeInput->callback(updateMinTime, this);
			maxTimeInput = new Fl_Float_Input(xDis + 640 + esx, yDis + esy, 60, 2 * yDis, "");
			maxTimeInput->callback(updateMaxTime, this);
		buttonGroup->end();

		editTransitionTimeMarkGroup = new Fl_Scroll(csx, esy + 3 * yDis + 10, workspaceWidth, 2 * workspaceHeight / 3 - 5 * yDis - 10);
		editTransitionTimeMarkGroup->type(Fl_Scroll::VERTICAL_ALWAYS);
		editTransitionTimeMarkGroup->begin();
			transitionTimeMarkWidget = new ParamAnimEditorWidget(this, csx + 10, 5 * yDis + esy, workspaceWidth - 10, 2 * workspaceHeight / 3 - 7 * yDis, (char*) "");
		editTransitionTimeMarkGroup->end();
		editTransitionTimeMarkGroup->resizable(transitionTimeMarkWidget);
		editTransitionTimeMarkGroup->box(FL_BORDER_BOX);
		this->resizable(editTransitionTimeMarkGroup);
		
	this->end();

	transitionEditorNleModel = new nle::NonLinearEditorModel();
	transitionTimeMarkWidget->setModel(transitionEditorNleModel);
	transitionEditorNleModel->addModelListener(transitionTimeMarkWidget);
	transitionTimeMarkWidget->setup();

	ParamAnimTrack* track1 = new ParamAnimTrack();
	track1->setName("transition1");
	ParamAnimBlock* block1 = new ParamAnimBlock();
	track1->addBlock(block1);
	transitionEditorNleModel->addTrack(track1);

	ParamAnimTrack* track2 = new ParamAnimTrack();
	track2->setName("transition2");
	ParamAnimBlock* block2 = new ParamAnimBlock();
	track2->addBlock(block2);
	transitionEditorNleModel->addTrack(track2);

	creator = NULL;
	lastSelectedMotion = "";

	loadStates();
}

PATransitionEditor2::~PATransitionEditor2()
{
}

void PATransitionEditor2::loadStates()
{
	state1List->clear();
	state1List->add("---");
	// states may have names that conflict with FLTK's parsing, such as a '@'
	std::vector<std::string> blendNames = SmartBody::SBScene::getScene()->getBlendManager()->getBlendNames();
	for (std::vector<std::string>::iterator iter = blendNames.begin();
		 iter != blendNames.end();
		 iter++)
	{
		state1List->add((*iter).c_str());
	}
	
	state1List->value(0);

	state2List->clear();
	state2List->add("---");
	// states may have names that conflict with FLTK's parsing, such as a '@'
	
	for (std::vector<std::string>::iterator iter = blendNames.begin();
		 iter != blendNames.end();
		 iter++)
	{
		state2List->add((*iter).c_str());
	}
	
	state2List->value(0);
}

void PATransitionEditor2::updateTransitionTimeMarkEditor(Fl_Widget* widget, void* data, bool toAdd)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;

	if (editor->state1AnimationList->size() == 0)
		editor->transitionEditorNleModel->removeAllTracks();

	if (toAdd)
	{
		for (int i = 0; i < editor->state1AnimationList->size(); i++)
		{
			std::string motionName = editor->state1AnimationList->text(i+1);//text(i+1);
			nle::Track* track = editor->transitionEditorNleModel->getTrack(motionName);
			if (!track && toAdd)
			{
				ParamAnimTrack* newTrack = new ParamAnimTrack();
				newTrack->setName(motionName.c_str());
				SmartBody::SBMotion * motion = SmartBody::SBScene::getScene()->getMotion(motionName);
				ParamAnimBlock* block = new ParamAnimBlock();
				block->setName(motionName.c_str());
				block->setStartTime(0);
				block->setEndTime(motion->duration());
				editor->transitionEditorNleModel->addTrack(newTrack);
				newTrack->addBlock(block);		
				editor->transitionEditorNleModel->update();
				editor->transitionTimeMarkWidget->setViewableTimeEnd(editor->transitionEditorNleModel->getEndTime());
			}
		}
	}
	else
	{
		std::string motionName;
		for (int i = 0; i < editor->transitionEditorNleModel->getNumTracks(); i++)
		{
			bool del = true;
			for (int j = 0; j < editor->state1AnimationList->size(); j++)
			{
				std::string motionName = editor->state1AnimationList->text(j + 1);
				if (editor->transitionEditorNleModel->getTrack(i)->getName() == motionName)
				{
					del = false;
					break;
				}
			}
			if (del)
			{
				editor->transitionEditorNleModel->removeTrack(editor->transitionEditorNleModel->getTrack(i)->getName());
				i--;
			}
		}
	}
	editor->paWindow->redraw();
}

void PATransitionEditor2::editStateMotions(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;

	bool isCreateMode = true;
	std::string stateName = "";
	if (std::string("Edit State") == editor->createTransitionButton->label())
	{
		isCreateMode = false;
		stateName = editor->state1List->menu()[editor->state1List->value()].label();
	}
	if (!editor->creator)
	{
		editor->creator = new PATransitionCreator(editor, editor->paWindow->x() + 50, editor->paWindow->y() + 50, 800, 600);
	}
	editor->creator->setInfo(isCreateMode, stateName);
	
	editor->creator->show();
}

void PATransitionEditor2::changeState1List(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;
	int stateValue = editor->state1List->value();
	int stateValueP = editor->state2List->value();
	if (stateValue < 0 || stateValueP < 0)
		return;

	editor->loadStates();	
	editor->state1List->value(stateValue);
	editor->state2List->value(stateValueP);

	SmartBody::SBAnimationBlend* state1 = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(editor->state1List->text(stateValue));
	
	editor->state1AnimationList->clear();
	if (state1)
	{
		for (int i = 0; i < state1->getNumMotions(); i++)
			editor->state1AnimationList->add(state1->motions[i]->getName().c_str());
	}
	for (int i = 0; i < editor->state1AnimationList->size(); i++)
		editor->state1AnimationList->select(i, false);

	// get any existing transition for the state list
	SmartBody::SBAnimationBlend* state2 = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(editor->state2List->text(stateValueP));
	
	if (state1 && state2)
	{
		// find any transition from state1 to state2
		SmartBody::SBAnimationBlendManager* manager = SmartBody::SBScene::getScene()->getBlendManager();
		SmartBody::SBAnimationTransition* transition = manager->getTransition(state1->stateName, state2->stateName);
		if (transition)
		{
			editor->curTransition = transition;
			// select the appropriate motions
			const std::string& sourceMotionName = transition->getSourceMotionName();
			const std::string& destinationMotionName = transition->getDestinationMotionName();
			if (sourceMotionName != "")
			{
				int motionId = transition->getSourceBlend()->getMotionId(sourceMotionName);
				if (motionId != -1)
				{
					editor->state1AnimationList->select(motionId + 1);
				}
			}
			if (destinationMotionName != "")
			{
				int motionId = transition->getDestinationBlend()->getMotionId(destinationMotionName);
				if (motionId != -1)
				{
					editor->state2AnimationList->select(motionId + 1);
				}
			}
		}
		else
		{
			editor->curTransition = NULL;
		}
	}
	editor->updateIntervalMarks(editor->curTransition);
	editor->transitionTimeMarkWidget->setup();
	editor->editTransitionTimeMarkGroup->scroll_to(0, 0);
	editor->paWindow->redraw();
}

void PATransitionEditor2::changeState2List(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;
	int stateValue = editor->state2List->value();
	int stateValueP = editor->state1List->value();
	if (stateValue < 0 || stateValueP < 0)
		return;

	editor->loadStates();
	SmartBody::SBAnimationBlend* state2 = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(editor->state2List->text(stateValue));
	editor->state2List->value(stateValue);
	editor->state1List->value(stateValueP);
	editor->state2AnimationList->clear();
	if (state2)
	{
		for (int i = 0; i < state2->getNumMotions(); i++)
			editor->state2AnimationList->add(state2->motions[i]->getName().c_str());
	}
	for (int i = 0; i < editor->state2AnimationList->size(); i++)
		editor->state2AnimationList->select(i, false);

	// get any existing transition for the state list
	SmartBody::SBAnimationBlend* state1 = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(editor->state1List->text(stateValueP));
	
	if (state1 && state2)
	{
		// find any transition from state1 to state2
		SmartBody::SBAnimationBlendManager* manager = SmartBody::SBScene::getScene()->getBlendManager();
		SmartBody::SBAnimationTransition* transition = manager->getTransition(state1->stateName, state2->stateName);
		if (transition)
		{
			editor->curTransition = transition;
			// select the appropriate motions
			const std::string& sourceMotionName = transition->getSourceMotionName();
			const std::string& destinationMotionName = transition->getDestinationMotionName();
			if (sourceMotionName != "")
			{
				int motionId = transition->getSourceBlend()->getMotionId(sourceMotionName);
				if (motionId != -1)
				{
					editor->state1AnimationList->select(motionId + 1);
				}
			}
			if (destinationMotionName != "")
			{
				int motionId = transition->getDestinationBlend()->getMotionId(destinationMotionName);
				if (motionId != -1)
				{
					editor->state2AnimationList->select(motionId + 1);
				}
			}
		}
		else
		{
			editor->curTransition = NULL;
		}
	}
	editor->updateIntervalMarks(editor->curTransition);
	editor->transitionTimeMarkWidget->setup();
	editor->editTransitionTimeMarkGroup->scroll_to(0, 0);
	editor->paWindow->redraw();
}

void PATransitionEditor2::updateMaxTime(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;
	editor->transitionTimeMarkWidget->setViewableTimeEnd(atof(editor->maxTimeInput->value()));
	editor->paWindow->redraw();
}

void PATransitionEditor2::updateMinTime(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2* ) data;
	editor->transitionTimeMarkWidget->setViewableTimeStart(atof(editor->minTimeInput->value()));
	editor->paWindow->redraw();
}



void PATransitionEditor2::addTransitionTimeMark(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;

	if (!editor->curTransition)
		return;
	
	std::vector<double> localTimes = editor->transitionTimeMarkWidget->getLocalTimes();

/*	state->addCorrespondencePoints(motionNames, localTimes);

	editor->updateCorrespondenceMarks(editor->curTransition);
	
//	editor->paWindow->addTimeMark(editor->transitionEditorNleModel);
	editor->transitionTimeMarkWidget->setup();
	editor->selectState1Animations(editor->state1AnimationList, editor);
	editor->scrub(editor->sliderScrub, editor);
	editor->paWindow->redraw();
*/
}

void PATransitionEditor2::removeTransitionTimeMark(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;

	// determine where to add the time marks
	std::string stateName = editor->state1List->text();

	SmartBody::SBAnimationBlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	if (!state)
		return;

	// which correspondence point has been selected?
	int keyIndex = -1;
	CorrespondenceMark* attachedMark = NULL;
	for (int t = 0; t < editor->transitionEditorNleModel->getNumTracks(); t++)
	{
		nle::Track* track = editor->transitionEditorNleModel->getTrack(t);
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
		editor->updateIntervalMarks(editor->curTransition);

		editor->transitionTimeMarkWidget->setup();
		editor->selectState1Animations(editor->state1AnimationList, editor);
		editor->scrub(editor->sliderScrub, editor);
		editor->paWindow->redraw();
	}

	
}

void PATransitionEditor2::snapTimeMark(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;

	// determine where to add the time marks
	std::string stateName = editor->state1List->text();

	SmartBody::SBAnimationBlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	if (!state)
		return;

	// which correspondence point has been selected?
	int keyIndex = -1;
	CorrespondenceMark* attachedMark = NULL;
	int motionIndex = -1;
	for (int t = 0; t < editor->transitionEditorNleModel->getNumTracks(); t++)
	{
		nle::Track* track = editor->transitionEditorNleModel->getTrack(t);
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
		std::vector<double> localTimes = editor->transitionTimeMarkWidget->getLocalTimes();
		state->setCorrespondencePoints(motionIndex, keyIndex, localTimes[motionIndex]);
		editor->updateIntervalMarks(editor->curTransition);

		editor->transitionTimeMarkWidget->setup();
		editor->selectState1Animations(editor->state1AnimationList, editor);
		// reselect the equivalent mark
		editor->transitionTimeMarkWidget->getModel()->getTrack(motionIndex)->getBlock(0)->getMark(keyIndex)->setSelected(true);

		editor->sliderScrub->value(localTimes[motionIndex]);
		editor->scrub(editor->sliderScrub, editor);
		editor->paWindow->redraw();
	}
}

void PATransitionEditor2::snapStartTimeMark(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;

	std::string stateName = editor->state1List->text();

	if (!editor->curTransition)
		return;

	// which correspondence point has been selected?
	int keyIndex = -1;
	CorrespondenceMark* attachedMark = NULL;
	int motionIndex = -1;
	for (int t = 0; t < editor->transitionEditorNleModel->getNumTracks(); t++)
	{
		nle::Track* track = editor->transitionEditorNleModel->getTrack(t);
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

	/*
	if (keyIndex > -1 && keyIndex < state->getNumKeys())
	{
		// get the local times
		std::vector<double> localTimes = editor->transitionTimeMarkWidget->getLocalTimes();
		state->setCorrespondencePoints(motionIndex, keyIndex, 0.0);
		editor->updateCorrespondenceMarks(state);

		editor->transitionTimeMarkWidget->setup();
		editor->selectState1Animations(editor->state1AnimationList, editor);
		// reselect the equivalent mark
		editor->transitionTimeMarkWidget->getModel()->getTrack(motionIndex)->getBlock(0)->getMark(keyIndex)->setSelected(true);

		editor->sliderScrub->value(0.0);
		editor->scrub(editor->sliderScrub, editor);
		editor->paWindow->redraw();
	}
	*/
}

void PATransitionEditor2::snapEndTimeMark(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;

	std::string stateName = editor->state1List->text();

	SmartBody::SBAnimationBlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	if (!state)
		return;

	// which interval point has been selected?
	int keyIndex = -1;
	CorrespondenceMark* attachedMark = NULL;
	int motionIndex = -1;
	for (int t = 0; t < editor->transitionEditorNleModel->getNumTracks(); t++)
	{
		nle::Track* track = editor->transitionEditorNleModel->getTrack(t);
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

	/*
	if (keyIndex > -1 && keyIndex < state->getNumKeys())
	{
		// get the local times
		std::vector<double> localTimes = editor->transitionTimeMarkWidget->getLocalTimes();
		// get the end time of the motion
		double endTime = SmartBody::SBScene::getScene()->getMotion(state->getMotion(motionIndex))->duration();
		state->setCorrespondencePoints(motionIndex, keyIndex, endTime);
		editor->updateCorrespondenceMarks(state);

		editor->transitionTimeMarkWidget->setup();
		editor->selectState1Animations(editor->state1AnimationList, editor);
		// reselect the equivalent mark
		editor->transitionTimeMarkWidget->getModel()->getTrack(motionIndex)->getBlock(0)->getMark(keyIndex)->setSelected(true);

		editor->sliderScrub->value(endTime);
		editor->scrub(editor->sliderScrub, editor);
		editor->paWindow->redraw();
	}
	*/
}

void PATransitionEditor2::updateIntervalMarks(SmartBody::SBAnimationTransition* transition)
{
	for (int i = 0; i < transitionEditorNleModel->getNumTracks(); i++)
	{
		nle::Track* track = transitionEditorNleModel->getTrack(i);
		nle::Block* block = track->getBlock(0);
		block->removeAllMarks();
	}

	if (transition)
	{
		nle::Track* track1 = transitionEditorNleModel->getTrack(0);
		nle::Block* block1 = track1->getBlock(0);

		nle::Track* track2 = transitionEditorNleModel->getTrack(1);
		nle::Block* block2 = track2->getBlock(0);

		std::string sourceMotion = transition->getSourceMotionName();
		SmartBody::SBMotion* motion1 = SmartBody::SBScene::getScene()->getMotion(sourceMotion);
		SmartBody::SBMotion* motion2 = SmartBody::SBScene::getScene()->getMotion(sourceMotion);
		
		if (motion1)
		{
			block1->setStartTime(0);
			block1->setEndTime(motion1->duration());
		}
		if (motion2)
		{
			block2->setStartTime(0);
			block2->setEndTime(motion2->duration());
		}

		// get the ease-in points
		IntervalMark* easeOutMark = new IntervalMark();
		easeOutMark->setStartTime(transition->getEaseInStart());
		easeOutMark->setEndTime(transition->getEaseInEnd());
		block2->addMark(easeOutMark);

		for (int i = 0; i < transition->getNumEaseOutIntervals(); i++)
		{
			IntervalMark* easeInMark = new IntervalMark();
			std::vector<double> intervals = transition->getEaseOutInterval(i);
			easeInMark->setStartTime(intervals[0]);
			easeInMark->setEndTime(intervals[1]);
			block1->addMark(easeInMark);
		}
	}
/*	
	for (int i = 0; i < state->getNumMotions(); i++)
	{
		nle::Track* track = transitionEditorNleModel->getTrack(i);
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

	for (int i = 0; i < transitionEditorNleModel->getNumTracks() - 1; i++)
	{
		nle::Track* track1 = transitionEditorNleModel->getTrack(i);
		nle::Track* track2 = transitionEditorNleModel->getTrack(i + 1);
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
	*/
}

void PATransitionEditor2::refresh()
{
	int origStateValue = state1List->value();
	transitionEditorNleModel->removeAllTracks();
	transitionTimeMarkWidget->setup();
	state1AnimationList->clear();
	loadStates();
	state1List->value(origStateValue);
	changeState1List(state1List, this);
}

void PATransitionEditor2::save(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;
	
	if (!editor->curTransition)
	{
		fl_alert("No transition, cannot save.");
		return;
	}

	std::string mediaPath = SmartBody::SBScene::getScene()->getMediaPath();

	std::string transitionFileName = BaseWindow::chooseFile("Transition File:", "Python\t*.py\n", mediaPath);
	if (transitionFileName == "")
		return;

	std::string state1Name = editor->state1List->text();
	std::string state2Name = editor->state2List->text();

	std::string transitionNameVariable = "transition" + state1Name + "To" + state2Name;
	std::stringstream strstr;
	strstr << "# transition " << transitionNameVariable << "\n";
	strstr << "# autogenerated by SmartBody\n";
	strstr << "\n";
	strstr << "stateManager = scene.getStateManager()\n";
	// create the transition
	strstr << "\n";
	strstr << transitionNameVariable << " = stateManager.createTransition(\"" << state1Name << "\", \"state2Name\")\n";
	strstr << transitionNameVariable << ".setEaseInInterval(\"" << editor->curTransition->getDestinationMotionName() << "\", " << editor->curTransition->getEaseInStart() << ", " << editor->curTransition->getEaseInEnd() << ")\n";
	strstr << "\n";
	for (int x = 0; x < editor->curTransition->getNumEaseOutIntervals(); x++)
	{
		std::vector<double> interval = editor->curTransition->getEaseOutInterval(x);
		strstr << transitionNameVariable << ".addEaseOutInterval(\"" << editor->curTransition->getSourceMotionName() << "\", " << interval[0] << ", " << interval[1] << ")\n";
	}

	// save to the file
	std::ofstream transitionFile(transitionFileName.c_str());
	if (transitionFile.is_open() != true)
	{
		fl_alert("Problem writing to file %s, transition was not saved.", transitionFileName.c_str());
		return;
	}
	transitionFile << strstr.str();
	transitionFile.close();

	/*
	// output to the window
	Fl_Text_Display* display = editor->paWindow->textDisplay;	
	display->insert(strstr.str().c_str());
	display->insert("\n");
	display->redraw();
	display->show_insert_position();
	*/
}


void PATransitionEditor2::selectState1Animations(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;
	std::string motionName1 = "";
	for (int i = 0; i < editor->state1AnimationList->size(); i++)
	{
		if (editor->state1AnimationList->selected(i+1))
		{
			motionName1 = editor->state1AnimationList->text(i+1);
			break;
		}
	}
	std::string motionName2 = "";
	for (int i = 0; i < editor->state2AnimationList->size(); i++)
	{
		if (editor->state2AnimationList->selected(i+1))
		{
			motionName2 = editor->state2AnimationList->text(i+1);
			break;
		}
	}
	if (motionName1 != "")
	{
		SmartBody::SBMotion * motion = SmartBody::SBScene::getScene()->getMotion(motionName1);
		nle::Block* block = editor->transitionEditorNleModel->getTrack(0)->getBlock(0);
		block->removeAllMarks();
		block->setName(motionName1);
		block->setStartTime(0);
		block->setEndTime(motion->duration() - editor->precisionCompensate);
	}
	if (motionName2 != "")
	{
		SmartBody::SBMotion * motion = SmartBody::SBScene::getScene()->getMotion(motionName2);
		nle::Block* block = editor->transitionEditorNleModel->getTrack(1)->getBlock(0);
		block->removeAllMarks();
		block->setName(motionName2);
		block->setStartTime(0);
		block->setEndTime(motion->duration() - editor->precisionCompensate);
	}

	editor->transitionTimeMarkWidget->setup();
	editor->paWindow->redraw();
}

void PATransitionEditor2::selectState2Animations(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;
	std::string motionName1 = "";
	for (int i = 0; i < editor->state1AnimationList->size(); i++)
	{
		if (editor->state1AnimationList->selected(i+1))
		{
			motionName1 = editor->state1AnimationList->text(i+1);
			break;
		}
	}
	std::string motionName2 = "";
	for (int i = 0; i < editor->state2AnimationList->size(); i++)
	{
		if (editor->state2AnimationList->selected(i+1))
		{
			motionName2 = editor->state2AnimationList->text(i+1);
			break;
		}
	}
	if (motionName1 != "")
	{
		SmartBody::SBMotion * motion = SmartBody::SBScene::getScene()->getMotion(motionName1);
		nle::Block* block = editor->transitionEditorNleModel->getTrack(0)->getBlock(0);
		block->removeAllMarks();
		block->setName(motionName1);
		block->setStartTime(0);
		block->setEndTime(motion->duration() - editor->precisionCompensate);
	}
	if (motionName2 != "")
	{
		SmartBody::SBMotion * motion = SmartBody::SBScene::getScene()->getMotion(motionName2);
		nle::Block* block = editor->transitionEditorNleModel->getTrack(1)->getBlock(0);
		block->removeAllMarks();
		block->setName(motionName2);
		block->setStartTime(0);
		block->setEndTime(motion->duration() - editor->precisionCompensate);
	}

	editor->transitionTimeMarkWidget->setup();
	editor->paWindow->redraw();
}

void PATransitionEditor2::scrub(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;

	std::vector<std::string> selectedMotions;

	for (int i = 0; i < editor->state1AnimationList->size(); i++)
	{
		if (editor->state1AnimationList->selected(i+1))
		{
			selectedMotions.push_back(editor->state1AnimationList->text(i + 1));
		}
	}

	if (selectedMotions.size() == 1)
	{
		std::string currentStateName = editor->state1List->menu()[editor->state1List->value()].label();
		SmartBody::SBAnimationBlend* currentState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(currentStateName);
		
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
		editor->transitionTimeMarkWidget->setLocalTimes(times);

		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(selectedMotions[0]);
		double time = editor->sliderScrub->value();
		double delta = motion->duration() / double(motion->frames() - 1);
		int frameNumber = int(time / delta);
		std::string charName = editor->paWindow->characterList->menu()[editor->paWindow->characterList->value()].label();
		std::stringstream command;
		command << "motionplayer " << charName << " " << selectedMotions[0] << " " << frameNumber;
		SmartBody::SBScene::getScene()->command(command.str());

		editor->transitionTimeMarkWidget->redraw();
	}
	
}

void PATransitionEditor2::playmotion(Fl_Widget* widget, void* data)
{
	PATransitionEditor2* editor = (PATransitionEditor2*) data;

	PATransitionEditor2::selectState1Animations(widget, data);

	std::string charName = editor->paWindow->characterList->menu()[editor->paWindow->characterList->value()].label();

	std::stringstream command;
	if (editor->isPlaying == false)
	{
		editor->isPlaying = true;
		editor->buttonPlay->label("@square");
		editor->sliderScrub->activate();
		command << "motionplayer " << charName << " on";
		editor->transitionTimeMarkWidget->setShowScrubLine(true);
	}
	else
	{
		editor->isPlaying = false;
		editor->buttonPlay->label("@>");
		editor->sliderScrub->deactivate();
		command << "motionplayer " << charName << " off";
		editor->transitionTimeMarkWidget->setShowScrubLine(false);
	}
	SmartBody::SBScene::getScene()->command(command.str());
}




