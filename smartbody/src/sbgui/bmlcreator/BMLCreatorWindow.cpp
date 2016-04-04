#include "vhcl.h"
#include <sstream>
#include "BMLCreatorWindow.h"
#include "AttributeWindow.h"
#include "bml/BMLObject.h"
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <sb/SBScene.h>
#include "bml/BMLBodyObject.h"
#include "bml/BMLLocomotionObject.h"
#include "bml/BMLAnimationObject.h"
#include "bml/BMLGestureObject.h"
#include "bml/BMLReachObject.h"
#include "bml/BMLHandObject.h"
#include "bml/BMLGazeObject.h"
#include "bml/BMLConstraintObject.h"
#include "bml/BMLHeadObject.h"
#include "bml/BMLFaceObject.h"
#include "bml/BMLSpeechObject.h"
#include "bml/BMLEventObject.h"
#include "bml/BMLSaccadeObject.h"
#include "bml/BMLStateObject.h"
#include "bml/BMLNoiseObject.h"

#ifndef WIN32
#define _strdup strdup
#endif

BMLCreatorWindow::BMLCreatorWindow(int x,int y,int w,int h, const char* s) : Fl_Double_Window(x, y, w, h, s)
{

	BMLObject* body = new BMLBodyObject();
	BMLObject* locomotion = new BMLLocomotionObject();
	BMLObject* animation = new BMLAnimationObject();
	BMLObject* gesture = new BMLGestureObject();
	BMLObject* reach = new BMLReachObject();
	BMLObject* hand = new BMLHandObject();
	BMLObject* gaze = new BMLGazeObject();
	BMLObject* constraint = new BMLConstraintObject();
	BMLObject* head = new BMLHeadObject();
	BMLObject* face = new BMLFaceObject();
	BMLObject* speech = new BMLSpeechObject();
	BMLObject* saccade = new BMLSaccadeObject();
	BMLObject* event = new BMLEventObject();
	BMLObject* state = new BMLStateObject();
	BMLObject* noise = new BMLNoiseObject();

	_bmlObjects.push_back(body);
	_bmlObjects.push_back(locomotion);
	_bmlObjects.push_back(animation);
	_bmlObjects.push_back(gesture);
	_bmlObjects.push_back(reach);
	_bmlObjects.push_back(hand);
	_bmlObjects.push_back(gaze);
	_bmlObjects.push_back(constraint);
	_bmlObjects.push_back(head);
	_bmlObjects.push_back(face);
	_bmlObjects.push_back(speech);
	_bmlObjects.push_back(saccade);
	_bmlObjects.push_back(event);
	_bmlObjects.push_back(state);
	_bmlObjects.push_back(noise);

	for (size_t i = 0; i < _bmlObjects.size(); i++)
	{
		_bmlObjects[i]->registerObserver(this);
	}

	this->begin();

	Fl_Tabs* tabs = new Fl_Tabs(10, 10, w - 20, 350);
	tabs->begin();
	for (size_t i = 0; i < _bmlObjects.size(); i++)
	{
		Fl_Scroll* scroller = new Fl_Scroll(10, 40, w - 40, 310, _strdup(_bmlObjects[i]->getName().c_str()));
		scroller->type(FL_VERTICAL);
		scroller->begin();
			AttributeWindow* attrWindow = new AttributeWindow(_bmlObjects[i], 20, 70, w - 80, 280, _strdup(_bmlObjects[i]->getName().c_str()));
			attrWindow->begin();
			attrWindow->end();
		scroller->end();
	}
	tabs->end();

	_choiceCharacters = new Fl_Choice(120, h - 190, 200, 25, "Characters");
	_choiceCharacters->callback(ChooseCharactersCB, this);
	_choiceCharacters->add("*");
	_choiceCharacters->value(0);
	_curCharacter = "*";

	Fl_Button* refreshCharacters= new Fl_Button(320, h - 190, 100, 25, "Refresh");
	refreshCharacters->callback(RefreshCharactersCB, this);

	Fl_Button* resetBML= new Fl_Button(450, h - 190, 100, 25, "Reset BML");
	resetBML->callback(ResetBMLCB, this);

	Fl_Button* buttonBML = new Fl_Button(600, h - 190, 100, 25, "Run");
	buttonBML->callback(RunBMLCB, this);

	_editor = new Fl_Text_Editor(0, h - 160, w, 160);
	Fl_Text_Buffer* buffer = new Fl_Text_Buffer();
	_editor->buffer(buffer);

	this->resizable(tabs);


	this->end();

	this->size_range(800, 600);

	_curBML = "";
}

