#include "MotionEditorWindow.h"
#include <sb/SBScene.h>
#include <sb/SBBmlProcessor.h>
#include <FL/Fl_File_Chooser.H>
#include <boost/version.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

MotionEditorWindow::MotionEditorWindow(int x, int y, int w, int h, char* label) : Fl_Double_Window(x, y, w, h, label)
{
	this->label("Motion Editor");
	this->begin();
		_choiceCharacterList = new Fl_Choice(80, 10, 100, 20, "Characters");
		_choiceCharacterList->callback(OnChoiceCharacterList, this);
		_buttonRefresh = new Fl_Button(200, 10, 100, 20, "Refresh");
		_buttonRefresh->callback(OnButtonRefresh, this);
		_buttonSaveMotion = new Fl_Button(300, 10, 100, 20, "Save");
		_buttonSaveMotion->callback(OnButtonSaveMotion, this);

		_animationSearchFilter  = new Fl_Input(80, 40, 80, 20, "Search Filter");
		_animationSearchFilter->when(FL_WHEN_CHANGED);
		_animationSearchFilter->callback(OnAnimationFilterTextChanged, this);

		_browserMotionList = new Fl_Hold_Browser(10, 70, 300, 150, "Motion List");
		_browserMotionList->callback(OnBrowserMotionList, this);


      if (SmartBody::SBScene::getScene()->isRemoteMode())
      {
         _buttonQueryAnims = new Fl_Button(320, 140, 80, 20, "Query Anims");
		   _buttonQueryAnims->callback(OnButtonQueryAnims, this);
      }
		_buttonPlayMotion = new Fl_Button(320, 165, 80, 20, "Play");
		_buttonPlayMotion->callback(OnButtonPlayMotion, this);
		 _buttonPlayMotion = new Fl_Button(320, 190, 80, 20, "Set Posture");
		_buttonPlayMotion->callback(OnButtonSetPosture, this);
		_checkButtonPlayMotion = new Fl_Check_Button(10, 240, 50, 20, "Scrub");
		_checkButtonPlayMotion->callback(OnCheckButtonPlayMotion, this);
		_checkButtonPlayMotion->deactivate();
		_sliderMotionFrame = new Fl_Value_Slider(60, 240, 300, 20);
		_sliderMotionFrame->type(FL_HORIZONTAL);
		_sliderMotionFrame->callback(OnSliderMotionFrame, this);
		_sliderMotionFrame->deactivate();
		_outputMotionFrameNumber = new Fl_Output(360, 240, 30, 20);
		_outputMotionFrameNumber->value("0");
		_buttonPlayMotionFolder = new Fl_Button(10, 260, 80, 20, "Play Folder");
		_buttonPlayMotionFolder->callback(OnButtonPlayMotionFolder, this);
		_inputFilePath = new Fl_Input(90, 260, 270, 20, "");

		_groupMetaInfo = new Fl_Group(10, 300, 400, 355, "Motion MetaData");
		int groupMetaInfoX = _groupMetaInfo->x();
		int groupMetaInfoY = _groupMetaInfo->y();
		_groupMetaInfo->begin();
			_sliderStart = new Fl_Value_Slider(80 + groupMetaInfoX, 10 + groupMetaInfoY, 240, 20, "start");
			_sliderStart->type(FL_HORIZONTAL);
			_sliderStart->align(FL_ALIGN_LEFT);
			_sliderStart->callback(OnSliderSyncPoints, this);
			_buttonGetStartTime = new Fl_Button(325 + groupMetaInfoX, 10 + groupMetaInfoY, 25, 20, "<<");
			_buttonGetStartTime->tooltip("start");
			_buttonGetStartTime->callback(OnButtonGetSyncPoints, this);

			_sliderReady = new Fl_Value_Slider(80 + groupMetaInfoX, 30 + groupMetaInfoY, 240, 20, "ready");
			_sliderReady->type(FL_HORIZONTAL);
			_sliderReady->align(FL_ALIGN_LEFT);
			_sliderReady->callback(OnSliderSyncPoints, this);
			_buttonGetReadyTime = new Fl_Button(325 + groupMetaInfoX, 30 + groupMetaInfoY, 25, 20, "<<");
			_buttonGetReadyTime->tooltip("ready");
			_buttonGetReadyTime->callback(OnButtonGetSyncPoints, this);

			_sliderStrokeStart = new Fl_Value_Slider(80 + groupMetaInfoX, 50 + groupMetaInfoY, 240, 20, "stroke_start");
			_sliderStrokeStart->type(FL_HORIZONTAL);
			_sliderStrokeStart->align(FL_ALIGN_LEFT);
			_sliderStrokeStart->callback(OnSliderSyncPoints, this);
			_buttonGetStrokeStartTime = new Fl_Button(325 + groupMetaInfoX, 50 + groupMetaInfoY, 25, 20, "<<");
			_buttonGetStrokeStartTime->tooltip("stroke_start");
			_buttonGetStrokeStartTime->callback(OnButtonGetSyncPoints, this);

			_sliderStroke = new Fl_Value_Slider(80 + groupMetaInfoX, 70 + groupMetaInfoY, 240, 20, "stroke");
			_sliderStroke->type(FL_HORIZONTAL);
			_sliderStroke->align(FL_ALIGN_LEFT);
			_sliderStroke->callback(OnSliderSyncPoints, this);
			_buttonGetStrokeTime = new Fl_Button(325 + groupMetaInfoX, 70 + groupMetaInfoY, 25, 20, "<<");
			_buttonGetStrokeTime->tooltip("stroke");
			_buttonGetStrokeTime->callback(OnButtonGetSyncPoints, this);

			_sliderStrokeEnd = new Fl_Value_Slider(80 + groupMetaInfoX, 90 + groupMetaInfoY, 240, 20, "stroke_stop");
			_sliderStrokeEnd->type(FL_HORIZONTAL);
			_sliderStrokeEnd->align(FL_ALIGN_LEFT);
			_sliderStrokeEnd->callback(OnSliderSyncPoints, this);
			_buttonGetStrokeEndTime = new Fl_Button(325 + groupMetaInfoX, 90 + groupMetaInfoY, 25, 20, "<<");
			_buttonGetStrokeEndTime->tooltip("stroke_stop");
			_buttonGetStrokeEndTime->callback(OnButtonGetSyncPoints, this);

			_sliderRelax = new Fl_Value_Slider(80 + groupMetaInfoX, 110 + groupMetaInfoY, 240, 20, "relax");
			_sliderRelax->type(FL_HORIZONTAL);
			_sliderRelax->align(FL_ALIGN_LEFT);
			_sliderRelax->callback(OnSliderSyncPoints, this);
			_buttonGetRelaxTime = new Fl_Button(325 + groupMetaInfoX, 110 + groupMetaInfoY, 25, 20, "<<");
			_buttonGetRelaxTime->tooltip("relax");
			_buttonGetRelaxTime->callback(OnButtonGetSyncPoints, this);

			_sliderEnd = new Fl_Value_Slider(80 + groupMetaInfoX, 130 + groupMetaInfoY, 240, 20, "stop");
			_sliderEnd->type(FL_HORIZONTAL);
			_sliderEnd->align(FL_ALIGN_LEFT);
			_sliderEnd->callback(OnSliderSyncPoints, this);
			_buttonGetEndTime = new Fl_Button(325 + groupMetaInfoX, 130 + groupMetaInfoY, 25, 20, "<<");
			_buttonGetEndTime->tooltip("stop");
			_buttonGetEndTime->callback(OnButtonGetSyncPoints, this);

			_browserMetaNames = new Fl_Hold_Browser(10 + groupMetaInfoX, 160 + groupMetaInfoY, 150, 100, "Extra Meta Names");
			_browserMetaNames->callback(OnBrowserMetaNames, this);
			_browserMetaValues = new Fl_Hold_Browser(200 + groupMetaInfoX, 160 + groupMetaInfoY, 150, 100, "Extra Meta Values");
			_browserMetaValues->callback(OnBrowserMetaValues, this);
			_inputMetaNames = new Fl_Input(100 + groupMetaInfoX, 280 + groupMetaInfoY, 250, 20, "MetaData Name:");
			_inputMetaValues = new Fl_Input(100 + groupMetaInfoX, 300 + groupMetaInfoY, 250, 20, "MetaData Value:");
			_buttonAddMetaEntry = new Fl_Button(80 + groupMetaInfoX, 325 + groupMetaInfoY, 100, 20, "Add");
			_buttonAddMetaEntry->callback(OnButtonAddMetaEntry, this);
			_buttonDeleteMetaEntry = new Fl_Button(200 + groupMetaInfoX, 325 + groupMetaInfoY, 100, 20, "Remove");
			_buttonDeleteMetaEntry->callback(OnButtonDeleteMetaEntry, this);
		_groupMetaInfo->end();
		_groupMetaInfo->box(FL_BORDER_BOX);

      _groupGazeInfo = new Fl_Group(10, 675, 400, 30, "Gaze");
      _groupGazeInfo->begin();
		   int groupGazeInfoX = _groupGazeInfo->x();
		   int groupGazeInfoY = _groupGazeInfo->y();

         _choiceGazeTargetList = new Fl_Choice(groupGazeInfoX + 85, groupGazeInfoY + 5, 100, 20, "Gaze Targets");
		   _choiceGazeTargetList->callback(OnChoiceCharacterList, this);

         _buttonGazeAt = new Fl_Button(groupGazeInfoX + 190, groupGazeInfoY + 5, 100, 20, "Gaze At");
		   _buttonGazeAt->callback(OnButtonGazeAt, this);

         _buttonStopGaze = new Fl_Button(groupGazeInfoX + 295, groupGazeInfoY + 5, 100, 20, "Stop Gaze");
		   _buttonStopGaze->callback(OnButtonStopGaze, this);

      _groupGazeInfo->end();
		_groupGazeInfo->box(FL_BORDER_BOX);
	this->end();

	_dirty = true;
	_selectedCharacter = "*";
	_selectedGazeTarget = "";
	_selectedMotion = "";
	_isScrubbing = false;
	_scrubTime = 0.0;

	loadCharacters();
	loadMotions();

	redraw();
}

