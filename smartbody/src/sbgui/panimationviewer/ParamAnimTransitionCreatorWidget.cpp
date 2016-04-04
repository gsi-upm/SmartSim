#include "vhcl.h"
#include "ParamAnimTransitionCreatorWidget.h"
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBScene.h>

int PATransitionCreator::lastNameIndex = 0;

PATransitionCreator::PATransitionCreator(PATransitionEditor2* editor, int x, int y, int w, int h) : Fl_Window(x, y, w, h)
{
	set_modal();
	isCreateMode = true;
	transitionEditor = editor;
	xDis = 10;
	yDis = 10;
	this->label("Edit State");
	this->begin();

	int csx = xDis;
	int csy = 4 * yDis;

	inputStateName = new Fl_Input(xDis + csx + 20, yDis, 10 * xDis, 2 * yDis, "Name");
	choiceStateType = new Fl_Choice(xDis + csx + 60, 2 * yDis + csy, 150, 20, "State Type");
	choiceStateType->add("0D");
	choiceStateType->add("1D");
	choiceStateType->add("2D");
	choiceStateType->add("3D");
	choiceStateType->value(0);

	
	animationList = new Fl_Multi_Browser(xDis + csx, 4 * yDis + csy, 350, 250, "All Motions");
	stateAnimationList = new Fl_Multi_Browser(xDis + csx + 420, 4 * yDis + csy, 350, 250, "Motions in State");
	animationAdd = new Fl_Button(xDis + csx + 360, 4 * yDis + csy + 50, 50, 20, ">>>");
	animationAdd->callback(addMotion, this);
	animationRemove = new Fl_Button(xDis + csx + 360, 4 * yDis + csy + 100, 50, 20,  "<<<");
	animationRemove->callback(removeMotion, this);

	buttonCreateState = new Fl_Button(xDis + csx, 4 * yDis + csy + 300, 100, 20, "Create State");
	buttonCreateState->callback(createState, this);
	
	buttonCancelState = new Fl_Button(xDis + csx + 100, 4 * yDis + csy + 300, 60, 20, "Cancel");
	buttonCancelState->callback(cancelState, this);

	this->end();
}

PATransitionCreator::~PATransitionCreator()
{
}

void PATransitionCreator::setInfo(bool isCreateMode, const std::string& stateName)
{
	if (isCreateMode)
	{
		inputStateName->value(getUniqueStateName("state").c_str());
		inputStateName->activate();
		buttonCreateState->label(strdup("Create State"));
	}
	else
	{
		inputStateName->value(stateName.c_str());
		inputStateName->deactivate();
		buttonCreateState->label(strdup("Save Changes"));
	}

	loadMotions();
	if (!isCreateMode)
	{
		SmartBody::SBAnimationBlendManager* stateManager = SmartBody::SBScene::getScene()->getBlendManager();
		SmartBody::SBAnimationBlend* state = stateManager->getBlend(stateName);
		if (!state)
		{
			fl_alert("State %s does not exist.", stateName.c_str());
			cancelState(this, NULL);
			return;
		}
		std::vector<SmartBody::SBMotion*>& motions = state->motions;
		for (std::vector<SmartBody::SBMotion*>::iterator iter = motions.begin();
			 iter != motions.end();
			 iter++)
		{
			stateAnimationList->add((*iter)->getName().c_str());
		}


		SmartBody::SBAnimationBlend0D* state0d = dynamic_cast<SmartBody::SBAnimationBlend0D*>(state);
		if (state0d)
			choiceStateType->value(0);
		SmartBody::SBAnimationBlend1D* state1d = dynamic_cast<SmartBody::SBAnimationBlend1D*>(state);
		if (state1d)
			choiceStateType->value(1);
		SmartBody::SBAnimationBlend2D* state2d = dynamic_cast<SmartBody::SBAnimationBlend2D*>(state);
		if (state2d)
			choiceStateType->value(2);
		SmartBody::SBAnimationBlend3D* state3d = dynamic_cast<SmartBody::SBAnimationBlend3D*>(state);
		if (state3d)
			choiceStateType->value(3);


	}
}

void PATransitionCreator::loadMotions()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	std::vector<std::string> motionNames = scene->getMotionNames();
	std::map<std::string, SkMotion*>::iterator iter;
	for (std::vector<std::string>::iterator iter = motionNames.begin();
		iter != motionNames.end();
		iter++)
		animationList->add(iter->c_str());
}

void PATransitionCreator::addMotion(Fl_Widget* widget, void* data)
{
	PATransitionCreator* creator = (PATransitionCreator*) data;
	for (int i = 0; i < creator->animationList->size(); i++)
	{
		if (creator->animationList->selected(i+1))
		{
			bool shouldAdd = true;
			for (int j = 0; j < creator->stateAnimationList->size(); j++)
			{
				if (strcmp(creator->stateAnimationList->text(j+1), creator->animationList->text(i+1)) == 0)
				{
					shouldAdd = false;
					break;
				}
			}
			if (shouldAdd)
				creator->stateAnimationList->add(creator->animationList->text(i+1));
		}
	}
}

void PATransitionCreator::removeMotion(Fl_Widget* widget, void* data)
{
	PATransitionCreator* creator = (PATransitionCreator*) data;

	std::vector<std::string> selectedMotions;
	for (int i = 0; i < creator->stateAnimationList->size(); i++)
	{
		if (creator->stateAnimationList->selected(i+1))
		{
			selectedMotions.push_back(creator->stateAnimationList->text(i + 1));
			creator->stateAnimationList->remove(i + 1);
			i--;
		}
	}
}

