#include "vhcl.h"
#include "ParamAnimParameterEditor.h"
#include <sb/SBAnimationState.h>
#include <sb/SBCharacter.h>
#include <sb/SBScene.h>
#include <sb/SBJoint.h>
#include <sb/SBSkeleton.h>
#include <sb/SBMotion.h>
#include "ParamAnimStateEditor.h"

PAParameterEditor::PAParameterEditor(PABlendEditor* editor, int x, int y, int w, int h) : Fl_Window(x, y, w, h)
{
	set_modal();
	stateEditor = editor;
	this->label("Edit Parameter");
	this->begin();
	xDis = 10;
	yDis = 10;
	int csx = xDis;
	int csy = 4 * yDis;	

	stateName = new Fl_Input(xDis + csx + 100, yDis, 10 * xDis, 2 * yDis, "State Name");
	stateName->value(editor->getCurrentState()->stateName.c_str());
	stateName->deactivate();

	choiceDimension = new Fl_Choice(xDis + csx + 100, 4 * yDis, 10 * xDis, 2 * yDis, "Dimension");
	PABlend* currentState = editor->getCurrentState();
	SmartBody::SBAnimationBlend1D* state1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(currentState);
	SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(currentState);
	SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(currentState);
	if (state1D || state2D || state3D)
	{
		choiceDimension->add("X");
	}
	if (state2D || state3D)
	{
		choiceDimension->add("Y");
	}
	if (state3D)
	{
		choiceDimension->add("Z");
	}
	choiceDimension->value(0);

	choiceParameter = new Fl_Choice(xDis + csx + 100, 7 * yDis, 10 * xDis, 2 * yDis, "Motion Parameter");
	choiceParameter->add("JointSpeed");
	choiceParameter->add("JointSpeedAxis");
	choiceParameter->add("JointAngularSpeed");
	choiceParameter->add("JointAngularSpeedAxis");
	choiceParameter->add("JointTransition");
	choiceParameter->callback(changeParameter, this);
	choiceParameter->value(0);

	inputJoint = new Fl_Choice(xDis + csx + 100, 10 * yDis, 10 * xDis, 2 * yDis, "Joint");
	SmartBody::SBCharacter* character = stateEditor->paWindow->getCurrentCharacter();
	if (character)
	{
		const std::vector<std::string>& charJNames = character->getSkeleton()->getJointNames();
		for (size_t i = 0; i < charJNames.size(); i++)
			inputJoint->add(charJNames[i].c_str());
		inputJoint->value(0);
	}

	choiceAxis = new Fl_Choice(xDis + csx + 100, 13 * yDis, 10 * xDis, 2 * yDis, "Axis");
	choiceAxis->add("X");
	choiceAxis->add("Y");
	choiceAxis->add("Z");
	choiceAxis->value(0);
	choiceAxis->deactivate();

	buttonConfirm = new Fl_Button(xDis + csx, 16 * yDis, 10 * xDis, 2 * yDis, "Apply");
	buttonConfirm->callback(confirmEditting, this);

	buttonCancel = new Fl_Button(xDis + csx + 120, 16 * yDis, 10 * xDis, 2 * yDis, "Leave");
	buttonCancel->callback(cancelEditting, this);
	this->end();
}

PAParameterEditor::~PAParameterEditor()
{
}

void PAParameterEditor::changeParameter(Fl_Widget* widget, void* data)
{
	PAParameterEditor* paramEditor = (PAParameterEditor*) data;
	std::string selectedParameter = paramEditor->choiceParameter->text();
	if (selectedParameter == "JointSpeedAxis" || selectedParameter == "JointAngularSpeedAxis" || selectedParameter == "JointTransition")
	{
		paramEditor->choiceAxis->activate();
	}
	else
	{
		paramEditor->choiceAxis->deactivate();
	}
}

