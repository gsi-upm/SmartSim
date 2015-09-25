/*
 *  ParamAnimTransitionEditor.cpp - part of SmartBody-lib's Test Suite
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
#include "ParamAnimTransitionEditor.h"
#include <FL/Fl_Hold_Browser.H>
#include "sb/SBScene.h"
#include "sb/SBMotion.h"

#include "ParamAnimBlock.h"
#include "ParamAnimEditorWidget.h"
#include "PanimationWindow.h"
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationStateManager.h>


PATransitionEditor::PATransitionEditor(int x, int y, int w, int h, PanimationWindow* window) : Fl_Group(x, y, w, h), paWindow(window)
{
	
	this->label("Transition Editor");
	this->begin();
		transitionEditorMode = new Fl_Check_Button(xDis + x, yDis + y, 200, 2 * yDis, "Create Transition Mode");
		transitionEditorMode->callback(changeTransitionEditorMode, this);
		
		int tgx = xDis + x;
		int tgy = 4*yDis + y;
		createTransitionGroup = new Fl_Group(tgx, tgy, w - 2 * xDis, h /2 - 4 * yDis, "new transition");
		int createTransitionGroupW = w - 2 * xDis;
		int createTransitionGroupH = h /2 - 4 * yDis;		
		createTransitionGroup->begin();
			createTransitionButton = new Fl_Button(xDis + tgx, yDis +tgy, 10 * xDis, 2 * yDis, "Create Transition");
			createTransitionButton->callback(createNewTransition, this);
			stateList1 = new Fl_Choice(xDis + 100 + tgx, 5 * yDis + tgy, 100, 2 * yDis, "State1");			
			stateList1->callback(changeStateList1, this);
			stateList1->when(FL_WHEN_CHANGED);
			animForTransition1 = new Fl_Hold_Browser(xDis + tgx, 8 * yDis + tgy, createTransitionGroupW / 2 - 5 * xDis, createTransitionGroupH - 9 * yDis);
			animForTransition1->callback(changeAnimForTransition, this);
			stateList2 = new Fl_Choice(createTransitionGroupW / 2 + 4 * xDis + 100 + tgx, 5 * yDis + tgy, 100, 2 * yDis, "State2");
			stateList2->callback(changeStateList2, this);
			stateList2->when(FL_WHEN_CHANGED);
			animForTransition2 = new Fl_Hold_Browser(createTransitionGroupW / 2 + 4 * xDis + tgx, 8 * yDis + tgy, createTransitionGroupW / 2 - 5 * xDis, createTransitionGroupH - 9 * yDis);
			animForTransition2->callback(changeAnimForTransition, this);
		createTransitionGroup->end();
		createTransitionGroup->box(FL_BORDER_BOX);
		this->resizable(createTransitionGroup);

		int etx = xDis + x;
		int ety = h / 2 + yDis + y;
		editTransitionTimeMarkGroup = new Fl_Group(etx, ety, w - 2 * xDis, h / 2 - 2 * yDis);
		editTransitionTimeMarkGroup->begin();
			transitionList = new Fl_Choice(10 * xDis + etx, yDis + ety, 100, 2 * yDis, "Transition List");
			transitionList->callback(changeTransitionList, this);
			transitionList->when(FL_WHEN_CHANGED);
			addMark1 = new Fl_Button(12 * xDis + 100 + etx, yDis + ety, 100 , 2 * yDis, "Add Mark");
			addMark1->callback(addTransitionTimeMark, this);
			removeMark1 = new Fl_Button(13 * xDis + 200 + etx, yDis + ety, 100 , 2 * yDis, "Delete Mark");
			removeMark1->callback(removeTransitionTimeMark, this);
			updateMark1 = new Fl_Button(14 * xDis + 300 + etx, yDis + ety, 100 , 2 * yDis, "Update Mark");
			updateMark1->callback(updateTransitionTimeMark, this);
			transitionTimeMarkWidget = new ParamAnimEditorWidget(this, 2 * xDis+ etx, 5 * yDis + ety, w - 5 * xDis, h / 2 - 6 * yDis, (char*) "");
		editTransitionTimeMarkGroup->end();
	transitionEditorNleModel = new nle::NonLinearEditorModel();
	transitionTimeMarkWidget->setModel(transitionEditorNleModel);

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
//	transitionTimeMarkWidget->setup();

	transitionEditorMode->value(true);
	createTransitionGroup->activate();
	transitionList->deactivate();

	loadStates();
	loadTransitions();
}

PATransitionEditor::~PATransitionEditor()
{
}

void PATransitionEditor::loadStates()
{
	stateList1->clear();
	stateList2->clear();
	stateList1->add("---");
	stateList2->add("---");
	std::vector<std::string> blendNames = SmartBody::SBScene::getScene()->getBlendManager()->getBlendNames();
	for (std::vector<std::string>::iterator iter = blendNames.begin();
		 iter != blendNames.end();
		 iter++)
	{
		stateList1->add((*iter).c_str());
		stateList2->add((*iter).c_str());
	}
	stateList1->value(0);
	stateList2->value(0);
}

void PATransitionEditor::loadTransitions()
{
	transitionList->clear();
	transitionList->add("---");
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	int numTransitions = scene->getBlendManager()->getNumTransitions();
	for (int t = 0; t < numTransitions; t++)
	{
		SmartBody::SBAnimationTransition* transition = scene->getBlendManager()->getTransitionByIndex(t);
		std::string transitionName = transition->getSourceBlend()->stateName + 
									 std::string(" + ") +
									 transition->getDestinationBlend()->stateName;
		transitionList->add(transitionName.c_str());
	}
	transitionList->value(0);
}

void PATransitionEditor::changeTransitionEditorMode(Fl_Widget* widget, void* data)
{
	PATransitionEditor* editor = (PATransitionEditor*) data;
	if (editor->transitionEditorMode->value())		// create mode
	{
		editor->createTransitionGroup->activate();
		editor->transitionList->deactivate();
	}
	else										// edit mode
	{
		editor->createTransitionGroup->deactivate();
		editor->transitionList->activate();
	}	
}

void PATransitionEditor::changeStateList1(Fl_Widget* widget, void* data)
{
	PATransitionEditor* editor = (PATransitionEditor*) data;
	int stateValue = editor->stateList1->value();
	int stateValueP = editor->stateList2->value();
	editor->loadStates();	
	editor->stateList1->value(stateValue);
	editor->stateList2->value(stateValueP);

	PABlend* state1 = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(editor->stateList1->text(stateValue));
	
	editor->animForTransition1->clear();
	if (state1)
	{
		for (int i = 0; i < state1->getNumMotions(); i++)
			editor->animForTransition1->add(state1->motions[i]->getName().c_str());
	}
	for (int i = 0; i < editor->animForTransition1->size(); i++)
		editor->animForTransition1->select(i, false);
}

void PATransitionEditor::changeStateList2(Fl_Widget* widget, void* data)
{
	PATransitionEditor* editor = (PATransitionEditor*) data;
	int stateValue = editor->stateList2->value();
	int stateValueP = editor->stateList1->value();
	editor->loadStates();
	PABlend* state2 = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(editor->stateList2->menu()[stateValue].label());
	
	editor->stateList2->value(stateValue);
	editor->stateList1->value(stateValueP);
	editor->animForTransition2->clear();
	if (state2)
	{
		for (int i = 0; i < state2->getNumMotions(); i++)
			editor->animForTransition2->add(state2->motions[i]->getName().c_str());
	}
	for (int i = 0; i < editor->animForTransition2->size(); i++)
		editor->animForTransition2->select(i, false);
}

const double precisionCompensate = 0.0001;
void PATransitionEditor::changeAnimForTransition(Fl_Widget* widget, void* data)
{
	PATransitionEditor* editor = (PATransitionEditor*) data;
	std::string motionName1 = "";
	for (int i = 0; i < editor->animForTransition1->size(); i++)
	{
		if (editor->animForTransition1->selected(i+1))
		{
			motionName1 = editor->animForTransition1->text(i+1);
			break;
		}
	}
	std::string motionName2 = "";
	for (int i = 0; i < editor->animForTransition2->size(); i++)
	{
		if (editor->animForTransition2->selected(i+1))
		{
			motionName2 = editor->animForTransition2->text(i+1);
			break;
		}
	}
	if (motionName1 != "")
	{
		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(motionName1);
		nle::Block* block = editor->transitionEditorNleModel->getTrack(0)->getBlock(0);
		block->removeAllMarks();
		block->setName(motionName1);
		block->setStartTime(0);
		block->setEndTime(motion->duration() - precisionCompensate);
	}
	if (motionName2 != "")
	{
		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(motionName2);
		nle::Block* block = editor->transitionEditorNleModel->getTrack(1)->getBlock(0);
		block->removeAllMarks();
		block->setName(motionName2);
		block->setStartTime(0);
		block->setEndTime(motion->duration() - precisionCompensate);		
	}

	if (motionName1 != "" && motionName2 != "")
	{
		nle::Block* block1 = editor->transitionEditorNleModel->getTrack(0)->getBlock(0);
		nle::Block* block2 = editor->transitionEditorNleModel->getTrack(1)->getBlock(0);
		std::string stateName1 = editor->stateList1->menu()[editor->stateList1->value()].label();
		std::string stateName2 = editor->stateList2->menu()[editor->stateList2->value()].label();
		PABlend* fromState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName1);
		PABlend* toState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName2);
	
		for (int i = 0; i < fromState->getNumMotions(); i++)
		{
			if (motionName1 == fromState->motions[i]->getName())
			{
				int numKeys = fromState->getNumKeys();
				block1->setStartTime(fromState->keys[i][0]);
				block1->setEndTime(fromState->keys[i][numKeys - 1] - precisionCompensate);
			}
		}
		for (int i = 0; i < toState->getNumMotions(); i++)
		{
			if (motionName2 == toState->motions[i]->getName())
			{
				int numKeys = toState->getNumKeys();
				block2->setStartTime(toState->keys[i][0]);
				block2->setEndTime(toState->keys[i][numKeys - 1] - precisionCompensate);
			}
		}
	}
//	editor->transitionTimeMarkWidget->setup();
	editor->paWindow->redraw();
}

void PATransitionEditor::addTransitionTimeMark(Fl_Widget* widget, void* data)
{
	PATransitionEditor* editor = (PATransitionEditor*) data;
	editor->paWindow->addTimeMark(editor->transitionEditorNleModel, true);
//	editor->transitionTimeMarkWidget->setup();
	editor->paWindow->redraw();
}

void PATransitionEditor::removeTransitionTimeMark(Fl_Widget* widget, void* data)
{
	PATransitionEditor* editor = (PATransitionEditor*) data;
	editor->paWindow->removeTimeMark(editor->transitionEditorNleModel);
//	editor->transitionTimeMarkWidget->setup();
	editor->paWindow->redraw();
}

void PATransitionEditor::updateTransitionTimeMark(Fl_Widget* widget, void* data)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	PATransitionEditor* editor = (PATransitionEditor*) data;
	SmartBody::SBAnimationTransition* transition = NULL;
	std::string fromStateName = "";
	std::string toStateName = "";
	if (!editor->transitionList->active())
	{
		fromStateName = editor->stateList1->menu()[editor->stateList1->value()].label();
		toStateName = editor->stateList2->menu()[editor->stateList2->value()].label();
		transition = scene->getBlendManager()->getTransition(fromStateName, toStateName);
	}
	else
	{
		std::string fullName = editor->transitionList->menu()[editor->transitionList->value()].label();
		if (fullName == "---")
		{
			editor->transitionEditorNleModel->getTrack(0)->getBlock(0)->setName("");
			editor->transitionEditorNleModel->getTrack(1)->getBlock(0)->setName("");
			return;
		}
		size_t seperateMarkPos = fullName.find("+");
		fromStateName = fullName.substr(0, seperateMarkPos);
		toStateName = fullName.substr(seperateMarkPos + 1, fullName.size() - 1);
		transition = scene->getBlendManager()->getTransition(fromStateName, toStateName);
	}
	if (transition)
	{
		nle::Block* block1 = editor->transitionEditorNleModel->getTrack(0)->getBlock(0);
		nle::Block* block2 = editor->transitionEditorNleModel->getTrack(1)->getBlock(0);	
		
		SmartBody::SBAnimationBlend* fromState = scene->getBlendManager()->getBlend(fromStateName);
		SmartBody::SBAnimationBlend* toState = scene->getBlendManager()->getBlend(toStateName);
		
		transition->set(fromState, toState);
		
		if (block1->getNumMarks() == 0)	
		{
			transition->addEaseOutInterval(block1->getName(), 
											(float) scene->getMotion(transition->getSourceMotionName().c_str())->duration() - (float) defaultTransition,
											(float) scene->getMotion(transition->getSourceMotionName().c_str())->duration());
		}
		else
		{
			int numEaseOutIntervals = transition->getNumEaseOutIntervals();
			for (int i = 0; i < numEaseOutIntervals; i++)
				transition->removeEaseOutInterval(0);

			
			for (int i = 0; i < block1->getNumMarks() / 2; i++)
			{
				transition->addEaseOutInterval(block1->getName(), 
											   (float) block1->getMark(i * 2 + 0)->getStartTime(),
											   (float) block1->getMark(i * 2 + 1)->getStartTime());	
			}
		}

		
		if (block2->getNumMarks() == 0)
		{
			transition->setEaseInInterval(block2->getName(), 0.0f, (float) defaultTransition);
		}
		else
		{
			transition->setEaseInInterval(block2->getName(), (float) block2->getMark(0)->getStartTime(), (float) block2->getMark(1)->getStartTime());
		}
		for (int i = 0; i < block1->getNumMarks(); i++)
			block1->getMark(i)->setSelected(false);
		for (int i = 0; i < block2->getNumMarks(); i++)
			block2->getMark(i)->setSelected(false);
	}
	editor->paWindow->redraw();
}


void PATransitionEditor::createNewTransition(Fl_Widget* widget, void* data)
{
	PATransitionEditor* editor = (PATransitionEditor*) data;
	std::string fromStateName = editor->stateList1->menu()[editor->stateList1->value()].label();
	std::string toStateName = editor->stateList2->menu()[editor->stateList2->value()].label();

	SmartBody::SBAnimationTransition* transition = SmartBody::SBScene::getScene()->getBlendManager()->getTransition(fromStateName, toStateName);
	if (transition != NULL)
	{
		LOG("Transition %s to %s already exist.", fromStateName.c_str(), toStateName.c_str());
		return;
	}
	SmartBody::SBAnimationBlend* fromState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(fromStateName);
	SmartBody::SBAnimationBlend* toState = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(toStateName);
		
	if (fromState != NULL && toState != NULL)
	{
		transition = SmartBody::SBScene::getScene()->getBlendManager()->createTransition(fromStateName, toStateName);
		updateTransitionTimeMark(widget, data);
	}
}

void PATransitionEditor::changeTransitionList(Fl_Widget* widget, void* data)
{
	PATransitionEditor* editor = (PATransitionEditor*) data;
	int stateValue = editor->transitionList->value();
	editor->loadTransitions();
	editor->transitionList->value(stateValue);

	nle::Block* block1 = editor->transitionEditorNleModel->getTrack(0)->getBlock(0);
	block1->removeAllMarks();
	nle::Block* block2 = editor->transitionEditorNleModel->getTrack(1)->getBlock(0);		
	block2->removeAllMarks();

	std::string fullName = editor->transitionList->menu()[stateValue].label();
	if (fullName == "---")
	{
		editor->paWindow->redraw();
		return;
	}

	size_t seperateMarkPos = fullName.find("+");
	if (seperateMarkPos == std::string::npos)
		return;
	std::string fromStateName = fullName.substr(0, seperateMarkPos - 1);
	std::string toStateName = fullName.substr(seperateMarkPos + 2, fullName.size() - 1);
	SmartBody::SBAnimationTransition* transition = SmartBody::SBScene::getScene()->getBlendManager()->getTransition(fromStateName, toStateName);

	block1->removeAllMarks();
	block1->setName(transition->getSourceMotionName());
	SmartBody::SBMotion * motion = SmartBody::SBScene::getScene()->getMotion(transition->getSourceMotionName());
	block1->setStartTime(0);
	block1->setEndTime(motion->duration());
	for (int i = 0; i < transition->getNumEaseOutIntervals(); i++)
	{
		editor->paWindow->addTimeMarkToBlock(block1, transition->getEaseOutStart()[i]);
		editor->paWindow->addTimeMarkToBlock(block1, transition->getEaseOutEnd()[i]);
	}
	block2->removeAllMarks();
	block2->setName(transition->getDestinationMotionName());
	motion = SmartBody::SBScene::getScene()->getMotion(transition->getDestinationMotionName());
	block2->setStartTime(0);
	block2->setEndTime(motion->duration());
	editor->paWindow->addTimeMarkToBlock(block2, transition->getEaseInStart());
	editor->paWindow->addTimeMarkToBlock(block2, transition->getEaseInEnd());

	editor->paWindow->redraw();
}
