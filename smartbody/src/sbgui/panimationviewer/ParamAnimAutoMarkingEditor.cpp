#include "vhcl.h"
#include "ParamAnimAutoMarkingEditor.h"
#include <sb/SBCharacter.h>
#include <sb/SBJoint.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sb/SBMotion.h>
#include "ParamAnimEditorWidget.h"
#include "ParamAnimStateEditor.h"

PAAutoFootStepsEditor::PAAutoFootStepsEditor(PABlendEditor* editor, int x, int y, int w, int h) : Fl_Window(x, y, w, h)
{
	set_modal();
	stateEditor = editor;
	xDis = 10;
	yDis = 10;
	int csx = xDis;
	int csy = yDis;	
	isPrintDebugInfo = false;

	this->label("Auto Footsteps Editor");
	this->begin();
	inputFloorHeight = new Fl_Input(xDis + csx + 100, yDis, 10 * xDis, 2 * yDis, "FloorHeight");
	inputFloorHeight->value("0");
	inputHeightThreshold = new Fl_Input(xDis + csx + 100, 4 * yDis, 10 * xDis, 2 * yDis, "HeightThreshold");
	std::stringstream inputHeightSS;
	inputHeightSS << 0.15f / SmartBody::SBScene::getScene()->getScale();
	inputHeightThreshold->value(inputHeightSS.str().c_str());
	inputSpeedThreshold = new Fl_Input(xDis + csx + 100, 7 * yDis, 10 * xDis, 2 * yDis, "SpeedThreshold");
	std::stringstream inputSpeedSS;
	inputSpeedSS << 0.4f / SmartBody::SBScene::getScene()->getScale();
	inputSpeedThreshold->value(inputSpeedSS.str().c_str());

	inputSpeedDetectWindow = new Fl_Input(xDis + csx + 100, 10 * yDis, 10 * xDis, 2 * yDis, "SpeedDetectWindow");
	inputSpeedDetectWindow->value("3");
	inputSpeedDetectWindow->deactivate();
	
	browserJoint = new Fl_Multi_Browser(xDis + csx + 100, 13 * yDis, 28 * xDis, 20 * yDis, "Joint");
	SmartBody::SBCharacter* character = stateEditor->paWindow->getCurrentCharacter();
	if (character)
	{
		const std::vector<std::string>& charJNames = character->getSkeleton()->getJointNames();
		for (size_t i = 0; i < charJNames.size(); i++)
			browserJoint->add(charJNames[i].c_str());
	}

	inputStepsPerJoint = new Fl_Input(xDis + csx + 100, 36 * yDis, 10 * xDis, 2 * yDis, "StepsPerJoint");
	PABlend* curState = stateEditor->getCurrentState();
	std::vector<std::string> selectedMotions = stateEditor->getSelectedMotions();
	if (selectedMotions.size() == curState->getNumMotions())
		inputStepsPerJoint->activate();
	else
		inputStepsPerJoint->deactivate();
	int stepsPerJoint = 2;
	std::stringstream ss;
	ss << stepsPerJoint;
	inputStepsPerJoint->value(ss.str().c_str());


	browserSelectedMotions = new Fl_Browser(xDis + csx + 100, 39 * yDis, 28 * xDis, 10 * yDis, "SelectedMotions");
	browserSelectedMotions->deactivate();
	refreshSelectedMotions();

	checkDebugInfo = new Fl_Check_Button(xDis + csx + 100, 51 * yDis, 10 * xDis, 2 * yDis, "DumpDetailInformation");

	buttonConfirm = new Fl_Button(xDis + csx, 55 * yDis, 10 * xDis, 2 * yDis, "Apply");
	buttonConfirm->callback(confirmEditting, this);
	buttonCancel = new Fl_Button(xDis + csx + 120, 55 * yDis, 10 * xDis, 2 * yDis, "Leave");
	buttonCancel->callback(cancelEditting, this);
	this->end();
}


PAAutoFootStepsEditor::~PAAutoFootStepsEditor()
{
}