void PAParameterEditor::confirmEditting(Fl_Widget* widget, void* data)
{
	PAParameterEditor* paramEditor = (PAParameterEditor*) data;
	std::string selectedParameter = paramEditor->choiceParameter->text();
	std::string jointName = paramEditor->inputJoint->text();
	SmartBody::SBCharacter* curCharacter = paramEditor->stateEditor->paWindow->getCurrentCharacter();
	SmartBody::SBJoint* joint = curCharacter->getSkeleton()->getJointByName(jointName);
	if (!joint)
	{
		LOG("PAParameterEditor::confirmEditting WARNING: %s not found!", jointName.c_str());
		return;
	}

	std::string axisName = paramEditor->choiceAxis->text();
	std::string dimensionName = paramEditor->choiceDimension->text();

	// update the parameter changes to state
	PABlend* currentState = paramEditor->stateEditor->getCurrentState();
	if (!currentState)
		return;

	SmartBody::SBAnimationBlend1D* state1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(currentState);
	SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(currentState);
	SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(currentState);
	const std::vector<std::string>& selectedMotions = paramEditor->stateEditor->getSelectedMotions();
	
	for (size_t i = 0; i < selectedMotions.size(); i++)
	{
		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(selectedMotions[i]);
		motion->connect(curCharacter->getSkeleton());
		
		// get start time and end time
		int index = currentState->getMotionId(selectedMotions[i]);
		if (index < 0)
			continue;
		std::vector<double>& key = currentState->keys[index];
		float startTime = 0.0f;
		float endTime = (float)motion->getDuration();
		if (key.size() > 1)
		{
			startTime = (float)key[0];
			endTime = (float)key[key.size() - 1];
		}
		float param = 0.0f;

		if (selectedParameter == "JointSpeed")
		{
			param = motion->getJointSpeed(joint, startTime, endTime);
		}
		if (selectedParameter == "JointSpeedAxis")
		{
			param = motion->getJointSpeedAxis(joint, axisName, startTime, endTime);
		}
		if (selectedParameter == "JointAngularSpeed")
		{
			param = motion->getJointAngularSpeed(joint, startTime, endTime);
		}
		if (selectedParameter == "JointAngularSpeedAxis")
		{
			param = motion->getJointAngularSpeedAxis(joint, axisName, startTime, endTime);
		}
		if (selectedParameter == "JointTransition")
		{
			if (axisName == "X")
				param = motion->getJointTransition(joint, startTime, endTime)[0];
			if (axisName == "Y")
				param = motion->getJointTransition(joint, startTime, endTime)[1];
			if (axisName == "Z")
				param = motion->getJointTransition(joint, startTime, endTime)[2];
		}
		// set the parameters
		double x, y, z;
		if (state1D)
		{
			if (dimensionName == "X")
				state1D->setParameter(selectedMotions[i], param);
		}
		if (state2D)
		{
			state2D->getParameter(selectedMotions[i], x, y);
			if (dimensionName == "X")
				state2D->setParameter(selectedMotions[i], param, (float)y);
			if (dimensionName == "Y")
				state2D->setParameter(selectedMotions[i], (float)x, param);
		}
		if (state3D)
		{
			state3D->getParameter(selectedMotions[i], x, y, z);
			if (dimensionName == "X")
				state3D->setParameter(selectedMotions[i], param, (float)y, (float)z);
			if (dimensionName == "Y")
				state3D->setParameter(selectedMotions[i], (float)x, param, (float)z);
			if (dimensionName == "Z")
				state3D->setParameter(selectedMotions[i], (float)x, (float)y, param);
		}
		motion->disconnect();
	}
}

void PAParameterEditor::cancelEditting(Fl_Widget* widget, void* data)
{
	PAParameterEditor* paramEditor = (PAParameterEditor*) data;
	paramEditor->stateEditor->refresh();
	paramEditor->stateEditor->parameterEditor = NULL;
	paramEditor->hide();
	delete paramEditor;
}