MotionEditorWindow::~MotionEditorWindow()
{
}

void MotionEditorWindow::draw()
{
	if (_dirty)
	{
		loadCharacters();
		loadMotions();
		_dirty = false;
	}

	Fl_Double_Window::draw();

}


void MotionEditorWindow::reloadCharactersAndPawns()
{
	if (!_choiceCharacterList)
		return;
	if (!_choiceGazeTargetList)
		return;

	// get previous selected character & pawn
	std::string selectedCharacter = _choiceCharacterList->text();
	std::string selectedPawn = _choiceGazeTargetList->text();

	// clear and reload
	_choiceCharacterList->clear();
	_choiceGazeTargetList->clear();
	loadCharacters();

	// select again the character & pawn
	for (int c = 0; c < _choiceCharacterList->menu()->size(); c++)
	{
		if (_choiceCharacterList->menu()[c].label() == NULL)
			continue;

		if (selectedCharacter == _choiceCharacterList->menu()[c].label())
		{
			_choiceCharacterList->value(c);
		}
	}

	for (int c = 0; c < _choiceGazeTargetList->menu()->size(); c++)
	{
		if (_choiceGazeTargetList->menu()[c].label() == NULL)
			continue;

		if (selectedPawn == _choiceGazeTargetList->menu()[c].label())
		{
			_choiceGazeTargetList->value(c);
		}
	}
}


