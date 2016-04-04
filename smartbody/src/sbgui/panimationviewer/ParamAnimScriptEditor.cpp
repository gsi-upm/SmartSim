/*
 *  ParamAnimScriptEditor.cpp - part of SmartBody-lib's Test Suite
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
#include "ParamAnimScriptEditor.h"
#include <FL/Fl_Hold_Browser.H>
#include <controllers/me_ct_param_animation.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>

#define transitionTrace 0

PAScriptEditor::PAScriptEditor(int x, int y, int w, int h, PanimationWindow* window) : Fl_Group(x, y, w, h), paWindow(window)
{
	this->label("Script Editor");
	this->begin();
		availableStateList = new Fl_Hold_Browser(2 * xDis + x, 3 * yDis + y, w / 2 - 6 * xDis, 2 * h / 3 - 5 * yDis, "Available States"); 
		currentStateList = new Fl_Hold_Browser(w / 2 + 4 * xDis + x, 3 * yDis + y, w / 2 - 6 * xDis, 2 * h / 3 - 5 * yDis, "Scheduled States");
		currentStateList->when(FL_WHEN_ENTER_KEY_ALWAYS);
		currentStateList->callback(updateStateInfo, this);
		addStateButton = new Fl_Button(w / 2 - 3 * xDis+ x, h / 2+ y, 6 * xDis, 2 * yDis, ">>>");
		addStateButton->callback(addState, this);
		removeStateButton = new Fl_Button(w / 2 - 3 * xDis+ x, h / 2 + 3 * yDis+ y, 6 * xDis, 2 * yDis, "<<<");
		removeStateButton->callback(removeState, this);
		runStateList = new Fl_Button(2 * xDis+ x, 2 * h / 3+ y, 100, 20, "Run");
		runStateList->callback(run, this);
		currentStatePanel = new Fl_Output(10 * xDis+ x, h - 6 * yDis+ y, 200, 2 * yDis, "Current State:");
		currentStateWeight = new Fl_Value_Slider(11 * xDis + 200+ x, h - 6 * yDis+ y, 200, 2 * yDis, "Weight");
		currentStateWeight->type(FL_HORIZONTAL);
		currentStateWeight->callback(changeCurrentStateWeight, this);
		currentStateWeight->minimum(0);
		currentStateWeight->maximum(1);
		currentStateWeight->deactivate();
		nextStatePanel = new Fl_Output(10 * xDis+ x, h - 3 * yDis+ y, 200, 2 * yDis, "Next State:");
	this->end();
	initialAvailableStates();
}

PAScriptEditor::~PAScriptEditor()
{
}

void PAScriptEditor::addState(Fl_Widget* widget, void* data)
{
	PAScriptEditor* editor = (PAScriptEditor*) data;
	for (int i = 0; i < editor->availableStateList->size(); i++)
	{
		if (editor->availableStateList->selected(i+1))
		{
			std::string selectedState = editor->availableStateList->text(i+1);
			editor->currentStateList->add(selectedState.c_str());
#if transitionTrace
			editor->updateAvailableStates(selectedState);
#endif
			std::map<std::string, double>::iterator iter = editor->stateTimeOffset.find(selectedState);
			if (iter != editor->stateTimeOffset.end())
				editor->stateTimeOffset.erase(iter);
			editor->stateTimeOffset.insert(std::make_pair(selectedState, 0.1));
		
			int loopValue = fl_choice("Is this loop mode", "yes", "no", NULL);
			bool loop;
			if (loopValue == 0)	loop = true;
			else				loop = false;
			std::map<std::string, bool>::iterator iter1 = editor->stateLoopMode.find(selectedState);
			if (iter1 != editor->stateLoopMode.end())
				editor->stateLoopMode.erase(iter1);
			editor->stateLoopMode.insert(std::make_pair(selectedState, loop));
		}
	}
}

void PAScriptEditor::removeState(Fl_Widget* widget, void* data)
{
	PAScriptEditor* editor = (PAScriptEditor*) data;
	int size = editor->currentStateList->size();
	if (size > 0)
	{
		//Fl_Widget* curStateChild = editor->currentStateList->child(size);
		std::string selectedState = editor->currentStateList->text(size);
		editor->currentStateList->remove(size);

		bool deleteMap = true;
		for (int i = 0; i < editor->currentStateList->size(); i++)
		{
			if (selectedState == editor->currentStateList->text(i+1))
				deleteMap = false;
		}
		if (deleteMap)
		{
			std::map<std::string, double>::iterator iter = editor->stateTimeOffset.find(selectedState);
			editor->stateTimeOffset.erase(iter);

			std::map<std::string, bool>::iterator iter1 = editor->stateLoopMode.find(selectedState);
			editor->stateLoopMode.erase(iter1);
		}
	}

	if (editor->currentStateList->size() == 0)
		editor->initialAvailableStates();
#if transitionTrace
	else
	{
		editor->currentStateList->goto_index(editor->currentStateList->size() - 1)->set_selected();
		editor->updateAvailableStates(editor->currentStateList->goto_index(editor->currentStateList->size() - 1)->label());
	}
	for (int i = 0; i < editor->currentStateList->size(); i++)
	{
		if (editor->currentStateList->goto_index(i)->selected())
			editor->updateAvailableStates(editor->currentStateList->goto_index(i)->label());
	}
#endif
}

void PAScriptEditor::updateStateInfo(Fl_Widget* widget, void* data)
{
	PAScriptEditor* editor = (PAScriptEditor*) data;
	for (int i = 0; i < editor->currentStateList->size(); i++)
	{
		if (editor->currentStateList->selected(i+1))
		{
			std::string selectedState = editor->currentStateList->text(i+1);
			bool shouldAddTimeOffset;
			if (i == 1)	shouldAddTimeOffset = false;
			else
			{
				std::string previousState = editor->currentStateList->text(i + 1);
				std::map<std::string, bool>::iterator iter1 = editor->stateLoopMode.find(previousState);
				shouldAddTimeOffset = iter1->second;
				}
			if (shouldAddTimeOffset)
			{
				const char* offsetString = fl_input("time offset from previous state", "0.0");
				if (offsetString != NULL)
				{
					double offset = atof(offsetString);
					std::map<std::string, double>::iterator iter = editor->stateTimeOffset.find(selectedState);
					iter->second = offset;
				}
			}
			PABlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(selectedState);
	
			if (state->getNumMotions() > 1)
			{
/*				const char* ws = fl_input("weights (separate by white space)", "");
				if (ws == NULL)	return;
				std::string weights = ws;
				std::vector<std::string> weight = editor->paWindow->tokenize(weights, " ");
				if (state->getNumMotions() == weight.size())
					for (int i = 0; i < state->getNumMotions(); i++)
						state->weights[i] = (float)atof(weight[i].c_str());	
						*/
			}
		}
	}	
}

