#include "vhcl.h"
#include "FaceShiftViewer.h"
#include <FL/Fl_Value_Slider.H>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sbm/action_unit.hpp>
#include <sbm/lin_win.h>
#include <sb/SBVHMsgManager.h>
#include <sb/SBFaceDefinition.h>
#include <sb/SBFaceShiftManager.h>

FaceShiftViewer::FaceShiftViewer(int x, int y, int w, int h, char* name) : GenericViewer(x, y, w, h), Fl_Double_Window(x, y, w, h, name), SBWindowListener()
{
	begin();

	topGroup = new Fl_Group(0, 0, w, 40);
	topGroup->begin();	

	topGroup->end();

	bottomGroup = new Fl_Scroll(0, 45, w, h - 40);
	bottomGroup->begin();	
	bottomGroup->box(FL_DOWN_BOX);
		
	bottomGroup->end();	


	end();
	this->resizable(bottomGroup);

//	FaceShiftViewer::RefreshCB(this, this);
}

FaceShiftViewer::~FaceShiftViewer()
{
}
/*
void FaceShiftViewer::CharacterCB(Fl_Widget* widget, void* data)
{
	FaceShiftViewer* faceViewer = (FaceShiftViewer*) data;

	faceViewer->bottomGroup->clear();
	faceViewer->_sliders.clear();
	faceViewer->_weights.clear();

	int curY = faceViewer->bottomGroup->y() + 25;
	const Fl_Menu_Item* menu = faceViewer->choiceCharacters->menu();
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(menu[faceViewer->choiceCharacters->value()].label());
	if (character)
	{	
		SmartBody::SBFaceDefinition* faceDefinition = character->getFaceDefinition();
		if (!faceDefinition)
		{
			faceViewer->redraw();
			return;
		}

		const std::vector<int>& auNums = faceDefinition->getAUNumbers();
		for (size_t a = 0; a < auNums.size(); a++)
		{
			ActionUnit* au = faceDefinition->getAU(auNums[a]);
			if (au->is_left())
			{
				Fl_Value_Slider* slider = new Fl_Value_Slider(100, curY, 150, 25, _strdup(au->getLeftName().c_str()));
				slider->type(FL_HORIZONTAL);
				slider->align(FL_ALIGN_LEFT);
				slider->range(0.0, 1.0);
				faceViewer->bottomGroup->add(slider);
				faceViewer->_sliders.push_back(slider);
				faceViewer->_weights.push_back(NULL);
				curY += 30;
			}

			if (au->is_right())
			{
				Fl_Value_Slider* slider = new Fl_Value_Slider(100, curY, 150, 25, _strdup(au->getRightName().c_str()));
				slider->type(FL_HORIZONTAL);
				slider->align(FL_ALIGN_LEFT);
				slider->range(0.0, 1.0);
				faceViewer->bottomGroup->add(slider);
				faceViewer->_sliders.push_back(slider);
				faceViewer->_weights.push_back(NULL);
				curY += 30;
			}

			if (au->is_bilateral())
			{
				Fl_Value_Slider* slider = new Fl_Value_Slider(100, curY, 150, 25, _strdup(au->getBilateralName().c_str()));
				slider->type(FL_HORIZONTAL);
				slider->align(FL_ALIGN_LEFT);
				slider->range(0.0, 1.0);
				slider->callback(FaceCB, faceViewer);
				faceViewer->bottomGroup->add(slider);
				faceViewer->_sliders.push_back(slider);
				faceViewer->_weights.push_back(NULL);
				curY += 30;
			}
			

		}

		const std::vector<std::string>& visemeNames = faceDefinition->getVisemeNames();
		for (size_t v = 0; v < visemeNames.size(); v++)
		{
			Fl_Value_Slider* slider = new Fl_Value_Slider(100, curY, 150, 25, _strdup(visemeNames[v].c_str()));
			slider->type(FL_HORIZONTAL);
			slider->align(FL_ALIGN_LEFT);
			slider->range(0.0, 1.0);
			slider->callback(FaceCB, faceViewer);
			faceViewer->bottomGroup->add(slider);
			faceViewer->_sliders.push_back(slider);
			
			std::string weightLabel = visemeNames[v] + " weight";
			Fl_Value_Slider* weightSlider = new Fl_Value_Slider(330, curY, 100, 25, _strdup(weightLabel.c_str()));
			weightSlider->type(FL_HORIZONTAL);
			weightSlider->align(FL_ALIGN_LEFT);
			weightSlider->range(0.0, 1.0);
			weightSlider->callback(FaceWeightCB, faceViewer);
			faceViewer->bottomGroup->add(weightSlider);
			// set the initial weight
			float initialWeight = faceDefinition->getVisemeWeight(visemeNames[v]);
			weightSlider->value(initialWeight);
			faceViewer->_weights.push_back(weightSlider);

			curY += 30;
		}

		// also create slider for blend shape channels
		if (character->getSkeleton())
		{
			for (int jointCounter = 0; jointCounter < character->getSkeleton()->getNumJoints(); ++jointCounter)
			{
				if (character->getSkeleton()->getJoint(jointCounter)->getJointType() == SkJoint::TypeBlendShape)
				{
					Fl_Value_Slider* slider = new Fl_Value_Slider(100, curY, 150, 25, _strdup(character->getSkeleton()->getJoint(jointCounter)->getName().c_str()));
					slider->type(FL_HORIZONTAL);
					slider->align(FL_ALIGN_LEFT);
					slider->range(0.0, 1.0);
					slider->callback(FaceCB, faceViewer);
					faceViewer->bottomGroup->add(slider);
					faceViewer->_sliders.push_back(slider);
					faceViewer->_weights.push_back(NULL);
					curY += 30;
				}
			}
		}

		faceViewer->updateGUI();
		faceViewer->bottomGroup->damage(FL_DAMAGE_ALL);

		faceViewer->redraw();
	}
}
*/
void FaceShiftViewer::updateGUI()
{
	/*
		SmartBody::SBSkeleton* skeleton = character->getSkeleton();
		SmartBody::SBFaceDefinition* faceDefinition = character->getFaceDefinition();

		for (size_t s = 0; s < _sliders.size(); s++)
		{
			Fl_Value_Slider* slider = _sliders[s];
			SmartBody::SBJoint* joint = skeleton->getJointByName(slider->label());
			if (joint)
			{
				SrVec position = joint->getPosition();
				if (slider->value() != position.x)
					slider->value(position.x);
			}

			if (_weights[s])
			{
				float weight = faceDefinition->getVisemeWeight(slider->label());
				_weights[s]->value(weight);
			}
		}
*/
}


void FaceShiftViewer::show_viewer()
{
	show();
}

void FaceShiftViewer::hide_viewer()
{
	hide();
}

void FaceShiftViewer::show()
{
	SBWindowListener::windowShow();
	Fl_Double_Window::show();
}
void FaceShiftViewer::hide()
{
	SBWindowListener::windowHide();
	Fl_Double_Window::hide();
}



GenericViewer* FaceShiftViewerFactory::create(int x, int y, int w, int h)
{
	FaceShiftViewer* faceViewer = new FaceShiftViewer(x, y, w, h, (char*)"FaceShift View");
	return faceViewer;
}

void FaceShiftViewerFactory::destroy(GenericViewer* viewer)
{
	delete viewer;
}

FaceShiftViewerFactory::FaceShiftViewerFactory()
{

}