void MotionEditorWindow::show()
{
	_dirty = true;
	/*
	// if window is shown again, update character and pawns
	// For some weird reason, when you inserting to FLTK Choice, the size is one more than its real size
	if (_choiceCharacterList && _choiceGazeTargetList)
	{
		if ((_choiceCharacterList->size() - 1) != (SmartBody::SBScene::getScene()->getNumCharacters() + 1) 
			|| (_choiceGazeTargetList->size() - 1) != (SmartBody::SBScene::getScene()->getNumPawns() + SmartBody::SBScene::getScene()->getNumCharacters()))
		{
			reloadCharactersAndPawns();
		}
	}
	*/

	SBWindowListener::windowShow();
	Fl_Double_Window::show();
}

void MotionEditorWindow::hide()
{
	SBWindowListener::windowHide();
	Fl_Double_Window::hide();
}

void MotionEditorWindow::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
	_dirty = true;
	redraw();
	//reloadCharactersAndPawns();
}

void MotionEditorWindow::OnCharacterDelete( const std::string & name )
{
	_dirty = true;
	redraw();
	//reloadCharactersAndPawns();
}

void MotionEditorWindow::OnCharacterUpdate( const std::string & name )
{

}


void MotionEditorWindow::OnPawnCreate( const std::string & name )
{
	_dirty = true;
	redraw();
//	reloadCharactersAndPawns();
}

void MotionEditorWindow::OnPawnDelete( const std::string & name )
{
	_dirty = true;
	redraw();
	//reloadCharactersAndPawns();
}

void MotionEditorWindow::loadCharacters()
{
	_choiceCharacterList->clear();
	_choiceGazeTargetList->clear();
	const std::vector<std::string>& charNames = SmartBody::SBScene::getScene()->getCharacterNames();
   _choiceCharacterList->add("*");
   if (_selectedCharacter == "")
	   _choiceCharacterList->value(0);
	for (size_t i = 0; i < charNames.size(); ++i)
	{
		_choiceCharacterList->add(charNames[i].c_str());
		if (_selectedCharacter == charNames[i])
			_choiceCharacterList->value(i + 1);
		_choiceGazeTargetList->add(charNames[i].c_str());
		if (_selectedGazeTarget == charNames[i])
			_choiceGazeTargetList->value(i + 1);
	}

	const std::vector<std::string>& pawnNames = SmartBody::SBScene::getScene()->getPawnNames();
	for (size_t i = 0; i < pawnNames.size(); ++i)
	{
		_choiceGazeTargetList->add(pawnNames[i].c_str());
		if (_selectedGazeTarget == pawnNames[i])
			_choiceGazeTargetList->value(i + 1);
	}
}

