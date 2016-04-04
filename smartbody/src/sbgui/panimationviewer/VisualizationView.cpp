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

#include "VisualizationView.h"
#include <sb/SBCharacter.h>

#include <FL/gl.h>
#include <FL/fl_draw.H>
#include "ErrorVisualization.h"
#include <controllers/me_ct_param_animation.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>


VisualizationView::VisualizationView(int x, int y, int w, int h, PanimationWindow* window) : Fl_Group(x, y, w, h)
{
	paWindow = window;
	this->label("Visualization");
	this->begin();
	currentCycleState = new Fl_Output(2 * xDis + 100 + x, yDis + y, 200, 2 * yDis, "Current State");

	visShapeChoice = new Fl_Choice(2 * xDis + 100 + x, yDis*4 + y, 200, 2 * yDis, "Surface Type");
	visShapeChoice->add("curve");
	visShapeChoice->add("flat");
	visShapeChoice->value(0);

	currentViz = new Fl_Choice(2 * xDis + 100 + x, yDis*7 + y, 200, 2 * yDis, "Viz Type");
	currentViz->add("error");
	currentViz->add("smooth");
	currentViz->value(0);
	currentViz->callback(updateVizType,this);

	buildVizButton = new Fl_Button(2 * xDis + 350 + x, yDis*7 + y , 200, 2 * yDis, "Build Viz");
	buildVizButton->callback(buildViz,this);


	plotMotionButton = new Fl_Button(2 * xDis + 50 + x, yDis*10 + y , 100, 2 * yDis, "Plot Motion");
	plotMotionButton->callback(plotMotion,this);


	plotJointTrajButton = new Fl_Button(2 * xDis + 150 + x, yDis*10 + y , 100, 2 * yDis, "Plot Joint Traj");
	plotJointTrajButton->callback(plotJointTraj,this);
	plotJointChoice = new Fl_Choice(2 * xDis + 150 + x, yDis*12 + y , 100, 2 * yDis, "select joint:");
	plotJointButton = new Fl_Button(2 * xDis + 250 + x, yDis*12 + y , 100, 2 * yDis, "refresh list");
	plotJointButton->callback(refreshJointList, this);

	plotRandomColorCheckbox = new Fl_Check_Button(2 * xDis + 350 + x, yDis*12 + y , 100, 2 * yDis, "use random color");
	plotRandomColorCheckbox->value(true);

	clearMotionButton = new Fl_Button(2 * xDis + 250 + x, yDis*10 + y , 100, 2 * yDis, "Clear Motion");
	clearMotionButton->callback(clearMotion,this);


	plotVectorFlowButton = new Fl_Button(2 * xDis + 350 + x, yDis*10 + y , 100, 2 * yDis, "Plot VectorFlow");
	plotVectorFlowButton->callback(plotVectorFlow,this);

	clearVectorFlowButton = new Fl_Button(2 * xDis + 450 + x, yDis*10 + y , 100, 2 * yDis, "Clear VectorFlow");
	clearVectorFlowButton->callback(clearVectorFlow,this);


	parameterGroup = new Fl_Group(2 * xDis + x , h / 10 + 9 * yDis + y, w - 2 * xDis, 9 * h / 10 - 10 * yDis);
	parameterGroup->box(FL_UP_BOX);
	this->end();
	this->resizable(parameterGroup);	

	errorViz = new ErrorVisualization(this->parameterGroup->x(), this->parameterGroup->y(), parameterGroup->w(), parameterGroup->h(), "");
	parameterGroup->add(errorViz);
	errorViz->show();
	errorViz->redraw();
}

VisualizationView::~VisualizationView()
{
}

