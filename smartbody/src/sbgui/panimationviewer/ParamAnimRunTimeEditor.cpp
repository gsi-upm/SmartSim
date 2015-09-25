/*
 *  ParamAnimRunTimeEditor.cpp - part of SmartBody-lib's Test Suite
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
#include "ParamAnimRunTimeEditor.h"
#include <FL/gl.h>
#include <FL/fl_draw.H>

#include <controllers/me_ct_param_animation.h>
#include "ParameterGroup.h"
#include "ParameterVisualization.h"
#include "Parameter3DVisualization.h"
#include <sb/SBAnimationStateManager.h>
#include <sb/SBAnimationState.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>

PARunTimeEditor::PARunTimeEditor(int x, int y, int w, int h, PanimationWindow* window) : Fl_Group(x, y, w, h), paWindow(window)
{
	this->label("Run Time Editor");
	this->begin();
		currentCycleState = new Fl_Output(2 * xDis + 100 + x, yDis + y, 200, 2 * yDis, "Current State");
		nextCycleStates = new Fl_Hold_Browser(2 * xDis + x, 5 * yDis + y, w / 2 - 4 * xDis, h / 4, "Next State");
		nextCycleStates->callback(updateTransitionStates, this);
		
	
		availableTransitions = new Fl_Hold_Browser(w / 2 + 2 * xDis + x, 5 * yDis + y, w / 2 - 4 * xDis, h / 4, "Available Transitions");
		availableTransitions->callback(updateNonCycleState, this);
		availableTransitions->when(FL_WHEN_ENTER_KEY_ALWAYS);
		runNextState = new Fl_Button(2 * xDis + x, h / 4 + 6 * yDis + y, 100, 2 * yDis, "Run");
		runNextState->callback(run, this);
		parameterGroup = new Fl_Group(2 * xDis + x , h / 4 + 9 * yDis + y, w - 2 * xDis, 3 * h / 4 - 10 * yDis);
		parameterGroup->box(FL_UP_BOX);
	this->end();
	this->resizable(parameterGroup);
	paramGroup = NULL;
	initializeRunTimeEditor();
}

PARunTimeEditor::~PARunTimeEditor()
{
}

void PARunTimeEditor::update()
{
	std::string charName = paWindow->characterList->menu()[paWindow->characterList->value()].label();
	SmartBody::SBCharacter * character = SmartBody::SBScene::getScene()->getCharacter(charName);
	if (!character)
		return;

	if (!character->param_animation_ct)
		return;

	std::string currentState = "";
	if (character->param_animation_ct->getCurrentPABlendData())
	{
		currentState = character->param_animation_ct->getCurrentPABlendData()->state->stateName;
	}
	if (prevCycleState != currentState)
	{
		updateRunTimeStates(currentState);
		prevCycleState = currentState;
		currentCycleState->value(currentState.c_str());
		paWindow->redraw();
	}

	if (paramGroup)
	{
		PABlendData* curStateData = character->param_animation_ct->getCurrentPABlendData();
		if (!curStateData)
			return;
		if (curStateData)
		{
	//		if (curState->cycle)
			{
				if (paramGroup->paramVisualization)
				{
					float x = 0.0f, y = 0.0f;
					curStateData->state->getParametersFromWeights(x, y, curStateData->weights);
					int actualPixelX = 0;
					int actualPixelY = 0;
					paramGroup->paramVisualization->getActualPixel(x, y, actualPixelX, actualPixelY);
					paramGroup->paramVisualization->setPoint(actualPixelX, actualPixelY);	
					paramGroup->paramVisualization->redraw();
				}
				if (paramGroup->param3DVisualization)
				{
					float x = 0.0f, y = 0.0f, z = 0.0f;
					curStateData->state->getParametersFromWeights(x, y, z, curStateData->weights);
// 					paramGroup->xAxis->value(x);
// 					paramGroup->yAxis->value(y);
// 					paramGroup->zAxis->value(z);
					paramGroup->param3DVisualization->redraw();
				}
			}
		}
	}
}

void PARunTimeEditor::updateRunTimeStates(std::string currentState)
{
	nextCycleStates->clear();
	PABlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(currentState);
//	if (blendData)
//		if (!blendData->cycle)
//			return;

	if (currentState == "")
	{
		if (paramGroup)
		{
			parameterGroup->remove(paramGroup);
			delete paramGroup;
			paramGroup = NULL;
		}
		return;
	}

	if (currentState == PseudoIdleState)
	{
		std::vector<std::string> blendNames = SmartBody::SBScene::getScene()->getBlendManager()->getBlendNames();
		for (std::vector<std::string>::iterator iter = blendNames.begin();
			 iter != blendNames.end();
			 iter++)
		{
//			if (mcu.param_anim_blends[i]->cycle)
				addItem(nextCycleStates, (*iter));
		}
	}
	else
	{
//		if (blendData->toStates.size() == 0)
			addItem(nextCycleStates, PseudoIdleState);
		std::vector<std::string> nextStates;

/*		for (size_t i = 0; i < state->toStates.size(); i++)
			for (size_t j = 0; j < state->toStates[i]->toStates.size(); j++)
				addItem(nextCycleStates, state->toStates[i]->toStates[j]->stateName.c_str());
*/	}
	for (int i = 0; i < nextCycleStates->size(); i++)
		nextCycleStates->select(i+1, false);
	availableTransitions->clear();

	if (paramGroup)
	{
		parameterGroup->remove(paramGroup);
		delete paramGroup;
		paramGroup = NULL;
	}
	if (state)
	{
		std::vector<double> weights;
		PABlendData* blendData = new PABlendData(state, weights);
		// memory leak!
		paramGroup = new ParameterGroup(this->parameterGroup->x(), this->parameterGroup->y(), parameterGroup->w(), parameterGroup->h(), (char*)"", blendData, paWindow);
		parameterGroup->add(paramGroup);
		paramGroup->show();
		paramGroup->redraw();
		if (paramGroup->param3DVisualization)
			paramGroup->param3DVisualization->show();
	}
}