//SmartBody::SBCharacter* MotionEditorWindow::getCurrentCharacter()
//{
	//std::string curCharName = _choiceCharacterList->text();
	//return SmartBody::SBScene::getScene()->getCharacter(curCharName);
//}

void MotionEditorWindow::loadMotions()
{
	_browserMotionList->clear();
	const std::vector<std::string>& motionNames = SmartBody::SBScene::getScene()->getMotionNames();
	for (size_t i = 0; i < motionNames.size(); ++i)
	{
		_browserMotionList->add(motionNames[i].c_str());
		if (_selectedMotion == motionNames[i])
			_browserMotionList->value(i);
	}
}

void MotionEditorWindow::loadMotions(const std::string& filterString)
{
   // force it to lower
   std::string filter = filterString;
   std::transform(filterString.begin(), filterString.end(), filter.begin(), ::tolower);
   const std::vector<std::string>& motionNames = SmartBody::SBScene::getScene()->getMotionNames();
	for (size_t i = 0; i < motionNames.size(); ++i)
	{
      // case insensitive comparison
      std::string motionNameLower = motionNames[i];
      std::transform(motionNameLower.begin(), motionNameLower.end(), motionNameLower.begin(), ::tolower);
      if (motionNameLower.find(filter) != std::string::npos)
      {
         _browserMotionList->add(motionNames[i].c_str());
      }
	}
}


void MotionEditorWindow::OnChoiceCharacterList(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;

	int selected = editor->_choiceCharacterList->value();
	if (selected >= 0)
	{
		editor->_selectedCharacter = editor->_choiceCharacterList->text(selected);
	}
	//MotionEditorWindow::OnButtonRefresh(widget, data);
}

void MotionEditorWindow::OnButtonRefresh(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;
	editor->_dirty = true;
	editor->redraw();
}

void MotionEditorWindow::OnButtonSaveMotion(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;
	SmartBody::SBMotion* motion = dynamic_cast<SmartBody::SBMotion*>(SmartBody::SBScene::getScene()->getMotion(editor->_selectedMotion));
	if (!motion)
		return;
	motion->saveToSkm(motion->getMotionFileName());
}

void MotionEditorWindow::OnBrowserMotionList(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;

	int selected = editor->_browserMotionList->value();
	if (selected >= 0)
	{
		const char* motionStr = editor->_browserMotionList->text(selected);
		if (!motionStr)
			editor->_selectedMotion = "";
		else
			editor->_selectedMotion = motionStr;
	}
	//
	editor->updateSyncPointsUI();
	editor->updateMetaDataUI();

	SmartBody::SBMotion* curMotion = dynamic_cast<SmartBody::SBMotion*>(SmartBody::SBScene::getScene()->getMotion(editor->_selectedMotion));
	if (!curMotion)
	{
		editor->_checkButtonPlayMotion->deactivate();
		editor->_sliderMotionFrame->deactivate();
		return;
	}
	else
	{
		editor->_checkButtonPlayMotion->activate();
		editor->_sliderMotionFrame->activate();
	}
	double dur = curMotion->getDuration();
	editor->_sliderMotionFrame->range(0, dur);
	editor->redraw();
}

void MotionEditorWindow::OnButtonQueryAnims(Fl_Widget* widget, void* data)
{
   std::stringstream ss;
	SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
	if (sbScene->isRemoteMode())
	{
		ss << "send sbm vhmsg log on";
      sbScene->command(ss.str());
      ss.clear();
      ss << "send sb motionNames = scene.getMotionNames()\nfor i in range(0, len(motionNames)):\n\tprint motionNames[i]";
      sbScene->command(ss.str());
	}
}

void MotionEditorWindow::OnButtonPlayMotion(Fl_Widget* widget, void* data)
{
   MotionEditorWindow* editor = (MotionEditorWindow*) data;

   if (editor->_selectedCharacter == "*")
   {
      for (int i = 1; i < editor->_choiceCharacterList->size() - 1; i++) // start at 1 to get past *
      {
         editor->PlayAnimation(editor->_choiceCharacterList->text(i), editor->_selectedMotion, false);
      }
   }
   else
   {
      editor->PlayAnimation(editor->_selectedCharacter, editor->_selectedMotion, false);
   }
}