void PAScriptEditor::run(Fl_Widget* widget, void* data)
{
	PAScriptEditor* editor = (PAScriptEditor*) data;
	std::string charName = editor->paWindow->characterList->menu()[editor->paWindow->characterList->value()].label();
	double offset = 0.0;
	for (int i = 0; i < editor->currentStateList->size(); i++)
	{
		std::string stateName = editor->currentStateList->text(i+1);
		std::map<std::string, bool>::iterator iter = editor->stateLoopMode.find(stateName);
		bool loop = iter->second;
		std::string loopString;
		if (loop)	loopString = "true";
		else		loopString = "false";
		std::map<std::string, double>::iterator iter1 = editor->stateTimeOffset.find(stateName);
		if (i != 0)
			offset += iter1->second;
		std::stringstream command;
		command << "panim schedule char " << charName << " state " << stateName << " loop " << loopString <<  " playnow false additive false joint null ";
		PABlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	
		int wNumber = state->getNumMotions();
		/*
		for (int j = 0; j < wNumber; j++)
			command << " " << state->weights[j];
		editor->paWindow->execCmd(editor->paWindow, command.str(), offset);
		*/
	}
}

void PAScriptEditor::changeCurrentStateWeight(Fl_Widget* widget, void* data)
{
	PAScriptEditor* editor = (PAScriptEditor*) data;
	double weight = editor->currentStateWeight->value();
	std::string charName = editor->paWindow->characterList->menu()[editor->paWindow->characterList->value()].label();
	std::string stateName = editor->currentStatePanel->value();
	std::stringstream command;
	command << "panim update char " << charName;
	PABlend* state = SmartBody::SBScene::getScene()->getBlendManager()->getBlend(stateName);
	
	if (!state)
		return;
	int wNumber = state->getNumMotions();
	int id = int(weight);
	for (int j = 0; j < wNumber; j++)
	{
		if (j == id)
			command << " " << 1 - (weight - id);
		else if (j == (id + 1))
			command << " " << (weight - id);
		else
			command << " 0.0";
	}
	editor->paWindow->execCmd(editor->paWindow, command.str());
}

void PAScriptEditor::initialAvailableStates()
{
	availableStateList->clear();
	std::vector<std::string> blendNames = SmartBody::SBScene::getScene()->getBlendManager()->getBlendNames();
	for (std::vector<std::string>::iterator iter = blendNames.begin();
		 iter != blendNames.end();
		 iter++)
	{
		availableStateList->add((*iter).c_str());
	}
}

void PAScriptEditor::updateAvailableStates(std::string currentState)
{
	availableStateList->clear();

	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	int numTransitions = scene->getBlendManager()->getNumTransitions();
	for (int t = 0; t < numTransitions; t++)
	{
		SmartBody::SBAnimationTransition* transition = scene->getBlendManager()->getTransitionByIndex(t);
		if (transition->getSourceBlend()->stateName == currentState)
			availableStateList->add(transition->getDestinationBlend()->stateName.c_str());
	}
}

void PAScriptEditor::refresh()
{
	initialAvailableStates();
	currentStateList->clear();
	currentStatePanel->value("");
	nextStatePanel->value("");
	currentStateWeight->deactivate();
	stateTimeOffset.clear();
	stateLoopMode.clear();
}

void PAScriptEditor::update()
{
	std::string charName = paWindow->characterList->menu()[paWindow->characterList->value()].label();
	SmartBody::SBCharacter * character = SmartBody::SBScene::getScene()->getCharacter(charName);
	if (!character)
		return;
	if (!character->param_animation_ct)
		return;
	std::string curStateName = character->param_animation_ct->getCurrentPABlendData()->state->stateName;
	std::string nextStateName = character->param_animation_ct->getNextStateName();
	currentStatePanel->value(curStateName.c_str());
	nextStatePanel->value(nextStateName.c_str());

	if (curStateName != "" && nextStateName != "")
	{
		currentStateWeight->deactivate();
	}
	else
	{
		int numWeights = character->param_animation_ct->getNumWeights();
		if (numWeights > 1)
		{
			currentStateWeight->activate();
			currentStateWeight->maximum(numWeights - 1);
		}
	}	
}