void VisualizationView::update()
{
	std::string charName = paWindow->characterList->menu()[paWindow->characterList->value()].label();

	// pass current selected char to mcu for visualization plots
	paWindow->setCurrentCharacterName(charName);

	SbmCharacter* character = SmartBody::SBScene::getScene()->getCharacter(charName);
	if (!character)
		return;

	if (!character->param_animation_ct)
		return;

	std::string currentState = "";
	if (character->param_animation_ct->getCurrentPABlendData())
	{
		currentState = character->param_animation_ct->getCurrentPABlendData()->state->stateName;

		PABlendData* curStateData = character->param_animation_ct->getCurrentPABlendData();		
		SmartBody::SBAnimationBlend* curBlend = dynamic_cast<SmartBody::SBAnimationBlend*>(curStateData->state);
		if (curBlend)
		{
			errorViz->setAnimationState(curBlend);
		}
	}	
	if (prevCycleState != currentState)
	{		
		prevCycleState = currentState;
		currentCycleState->value(currentState.c_str());
		paWindow->redraw();
	}	
	errorViz->redraw();
}

void VisualizationView::buildVisualization()
{
	SmartBody::SBCharacter* sbChar = paWindow->getCurrentCharacter();
	if (!sbChar) return;
	if (!sbChar->param_animation_ct) return;
	PABlendData* blendData = sbChar->param_animation_ct->getCurrentPABlendData();
	if (!blendData) return;
	SmartBody::SBAnimationBlend* curBlend = dynamic_cast<SmartBody::SBAnimationBlend*>(blendData->state);
	if (!curBlend) return;
	std::string surfType = visShapeChoice->text(visShapeChoice->value());
	curBlend->buildVisSurfaces("error",surfType,4,50);
	curBlend->buildVisSurfaces("smooth",surfType,4,50);
	errorViz->setAnimationState(curBlend);
}

void VisualizationView::buildViz( Fl_Widget* widget, void* data )
{
	VisualizationView* vizView = (VisualizationView*)(data);
	vizView->buildVisualization();
}

void VisualizationView::updateVizType( Fl_Widget* widget, void* data )
{
	VisualizationView* vizView = (VisualizationView*)(data);
	std::string drawType = vizView->currentViz->text(vizView->currentViz->value());
	vizView->errorViz->setDrawType(drawType);
}

void VisualizationView::plotMotion(bool randomColor)
{
	SmartBody::SBCharacter* sbChar = paWindow->getCurrentCharacter();
	if (!sbChar) return;
	if (!sbChar->param_animation_ct) return;
	PABlendData* blendData = sbChar->param_animation_ct->getCurrentPABlendData();
	if (!blendData) return;
	SmartBody::SBAnimationBlend* curBlend = dynamic_cast<SmartBody::SBAnimationBlend*>(blendData->state);
	if (!curBlend) return;

	curBlend->setChrPlotMotionTransform(sbChar->getName());

	for(int i=0; i<curBlend->getNumMotions(); i++)
	{
		std::string moName = curBlend->getMotion(i);
		//SkMotion* mo = getSkMotion(moName);
		bool randomColor = plotRandomColorCheckbox->value()? 1 : 0;
		curBlend->plotMotion(moName, sbChar->getName(), 10, false, randomColor);
	}
}
void VisualizationView::plotMotion(Fl_Widget* widget, void* data)
{
	VisualizationView* vizView = (VisualizationView*)(data);
	bool randomColor = vizView->plotRandomColorCheckbox->value()? 1 : 0;
	vizView->plotMotion(randomColor);
}

void VisualizationView::plotJointTraj(const std::string& jntName, bool randomColor)
{
	SmartBody::SBCharacter* sbChar = paWindow->getCurrentCharacter();
	if (!sbChar) return;
	SmartBody::SBAnimationBlend* curBlend = getCurrentBlend();
	if (!curBlend) return;

	curBlend->setChrPlotMotionTransform(sbChar->getName());

	for(int i=0; i<curBlend->getNumMotions(); i++)
	{
		std::string moName = curBlend->getMotion(i);
		curBlend->plotMotionJointTrajectory(moName, sbChar->getName(), jntName, 0.0f, 0.0f, randomColor);
	}
}
void VisualizationView::plotJointTraj(Fl_Widget* widget, void* data)
{
	VisualizationView* vizView = (VisualizationView*)(data);
	int sel = vizView->plotJointChoice->value();
	if(sel >=0 && sel < vizView->plotJointChoice->size())
	{
		std::string jntName = vizView->plotJointChoice->text(sel);
		bool randomColor = vizView->plotRandomColorCheckbox->value()? 1 : 0;
		vizView->plotJointTraj(jntName, randomColor);
	}
	else
	{
		LOG("No joint is selected for trajectory plot! Refresh joint list and select.");
	}
}

