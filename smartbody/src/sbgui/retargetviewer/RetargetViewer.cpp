#include "vhcl.h"
#include "RetargetViewer.h"
#include <sb/SBBehaviorSetManager.h>
#include <sb/SBBehaviorSet.h>
#include <sb/SBScene.h>
#include <FL/Fl_Check_Button.H>
#include <sstream>
#include <cstring>

#ifndef WIN32
#define _strdup strdup
#endif

RetargetViewer::RetargetViewer(int x, int y, int w, int h, char* name) : Fl_Double_Window(x, y, w, h, name)
{
	rootWindow = NULL;
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	begin();

	int curY = 10;

	curY += 25;

	behaviorSetCurY = curY;
	
	int scrollHeight = this->h() - curY - 50;
	_scrollGroup = new Fl_Scroll(10, curY, this->w() - 20, scrollHeight, "");
	_scrollGroup->type(Fl_Scroll::VERTICAL);
	_scrollGroup->begin();	
	_scrollGroup->end();	
	end();
	updateBehaviorSet();
	curY += scrollHeight;
	_retargetButton = new Fl_Button(40, curY, 120, 20, "Retarget");
	_retargetButton->callback(RetargetCB, this);
	this->add(_retargetButton);
	_cancelButton = new Fl_Button(180, curY, 120, 20, "Cancel");
	_cancelButton->callback(CancelCB, this);
	this->add(_cancelButton);
}

RetargetViewer::~RetargetViewer()
{
}

int RetargetViewer::updateBehaviorSet()
{
	int widgetY = behaviorSetCurY;
	int itemWidth = this->w() - 40 - 20;
	SmartBody::SBBehaviorSetManager* behavMgr = SmartBody::SBScene::getScene()->getBehaviorSetManager();
	std::map<std::string, SmartBody::SBBehaviorSet*>& behavSets = behavMgr->getBehaviorSets();
	_scrollGroup->clear();
	for (std::map<std::string, SmartBody::SBBehaviorSet*>::iterator iter = behavSets.begin();
		iter != behavSets.end();
		iter++)
	{
		std::string name = (*iter).first;
		Fl_Check_Button* check = new Fl_Check_Button(40, widgetY, itemWidth, 20, _strdup(name.c_str()));
		_scrollGroup->add(check);
		widgetY += 25;
	}
	return widgetY;
}

void RetargetViewer::setCharacterName(const std::string& name)
{
	_charName = name;	

}

const std::string& RetargetViewer::getCharacterName()
{
	return _charName;
}

void RetargetViewer::setShowButton(bool showButton)
{
	if (showButton)
	{
		_retargetButton->show();
		_cancelButton->show();
	}
	else
	{
		_retargetButton->hide();
		_cancelButton->hide();
	}	
}


void RetargetViewer::RetargetCB(Fl_Widget* widget, void* data)
{
	RetargetViewer* viewer = (RetargetViewer*) data;

	SmartBody::SBBehaviorSetManager* behavMgr = SmartBody::SBScene::getScene()->getBehaviorSetManager();

	// run the script associated with any checked behavior sets
	int numChildren = viewer->_scrollGroup->children();
	for (int c = 0; c < numChildren; c++)
	{
		Fl_Check_Button* check = dynamic_cast<Fl_Check_Button*>(viewer->_scrollGroup->child(c));
		if (check)
		{
			if (check->value())
			{
				SmartBody::SBBehaviorSet* behavSet = behavMgr->getBehaviorSet(check->label());				
				if (behavSet && viewer->getCharacterName() != "")
				{
					LOG("Retargetting %s on %s ...", check->label(), viewer->getCharacterName().c_str());
					const std::string& script = behavSet->getScript();
					SmartBody::SBScene::getScene()->runScript(script.c_str());
					std::stringstream strstr;
					strstr << "setupBehaviorSet()";
					SmartBody::SBScene::getScene()->run(strstr.str());
					std::stringstream strstr2;
					strstr2 << "retargetBehaviorSet('" << viewer->getCharacterName() << "')";
					SmartBody::SBScene::getScene()->run(strstr2.str());
				}
			}
		}
	}
	if (viewer->rootWindow)
	{
		viewer->rootWindow->hide();
	}
}

void RetargetViewer::CancelCB(Fl_Widget* widget, void* data)
{
	RetargetViewer* viewer = (RetargetViewer*) data;
	if (viewer->rootWindow)
	{
		viewer->rootWindow->hide();
	}
}