void MotionEditorWindow::OnButtonSetPosture(Fl_Widget* widget, void* data)
{
   MotionEditorWindow* editor = (MotionEditorWindow*) data;
   if (editor->_selectedCharacter == "*")
   {
      for (int i = 1; i < editor->_choiceCharacterList->size() - 1; i++) // start at 1 to get past *
      {
         editor->PlayAnimation(editor->_choiceCharacterList->text(i), editor->_selectedMotion, true);
      }
   }
   else
   {
      editor->PlayAnimation(editor->_selectedCharacter, editor->_selectedMotion, true);
   }
}

void MotionEditorWindow::OnButtonGazeAt(Fl_Widget* widget, void* data)
{
   MotionEditorWindow* editor = (MotionEditorWindow*) data;
   if (editor->_selectedCharacter == "*")
   {
      for (int i = 1; i < editor->_choiceCharacterList->size() - 1; i++) // start at 1 to get past *
      {
         editor->GazeAt("*", editor->_choiceGazeTargetList->text());
      }
   }
   else
   {
      editor->GazeAt(editor->_selectedCharacter, editor->_choiceGazeTargetList->text());
   }
}

void MotionEditorWindow::OnButtonStopGaze(Fl_Widget* widget, void* data)
{
   MotionEditorWindow* editor = (MotionEditorWindow*) data;
   if (editor->_selectedCharacter == "*")
   {
      for (int i = 1; i < editor->_choiceCharacterList->size() - 1; i++) // start at 1 to get past *
      {
         editor->StopGaze(editor->_choiceCharacterList->text(i));
      }
   }
   else
   {
      editor->StopGaze(editor->_selectedCharacter);
   }
}

void MotionEditorWindow::OnAnimationFilterTextChanged(Fl_Widget* widget, void* data)
{
   MotionEditorWindow* editor = (MotionEditorWindow*) data;
   editor->_browserMotionList->clear();
   editor->loadMotions(editor->_animationSearchFilter->value());
}

void MotionEditorWindow::PlayAnimation(const std::string& characterName, const std::string& animName, bool setAsPosture)
{
   if (characterName.length() == 0 || animName.length() == 0)
   {
      LOG("failed to play animation %s on character %s", animName.c_str(), characterName.c_str());
      return;
   }

   std::string commandType = setAsPosture ? "body posture=" : "animation name=";
	std::string bml = "<" + commandType + "\"" + animName + "\"/>";

   // this now works for remote mode
   SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
	if (!sbScene->isRemoteMode())
	{
      SmartBody::SBScene::getScene()->getBmlProcessor()->execBML(characterName, bml);
	}
	else
	{
      std::stringstream ss;
      ss << "send sb " << "bml.execBML(\'" << characterName << "\', \'" << bml << "\')";
      std::string sendStr = ss.str();
      LOG("%s", sendStr.c_str());
		SmartBody::SBScene::getScene()->command(sendStr);
	}
}

void MotionEditorWindow::GazeAt(const std::string& characterName, const std::string& gazeTarget)
{
   std::string commandType = "gaze target=";
	std::string bml = "<" + commandType + "\"" + gazeTarget + "\"/>";

   // this now works for remote mode
   SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
	if (!sbScene->isRemoteMode())
	{
      SmartBody::SBScene::getScene()->getBmlProcessor()->execBML(characterName, bml);
	}
	else
	{
      std::stringstream ss;
      ss << "send sb " << "bml.execBML(\'" << characterName << "\', \'" << bml << "\')";
      std::string sendStr = ss.str();
      LOG("%s", sendStr.c_str());
		SmartBody::SBScene::getScene()->command(sendStr);
   }
}

void MotionEditorWindow::StopGaze(const std::string& characterName)
{
   std::string cmd = "char " + characterName + " gazefade out 1";
   SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
   if (!sbScene->isRemoteMode())
	{
      SmartBody::SBScene::getScene()->command(cmd);
	}
	else
	{
      std::stringstream ss;
      ss << "send sb scene.command(\'" << cmd <<"\')";
      std::string sendStr = ss.str();
      LOG("%s", sendStr.c_str());
		SmartBody::SBScene::getScene()->command(sendStr);
	}
}

