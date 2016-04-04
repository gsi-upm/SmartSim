#include "CharacterCreatorWindow.h"
#include <sstream>
#include <FL/fl_ask.H>
#include <sb/SBScene.h>
#include "RootWindow.h"



CharacterCreatorWindow::CharacterCreatorWindow(int x, int y, int w, int h, char* name) : Fl_Double_Window(x, y, w, h, name)
{
	numCharacter = 0;
	begin();
		choiceSkeletons = new Fl_Choice(100, 30, 300, 25, "Skeleton");
		choiceSkeletons->when(FL_WHEN_CHANGED);
		inputName = new Fl_Input(100, 70, 100, 25, "Name");
		buttonCreate = new Fl_Button(100, 95, 60, 25, "Create");
		buttonCreate->callback(CreateCB, this);
	end();
}

CharacterCreatorWindow::~CharacterCreatorWindow()
{
}

void CharacterCreatorWindow::setSkeletons(std::vector<std::string>& skeletonNames)
{
	choiceSkeletons->clear();

	for (size_t x = 0; x < skeletonNames.size(); x++)
	{
		choiceSkeletons->add(skeletonNames[x].c_str());
	}
}

void CharacterCreatorWindow::CreateCB(Fl_Widget* w, void* data)
{
	CharacterCreatorWindow* creator = (CharacterCreatorWindow*) (data);
	
	if (creator->inputName->size() == 0)
	{
		fl_alert("Please enter a character name.");
		return;
	}

	SmartBody::SBCharacter* existingChar = SmartBody::SBScene::getScene()->getCharacter(creator->inputName->value());
	if (existingChar)
	{
		fl_alert("Character name already exists.");
		return;
	}
	int skeletonValue = creator->choiceSkeletons->value();	
	if (skeletonValue < 0)
	{
		fl_alert("Please select a valid skeleton.");
		return;
	}
	std::string skel = creator->choiceSkeletons->menu()[creator->choiceSkeletons->value()].label();
	
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->createCharacter(creator->inputName->value(), "");
	if (!character)
	{
		fl_alert("Character named '%s' could not be created.", creator->inputName->value());
		return;
	}
	SmartBody::SBSkeleton* skeleton = SmartBody::SBScene::getScene()->createSkeleton(skel);
	if (!skeleton)
	{
		fl_alert("Character named '%s' could not be created. Problem creating skeleton '%s'.", creator->inputName->value(), skel.c_str());
		SmartBody::SBScene::getScene()->removeCharacter(character->getName());
		return;
	}
	character->setSkeleton(skeleton);
	character->createStandardControllers();
	creator->numCharacter++;
	creator->hide();
}

ResolutionWindow::ResolutionWindow( int x, int y, int w, int h, char* name ): Fl_Double_Window(x, y, w, h, name)
{
	baseWin = NULL;
	begin();	
	inputXRes = new Fl_Input(100, 20, 100, 25, "X Res");
	inputYRes = new Fl_Input(100, 50, 100, 25, "Y Res");
	buttonSet = new Fl_Button(100, 95, 60, 25, "Set Res");
	buttonSet->callback(SetCB, this);
	end();
}

ResolutionWindow::~ResolutionWindow()
{

}

void ResolutionWindow::SetCB( Fl_Widget* w, void* data )
{
	ResolutionWindow* resWin = static_cast<ResolutionWindow*>(data);
	resWin->setResolution();
	resWin->hide();
}

void ResolutionWindow::setResolution()
{
	int resX = atoi(inputXRes->value());
	int resY = atoi(inputYRes->value());
	if (resX > 0 && resY > 0 && baseWin)
	{
		baseWin->resize(baseWin->x(),baseWin->y(),resX,resY);
	}
}