void VisualizationView::refreshJointList()
{
	plotJointChoice->clear();

	SmartBody::SBCharacter* sbChar = paWindow->getCurrentCharacter();
	if (!sbChar) return;
	SmartBody::SBSkeleton* sk = sbChar->getSkeleton();
	if (!sk) return;
	const std::vector<SkJoint*>& jnts = sk->joints();
	for(unsigned int i=0; i<jnts.size(); i++)
	{
		std::string jname = jnts[i]->jointName();
		if(jname.compare("face")!=0) continue;
		if(jname.compare("brow")!=0) continue;
		if(jname.compare("eye")!=0)  continue;
		if(jname.compare("nose")!=0) continue;
		if(jname.compare("lid")!=0)  continue;
		if(jname.compare("jaw")!=0)  continue;
		if(jname.compare("tongue")!=0) continue;
		if(jname.compare("lip")!=0)    continue;
		if(jname.compare("cheek")!=0)  continue;
		if(jname.compare("finger")!=0) continue;
		if(jname.compare("thumb")!=0)  continue;
		if(jname.compare("index")!=0)  continue;
		if(jname.compare("middle")!=0) continue;
		if(jname.compare("pinky")!=0)  continue;
		if(jname.compare("ring")!=0)   continue;
		plotJointChoice->add(jnts[i]->jointName().c_str());
	}
}
void VisualizationView::refreshJointList(Fl_Widget* widget, void* data)
{
	VisualizationView* vizView = (VisualizationView*)(data);
	vizView->refreshJointList();
}

SmartBody::SBAnimationBlend* VisualizationView::getCurrentBlend(void)
{
	SmartBody::SBCharacter* sbChar = paWindow->getCurrentCharacter();
	if (!sbChar) return 0;
	if (!sbChar->param_animation_ct) return 0;
	PABlendData* blendData = sbChar->param_animation_ct->getCurrentPABlendData();
	if (!blendData) return 0;
	SmartBody::SBAnimationBlend* curBlend = dynamic_cast<SmartBody::SBAnimationBlend*>(blendData->state);
	return curBlend;
}

void VisualizationView::clearMotion()
{
	SmartBody::SBCharacter* sbChar = paWindow->getCurrentCharacter();
	if (!sbChar) return;
	SmartBody::SBAnimationBlend* curBlend = getCurrentBlend();
	if (!curBlend) return;

	curBlend->clearPlotMotion();
	curBlend->clearPlotMotionTransform();
}
void VisualizationView::clearMotion(Fl_Widget* widget, void* data)
{
	VisualizationView* vizView = (VisualizationView*)(data);
	vizView->clearMotion();
}

void VisualizationView::plotVectorFlow()
{
	SmartBody::SBCharacter* sbChar = paWindow->getCurrentCharacter();
	if (!sbChar) return;
	SmartBody::SBAnimationBlend* curBlend = getCurrentBlend();
	if (!curBlend) return;

	curBlend->setChrPlotVectorFlowTransform(sbChar->getName());

	for(int i=0; i<curBlend->getNumMotions(); i++)
	{
		std::string moName = curBlend->getMotion(i);
		curBlend->createMotionVectorFlow(moName, sbChar->getName());
	}
}
void VisualizationView::plotVectorFlow(Fl_Widget* widget, void* data)
{
	VisualizationView* vizView = (VisualizationView*)(data);
	vizView->plotVectorFlow();
}

void VisualizationView::clearVectorFlow()
{
	SmartBody::SBCharacter* sbChar = paWindow->getCurrentCharacter();
	if (!sbChar) return;
	SmartBody::SBAnimationBlend* curBlend = getCurrentBlend();
	if (!curBlend) return;

	curBlend->clearMotionVectorFlow();
	curBlend->clearPlotVectorFlowTransform();
}
void VisualizationView::clearVectorFlow(Fl_Widget* widget, void* data)
{
	VisualizationView* vizView = (VisualizationView*)(data);
	vizView->clearVectorFlow();
}