std::string PATransitionCreator::getUniqueStateName(std::string prefix)
{
	SmartBody::SBAnimationBlendManager* stateManager = SmartBody::SBScene::getScene()->getBlendManager();
	
	std::stringstream strstr;

	bool isUnique = false;
	while (!isUnique)
	{
		isUnique = true;

        strstr.str("");
		strstr << prefix << lastNameIndex;
		lastNameIndex++;

		SmartBody::SBAnimationBlend* state = stateManager->getBlend(strstr.str());
		if (state)
			lastNameIndex++;
	}

	return strstr.str();
} 

void PATransitionCreator::createState(Fl_Widget* widget, void* data)
{
	PATransitionCreator* creator = (PATransitionCreator*) data;

	// get the animation type and name
	std::string type = creator->choiceStateType->menu()[creator->choiceStateType->value()].label();
	std::string stateName = creator->inputStateName->value();

	// make sure the name is valid
	SmartBody::SBAnimationBlendManager* stateManager = SmartBody::SBScene::getScene()->getBlendManager();
	SmartBody::SBAnimationBlend* state = stateManager->getBlend(stateName);

	if (creator->isCreateMode)
	{
		if (state)
		{
			fl_alert("State name %s already exists. Please choose a different name.", stateName.c_str());
			return;
		}

		if (type == "0D")
		{
			SmartBody::SBAnimationBlend0D* animState = stateManager->createBlend0D(stateName);
			
			for (int i = 0; i < creator->stateAnimationList->size(); i++)
			{
				animState->addMotion(creator->stateAnimationList->text(i + 1));
			}
		}
		else if (type == "1D")
		{
			SmartBody::SBAnimationBlend1D* animState = stateManager->createBlend1D(stateName);

			for (int i = 0; i < creator->stateAnimationList->size(); i++)
			{
				animState->addMotion(creator->stateAnimationList->text(i + 1), 0);
			}

			
		}
		else if (type == "2D")
		{
			SmartBody::SBAnimationBlend2D* animState = stateManager->createBlend2D(stateName);

			for (int i = 0; i < creator->stateAnimationList->size(); i++)
			{
				animState->addMotion(creator->stateAnimationList->text(i + 1), 0, 0);
			}
		}
		else if (type == "3D")
		{
			SmartBody::SBAnimationBlend3D* animState = stateManager->createBlend3D(stateName);

			for (int i = 0; i < creator->stateAnimationList->size(); i++)
			{
				animState->addMotion(creator->stateAnimationList->text(i + 1), 0, 0, 0);
			}
		}
	}
	else // edit mode
	{
		// compare motions in state against motions list and add or remove accordingly
		std::vector<std::string> updatedMotions;
		for (int i = 0; i < creator->stateAnimationList->size(); i++)
		{
			updatedMotions.push_back(creator->stateAnimationList->text(i + 1));
		}

		std::vector<SmartBody::SBMotion*>& motions = state->motions;

		std::set<std::string> updatedMap;
		for (size_t u = 0; u < updatedMotions.size(); u++)
		{
			updatedMap.insert(updatedMotions[u]);
		}

		std::set<std::string> motionMap;
		for (size_t m = 0; m < motions.size(); m++)
		{
			motionMap.insert(motions[m]->getName());
		}

		for (std::set<std::string>::iterator newMotionIter = updatedMap.begin();
			 newMotionIter != updatedMap.end();
			 newMotionIter++)
		{
			std::set<std::string>::iterator foundIter = motionMap.find((*newMotionIter));
			if (foundIter == motionMap.end())
			{
				// new motion to be added to the state
				SmartBody::SBAnimationBlend0D* state0D = dynamic_cast<SmartBody::SBAnimationBlend0D*>(state);	
				SmartBody::SBAnimationBlend1D* state1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(state);
				SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(state);
				SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(state);
				if (state0D)
				{
					state0D->addMotion((*newMotionIter));
				}
				else if (state1D)
				{
					state1D->addMotion((*newMotionIter), 0);
				}
				else if (state2D)
				{
					state2D->addMotion((*newMotionIter), 0, 0);
				}
				else if (state3D)
				{
					state3D->addMotion((*newMotionIter), 0, 0, 0);
				}
			}
		}

		for (std::set<std::string>::iterator toDeleteIter = motionMap.begin();
			 toDeleteIter != motionMap.end();
			 toDeleteIter++)
		{
			std::set<std::string>::iterator foundIter = updatedMap.find((*toDeleteIter));
			if (foundIter == updatedMap.end())
			{
				// motion to be removed from the state
				SmartBody::SBAnimationBlend0D* state0D = dynamic_cast<SmartBody::SBAnimationBlend0D*>(state);	
				SmartBody::SBAnimationBlend1D* state1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(state);
				SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(state);
				SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(state);
				if (state0D)
				{
					state0D->removeMotion((*toDeleteIter));
				}
				else if (state1D)
				{
					state1D->removeMotion((*toDeleteIter));
				}
				else if (state2D)
				{
					state2D->removeMotion((*toDeleteIter));
				}
				else if (state3D)
				{
					state3D->removeMotion((*toDeleteIter));
				}

			}
		}
	}

	// bug in FLTK? Cannot set value to the last choice item.
	//int numStates = creator->transitionEditor->stateList->size();
	//creator->transitionEditor->stateList->value(numStates - 1);
	
	creator->transitionEditor->state2List->value(0);
	creator->transitionEditor->refresh();
	creator->hide();
	
}

void PATransitionCreator::cancelState(Fl_Widget* widget, void* data)
{
	PATransitionCreator* creator = (PATransitionCreator*) data;
	creator->transitionEditor->creator = NULL;
	creator->hide();
	delete creator;
}