void PAAutoFootStepsEditor::confirmEditting(Fl_Widget* widget, void* data)
{
	PAAutoFootStepsEditor* footStepEditor = (PAAutoFootStepsEditor*) data;
	
	PABlend* currentState = footStepEditor->stateEditor->getCurrentState();
	if (!currentState)
	{	
		LOG("PAAutoFootStepsEditor::confirmEditting WARNING: please select a state!");
		return;
	}

	// take down previous correspondence points first
	footStepEditor->stateEditor->previousKeys.clear();
	footStepEditor->stateEditor->previousKeys.resize(currentState->getNumMotions());
	for (int i = 0; i < currentState->getNumMotions(); i++)
	{
		footStepEditor->stateEditor->previousKeys[i].resize(currentState->getNumKeys());
		footStepEditor->stateEditor->previousKeys[i] = currentState->keys[i];
	}

	// auto foot steps algorithm
	float floorHeight = (float)atof(footStepEditor->inputFloorHeight->value());
	float heightThresh = (float)atof(footStepEditor->inputHeightThreshold->value());
	float speedThresh = (float)atof(footStepEditor->inputSpeedThreshold->value());
	int speedWindow = atoi(footStepEditor->inputSpeedDetectWindow->value());
	int stepsPerJoint = atoi(footStepEditor->inputStepsPerJoint->value());
	if (stepsPerJoint < 1)
		stepsPerJoint = 1;
	int checkDebugInfoVal = footStepEditor->checkDebugInfo->value();
	if (checkDebugInfoVal == 0)
		footStepEditor->isPrintDebugInfo = false;
	if (checkDebugInfoVal == 1)
		footStepEditor->isPrintDebugInfo = true;

	const std::vector<std::string>& selectedMotions = footStepEditor->stateEditor->getSelectedMotions();
	SmartBody::SBCharacter* curCharacter = footStepEditor->stateEditor->paWindow->getCurrentCharacter();
	std::vector<std::string> selectedJoints;
	for (int i = 0; i < footStepEditor->browserJoint->size(); i++)
	{
		if (footStepEditor->browserJoint->selected(i+1))
		{
			selectedJoints.push_back(footStepEditor->browserJoint->text(i + 1));
		}
	}
	if (selectedJoints.size() == 0)
	{
		fl_alert("Please select at least one joint.");
		return;
	}

	std::stringstream finalMessage;
	std::vector<std::string> motionsNeedManualAdjusting;
	finalMessage << "Current State: " << currentState->stateName << "\n";

	bool isConvergent = true;
	for (size_t m = 0; m < selectedMotions.size(); m++)
	{
		// shared
		std::vector<double> possibleTiming;

		// divided
		std::vector<std::vector<double> > vecOutMeans;
		vecOutMeans.resize(selectedJoints.size());
		std::vector<std::vector<double> > vecTiming;
		vecTiming.resize(selectedJoints.size());

		SmartBody::SBMotion* motion = SmartBody::SBScene::getScene()->getMotion(selectedMotions[m]);
		if (!motion)
			continue;

#if 1 // feng : I tried to refactor the footstep detection into a API function in SBMotion. 
	  // Still contains too many parameters to be used as an API. But would clean it up more and see if we can come up with 
	  // a version that can also detect the number of steps as the input. If this is interfering with the editor, set this back to 0.	  
		std::vector<double> outMeans;
		int maxNumSteps = footStepEditor->stateEditor->getCurrentState()->getNumKeys();
		if (!footStepEditor->isProcessAll)
		{
			stepsPerJoint = maxNumSteps / selectedJoints.size();
		}
		else
		{
			maxNumSteps = stepsPerJoint * selectedJoints.size();
		}

// 		std::vector<FootStepRecord> footSteps;
// 		motion->autoFootPlantDetection(curCharacter->getSkeleton(),selectedJoints,floorHeight, heightThresh,speedThresh,footSteps);
// 
// 		for (int i=0;i<footSteps.size();i++)
// 		{
// 			FootStepRecord& record = footSteps[i];
// 			LOG("Footstep joint = %s, start frame = %f, end frame = %f",record.jointName.c_str(), record.startTime, record.endTime);
// 		}


		isConvergent = motion->autoFootStepDetection(outMeans, stepsPerJoint, maxNumSteps, curCharacter->getSkeleton(), selectedJoints,
			                          floorHeight, heightThresh, speedThresh, speedWindow, footStepEditor->isPrintDebugInfo);

#else 
		motion->connect(curCharacter->getSkeleton());

		for(int f = 0; f < motion->getNumFrames(); f++)
		{
			motion->apply_frame(f);
			motion->connected_skeleton()->update_global_matrices();

			for (size_t jointId = 0; jointId < selectedJoints.size(); jointId ++)
			{
				std::string jointName = selectedJoints[jointId];
				SBJoint* joint = curCharacter->getSkeleton()->getJointByName(jointName);
				if (!joint)
					continue;

				// get height
				const SrMat& gMat = joint->gmat();
				SrVec gPos = SrVec(gMat.get(12), gMat.get(13), gMat.get(14));

				// get speed
				int startFrame = f - speedWindow / 2;
				int endFrame = f + speedWindow / 2;
				float startTime = startFrame * (float)motion->getFrameRate();
				float endTime = endFrame * (float)motion->getFrameRate();
				float speed = motion->getJointSpeed(joint, startTime, endTime);
				
				// print info
				if (footStepEditor->isPrintDebugInfo)
					LOG("motion %s at time %f-> speed is %f, height is %f, joint is %s", motion->getName().c_str(), f * motion->getFrameRate(), speed, gPos.y, jointName.c_str());

				// filter for height
				if (gPos.y < floorHeight || gPos.y > (floorHeight + heightThresh))
					continue;

				// filter speed
				if (speed <= speedThresh)
				{
					vecTiming[jointId].push_back(f * (float)motion->getFrameRate());
					possibleTiming.push_back(f * (float)motion->getFrameRate());
				}
			}
		}

		// K means algorithm according to desired number of steps
		std::vector<double> outMeans;
		int maxNumSteps = footStepEditor->stateEditor->getCurrentState()->getNumKeys();
		if (!footStepEditor->isProcessAll)
		{
			stepsPerJoint = maxNumSteps / selectedJoints.size();
		}
		else
		{
			maxNumSteps = stepsPerJoint * selectedJoints.size();
		}

		/*
		int numSteps = footStepEditor->stateEditor->getCurrentState()->getNumKeys();
		isConvergent = footStepEditor->kMeansClustering1D(numSteps, possibleTiming, outMeans);
		*/

		for (size_t jointId = 0; jointId < selectedJoints.size(); jointId++)
		{
			if (jointId == (selectedJoints.size() - 1) && !footStepEditor->isProcessAll)
			{
				int mod = footStepEditor->stateEditor->getCurrentState()->getNumKeys() % selectedJoints.size();
				stepsPerJoint += mod;
			}

			bool retBoolean = footStepEditor->kMeansClustering1D(stepsPerJoint, vecTiming[jointId], vecOutMeans[jointId]);
			if (!retBoolean)
			{
				isConvergent = false;
				break;
			}
		}
		if (isConvergent)
		{
			outMeans.clear();
			for (size_t joinId = 0; joinId < selectedJoints.size(); joinId++)
			{
				for (size_t meanId = 0; meanId < vecOutMeans[joinId].size(); meanId++)
					outMeans.push_back(vecOutMeans[joinId][meanId]);
			}
			std::sort(outMeans.begin(), outMeans.end());
		}
#endif

		// apply it to corresponding points
		// also appending starting and ending corresponding points
		int motionIndex = currentState->getMotionId(selectedMotions[m]);
		if (isConvergent)
		{
			std::stringstream ss;
			ss << "[" << motion->getName() << "]detected ";
			for (size_t i = 0; i < outMeans.size(); i++)
				ss << outMeans[i] << " ";
			LOG("%s", ss.str().c_str());
			finalMessage << ss.str() << "\n";
			currentState->keys[motionIndex].clear();
			if (footStepEditor->isProcessAll)
				currentState->keys[motionIndex].push_back(0);
			for (size_t i = 0; i < outMeans.size(); i++)
				currentState->keys[motionIndex].push_back(outMeans[i]);
			if (footStepEditor->isProcessAll)
				currentState->keys[motionIndex].push_back(motion->getDuration());
		}
		else
		{
			motionsNeedManualAdjusting.push_back(motion->getName());
			std::stringstream ss;
			ss << "[" << motion->getName() << "]NOT detected(evenly distrubted): ";
			int actualNum = maxNumSteps;
			currentState->keys[motionIndex].clear();
			if (footStepEditor->isProcessAll)
				actualNum += 2;
			for (int i = 0; i < actualNum; i++)
			{
				double step = motion->getDuration() / double(actualNum - 1);
				currentState->keys[motionIndex].push_back(step * i);
				ss << step * i << " ";
			}
			LOG("%s", ss.str().c_str());
			finalMessage << ss.str() << "\n";
		}
		motion->disconnect();
	}

	// print out final message
	finalMessage << "\n\n=======Summary=======\n";
	for (size_t m = 0; m < motionsNeedManualAdjusting.size(); m++)
	{
		finalMessage << motionsNeedManualAdjusting[m] << " may need manual adjusting\n";
	}
	fl_message("%s", finalMessage.str().c_str());

	// refresh state editor
	footStepEditor->stateEditor->buttonUndoAutoFootSteps->activate();
	footStepEditor->stateEditor->refresh();
	footStepEditor->hide();	
}

void PAAutoFootStepsEditor::cancelEditting(Fl_Widget* widget, void* data)
{
	PAAutoFootStepsEditor* footStepEditor = (PAAutoFootStepsEditor*) data;
	footStepEditor->stateEditor->refresh();
	footStepEditor->stateEditor->autoFootStepsEditor = NULL;
	footStepEditor->hide();
	delete footStepEditor;
}

void PAAutoFootStepsEditor::refreshSelectedMotions()
{
	browserSelectedMotions->clear();
	const std::vector<std::string>& selectedMotions = stateEditor->getSelectedMotions();
	for (size_t i = 0; i < selectedMotions.size(); i++)
		browserSelectedMotions->add(selectedMotions[i].c_str());

	if (stateEditor->getCurrentState()->getNumMotions() == stateEditor->getSelectedMotions().size())
	{
		inputStepsPerJoint->activate();
		isProcessAll = true;
	}
	else
	{
		inputStepsPerJoint->deactivate();
		isProcessAll = false;
	}
}