void MotionEditorWindow::OnCheckButtonPlayMotion(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;
	SmartBody::SBCharacter* curChar = dynamic_cast<SmartBody::SBCharacter*>(SmartBody::SBScene::getScene()->getCharacter(editor->_selectedCharacter));
	if (!curChar)
	{
		if (editor->_selectedCharacter != "*")
			return;
	}

	editor->_isScrubbing = editor->_checkButtonPlayMotion->value() == 1? true: false;

	SmartBody::SBMotion* curMotion = dynamic_cast<SmartBody::SBMotion*>(SmartBody::SBScene::getScene()->getMotion(editor->_selectedMotion));
	if (!curMotion)	
	{
		const std::vector<std::string>& charNames = SmartBody::SBScene::getScene()->getCharacterNames();
		std::stringstream ss;
		for (size_t i = 0; i < charNames.size(); ++i)
		{
			ss.str("");
			if (SmartBody::SBScene::getScene()->isRemoteMode())
			{
				ss << "send sbm ";
			}
			ss << "motionplayer " << charNames[i] << " off";
			SmartBody::SBScene::getScene()->command(ss.str());
		}
		return;
	}

	// determine the set of characters that are affected
	std::vector<std::string> names;
	if (editor->_selectedCharacter == "*")
		names = SmartBody::SBScene::getScene()->getCharacterNames();
	else
		names.push_back(editor->_selectedCharacter);

	if (editor->_isScrubbing)
	{
		editor->_sliderMotionFrame->activate();
		double playTime = editor->_sliderMotionFrame->value();
		double delta = curMotion->duration() / double(curMotion->frames() - 1);
		int frameNumber = int(playTime / delta);
		for (size_t c = 0; c < names.size(); c++)
		{
			std::stringstream ss;
			if (SmartBody::SBScene::getScene()->isRemoteMode())
			{
				ss << "send sbm ";
			}
			ss << "motionplayer " << names[c] << " on";		
			SmartBody::SBScene::getScene()->command(ss.str());
			ss.str("");
			if (SmartBody::SBScene::getScene()->isRemoteMode())
			{
				ss << "send sbm ";
			}
			ss << "motionplayer " << names[c] << " " << curMotion->getName() << " " << frameNumber;
			SmartBody::SBScene::getScene()->command(ss.str());
		}

	}
	else
	{		
		editor->_sliderMotionFrame->deactivate();
		for (size_t c = 0; c < names.size(); c++)
		{
			std::stringstream ss;
			if (SmartBody::SBScene::getScene()->isRemoteMode())
			{
				ss << "send sbm ";
			}
			ss << "motionplayer " <<  names[c] << " off";
			SmartBody::SBScene::getScene()->command(ss.str());
		}
		editor->_sliderMotionFrame->value(0);
		editor->_outputMotionFrameNumber->value("0");
	}
}

void MotionEditorWindow::OnSliderMotionFrame(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;
	
	SmartBody::SBMotion* curMotion = dynamic_cast<SmartBody::SBMotion*>(SmartBody::SBScene::getScene()->getMotion(editor->_selectedMotion));
	if (!curMotion)
		return;
	
	std::vector<std::string> names;
	if (editor->_selectedCharacter == "*")
	{
		names = SmartBody::SBScene::getScene()->getCharacterNames();
	}
	else
	{
		names.push_back(editor->_selectedCharacter);
	}

	double playTime = editor->_sliderMotionFrame->value();
	double delta = curMotion->duration() / double(curMotion->frames() - 1);
	int frameNumber = int(playTime / delta);
	for (size_t c = 0; c < names.size(); c++)
	{
		std::stringstream ss;
		SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
		if (sbScene->isRemoteMode())
		{
			ss << "send sbm ";
		}
		ss << "motionplayer " << names[c] << " " << curMotion->getName() << " " << frameNumber;
		SmartBody::SBScene::getScene()->command(ss.str());
	}
	std::stringstream ss1;
	ss1 << frameNumber;
	editor->_outputMotionFrameNumber->value(ss1.str().c_str());
}

/*
	For now, not recursive, only reading skm
*/
void MotionEditorWindow::OnButtonPlayMotionFolder(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;
	SmartBody::SBCharacter* curChar = dynamic_cast<SmartBody::SBCharacter*>(SmartBody::SBScene::getScene()->getCharacter(editor->_selectedCharacter));
	if (!curChar)
		return;

	std::string motionFolderPath = editor->_inputFilePath->value();
	boost::filesystem::path motionFolder(motionFolderPath);
#if (BOOST_VERSION > 104400)
	if (!boost::filesystem::is_directory(motionFolder))
#else
	if (!boost::filesystem2::is_directory(motionFolder))
#endif
	{
		LOG("MotionEditorWindow::OnButtonPlayMotionFolder ERR: Please input a valid directory %s", motionFolderPath.c_str());
		return;
	}

	std::vector<std::string> skmMotionNames;
	boost::filesystem::directory_iterator end;
	for (boost::filesystem::directory_iterator iter(motionFolder); iter != end; ++iter)
	{
		const boost::filesystem::path& cur = *iter;
		if (boost::filesystem::is_directory(cur))
			continue;
		std::string ext = boost::filesystem::extension(cur);
		if (ext != ".skm")
			continue;
		std::string fileName = boost::filesystem::basename(cur);
		skmMotionNames.push_back(fileName);
	}
	LOG("Playing animations in folder %s", motionFolderPath.c_str());
	std::stringstream command;
	for (size_t i = 0; i < skmMotionNames.size(); ++i)
	{
		LOG("%s", skmMotionNames[i].c_str());
		command << "<animation name=\"" << skmMotionNames[i] << "\" id=\"anim" << i << "\"";
		if (i > 0)
			command << " start=\"anim" << (i - 1) << ":end\"/>";
		else
			command << " start=\"0\"/>";
	}
	SmartBody::SBScene::getScene()->getBmlProcessor()->execBML(curChar->getName(), command.str());
}

