#include "vhcl.h"
#include "ParameterGroup.h"
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>

#include <controllers/me_ct_param_animation.h>
#include "ParameterVisualization.h"
#include "Parameter3DVisualization.h"
#include "PanimationWindow.h"

ParameterGroup::ParameterGroup(int x, int y, int w, int h, char* name, PABlendData* s, PanimationWindow* window, bool ex) : Fl_Group(x, y, w, h, name), blendData(s), paWindow(window), exec(ex)
{
	//printf("Create parameter group, x = %d, y = %d\n",x,y);
	this->label(blendData->state->stateName.c_str());
	this->begin();
		int type = blendData->state->getType();
		if (type == 0)
		{			
			int paraH =  h - 5 * yDis;
			paramVisualization = new ParameterVisualization(true, 4 * xDis + x, yDis + y, w - 5 * xDis, paraH, (char*)"", s, this);
			// since begin() is automatically called by the constructor for Fl_Group
			paramVisualization->end();

			this->resizable(paramVisualization);
			yAxis = NULL;
			zAxis = NULL;
			double min = blendData->state->getVec(blendData->state->getMinVecX()).x;
			double max = blendData->state->getVec(blendData->state->getMaxVecX()).x;
			xAxis = new Fl_Value_Slider(4 * xDis + x, h - 4 * yDis + y, w - 5 * xDis, 2 * yDis, "X");
			xAxis->minimum(min);
			xAxis->maximum(max);			
			xAxis->type(FL_HORIZONTAL);			
			xAxis->callback(updateXAxisValue, this);
			float actualValue;
			blendData->state->getParametersFromWeights(actualValue, blendData->weights);
			int actualX = 0;
			int actualY = 0;
			paramVisualization->getActualPixel(actualValue, 0.0f, actualX, actualY);
			param3DVisualization = NULL;
		}
		else if (type == 1)
		{
			int paraH =  h - 5 * yDis;
			paramVisualization = new ParameterVisualization(true, 4 * xDis + x, yDis + y, w - 5 * xDis, h - 5 * yDis, (char*)"", s, this);
			paramVisualization->end();
			this->resizable(paramVisualization);
			double minX = blendData->state->getVec(blendData->state->getMinVecX()).x;
			double maxX = blendData->state->getVec(blendData->state->getMaxVecX()).x;
			double minY = blendData->state->getVec(blendData->state->getMinVecY()).y;
			double maxY = blendData->state->getVec(blendData->state->getMaxVecY()).y;
			xAxis = new Fl_Value_Slider(4 * xDis + x, h - 4 * yDis + y, w - 5 * xDis, 2 * yDis, "X");
			xAxis->minimum(minX);
			xAxis->maximum(maxX);
			xAxis->type(FL_HORIZONTAL);
			xAxis->callback(updateXYAxisValue, this);
			yAxis = new Fl_Value_Slider(xDis + x, yDis + y, 3 * xDis, h - 5 * yDis, "Y");
			yAxis->minimum(minY);
			yAxis->maximum(maxY);
			yAxis->callback(updateXYAxisValue, this);
			yAxis->type(FL_VERTICAL);
			float actualValueX, actualValueY;
			blendData->state->getParametersFromWeights(actualValueX, actualValueY, blendData->weights);
			int actualX = 0;
			int actualY = 0;
			paramVisualization->getActualPixel(actualValueX, actualValueY, actualX, actualY);
			param3DVisualization = NULL;
		}
		else if (type == 2)
		{
			param3DVisualization = new Parameter3DVisualization(4 * xDis + x, 4 * yDis + y, w - 5 * xDis, h - 8 * yDis, (char*)"", s, this);
			param3DVisualization->end();
			this->resizable(param3DVisualization);	
			paramVisualization = NULL;
			double minX = blendData->state->getVec(blendData->state->getMinVecX()).x;
			double maxX = blendData->state->getVec(blendData->state->getMaxVecX()).x;
			double minY = blendData->state->getVec(blendData->state->getMinVecY()).y;
			double maxY = blendData->state->getVec(blendData->state->getMaxVecY()).y;
			double minZ = blendData->state->getVec(blendData->state->getMinVecZ()).z;
			double maxZ = blendData->state->getVec(blendData->state->getMaxVecZ()).z;
			xAxis = new Fl_Value_Slider(4 * xDis + x, h - 4 * yDis + y, w - 5 * xDis, 2 * yDis, "X");
			xAxis->minimum(minX);
			xAxis->maximum(maxX);
			xAxis->type(FL_HORIZONTAL);
			xAxis->callback(updateXYZAxisValue, this);
			yAxis = new Fl_Value_Slider(xDis + x, yDis + y, 3 * xDis, h - 5 * yDis, "Y");
			yAxis->minimum(minY);
			yAxis->maximum(maxY);
			yAxis->callback(updateXYZAxisValue, this);
			yAxis->type(FL_VERTICAL);
			zAxis = new Fl_Value_Slider(4 * xDis + x, yDis + y, w - 5 * xDis, 2 * yDis, "Z");
			zAxis->minimum(minZ);
			zAxis->maximum(maxZ);
			zAxis->type(FL_HORIZONTAL);
			zAxis->callback(updateXYZAxisValue, this);
		}
		else
		{
			param3DVisualization = NULL;
			paramVisualization = NULL;
		}
	this->end();	
	this->redraw();
	paWindow->redraw();
}

