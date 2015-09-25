/*
 *  PanimationWindow.cpp - part of SmartBody-lib's Test Suite
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
#include "PanimationWindow.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <bml/bml.hpp>
#include "ParamAnimBlock.h"
#include "ParamAnimEditorWidget.h"
#include "ParamAnimStateEditor.h"
#include "ParamAnimTransitionEditor.h"
#include "ParamAnimTransitionEditor2.h"
#include "ParamAnimRunTimeEditor.h"
#include "ParamAnimScriptEditor.h"
#include "VisualizationView.h"
#include <sb/SBScene.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBCommandManager.h>


PanimationWindowListener::PanimationWindowListener(PanimationWindow* window)
{
	_window = window;
}

void PanimationWindowListener::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
	_window->refreshUI(_window->characterList, _window);
}

void PanimationWindowListener::OnCharacterDelete( const std::string & name )
{
	if (_window->characterList->size() > 0)
	{
		std::string charName = _window->characterList->menu()[_window->characterList->value()].label();
		if (charName == name)
		{
			_window->characterList->value(0);
			_window->refreshUI(_window->characterList, _window);
		}
	}
}

void PanimationWindowListener::OnCharacterUpdate( const std::string & name )
{
}
      
void PanimationWindowListener::OnPawnCreate( const std::string & name )
{
}

void PanimationWindowListener::OnPawnDelete( const std::string & name )
{
}

void PanimationWindowListener::OnSimulationStart()
{
}

void PanimationWindowListener::OnSimulationEnd()
{
}

void PanimationWindowListener::OnSimulationUpdate()
{
	_window->update_viewer();
}

PanimationWindow::PanimationWindow(int x, int y, int w, int h, char* name) : Fl_Double_Window(w, h, name), GenericViewer(x, y, w, h)
{
	this->begin();
		int tabGroupX = 10;
		int tabGroupY = 30;
		int tabGroupW = w - 20;
		int tabGroupH = h - 100;
		int childGroupX = 0;
		int childGroupY = 2 * yDis ;
		int childGroupW = tabGroupW- 10;
		int childGroupH = tabGroupH - 2 * yDis;

		characterList = new Fl_Choice(300, h - 40, 200, 20, "Character List");
		loadCharacters(characterList);
		
		refresh = new Fl_Button(550, h - 40, 100, 20, "Refresh");
		refresh->callback(refreshUI, this);
		resetCharacter = new Fl_Button(670, h - 40, 100, 20, "Reset");
		resetCharacter->callback(reset, this);

		tabGroup = new Fl_Tabs(tabGroupX, tabGroupY, tabGroupW, tabGroupH);
		tabGroup->callback(changeTabGroup, this);
		tabGroup->begin();
			stateEditor = new PABlendEditor(childGroupX + tabGroupX, childGroupY + tabGroupY, childGroupW, childGroupH, this);
			stateEditor->begin();
			stateEditor->end();
//			transitionEditor = new PATransitionEditor(childGroupX + tabGroupX, childGroupY + tabGroupY, childGroupW, childGroupH, this);
//			transitionEditor->begin();
//			transitionEditor->end();
			transitionEditor2 = new PATransitionEditor2(childGroupX + tabGroupX, childGroupY + tabGroupY, childGroupW, childGroupH, this);
			transitionEditor2->begin();
			transitionEditor2->end();
//			scriptEditor = new PAScriptEditor(childGroupX + tabGroupX, childGroupY + tabGroupY, childGroupW, childGroupH, this);
//			scriptEditor->begin();
//			scriptEditor->end();
			runTimeEditor = new PARunTimeEditor(childGroupX + tabGroupX, childGroupY + tabGroupY, childGroupW, childGroupH, this);
			runTimeEditor->begin();
			runTimeEditor->end();
#if 0
			visView = new VisualizationView(childGroupX + tabGroupX, childGroupY + tabGroupY, childGroupW, childGroupH, this);
			visView->begin();
			visView->end();
#endif
		tabGroup->end();
	this->end();
	this->resizable(tabGroup);
	this->size_range(800, 740);
	redraw();

//	tabGroup->selected_child(stateEditor);
//	tabGroup->selected_child(transitionEditor);
//	tabGroup->selected_child(scriptEditor);
	tabGroup->value(runTimeEditor);
	lastCommand = "";
	_currentCharacterName = "";

	_listener = new PanimationWindowListener(this);
}


PanimationWindow::~PanimationWindow()
{
	SmartBody::SBScene::getScene()->removeSceneListener(_listener);
	delete _listener;
}

void PanimationWindow::draw()
{
	Fl_Double_Window::draw();   
}

void PanimationWindow::label_viewer(std::string name)
{
	this->label(strdup(name.c_str()));
}

void PanimationWindow::show_viewer()
{
	this->show();
}

void PanimationWindow::hide_viewer()
{
	this->hide();
}

void PanimationWindow::update_viewer()
{
	if (characterList->size() == 0)
		return;

	std::string charName = characterList->menu()[characterList->value()].label();
	SmartBody::SBCharacter * character = SmartBody::SBScene::getScene()->getCharacter(charName);
	if (!character)
		return;

//	if (tabGroup->value() == scriptEditor)
//		scriptEditor->update();
	if (tabGroup->value() == runTimeEditor)
		runTimeEditor->update();
	if (tabGroup->value() == visView)
		visView->update();
		
}

void PanimationWindow::show()
{    
	refreshUI(characterList, this);

	SmartBody::SBScene::getScene()->addSceneListener(_listener);
    Fl_Double_Window::show();   
}

void PanimationWindow::hide()
{    
	SmartBody::SBScene::getScene()->removeSceneListener(_listener);
    Fl_Double_Window::hide();   
}


void PanimationWindow::getSelectedMarkInfo(nle::NonLinearEditorModel* model, std::string& blockName, double& time)
{
	for (int i = 0; i < model->getNumTracks(); i++)
	{
		nle::Track* track = model->getTrack(i);
		nle::Block* block = track->getBlock(0);
		for (int j = 0; j < block->getNumMarks(); j++)
		{
			nle::Mark* mark = block->getMark(j);
			if (mark->isSelected())
			{
				blockName = block->getName();
				time = mark->getStartTime();
				break;
			}
		}
		if (blockName != "")
			break;
	}
}

bool PanimationWindow::checkCommand(std::string command)
{
	if (lastCommand == command)
		return false;
	else
		lastCommand = command;
	return true;
}

void PanimationWindow::execCmd(PanimationWindow* window, std::string cmd, double tOffset)
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	

	BML::SbmCommand* command = new BML::SbmCommand(cmd, (float)(SmartBody::SBScene::getScene()->getSimulationManager()->getTime() + tOffset));
	bool success = true;
	srCmdSeq *seq = new srCmdSeq(); //sequence that holds the commands
	if( command != NULL ) 
	{
		if( seq->insert( (float)(command->time), command->command.c_str() ) != CMD_SUCCESS ) 
		{
			std::stringstream strstr;
			strstr << "ERROR: PanimationWindow::generateBML \""
			     << "Failed to insert SbmCommand \"" << (command->command) << "\" at time " << (command->time) << "Aborting remaining commands.";
			LOG(strstr.str().c_str());
			success = false;
		}
		delete command;
	}
	if( success )
	{
		if (!scene->isRemoteMode())
		{
			if( SmartBody::SBScene::getScene()->getCommandManager()->execute_seq(seq) != CMD_SUCCESS ) 
				LOG("ERROR: PanimationWindow::generateBML: Failed to execute sequence.");			
		}
		else
		{
			//if( mcu.execute_seq(seq) != CMD_SUCCESS ) 
			//	LOG("ERROR: PanimationWindow::generateBML: Failed to execute sequence.");				
			SmartBody::SBScene::getScene()->command((char*)cmd.c_str());
			std::string sendStr = "send sbm " + cmd;
			SmartBody::SBScene::getScene()->command((char*) sendStr.c_str());
		}		
	}
}

void PanimationWindow::addTimeMark(nle::NonLinearEditorModel* model, bool selective)
{
	for (int i = 0; i < model->getNumTracks(); i++)
	{
		if (selective && !model->getTrack(i)->isSelected())
			continue;

		nle::Block* block = model->getTrack(i)->getBlock(0);
		CorrespondenceMark* toAddMark = new CorrespondenceMark();
		toAddMark->setStartTime(0.0);
		toAddMark->setEndTime(toAddMark->getStartTime());
		toAddMark->setColor(FL_RED);
		char buff[256];
		sprintf(buff, "%6.2f", toAddMark->getStartTime());
		toAddMark->setName(buff);
		toAddMark->setShowName(true);
		if (block)
			block->addMark(toAddMark);
	}
}

void PanimationWindow::removeTimeMark(nle::NonLinearEditorModel* model)
{
	CorrespondenceMark* attachedMark = NULL;
	for (int t = 0; t < model->getNumTracks(); t++)
	{
		nle::Track* track = model->getTrack(t);
		for (int b = 0; b < track->getNumBlocks(); b++)
		{
			nle::Block* block = track->getBlock(b);
			for (int m = 0; m < block->getNumMarks(); m++)
			{
				nle::Mark* mark = block->getMark(m);
				if (mark->isSelected())
				{
					CorrespondenceMark* cMark = dynamic_cast<CorrespondenceMark*>(mark);
					cMark->getAttachedMark().clear();
				//	attachedMark = cMark->getAttachedMark();
				//	if (attachedMark)	attachedMark = NULL;//attachedMark->attach(NULL);
					block->removeMark(mark);
				}
			}
		}
	}
}

void PanimationWindow::addTimeMarkToBlock(nle::Block* block, double t)
{
	CorrespondenceMark* mark = new CorrespondenceMark();
	mark->setStartTime(t);
	mark->setEndTime(mark->getStartTime());
	mark->setColor(FL_RED);
	char buff[256];
	sprintf(buff, "%6.2f", mark->getStartTime());
	mark->setName(buff);
	mark->setShowName(true);
	block->addMark(mark);	
}

std::vector<std::string> PanimationWindow::tokenize(const std::string& str,const std::string& delimiters)
{
	std::vector<std::string> tokens;
    	
	// skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	
	// find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
    	// found a token, add it to the vector.
    	tokens.push_back(str.substr(lastPos, pos - lastPos));
	
    	// skip delimiters.  Note the "not_of"
    	lastPos = str.find_first_not_of(delimiters, pos);
	
    	// find next "non-delimiter"
    	pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}

void PanimationWindow::refreshUI(Fl_Widget* widget, void* data)
{
	PanimationWindow* window = (PanimationWindow*) data;
	loadCharacters(window->characterList); // reload all character names
	if (window->tabGroup->value() == window->runTimeEditor)
		window->runTimeEditor->initializeRunTimeEditor();

	window->stateEditor->isPlaying = true;
	window->stateEditor->playmotion(window->stateEditor->buttonPlay, window->stateEditor);
}

void PanimationWindow::loadCharacters(Fl_Choice* characterList)
{
	const std::vector<std::string>& characterNames = SmartBody::SBScene::getScene()->getCharacterNames();

	characterList->clear();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		characterList->add((*iter).c_str());
	}
	characterList->value(0);
}

void PanimationWindow::reset(Fl_Widget* widget, void* data)
{	
	PanimationWindow* window = (PanimationWindow*) data;
	std::string charName = window->characterList->menu()[window->characterList->value()].label();
	std::stringstream command;
	command << "panim unschedule char " << charName;
	execCmd(window, command.str());
	std::stringstream resetPosCommand;
	resetPosCommand << "set char " << charName << " world_offset x 0 z 0 h 0 p 0 r 0";
	execCmd(window, resetPosCommand.str());		
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SmartBody::SBCharacter* sbChar = scene->getCharacter(charName);
	if (sbChar)
	{
		sbChar->setJointTrajBlendWeight(0.f);
		sbChar->setUseJointConstraint(false);
	}
}

void PanimationWindow::changeTabGroup(Fl_Widget* widget, void* data)
{
	refreshUI(widget, data);
}

PanimationWindow* PanimationWindow::getPAnimationWindow( Fl_Widget* w )
{
	PanimationWindow* panimWindow = NULL;
	Fl_Widget* widget = w;
	while (widget)
	{
		panimWindow = dynamic_cast<PanimationWindow*>(widget);
		if (panimWindow)
			break;
		else
			widget = widget->parent();
	}
	return panimWindow;
}

void PanimationWindow::setCurrentCharacterName(const std::string& name)
{
	_currentCharacterName = name;
}

const std::string& PanimationWindow::getCurrentCharacterName()
{
	return _currentCharacterName;
}

SmartBody::SBCharacter* PanimationWindow::getCurrentCharacter()
{
	if (!characterList->menu())
		return NULL;

	std::string charName = characterList->menu()[characterList->value()].label();
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	return scene->getCharacter(charName);
}

PanimationViewerFactory::PanimationViewerFactory()
{
}

GenericViewer* PanimationViewerFactory::create(int x, int y, int w, int h)
{
	PanimationWindow* panimationWindow = new PanimationWindow(x, y, w, h, (char*)"Blends");
	return panimationWindow;
}

void PanimationViewerFactory::destroy(GenericViewer* viewer)
{
	delete viewer;
}