void MotionEditorWindow::updateSyncPointsUI()
{
	SmartBody::SBMotion* curMotion = dynamic_cast<SmartBody::SBMotion*>(SmartBody::SBScene::getScene()->getMotion(this->_selectedMotion));
	if (!curMotion)
	{
		_sliderStart->range(0, 1);
		_sliderReady->range(0, 1);
		_sliderStrokeStart->range(0, 1);
		_sliderStroke->range(0, 1);
		_sliderStrokeEnd->range(0, 1);
		_sliderRelax->range(0, 1);
		_sliderEnd->range(0, 1);
		_sliderStart->value(0);
		_sliderReady->value(0);
		_sliderStrokeStart->value(0);
		_sliderStroke->value(0);
		_sliderStrokeEnd->value(0);
		_sliderRelax->value(0);
		_sliderEnd->value(0);
		redraw();
		return;
	}
	double dur = curMotion->getDuration();
	_sliderStart->range(0, dur);
	_sliderReady->range(0, dur);
	_sliderStrokeStart->range(0, dur);
	_sliderStroke->range(0, dur);
	_sliderStrokeEnd->range(0, dur);
	_sliderRelax->range(0, dur);
	_sliderEnd->range(0, dur);

	_sliderStart->value(curMotion->synch_points.get_time(srSynchPoints::START));
	_sliderReady->value(curMotion->synch_points.get_time(srSynchPoints::READY));
	_sliderStrokeStart->value(curMotion->synch_points.get_time(srSynchPoints::STROKE_START));
	_sliderStroke->value(curMotion->synch_points.get_time(srSynchPoints::STROKE));
	_sliderStrokeEnd->value(curMotion->synch_points.get_time(srSynchPoints::STROKE_STOP));
	_sliderRelax->value(curMotion->synch_points.get_time(srSynchPoints::RELAX));
	_sliderEnd->value(curMotion->synch_points.get_time(srSynchPoints::STOP));
	redraw();
}

void MotionEditorWindow::updateMotionSyncPoints(const std::string& type)
{
	SmartBody::SBMotion* curMotion = dynamic_cast<SmartBody::SBMotion*>(SmartBody::SBScene::getScene()->getMotion(this->_selectedMotion));
	if (!curMotion)
		return;
	curMotion->validateSyncPoint(type);
	
}

void MotionEditorWindow::OnSliderSyncPoints(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;
	SmartBody::SBMotion* curMotion = dynamic_cast<SmartBody::SBMotion*>(SmartBody::SBScene::getScene()->getMotion(editor->_selectedMotion));
	if (!curMotion)
		return;

	const std::string& type = widget->label();
	if (type == "start")
	{
		double playTime = editor->_sliderStart->value();
		curMotion->synch_points.set_time(srSynchPoints::START, playTime);
	}
	if (type == "ready")
	{
		double playTime = editor->_sliderReady->value();
		curMotion->synch_points.set_time(srSynchPoints::READY, playTime);
	}
	if (type == "stroke_start")
	{
		double playTime = editor->_sliderStrokeStart->value();
		curMotion->synch_points.set_time(srSynchPoints::STROKE_START, playTime);
	}
	if (type == "stroke")
	{
		double playTime = editor->_sliderStroke->value();
		curMotion->synch_points.set_time(srSynchPoints::STROKE, playTime);
	}
	if (type == "stroke_stop")
	{
		double playTime = editor->_sliderStrokeEnd->value();
		curMotion->synch_points.set_time(srSynchPoints::STROKE_STOP, playTime);
	}
	if (type == "relax")
	{
		double playTime = editor->_sliderRelax->value();
		curMotion->synch_points.set_time(srSynchPoints::RELAX, playTime);
	}
	if (type == "stop")
	{
		double playTime = editor->_sliderEnd->value();
		curMotion->synch_points.set_time(srSynchPoints::STOP, playTime);
	}
	editor->updateMotionSyncPoints(type);
	editor->updateSyncPointsUI();
}