void PARunTimeEditor::addItem(Fl_Browser* browser, std::string item)
{
	for (int i = 0; i < browser->size(); i++)
	{
		const char* text = browser->text(i+1);		
		if (item == text)
			return;
	}
	browser->add(item.c_str());
	const char* newText = browser->text(1);
}

void PARunTimeEditor::initializeRunTimeEditor()
{
	if (paWindow->characterList->size() == 0)
		return;

	std::string charName = paWindow->characterList->menu()[paWindow->characterList->value()].label();
	SmartBody::SBCharacter * character = SmartBody::SBScene::getScene()->getCharacter(charName);
	if (character)
	{
		if (character->param_animation_ct == NULL)
			return;
		PABlendData* blendData = character->param_animation_ct->getCurrentPABlendData();
		if (blendData)
			currentCycleState->value(blendData->state->stateName.c_str());

		nextCycleStates->clear();
		availableTransitions->clear();
		updateRunTimeStates(currentCycleState->value());
	}
	prevCycleState = "";
}

void PARunTimeEditor::updateNonCycleState(Fl_Widget* widget, void* data)
{
	PARunTimeEditor* editor = (PARunTimeEditor*) data;

	std::string nonCycleState;
	for (int i = 0; i < editor->availableTransitions->size(); i++)
	{
		if (editor->availableTransitions->selected(i+1))
			nonCycleState = editor->availableTransitions->text(i+1);
	}
	PABlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(nonCycleState);
	
	if (state && state->getNumParameters() > 0)
	{
		if (editor->paramGroup)
		{
			editor->parameterGroup->remove(editor->paramGroup);
			delete editor->paramGroup;
			editor->paramGroup = NULL;
		}
		
		std::vector<double> weights;
		PABlendData* blendData = new PABlendData(state, weights);
		// memory leak here!
		editor->paramGroup = new ParameterGroup(editor->parameterGroup->x(), editor->parameterGroup->y(), editor->parameterGroup->w(), editor->parameterGroup->h(), (char*)"", blendData, editor->paWindow);
		editor->parameterGroup->add(editor->paramGroup);
		editor->paramGroup->show();
		editor->paramGroup->redraw();		
	}
}