BMLCreatorWindow::~BMLCreatorWindow()
{
}

void BMLCreatorWindow::updateBMLBuffer()
{
	_editor->buffer()->remove(0, _editor->buffer()->length());
	std::stringstream strstr;
	if (_curBML.size() > 0)
		strstr << "bml.execBML('" << _curCharacter << "', '";
	
	_editor->buffer()->insert(0, strstr.str().c_str());
	_editor->buffer()->insert(_editor->buffer()->length(), _curBML.c_str());
	_editor->buffer()->insert(_editor->buffer()->length(), "')");
}

void BMLCreatorWindow::notify(SmartBody::SBSubject* subject)
{
	BMLObject* bmlObject = dynamic_cast<BMLObject*>(subject);
	if (bmlObject)
	{
		_curBML = bmlObject->getBML();
		updateBMLBuffer();
	}
}

void BMLCreatorWindow::RunBMLCB(Fl_Widget* w, void *data)
{
	BMLCreatorWindow* window = (BMLCreatorWindow*) data;
	
	SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
	if (!sbScene->isRemoteMode())
	{
		SmartBody::SBScene::getScene()->run(window->_editor->buffer()->text());
	}
	else
	{
		std::string sendStr = "send sbm python " + std::string(window->_editor->buffer()->text());
		SmartBody::SBScene::getScene()->command(sendStr);
	}
}

void BMLCreatorWindow::RefreshCharactersCB(Fl_Widget* w, void *data)
{
	BMLCreatorWindow* window = (BMLCreatorWindow*) data;
	
	window->_choiceCharacters->clear();
	window->_choiceCharacters->add("*");

	const std::vector<std::string>& charNames = SmartBody::SBScene::getScene()->getCharacterNames();
	for (size_t i = 0; i < charNames.size(); i++)
	{
		const std::string & charName = charNames[i];
		window->_choiceCharacters->add(charName.c_str());
	}
}

void BMLCreatorWindow::ResetBMLCB(Fl_Widget* w, void *data)
{
	BMLCreatorWindow* window = (BMLCreatorWindow*) data;
	
	for (size_t x = 0; x < window->_bmlObjects.size(); x++)
	{
		std::map<std::string, SmartBody::SBAttribute*>& attributes = window->_bmlObjects[x]->getAttributeList();
		for (std::map<std::string, SmartBody::SBAttribute*>::iterator iter = attributes.begin();
			iter != attributes.end();
			iter++)
		{
			SmartBody::BoolAttribute* boolAttr = dynamic_cast<SmartBody::BoolAttribute*>((*iter).second);
			if (boolAttr)
			{
				boolAttr->setValue(boolAttr->getDefaultValue());
			}

			SmartBody::IntAttribute* intAttr = dynamic_cast<SmartBody::IntAttribute*>((*iter).second);
			if (intAttr)
			{
				intAttr->setValue(intAttr->getDefaultValue());
			}

			SmartBody::DoubleAttribute* doubleAttr = dynamic_cast<SmartBody::DoubleAttribute*>((*iter).second);
			if (doubleAttr)
			{
				doubleAttr->setValue(doubleAttr->getDefaultValue());
			}

			SmartBody::Vec3Attribute* vec3Attr = dynamic_cast<SmartBody::Vec3Attribute*>((*iter).second);
			if (vec3Attr)
			{
				vec3Attr->setValue(vec3Attr->getDefaultValue());
			}

			SmartBody::StringAttribute* stringAttr = dynamic_cast<SmartBody::StringAttribute*>((*iter).second);
			if (stringAttr)
			{
				stringAttr->setValue(stringAttr->getDefaultValue());
			}
		}
	}

	window->_choiceCharacters->clear();
	window->_choiceCharacters->add("*");

	const std::vector<std::string>& charNames = SmartBody::SBScene::getScene()->getCharacterNames();
	for (size_t i = 0; i < charNames.size(); i++)
	{
		const std::string & charName = charNames[i];
		window->_choiceCharacters->add(charName.c_str());
	}
}

void BMLCreatorWindow::ChooseCharactersCB(Fl_Widget* w, void *data)
{
	BMLCreatorWindow* window = (BMLCreatorWindow*) data;
	
	int choice = window->_choiceCharacters->value();
	if (choice > -1)
	{
		window->_curCharacter = window->_choiceCharacters->menu()[window->_choiceCharacters->value()].label();
	}
	window->updateBMLBuffer();
}