void ParameterGroup::resize(int x, int y, int w, int h)
{
	Fl_Group::resize(x, y, w, h);
}


ParameterGroup::~ParameterGroup()
{
}

void ParameterGroup::updateXAxisValue(Fl_Widget* widget, void* data)
{
	ParameterGroup* group = (ParameterGroup*) data;
	PABlendData* blendData = group->getCurrentPABlendData();
	double w = group->xAxis->value();
	bool success = false;
	success = blendData->state->getWeightsFromParameters(w, blendData->weights);
	if (success)
	{		
		group->updateWeight(blendData->weights);		
	}
// 	if (success)
// 		group->getCurrentCharacter()->param_animation_ct->updateWeights(blendData->weights);
	group->redraw();
}

void ParameterGroup::updateXYAxisValue(Fl_Widget* widget, void* data)
{
	ParameterGroup* group = (ParameterGroup*) data;
	PABlendData* blendData = group->getCurrentPABlendData();
	double x = group->xAxis->value();
	double y = group->yAxis->value();
	bool success = false;
	std::vector<double> weights;
	weights.resize(blendData->state->getNumMotions());
	success = blendData->state->getWeightsFromParameters(x, y, weights);
	if (success)
	{
		blendData->weights = weights;
		group->updateWeight(weights);		
	}
	//if (success)
	//	group->getCurrentCharacter()->param_animation_ct->updateWeights(weights);
	group->redraw();
}

void ParameterGroup::updateXYZAxisValue(Fl_Widget* widget, void* data)
{		
	ParameterGroup* group = (ParameterGroup*) data;
	PABlendData* blendData = group->getCurrentPABlendData();
	double x = group->xAxis->value();
	double y = group->yAxis->value();
	double z = group->zAxis->value();
	bool success = false;
	std::vector<double> weights;
	weights.resize(blendData->state->getNumMotions());
	success = blendData->state->getWeightsFromParameters(x, y, z, weights);
	if (success)
	{
		blendData->weights = weights;
		group->updateWeight(weights);		
	}
	//if (success)
	//	group->getCurrentCharacter()->param_animation_ct->updateWeights(weights);
	group->redraw();	
}

void ParameterGroup::updateWeight(std::vector<double>& weights)
{
//	if (!state->cycle)
//		return;
	std::string charName = paWindow->characterList->menu()[paWindow->characterList->value()].label();
	std::stringstream command;
	command << "panim update char " << charName;
	int wNumber = blendData->state->getNumMotions();
	for (unsigned int j = 0; j < weights.size(); j++)
		command << " " << weights[j];
	//LOG("pgroup, weight size = %d",weights.size());
	paWindow->execCmd(paWindow, command.str());
	
}

PABlendData* ParameterGroup::getCurrentPABlendData()
{
	std::string charName = paWindow->characterList->menu()[paWindow->characterList->value()].label();
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(charName);
	if (!character)
		return NULL;
	if (!character->param_animation_ct)
		return NULL;
	return character->param_animation_ct->getCurrentPABlendData();
}

SmartBody::SBCharacter* ParameterGroup::getCurrentCharacter()
{
	std::string charName = paWindow->characterList->menu()[paWindow->characterList->value()].label();
	return SmartBody::SBScene::getScene()->getCharacter(charName);
}