void PARunTimeEditor::updateTransitionStates(Fl_Widget* widget, void* data)
{
	PARunTimeEditor* editor = (PARunTimeEditor*) data;
	editor->availableTransitions->clear();
	std::string currentState = editor->currentCycleState->value();
	std::string nextState = "";
	for (int i = 0; i < editor->nextCycleStates->size(); i++)
	{
		if (editor->nextCycleStates->selected(i+1))
			nextState = editor->nextCycleStates->text(i+1);
	}
	std::vector<std::string> blendNames = SmartBody::SBScene::getScene()->getBlendManager()->getBlendNames();
	for (std::vector<std::string>::iterator iter = blendNames.begin();
		 iter != blendNames.end();
		 iter++)
	{
		bool fromHit = false;
		bool toHit = false;
		if (currentState == PseudoIdleState)
		{
			/*
			if (mcu.param_anim_blends[i]->fromStates.size() == 0)
			{
				for (size_t j = 0; j < mcu.param_anim_blends[i]->toStates.size(); j++)
				{
					if (mcu.param_anim_blends[i]->toStates[j]->stateName == nextState)
					{
						editor->availableTransitions->add(mcu.param_anim_blends[i]->stateName.c_str());	
						break;
					}
				}
			}
			*/
		}
		else if (nextState == PseudoIdleState)
		{
			/*
			if (mcu.param_anim_blends[i]->toStates.size() == 0)
			{
				for (size_t j = 0; j < mcu.param_anim_blends[i]->fromStates.size(); j++)
				{
					if (mcu.param_anim_blends[i]->fromStates[j]->stateName == currentState)
					{
						editor->availableTransitions->add(mcu.param_anim_blends[i]->stateName.c_str());	
						break;
					}
				}
			}
			*/
		}
		else
		{
			/*
			for (size_t j = 0; j < mcu.param_anim_blends[i]->fromStates.size(); j++)
				if (mcu.param_anim_blends[i]->fromStates[j]->stateName == currentState)
				{
					fromHit = true;
					break;
				}
			for (size_t j = 0; j < mcu.param_anim_blends[i]->toStates.size(); j++)
				if (mcu.param_anim_blends[i]->toStates[j]->stateName == nextState)
				{
					toHit = true;
					break;
				}
				*/

			if (fromHit && toHit)
				editor->availableTransitions->add((*iter).c_str());	
		}
	}
}

void PARunTimeEditor::run(Fl_Widget* widget, void* data)
{
	PARunTimeEditor* editor = (PARunTimeEditor*) data;
	std::string charName = editor->paWindow->characterList->menu()[editor->paWindow->characterList->value()].label();
	std::string nextCycleState = "";
	for (int i = 0; i < editor->nextCycleStates->size(); i++)
		if (editor->nextCycleStates->selected(i+1))
		{	
			nextCycleState = editor->nextCycleStates->text(i+1);
			break;
		}
	std::string transitionState = "";
	for (int i = 0; i < editor->availableTransitions->size(); i++)
		if (editor->availableTransitions->selected(i+1))
		{	
			transitionState = editor->availableTransitions->text(i+1);
			break;
		}

	double timeoffset = 0.0;
	if (transitionState != "")
	{
		std::stringstream command1;
		command1 << "panim schedule char " << charName << " state " << transitionState << " loop false playnow false additive false joint null";
		editor->paWindow->execCmd(editor->paWindow, command1.str(), timeoffset);
		timeoffset += 0.1;
	}
	if (nextCycleState != PseudoIdleState && nextCycleState != "")
	{
		std::stringstream command2;
		command2 << "panim schedule char " << charName << " state " << nextCycleState << " loop true playnow false additive false joint null";
		editor->paWindow->execCmd(editor->paWindow, command2.str(), timeoffset);
	}
	
	if (nextCycleState == "") return;
	if (nextCycleState == PseudoIdleState)
	{
		std::stringstream command3;
		command3 << "panim schedule char " << charName << " state " << "PseudoIdle" << " loop true playnow true additive false joint null";
		editor->paWindow->execCmd(editor->paWindow, command3.str(), timeoffset);
	}
}