void MotionEditorWindow::OnButtonGetSyncPoints(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;
	SmartBody::SBMotion* curMotion = dynamic_cast<SmartBody::SBMotion*>(SmartBody::SBScene::getScene()->getMotion(editor->_selectedMotion));
	if (!curMotion)
		return;
	const std::string& type = widget->tooltip();
	double playTime = editor->_sliderMotionFrame->value();
	if (type == "start")
	{
		curMotion->synch_points.set_time(srSynchPoints::START, playTime);
	}
	if (type == "ready")
	{
		curMotion->synch_points.set_time(srSynchPoints::READY, playTime);
	}
	if (type == "stroke_start")
	{
		curMotion->synch_points.set_time(srSynchPoints::STROKE_START, playTime);
	}
	if (type == "stroke")
	{
		curMotion->synch_points.set_time(srSynchPoints::STROKE, playTime);
	}
	if (type == "stroke_stop")
	{
		curMotion->synch_points.set_time(srSynchPoints::STROKE_STOP, playTime);
	}
	if (type == "relax")
	{
		curMotion->synch_points.set_time(srSynchPoints::RELAX, playTime);
	}
	if (type == "stop")
	{
		curMotion->synch_points.set_time(srSynchPoints::STOP, playTime);
	}
	editor->updateMotionSyncPoints(type);
	editor->updateSyncPointsUI();
}

void MotionEditorWindow::updateMetaDataUI()
{
	SmartBody::SBMotion* curMotion = dynamic_cast<SmartBody::SBMotion*>(SmartBody::SBScene::getScene()->getMotion(this->_selectedMotion));
	if (!curMotion)
	{
		_browserMetaNames->clear();
		_browserMetaValues->clear();
		_inputMetaNames->value("");
		_inputMetaValues->value("");
		return;
	}
	_browserMetaNames->clear();
	_browserMetaNames->deselect();
	_browserMetaValues->clear();
	_browserMetaValues->deselect();
	const std::vector<std::string>& metaDataTags = curMotion->getMetaDataTags();
	for (size_t i = 0; i < metaDataTags.size(); ++i)
	{
		const std::string& metaDataString = curMotion->getMetaDataString(metaDataTags[i]);
		_browserMetaNames->add(metaDataTags[i].c_str());
		_browserMetaValues->add(metaDataString.c_str());
	}
	redraw();
}

void MotionEditorWindow::addMotionMetaData(const std::string& name, const std::string& value)
{
	SmartBody::SBMotion* curMotion = dynamic_cast<SmartBody::SBMotion*>(SmartBody::SBScene::getScene()->getMotion(this->_selectedMotion));
	if (!curMotion)
		return;

	curMotion->addMetaData(name, value);
}

void MotionEditorWindow::OnBrowserMetaNames(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;
	editor->_browserMetaValues->deselect();
	if (editor->_browserMetaNames->value() > 0)
		editor->_browserMetaValues->value(editor->_browserMetaNames->value());
}

void MotionEditorWindow::OnBrowserMetaValues(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;
	editor->_browserMetaNames->deselect();
	if (editor->_browserMetaValues->value() > 0)
		editor->_browserMetaNames->value(editor->_browserMetaValues->value());
}

void MotionEditorWindow::OnButtonAddMetaEntry(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;
	const std::string& name = editor->_inputMetaNames->value();
	const std::string& value = editor->_inputMetaValues->value();
	if (name != "" && value != "")
		editor->addMotionMetaData(name, value);
	editor->updateMetaDataUI();
}

void MotionEditorWindow::OnButtonDeleteMetaEntry(Fl_Widget* widget, void* data)
{
	MotionEditorWindow* editor = (MotionEditorWindow*) data;
	if (editor->_browserMetaNames->value() <= 0)
		return;
	SmartBody::SBMotion* curMotion = dynamic_cast<SmartBody::SBMotion*>(SmartBody::SBScene::getScene()->getMotion(editor->_selectedMotion));
	if (!curMotion)
		return;
	const std::string& name = editor->_browserMetaNames->text(editor->_browserMetaNames->value());
	curMotion->removeMetaData(name);
	editor->updateMetaDataUI();
}


void MotionEditorWindow::OnObjectCreate( SmartBody::SBObject* object )
{
	SmartBody::SBMotion* motion = dynamic_cast<SmartBody::SBMotion*>(object);
	if (motion)
	{
		_dirty = true;
		redraw();
		return;
	}

	SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*>(object);
	if (character)
	{
		_dirty = true;
		redraw();
		return;
	}
}


void MotionEditorWindow::OnObjectDelete( SmartBody::SBObject* object )
{
	SmartBody::SBMotion* motion = dynamic_cast<SmartBody::SBMotion*>(object);
	if (motion)
	{
		_dirty = true;
		redraw();
		return;
	}

	SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*>(object);
	if (character)
	{
		_dirty = true;
		redraw();
		return;
	}
}