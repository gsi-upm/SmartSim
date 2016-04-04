#include <vhcl.h>
#include <algorithm>
#include <cctype>
#include <stdlib.h>
#include <FL/Fl_Device.H>
#include <FL/fl_draw.H>
#include <sb/SBScene.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBFaceDefinition.h>
#include <sb/SBPhonemeManager.h>
#include <sb/SBAssetManager.h>
#include <sb/SBBmlProcessor.h>
#include <bml/bml_speech.hpp>
#include <bml/bml_processor.hpp>
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include "VisemeViewerWindow.h"
#include "VisemeCurveEditor.h"
#include "VisemeRunTimeWindow.h"
#include "ImageSequencePlayer.h"
#include "RootWindow.h"

#ifndef WIN32
#define _stricmp strcasecmp
#endif
 
VisemeViewerWindow::VisemeViewerWindow(int x, int y, int w, int h, char* name) : Fl_Double_Window(x, y, w, h), SBWindowListener()
{
	_phonemesSelected[0] = false;
	_phonemesSelected[1] = false;
	_lastUtterance = "";
	
	this->label("Lip Sync Viewer");
	this->begin();

	_sliderCurveAnimation = new Fl_Value_Slider(115, 460, 435, 30);
	_sliderCurveAnimation->type(5);
	_sliderCurveAnimation->textsize(14);
	_sliderCurveAnimation->deactivate();
	_sliderCurveAnimation->callback(OnSliderSelectCB, this);
	_checkEnableScrub = new Fl_Check_Button(40, 460, 65, 30, "Scrub");
	_checkEnableScrub->callback(OnEnableScrub, this);

	_buttonPlay = new Fl_Button(560, 460, 65, 30, "@>");
	_buttonPlay->callback(OnPlayCB, this);
	_inputPlayTime = new Fl_Input(630, 460, 60, 30, "");
	_inputPlayTime->value("0");

	_buttonPlayDialog = new Fl_Button(40, 500, 70, 30, "Speak");
	_buttonPlayDialog->callback(OnPlayDialogCB, this);
	_inputUtterance = new Fl_Input(115, 500, 435, 30);
	_buttonRunTimeCurves = new Fl_Button(560, 500, 100, 30, "RunTime Curves");
	_buttonRunTimeCurves->callback(OnRunTimeCurvesCB, this);

	_buttonPlayImageSequence = new Fl_Button(660, 500, 100, 30, "Play Image Sequence");
	_buttonPlayImageSequence->callback(OnPlayImageSequence, this);

	_buttonPlayAudioFile = new Fl_Button(40, 535, 70, 30, "Play Audio");
	_buttonPlayAudioFile->callback(OnPlayAudioFileCB, this);
	_inputAudioFile = new Fl_Input(115, 535, 435, 30);
	_choiceAudioFile = new Fl_Choice(560, 535, 100, 30, "");
	_choiceAudioFile->callback(OnAudioFileSelectCB, this);
	_buttonSetCharacterAudioFolder = new Fl_Button(40, 535, 70, 30, "Play Audio");
	_buttonSetCharacterAudioFolder->callback(OnPlayAudioFileCB, this);

	_choiceCharacter = new Fl_Choice(70, 35, 100, 25, "Character");
	_choiceCharacter->callback(OnCharacterSelectCB, this);

	_buttonRefreshCharacter = new Fl_Button(180, 35, 80, 25, "Refresh");
	_buttonRefreshCharacter->callback(OnCharacterRefreshCB, this);

	_checkStats = new Fl_Check_Button(330, 35, 100, 25, "Gather Stats");
	_checkStats->callback(OnGatherStatsCB, this);

	_buttonReset = new Fl_Button(430, 35, 60, 25, "Reset Stats");
	_buttonReset->callback(OnStatsResetCB, this);

	_buttonShowStats = new Fl_Button(500, 35, 60, 25, "Save Stats");
	_buttonShowStats->callback(OnShowStatsCB, this);

	_buttonNormalize = new Fl_Button(620, 35, 80, 25, "Normalize");
	_buttonNormalize->callback(OnNormalizeCB, this);
	
	_buttonDump = new Fl_Button(700, 35, 80, 25, "Dump");
	_buttonDump->callback(OnDumpCB, this);

	_browserPhoneme[0] = new Fl_Hold_Browser(10, 80, 70, 350, "Phoneme1");
	_browserPhoneme[0]->align(FL_ALIGN_TOP);
	_browserPhoneme[0]->callback(OnPhoneme1SelectCB, this);

	_browserPhoneme[1] = new  Fl_Hold_Browser(85, 80, 70, 350, "Phoneme2");
	_browserPhoneme[1]->align(FL_ALIGN_TOP);
	_browserPhoneme[1]->callback(OnPhoneme2SelectCB, this);

	_browserSinglePhoneme = new Fl_Hold_Browser(160, 80, 70, 350, "Single Phoneme");
	_browserSinglePhoneme->align(FL_ALIGN_TOP);
	_browserSinglePhoneme->callback(OnSinglePhonemeSelectCB, this);
	
	Fl_Menu_Bar* menuBar = new Fl_Menu_Bar(0, 0, w, 30);
	//menuBar->menu(menu_);
	menuBar->add("&File/Save lip sync file", 0, OnSaveCB, this, 0);
	menuBar->add("&File/Load lip sync file", 0, OnLoadCB, this, 0);

	menuBar->add("&Lip Sync/Phoneme Alignment Command", 0, OnPhonemeAlignmentCommandCB, this, 0);
	//menuBar->callback(OnMenuSelectCB, this);

	_curveEditor = new VisemeCurveEditor(235, 80, 395, 355, "Animation Curve");
	_curveEditor->setVisemeWindow(this);
	_curveEditor->color(FL_GRAY0, FL_GRAY0);

	_browserViseme = new Fl_Multi_Browser(650, 80, 70, 350, "Visemes");
	_browserViseme->align(FL_ALIGN_TOP);
	_browserViseme->callback(OnVisemeSelectCB, this);

	_browserDiphone = new Fl_Hold_Browser(725, 80, 70, 350, "Diphones");
	_browserDiphone->align(FL_ALIGN_TOP); 
	_browserDiphone->callback(OnDiphoneSelectCB, this);

	Fl_Group* phonemeAlignmentGroup = new Fl_Group(10, 590, 700, 165);
	phonemeAlignmentGroup->begin();

	Fl_Box* box = new Fl_Box(10, 590, 700, 165, "Phoneme Alignment");
	box->align(FL_ALIGN_TOP | FL_ALIGN_LEFT);
	box->box(FL_BORDER_BOX);
	
	_buttonSpeechFile = new Fl_Button(560, 600, 30, 25, "...");
	_buttonSpeechFile->callback(OnSoundFileLoadCB, this);
	_inputSpeechFile = new Fl_Input(115, 600, 435, 25, "Speech File");
	_inputTranscription = new Fl_Input(115, 630, 435, 25, "Transcription");
	_inputTranscription->when(FL_WHEN_CHANGED);
	_inputTranscription->callback(OnPhonemeTranslateCB, this);
	_buttonGeneratePhonemes = new Fl_Button(560, 630, 100, 25, "Generate Phonemes");
	_buttonGeneratePhonemes->callback(OnPhonemeTranslateCB, this);
	_inputPhonemes = new Fl_Input(115, 660, 435, 25, "Phonemes");
	_buttonDictionaryFile = new Fl_Button(560, 660, 100, 25, "Dictionary");
	_buttonDictionaryFile->callback(OnDictionaryLoadCB, this);
	_inputLipSyncFolder = new Fl_Input(115, 690, 435, 25, "Batch lip sync folder");
	_buttonLipSyncFolder = new Fl_Button(560, 690, 30, 25, "...");
	_buttonLipSyncFolder->callback(OnChangeLipSyncFolderCB, this);
	_checkRunBatchMode = new Fl_Check_Button(115, 720, 30, 25, "Batch Mode");
	_buttonGenerateLipSync = new Fl_Button(560, 720, 100, 25, "Generate lip sync");
	_buttonGenerateLipSync->callback(OnGenerateLipSyncCB, this);

	phonemeAlignmentGroup->end();

	this->end();

	_inputAudioFile->deactivate();
	_choiceAudioFile->deactivate();
	_buttonPlayAudioFile->deactivate();
	_inputUtterance->deactivate();
	_buttonPlayDialog->deactivate();

	_gatherStats = false;
	_useRemote = true;

	_windowVisemeRunTime = NULL;
	_imageSequenceViewer = NULL;

	loadData();

}

VisemeViewerWindow::~VisemeViewerWindow()
{
	if (_windowVisemeRunTime != NULL)
	{
		delete _windowVisemeRunTime;
		_windowVisemeRunTime = NULL;
	}
}
void VisemeViewerWindow::show()
{
	SBWindowListener::windowShow();
	Fl_Double_Window::show();

	BML::Processor* bp = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor();
	bp->registerRequestCallback(OnBmlRequestCB, this);
}

void VisemeViewerWindow::hide()
{
	SBWindowListener::windowHide();
	Fl_Double_Window::hide();

	BML::Processor* bp = SmartBody::SBScene::getScene()->getBmlProcessor()->getBMLProcessor();
	bp->registerRequestCallback(NULL, NULL);
}

void VisemeViewerWindow::update()
{
	const std::string& charName = this->getCurrentCharacterName();
	if (charName == "")
		return;

	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(charName);
	if (!character)
		return;

	const std::string& voice = character->getVoice();
	if (voice == "audiofile")
	{
		setUseRemote(false);
	}
	if (voice == "remote")
	{
		setUseRemote(true);
	}
}


void VisemeViewerWindow::selectViseme(int id)
{
	_browserViseme->select(id);
}

Fl_Menu_Item VisemeViewerWindow::menu_[] = {
	{"File", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
	{"Save", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{"Load", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
	{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}
};

void VisemeViewerWindow::draw()
{
	_curveEditor->redraw();

	Fl_Double_Window::draw();

	drawNames();
}

bool  VisemeViewerWindow::loadData()
{
	SmartBody::SBPhonemeManager* diphoneManager = SmartBody::SBScene::getScene()->getDiphoneManager();
	std::vector<std::string> commonPhonemes = diphoneManager->getCommonPhonemes();

	for (int x = 0; x < 2; x++)
	{
		_browserPhoneme[x]->clear();
		
		for (size_t p = 0; p < commonPhonemes.size(); p++)
		{
			std::string lowerCasePhoneme = commonPhonemes[p];
			std::transform(lowerCasePhoneme.begin(), lowerCasePhoneme.end(), lowerCasePhoneme.begin(), ::tolower);
			_browserPhoneme[x]->add(lowerCasePhoneme.c_str());
		}		
		_browserPhoneme[x]->deselect();
	}

	_browserSinglePhoneme->clear();
	for (size_t p = 0; p < commonPhonemes.size(); p++)
	{
		std::string lowerCasePhoneme = commonPhonemes[p];
		std::transform(lowerCasePhoneme.begin(), lowerCasePhoneme.end(), lowerCasePhoneme.begin(), ::tolower);
		_browserSinglePhoneme->add(lowerCasePhoneme.c_str());
	}
	_browserSinglePhoneme->deselect();

	std::string curSelectedCharacter = "";
	int curSelectedId = -1;
	if (_choiceCharacter->value() > -1)
	{
		curSelectedCharacter = _choiceCharacter->text(_choiceCharacter->value());
	}
	_choiceCharacter->clear();
	const std::vector<std::string>& characterNames = SmartBody::SBScene::getScene()->getCharacterNames();
	for (size_t i = 0; i < characterNames.size(); i++)
	{
		_choiceCharacter->add(characterNames[i].c_str());
		if (curSelectedCharacter == characterNames[i])
			curSelectedId = i;
	}
	if (characterNames.size() > 0)
	{
		if (curSelectedId < 0)
			curSelectedId = 0;
		_choiceCharacter->value(curSelectedId);

		OnCharacterSelectCB(this->_choiceCharacter, this);
	}
	initializeVisemes();
	update();
	return true;
}

void VisemeViewerWindow::initializeVisemes()
{
	_curveEditor->generateCurves(_browserViseme->size());
}

void VisemeViewerWindow::resetViseme()
{
	std::string characterName = getCurrentCharacterName();
	for (int i = 0; i < _browserViseme->size(); i++)
	{
		std::stringstream strstr;
		if (SmartBody::SBScene::getScene()->isRemoteMode())
			strstr << "send sbm ";
		strstr << "char " << characterName << " viseme " << _browserViseme->text(i + 1) << " " << 0.0f;		
		
		SmartBody::SBScene::getScene()->command(strstr.str());		
	}
	_checkEnableScrub->value(0);
	_sliderCurveAnimation->value(0);
	_sliderCurveAnimation->deactivate();
}

SmartBody::SBCharacter* VisemeViewerWindow::getCurrentCharacter()
{
	int characterIndex = _choiceCharacter->value();
	if (characterIndex >= 0)
	{
		const char* characterName = _choiceCharacter->menu()[_choiceCharacter->value()].label();
		SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(characterName);	
		return character;
	}
	else
		return NULL;
}

std::string VisemeViewerWindow::getCurrentCharacterName()
{
	int characterIndex = _choiceCharacter->value();
	if (characterIndex >= 0)
	{
		const char* characterName = _choiceCharacter->menu()[_choiceCharacter->value()].label();
		return characterName;
	}
	else
		return "";
}

SmartBody::SBDiphone* VisemeViewerWindow::getCurrentDiphone()
{
	std::string phoneme1 = "";
	std::string phoneme2 = "";
	if (_browserSinglePhoneme->value() > 0)
	{
		phoneme1 = _browserSinglePhoneme->text(_browserSinglePhoneme->value());
		phoneme2 = "-";
	}
	else if (_browserPhoneme[0]->value() > 0 && _browserPhoneme[1]->value() > 0)
	{
		phoneme1 = _browserPhoneme[0]->text(_browserPhoneme[0]->value());
		phoneme2 = _browserPhoneme[1]->text(_browserPhoneme[1]->value());
	}

	if (phoneme1 == "" || phoneme2 == "" || !getCurrentCharacter())
		return NULL;

	const std::string& diphoneMap = SmartBody::SBScene::getScene()->getCharacter(getCurrentCharacterName())->getStringAttribute("lipSyncSetName");
	// map the phones to their common set partner
	SmartBody::SBPhonemeManager* diphoneManager = SmartBody::SBScene::getScene()->getDiphoneManager();

	SmartBody::SBDiphone* diphone = diphoneManager->getMappedDiphone(phoneme1, phoneme2, diphoneMap);
	return diphone;
}

void VisemeViewerWindow::updateViseme()
{
	for(int i = 1; i <= _browserViseme->size(); i++)
	{
		if(_browserViseme->selected(i)> 0)
			_curveEditor->setVisibility(i - 1, true);
		else
			_curveEditor->setVisibility(i - 1, false);
	}
}

void VisemeViewerWindow::refreshData()
{
	SmartBody::SBPhonemeManager* diphoneManager = SmartBody::SBScene::getScene()->getDiphoneManager();
	SmartBody::SBDiphone* diphone = getCurrentDiphone();

	if (!diphone)
	{
		int value1 = _browserPhoneme[0]->value();
		int value2 = _browserPhoneme[1]->value();
		int value = _browserSinglePhoneme->value();
		std::string phoneme1 = "";
		std::string phoneme2 = "";

		if (value1 > 0 && value2 > 0)
		{
			phoneme1 = _browserPhoneme[0]->text(value1);
			phoneme2 = _browserPhoneme[1]->text(value2);
		}
		else if (value > 0)
		{
			phoneme1 = _browserSinglePhoneme->text(value);
			phoneme2 = "-";
		}
		const std::string& diphoneMap = SmartBody::SBScene::getScene()->getCharacter(getCurrentCharacterName())->getStringAttribute("lipSyncSetName");
		diphone = diphoneManager->createDiphone(phoneme1, phoneme2, diphoneMap);
	}
	else
	{
		diphone->clean();
	}

	for (int i = 0; i < _browserViseme->size(); i++)
	{
		if (_browserViseme->selected(i + 1))
		{
			std::vector<float>& key = diphone->getKeys(_browserViseme->text(i + 1));
			key.clear();

			for (size_t j = 0; j < _curveEditor->getCurves()[i].size(); j++)
			{
				key.push_back(_curveEditor->getCurves()[i][j].x);
				key.push_back(_curveEditor->getCurves()[i][j].y);
			}
		}
	}
}

bool VisemeViewerWindow::isPlayingViseme()
{
	int val = _checkEnableScrub->value();
	bool ret = false;
	if (val == 1)
		ret = true;
	return ret;
}

float VisemeViewerWindow::getSliderValue()
{
	return (float)_sliderCurveAnimation->value();
}

void VisemeViewerWindow::OnPhoneme1SelectCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;
	
	if(viewer->_browserPhoneme[0]->value() > 0)
		viewer->_phonemesSelected[0] = true;
	else 
	{
		viewer->_phonemesSelected[0] = false;
	}

	viewer->_browserDiphone->deselect();
	

	if(viewer->_phonemesSelected[0] && viewer->_phonemesSelected[1]){
		int lineSelected1 = viewer->_browserPhoneme[0]->value();
		int lineSelected2 = viewer->_browserPhoneme[1]->value();
		viewer->_browserSinglePhoneme->deselect();
		viewer->selectViseme(viewer->_browserPhoneme[0]->text(lineSelected1), viewer->_browserPhoneme[1]->text(lineSelected2));
	}
	viewer->resetViseme();
	viewer->updateViseme();
//	viewer->_curveEditor->redraw();
	viewer->redraw();
}

void VisemeViewerWindow::OnPhoneme2SelectCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;
	
	if(viewer->_browserPhoneme[1]->value() > 0)
		viewer->_phonemesSelected[1] = true;
	else 
		viewer->_phonemesSelected[1] = false;

	viewer->_browserDiphone->deselect();

	if(viewer->_phonemesSelected[0] && viewer->_phonemesSelected[1]){
		int lineSelected1 = viewer->_browserPhoneme[0]->value();
		int lineSelected2 = viewer->_browserPhoneme[1]->value();
		viewer->_browserSinglePhoneme->deselect();
		viewer->selectViseme(viewer->_browserPhoneme[0]->text(lineSelected1), viewer->_browserPhoneme[1]->text(lineSelected2));
	}
	viewer->resetViseme();
	viewer->updateViseme();
//	viewer->_curveEditor->redraw();
	viewer->redraw();
}


void VisemeViewerWindow::OnVisemeSelectCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;
	
	bool shouldContinue = false;
	if (viewer->_browserSinglePhoneme->value() > 0)
		shouldContinue = true;

	if (viewer->_browserPhoneme[0]->value() > 0 ||
		viewer->_browserPhoneme[1]->value() > 0)
	{
		shouldContinue = true;
	}

	if (!shouldContinue)
	{
		viewer->_browserViseme->deselect();
		return;
	}

	int viseme = viewer->_browserViseme->value();
	if (viseme <= 0)
		return;

	std::vector<float> curveData;
	std::vector<float> phonemeCurve1;
	std::vector<float> phonemeCurve2;
	SmartBody::SBDiphone* diphone = viewer->getCurrentDiphone();
	if (diphone)
	{
		const std::vector<std::string>& visemeNames = diphone->getVisemeNames();

		for (size_t i = 0; i < visemeNames.size(); i++)
		{
			if (visemeNames[i] == viewer->_browserViseme->text(viseme))
			{
				curveData = diphone->getKeys(visemeNames[i]);
				break;
			}
		}
	}

	viewer->_curveEditor->changeCurve(viseme - 1, curveData, phonemeCurve1, phonemeCurve2);
	viewer->refreshData();
	viewer->_curveEditor->refresh();

	viewer->_curveEditor->selectLine(viseme - 1);	
	viewer->resetViseme();
	viewer->updateViseme();
	viewer->redraw();
}

void VisemeViewerWindow::drawNames()
{
	std::vector<VisemeCurve>& curves = _curveEditor->getCurves();

	for (size_t i = 0; i < curves.size(); i++)
	{
		fl_color(FL_BLACK);

		if(!curves[i].isVisible())
			continue;

		SrVec point1 = _curveEditor->mapCurveData(curves[i][0]);

		fl_draw(_browserViseme->text((int)(i+1)), (int)point1.x, (int)point1.y - 10);
	}
}

void VisemeViewerWindow::selectViseme(const char * phoneme1, const char * phoneme2)
{
	if (_browserViseme->size() == 0)
		return;
	
	if (!phoneme1 || !phoneme2)
		return;

	std::string p1 = phoneme1;
	std::string p2 = phoneme2;

	std::transform(p1.begin(), p1.end(), p1.begin(), ::tolower);
	std::transform(p2.begin(), p2.end(), p2.begin(), ::tolower);

	const std::string& diphoneMap = SmartBody::SBScene::getScene()->getCharacter(getCurrentCharacterName())->getStringAttribute("lipSyncSetName");

	SmartBody::SBDiphone* diphone = SmartBody::SBScene::getScene()->getDiphoneManager()->getMappedDiphone(p1, p2, diphoneMap);
	SmartBody::SBDiphone* diphone1 = SmartBody::SBScene::getScene()->getDiphoneManager()->getMappedDiphone(p1, "-", diphoneMap);
	SmartBody::SBDiphone* diphone2 = SmartBody::SBScene::getScene()->getDiphoneManager()->getMappedDiphone(p2, "-", diphoneMap);
	
	_browserViseme->deselect();
	bool shouldProcess = true;
	if (diphone)
	{
		if (diphone->getNumVisemes() > 0)
		{
			shouldProcess = false;
			const std::vector<std::string>& visemeNames = diphone->getVisemeNames();
			for (int i = 0; i < _browserViseme->size(); i++)
			{
				for (size_t j = 0; j < visemeNames.size(); j++)
				{
					if (visemeNames[j] == _browserViseme->text(i + 1))
					{
						_browserViseme->select(i + 1);
						std::vector<float> phonemeCurve1;
						std::vector<float> phonemeCurve2;
						_curveEditor->changeCurve(i, diphone->getKeys(visemeNames[j]), phonemeCurve1, phonemeCurve2);
						// set the curve visible
						_curveEditor->setVisibility(i, true);
						_curveEditor->selectLine(i);
						break;
					}
				}
			}
		}
	}

	if (shouldProcess)
	{
		if (!diphone)
			diphone = SmartBody::SBScene::getScene()->getDiphoneManager()->createDiphone(p1, p2, diphoneMap);
		// need to improve the performance later
		if (diphone1)	// compose the diphone
		{
			const std::vector<std::string>& visemeNames = diphone1->getVisemeNames();
			for (int i = 0; i < _browserViseme->size(); i++)
			{
				for (size_t j = 0; j < visemeNames.size(); j++)
				{
					if (visemeNames[j] == _browserViseme->text(i + 1))
					{
						_browserViseme->select(i + 1);
						std::vector<float> phonemeCurve1 = diphone1->getKeys(visemeNames[j]);
						std::vector<float> phonemeCurve2;
						if (diphone2)
							phonemeCurve2 = diphone2->getKeys(visemeNames[j]);
						_curveEditor->changeCurve(i, diphone->getKeys(visemeNames[j]), phonemeCurve1, phonemeCurve2);
						// set the curve visible
						_curveEditor->setVisibility(i, true);
						_curveEditor->selectLine(i);
						break;
					}
				}
			}
		}
		if (diphone2)	// compose the diphone
		{
			const std::vector<std::string>& visemeNames = diphone2->getVisemeNames();
			for (int i = 0; i < _browserViseme->size(); i++)
			{
				for (size_t j = 0; j < visemeNames.size(); j++)
				{
					if (visemeNames[j] == _browserViseme->text(i + 1))
					{
						_browserViseme->select(i + 1);
						std::vector<float> phonemeCurve2 = diphone2->getKeys(visemeNames[j]);
						std::vector<float> phonemeCurve1;
						if (diphone1)
							phonemeCurve1 = diphone1->getKeys(visemeNames[j]);
						_curveEditor->changeCurve(i, diphone->getKeys(visemeNames[j]), phonemeCurve1, phonemeCurve2);
						// set the curve visible
						_curveEditor->setVisibility(i, true);
						_curveEditor->selectLine(i);
						break;
					}
				}
			}
		}
		refreshData();
	}


	/*
	int viseme = rand() % _browserViseme->size() + 1;

	// unselect first
	_browserViseme->deselect();
	_browserViseme->select(viseme);

	_curveEditor->setVisibility(viseme - 1, true);
	_curveEditor->selectLine(viseme - 1);
	*/
}

void VisemeViewerWindow::selectPhonemes(const char * viseme)
{
	/*
	int phoneme1 = rand() % _browserPhoneme[0]->size() + 1;
	int phoneme2 = rand() % _browserPhoneme[1]->size() + 1;

	// unselect phonemes first
	_browserPhoneme[0]->deselect();
	_browserPhoneme[1]->deselect();

	_browserPhoneme[0]->select(phoneme1);
	_browserPhoneme[1]->select(phoneme2);
	*/
}

void VisemeViewerWindow::OnSliderSelectCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;
	SmartBody::SBDiphone* curDiphone = viewer->getCurrentDiphone();
	if (!curDiphone)
		return;

	std::string characterName = viewer->getCurrentCharacterName();
	const std::vector<std::string>& visemeNames = curDiphone->getVisemeNames();
	float sliderValue = (float)viewer->_sliderCurveAnimation->value();
	for (size_t i = 0; i < visemeNames.size(); i++)
	{
		float curveValue = 0.0f;
		std::vector<float>& key = curDiphone->getKeys(visemeNames[i]);
		for (size_t k = 0; k < key.size() / 2 - 1; k++)
		{
			if (key[k * 2] <= sliderValue && key[(k + 1) * 2] >= sliderValue)
			{
				float f = (sliderValue - key[k * 2]) / (key[(k + 1) * 2] - key[k * 2]);
				curveValue = f * (key[(k + 1) * 2 + 1] - key[k * 2 + 1]) + key[k * 2 + 1];
				break;
			}
		}
		std::stringstream strstr;
		if (SmartBody::SBScene::getScene()->isRemoteMode())
			strstr << "send sbm ";
		strstr << "char " << characterName << " viseme " << visemeNames[i] << " " << curveValue;
		//LOG("%s", strstr.str().c_str());
		SmartBody::SBScene::getScene()->command(strstr.str());
	}
	viewer->_curveEditor->redraw();
}

void VisemeViewerWindow::OnCharacterSelectCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	SmartBody::SBCharacter* character = viewer->getCurrentCharacter();

	if (!character)
	{
		viewer->_browserViseme->clear();
		viewer->_browserDiphone->clear();
		viewer->_browserPhoneme[0]->clear();
		viewer->_browserPhoneme[1]->clear();
		viewer->_browserSinglePhoneme->clear();
		return;
	}



	SmartBody::SBFaceDefinition* faceDefinition = character->getFaceDefinition();
	if (faceDefinition)

	{
		int numViseme = faceDefinition->getNumVisemes();
		viewer->_browserViseme->clear();
		for (int i = 0; i < numViseme; i++){
			char* c_str = (char*)faceDefinition->getVisemeName(i).c_str();
//			viewer->enforceNamingConvention(c_str);
			viewer->_browserViseme->add(c_str);
		}

		// if it has blend shape, add those as well
		for (int jointCounter = 0; jointCounter < character->getSkeleton()->getNumJoints(); ++jointCounter)
		{
			if (character->getSkeleton()->getJoint(jointCounter)->getJointType() == SkJoint::TypeBlendShape)
			{
				const std::string& jointName = character->getSkeleton()->getJoint(jointCounter)->getName();
				viewer->_browserViseme->add(jointName.c_str());
			}
		}
	}

	viewer->initializeVisemes();
	viewer->_browserPhoneme[0]->deselect();
	viewer->_browserPhoneme[1]->deselect();
	viewer->resetViseme();
	viewer->updateViseme();
	viewer->_curveEditor->redraw();
}


void VisemeViewerWindow::OnEnableScrub(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;
	if (viewer->_checkEnableScrub->value() == 1)
	{
		viewer->_sliderCurveAnimation->activate();
	}
	else
	{	
		viewer->_sliderCurveAnimation->value(0);
		viewer->_sliderCurveAnimation->deactivate();
	}
}

void VisemeViewerWindow::OnPlayCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	float playTime = (float)atof(viewer->_inputPlayTime->value());
	if (playTime > 0)
	{
		for (int i = 0; i < viewer->_browserViseme->size(); i++)
		{
			if (viewer->_browserViseme->selected(i + 1))
			{
				std::stringstream strstr;
				if (SmartBody::SBScene::getScene()->isRemoteMode())
					strstr << "send sbm ";
				strstr << "char " << viewer->getCurrentCharacterName() << " viseme " << viewer->_browserViseme->text(i + 1) << " curve ";
				strstr << viewer->_curveEditor->getCurves()[i].size() << " ";
				for (size_t j = 0; j < viewer->_curveEditor->getCurves()[i].size(); j++)
					strstr << viewer->_curveEditor->getCurves()[i][j].x * playTime << " " << viewer->_curveEditor->getCurves()[i][j].y << " ";
				SmartBody::SBScene::getScene()->command(strstr.str());		
			}
		}
	}
	//viewer->OnCharacterRefreshCB(widget, data);
}


void VisemeViewerWindow::OnPlayDialogCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;
	
	std::string utterance = viewer->_inputUtterance->value(); 	
	std::string utteranceClean = vhcl::Replace(utterance, "'", "\\'");
	if (utterance != "")
	{
		std::stringstream strstr;
		if (SmartBody::SBScene::getScene()->isRemoteMode())
			strstr << "send sbm ";
		strstr << "python bml.execBML('" << viewer->getCurrentCharacterName() << "', '<speech type=\"text/plain\">" << utteranceClean << "</speech>')";
		SmartBody::SBScene::getScene()->command(strstr.str());
	}
}

void VisemeViewerWindow::OnPlayAudioFileCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;
	std::string fileName = viewer->_inputAudioFile->value();
	if (fileName != "")
	{
		std::stringstream strstr;
		if (SmartBody::SBScene::getScene()->isRemoteMode())
			strstr << "send sbm ";
		strstr << "python bml.execBML('" << viewer->getCurrentCharacterName() << "', '<speech type=\"text/plain\" ref=\"" << fileName << "\">" << "</speech>')";
		SmartBody::SBScene::getScene()->command(strstr.str());
	}
	//viewer->OnCharacterRefreshCB(widget, data);
}

void VisemeViewerWindow::loadAudioFiles()
{
	SmartBody::SBCharacter* character = getCurrentCharacter();
	if (!character)
		return;

	if (getUseRemote())
		return;


	_choiceAudioFile->clear();
	// if an audio path is present, use it
	bool useAudioPaths = true;
	std::vector<std::string> audioPaths = SmartBody::SBScene::getScene()->getAssetManager()->getAssetPaths("audio");
	std::string relativeAudioPath = "";
	for (size_t audioPathCounter = 0; audioPathCounter < audioPaths.size(); ++audioPathCounter)
	{
		relativeAudioPath = audioPaths[audioPathCounter];
		boost::filesystem::path p(relativeAudioPath);
		p /= character->getVoiceCode();

#if (BOOST_VERSION > 104400)
		boost::filesystem::path abs_p = boost::filesystem::absolute( p );	
		if( !boost::filesystem::exists( abs_p ))
#else
		boost::filesystem::path abs_p = boost::filesystem::complete( p );	
		if( !boost::filesystem2::exists( abs_p ))
#endif
		{
			//LOG( "VisemeViewerWindow::loadAudioFiles: path to audio file cannot be found: %s", abs_p.native_directory_string().c_str());
			continue;
		}
		boost::filesystem::directory_iterator end;
		for( boost::filesystem::directory_iterator i(abs_p); i!=end; ++i ) 
		{
			const boost::filesystem::path& cur = *i;
			if (boost::filesystem::is_directory(cur)) 
			{
				;	
			} 
			else 
			{
				std::string ext = boost::filesystem::extension(cur);
				if (_stricmp(ext.c_str(), ".bml" ) == 0)
				{
#if (BOOST_VERSION > 104400)
					_choiceAudioFile->add(cur.stem().string().c_str());
#else
					_choiceAudioFile->add(cur.stem().c_str());
#endif
				}
			}
		}
	}
}

void VisemeViewerWindow::OnAudioFileSelectCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	int audioFileIndex = viewer->_choiceAudioFile->value();
	if (audioFileIndex >= 0)
	{
		const char* audioFileName = viewer->_choiceAudioFile->menu()[viewer->_choiceAudioFile->value()].label();
		viewer->_inputAudioFile->value(audioFileName);	
	}
}

void VisemeViewerWindow::OnSaveCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	std::string mediaPath = SmartBody::SBScene::getScene()->getMediaPath();
	std::string fileName = BaseWindow::chooseFile("Lip Sync File:", "Python\t*.py\n", mediaPath);
	if (fileName == "")
		return;

	const std::string& diphoneMap = SmartBody::SBScene::getScene()->getCharacter(viewer->getCurrentCharacterName())->getStringAttribute("lipSyncSetName");

	// fill in
	std::stringstream strstr;
	strstr << "# lip sync set: " << diphoneMap << "\n";
	strstr << "# autogenerated by SmartBody\n";
	strstr << "\n";
	strstr << "diphoneManager = scene.getDiphoneManager()\n";

	std::string diphoneSetName = viewer->getCurrentCharacter()->getStringAttribute("lipSyncSetName");
	if (diphoneSetName == "")
		diphoneSetName = viewer->getCurrentCharacterName();

	strstr << "diphoneManager.deleteDiphoneSet(\"" << diphoneSetName << "\")\n";
	strstr << "\n";

	std::vector<SmartBody::SBDiphone*>& diphones = SmartBody::SBScene::getScene()->getDiphoneManager()->getDiphones(diphoneSetName);
	for (size_t i = 0; i < diphones.size(); i++)
	{
		strstr << "diphone = diphoneManager.createDiphone(\"" << diphones[i]->getFromPhonemeName() << "\", \"" << diphones[i]->getToPhonemeName() << "\", \"" << diphoneMap << "\")" << "\n";
		const std::vector<std::string>& visemeNames = diphones[i]->getVisemeNames();
		for (size_t n = 0; n < visemeNames.size(); n++)
		{
			std::vector<float>& key = diphones[i]->getKeys(visemeNames[n]);
			for (size_t k = 0; k < key.size() / 2; k++)
			{
				strstr << "diphone.addKey(\"" << visemeNames[n] << "\", " << key[k * 2 + 0] << ", " << key[k * 2 + 1] << ")" << "\n";
			}
			strstr << "\n";
		}
	}

	// save to the file
	std::ofstream file(fileName.c_str());	
	if (file.is_open() != true)
	{
		fl_alert("Problem writing to file %s, diphone was not saved.", fileName.c_str());
		return;
	}
	file << strstr.str();
	file.close();	
}

void VisemeViewerWindow::OnLoadCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	std::string mediaPath = SmartBody::SBScene::getScene()->getMediaPath();
	std::string fileName = BaseWindow::chooseFile("Lip Sync File:", "Python\t*.py\n", mediaPath);
	if (fileName == "")
		return;

	try {
		std::stringstream strstr;
		strstr << "execfile(\"" << fileName << "\")";
		PyRun_SimpleString(strstr.str().c_str());
		PyErr_Print();
		PyErr_Clear();
	} catch (...) {
		PyErr_Print();
	}

	viewer->updateViseme();
	viewer->redraw();
}

void VisemeViewerWindow::OnPhonemeAlignmentCommandCB(Fl_Widget* widget, void* data)
{
	std::string lipSyncCommand = "";
	lipSyncCommand = SmartBody::SBScene::getSystemParameter("lipsynccommand");

	const char* cmd = fl_input("Command to run phoneme scheduler:\n<command> <wavfile> <phones file> <transcriptionfile>", lipSyncCommand.c_str());
	if (cmd)
	{
		std::string newLipSyncCommand = cmd;
		SmartBody::SBScene::setSystemParameter("lipsynccommand", newLipSyncCommand);
	}
}

void VisemeViewerWindow::OnBmlRequestCB(BML::BmlRequest* request, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	if (viewer->_windowVisemeRunTime)
		viewer->_windowVisemeRunTime->retrievingCurveData(request);

	std::string utterance = viewer->_inputUtterance->value(); 	

	if(utterance == viewer->_lastUtterance && viewer->_useRemote)
		return;

	BML::VecOfBehaviorRequest b = request->behaviors;
	for (BML::VecOfBehaviorRequest::iterator iter = b.begin();
		iter != b.end();
		iter++)
	{
		BML::BehaviorRequestPtr requestPtr = (*iter);
		BML::BehaviorRequest* behavior = requestPtr.get();
		BML::SpeechRequest* speechRequest = dynamic_cast<BML::SpeechRequest*> (behavior);
		if (speechRequest)
		{
			viewer->_browserDiphone->clear();
			char* v[2];
			std::vector<SmartBody::VisemeData*>& phonemes = speechRequest->getPhonemes();
			for ( size_t i = 0; (i + 1) < phonemes.size(); i++ )
			{
				v[0] = (char*)phonemes[i]->id();
				v[1] = (char*)phonemes[i + 1]->id();
				
				for(int j = 0; j < 2; j++)
					viewer->enforceNamingConvention(v[j]);

				std::stringstream strstr;
				strstr << v[0] << " - " << v[1];
				

				viewer->_browserDiphone->add(strstr.str().c_str());

				if (viewer->_gatherStats)
				{
					std::map<std::string, int>::iterator iter = viewer->_diphoneStats.find(strstr.str());
					if (iter == viewer->_diphoneStats.end())
					{
						std::pair<std::string, int> entry;
						entry.first = strstr.str();
						entry.second = 1;
						viewer->_diphoneStats.insert(entry);
					}
					else
					{
						int& count = (*iter).second;
						count++;
					}
				}
			}

			if (phonemes.size() > 0)
			{
				float length = phonemes[phonemes.size() - 1]->time() - phonemes[0]->time();
				if (viewer->_imageSequenceViewer && viewer->_imageSequenceViewer->shown())
				{
					double timeDelay = atof(viewer->_imageSequenceViewer->_inputPlayTimeDelay->value());
					double inputLength = atof(viewer->_imageSequenceViewer->_inputAudioLength->value());
					if (inputLength > 0)
						length = (float)inputLength;
					viewer->_imageSequenceViewer->playbackSequence((float)SmartBody::SBScene::getScene()->getSimulationManager()->getTime(), length, (float)timeDelay);
				}
			}
		}
	}
	viewer->_lastUtterance = utterance;
}

void VisemeViewerWindow::OnDiphoneSelectCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	if(viewer->_browserDiphone->size() == 0)
		return;

	if (viewer->_browserDiphone->value() < 0 || viewer->_browserDiphone->value() >= (viewer->_browserDiphone->size() + 1))
		return;

	std::string str = viewer->_browserDiphone->text(viewer->_browserDiphone->value());
	std::vector<std::string> diphones;
	std::stringstream ss(str);

	std::string tok;

	while (ss >> tok)
	{
		if (tok == "-")
			continue;
        
		diphones.push_back(tok);
	}

	if (diphones.size() != 2)
	{
		LOG("Could not parse two phonemes from '%s'", str.c_str()); 
			return;
	}

	// get the mapped phoneme
	std::string mappedPhoneme[2];
	mappedPhoneme[0] = SmartBody::SBScene::getScene()->getDiphoneManager()->getPhonemeMapping(diphones[0]);
	mappedPhoneme[1] = SmartBody::SBScene::getScene()->getDiphoneManager()->getPhonemeMapping(diphones[1]);
	// convert the chosen diphones to lower case
	std::transform(mappedPhoneme[0].begin(), mappedPhoneme[0].end(), mappedPhoneme[0].begin(), ::tolower);
	std::transform(mappedPhoneme[1].begin(), mappedPhoneme[1].end(), mappedPhoneme[1].begin(), ::tolower);

	viewer->_browserPhoneme[0]->deselect();
	viewer->_browserPhoneme[1]->deselect();

	for(int i =0; i < 2; i++){
		for(int j = 1; j <= viewer->_browserPhoneme[0]->size(); j++){
			std::string diphone = viewer->_browserPhoneme[i]->text(j);
			
			if(diphone == mappedPhoneme[i])
			{
				viewer->_browserPhoneme[i]->select(j);
				break;
			}
		}
	}

	viewer->_phonemesSelected[0] = true;
	viewer->_phonemesSelected[1] = true;

	if(viewer->_phonemesSelected[0] && viewer->_phonemesSelected[1]){
		int lineSelected1 = viewer->_browserPhoneme[0]->value();
		int lineSelected2 = viewer->_browserPhoneme[1]->value();
		viewer->_browserSinglePhoneme->deselect();
		viewer->selectViseme(viewer->_browserPhoneme[0]->text(lineSelected1), viewer->_browserPhoneme[1]->text(lineSelected2));
	}
	viewer->resetViseme();
	viewer->updateViseme();
	viewer->redraw();
}

void VisemeViewerWindow::enforceNamingConvention(char * c_str)
{
	if(islower(c_str[0]))
		c_str[0] = toupper(c_str[0]);

	for(size_t k = 1; k < strlen(c_str); k++)
	{
		if(isupper(c_str[k]))
			c_str[k] = ::tolower(c_str[k]);
	}
}

void VisemeViewerWindow::setUseRemote(bool val)
{
	_useRemote = val;
	if (_useRemote)
	{
		_inputAudioFile->deactivate();
		_choiceAudioFile->deactivate();
		_inputUtterance->activate();
		_buttonPlayAudioFile->deactivate();
		_buttonPlayDialog->activate();
	}
	else
	{
		_inputAudioFile->activate();
		_choiceAudioFile->activate();
		_inputUtterance->deactivate();
		_buttonPlayAudioFile->activate();
		_buttonPlayDialog->deactivate();
	}
}

bool VisemeViewerWindow::getUseRemote()
{
	return _useRemote;
}

typedef std::pair<std::string, int> data_a;     
std::map<std::string, int>::iterator a_it;  

template<class T>    
struct less_second : std::binary_function<T,T,bool>    
{    
    inline bool operator()( const T& lhs, const T& rhs )    
    {    
        return lhs.second < rhs.second;    
    }    
};

std::vector<data_a> sort_by_weight(std::map<std::string, int>& map)    
{        
    std::vector< data_a > vec(map.begin(), map.end());           
    std::sort(vec.begin(), vec.end(), less_second<data_a>());       
    return vec;    
} 


void VisemeViewerWindow::OnShowStatsCB(Fl_Widget* widget, void* data)
{
	std::string mediaPath = SmartBody::SBScene::getScene()->getMediaPath();
	std::string filename = BaseWindow::chooseFile("Save Lip Sync Stats:", "TXT file\t*.txt\n", mediaPath);
	if (filename == "")
		return;

	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	// sort the map
	std::vector<data_a> sortedDiphones = sort_by_weight(viewer->_diphoneStats);

	// determine the total instance count
	int instanceCount = 0;
	for (std::vector<data_a>::iterator iter = sortedDiphones.begin();
		 iter != sortedDiphones.end();
		 iter++)
	{
		instanceCount +=  (*iter).second; 
	}

	if (instanceCount == 0)
	{
		fl_alert("No diphone instances found. No file written.");
		return;
	}

	float instanceCountF = (float) instanceCount;
	SmartBody::SBPhonemeManager* diphoneManager = SmartBody::SBScene::getScene()->getDiphoneManager();
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(viewer->getCurrentCharacterName());
	std::string curDiphoneSet = character->getStringAttribute("lipSyncSetName");
	std::vector<SmartBody::SBDiphone*>& diphones = diphoneManager->getDiphones(curDiphoneSet);
		
	std::stringstream strstr;
	for (std::vector<data_a>::iterator iter = sortedDiphones.begin();
		 iter != sortedDiphones.end();
		 iter++)
	{
		float percentage = float((*iter).second) / instanceCountF;
		strstr << (*iter).first << " " << (*iter).second << " " << percentage;
		// if the diphone is missing from the current set, highlight it
		std::vector<std::string> tmp;
		vhcl::Tokenize((*iter).first, tmp, " -");
		if (tmp.size() == 2)
		{
			SmartBody::SBDiphone* diphone = diphoneManager->getMappedDiphone(tmp[0], tmp[1], curDiphoneSet);
			if (!diphone)
				strstr << "**** missing";
		}

	     strstr << "\n"; 
	}


	std::ofstream file(filename.c_str());
	if (file.is_open() != true)
	{
		fl_alert("Problem writing to file %s, diphone stats were not saved.", filename.c_str());
		return;
	}
	file << strstr.str();
	file.close();	

	

}

void VisemeViewerWindow::OnStatsResetCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;
	viewer->_diphoneStats.clear();
}

void VisemeViewerWindow::OnGatherStatsCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	Fl_Check_Button* check = dynamic_cast<Fl_Check_Button*>(widget);
	viewer->_gatherStats = check->value()? 1 : 0;
}

void VisemeViewerWindow::OnCharacterRefreshCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	viewer->loadData();
	viewer->OnCharacterSelectCB(widget, data);
	viewer->loadAudioFiles();
	viewer->redraw();
}

void VisemeViewerWindow::OnDumpCB(Fl_Widget* widget, void* data)
{
	std::string mediaPath = SmartBody::SBScene::getSystemParameter("mediapath");
	std::string filename = BaseWindow::chooseFile("Dump missing phone bigrams to:", "TXT files\t*.txt\n", mediaPath);
	if (filename == "")
		return;

	std::ofstream file(filename.c_str());
	if (file.is_open() != true)
	{
		fl_alert("Problem writing to file %s, diphone stats were not saved.", filename.c_str());
		return;
	}

	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	// construct the entire list of diphones
	SmartBody::SBPhonemeManager* diphoneManager = SmartBody::SBScene::getScene()->getDiphoneManager();

	std::vector<std::string> commonPhonemes = diphoneManager->getCommonPhonemes();
	for (size_t i = 0; i < commonPhonemes.size(); i++)
	{
		std::transform(commonPhonemes[i].begin(), commonPhonemes[i].end(), commonPhonemes[i].begin(), ::tolower); 
	}

	std::set<std::string> diphoneSet;
	for (size_t i = 0; i < commonPhonemes.size(); i++)
	{
		for (size_t j = 0; j < commonPhonemes.size(); j++)
		{
			std::string str = commonPhonemes[i];
			str.append(" - ");
			str.append(commonPhonemes[j]);
			diphoneSet.insert(str);
		}
	}

	// gather the current diphones, compare to entire list, and show missing ones
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(viewer->getCurrentCharacterName());
	std::string curDiphoneSet = character->getStringAttribute("lipSyncSetName");
	std::vector<SmartBody::SBDiphone*>& diphones = diphoneManager->getDiphones(curDiphoneSet);

	
	file << "# Autogenerated by SmartBody " << "\n";
	file << "# Using the following common phonemes:\n";
	for (size_t t = 0; t < commonPhonemes.size(); t++)
	{
		if (t == 0)
			file << "# ";
		file << commonPhonemes[t];
		if (t != commonPhonemes.size() - 1)
			file << ", ";
		else
			file << "\n";
	}
	file << "# The following diphones are missing from diphone set '" << curDiphoneSet << "'\n";

	for (size_t i = 0; i < commonPhonemes.size(); i++)
	{
		for (size_t j = 0; j < commonPhonemes.size(); j++)
		{
			 SmartBody::SBDiphone* diphone = diphoneManager->getDiphone(commonPhonemes[i], commonPhonemes[j], curDiphoneSet);
			 if (!diphone)
			 {
				 file <<  commonPhonemes[i] << " - " <<  commonPhonemes[j] << "\n";
			 }
		}	
	}

	file.close();	
}

void VisemeViewerWindow::OnNormalizeCB(Fl_Widget* widget, void* data)
{
	int confirm = fl_choice("This will normalize the phone bigrames.\nContinue?", "No", "Yes", NULL);
	if (confirm != 1)
	{
		return;
	}

	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	// construct the entire list of diphones
	SmartBody::SBPhonemeManager* diphoneManager = SmartBody::SBScene::getScene()->getDiphoneManager();
	SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(viewer->getCurrentCharacterName());
	if (!character)
		return;

	std::string curDiphoneSet = character->getStringAttribute("lipSyncSetName");
	diphoneManager->normalizeCurves(curDiphoneSet);
	viewer->redraw();

}

void VisemeViewerWindow::OnRunTimeCurvesCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	if (viewer->_windowVisemeRunTime == NULL)
	{
		viewer->_windowVisemeRunTime = new VisemeRunTimeWindow(150, 150, 800, 600, "Diphone Runtime Window");
	}
	viewer->_windowVisemeRunTime->show();
}

// single phoneme means selecting a diphone with phoneme1 and "-"
void VisemeViewerWindow::OnSinglePhonemeSelectCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	int lineSelected = viewer->_browserSinglePhoneme->value();
	if (lineSelected > 0)
	{
		viewer->_browserPhoneme[0]->deselect();
		viewer->_browserPhoneme[1]->deselect();	
		viewer->_browserDiphone->deselect();
		viewer->selectViseme(viewer->_browserSinglePhoneme->text(lineSelected), "-");
	}
	viewer->resetViseme();
	viewer->updateViseme();
	//	viewer->_curveEditor->redraw();
	viewer->redraw();
}

void VisemeViewerWindow::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
	this->loadData();
}

void VisemeViewerWindow::OnCharacterDelete( const std::string & name )
{
	if (this->getCurrentCharacterName() == name)
	{
		this->_choiceCharacter->clear();
		OnCharacterSelectCB(this->_choiceCharacter, this);
		this->redraw();
	}
}

void VisemeViewerWindow::OnCharacterUpdate( const std::string & name )
{
	if (this->getCurrentCharacterName() == name)
	{
		this->_choiceCharacter->value(0);
		this->loadData();
		this->redraw();
	}
}
      
void VisemeViewerWindow::OnSimulationUpdate()
{
	this->update();
}


void VisemeViewerWindow::OnPlayImageSequence(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;
	if (viewer->_imageSequenceViewer == NULL)
	{
		viewer->_imageSequenceViewer = new ImageSequenceViewer(100, 100, 500, 500, "Image Sequence Viewer");
	}
	viewer->_imageSequenceViewer->show();
}


void VisemeViewerWindow::OnDictionaryLoadCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	std::string file = BaseWindow::chooseFile("Dictionary File", "*.*", SmartBody::SBScene::getScene()->getMediaPath());

	SmartBody::SBPhonemeManager* diphoneManager = SmartBody::SBScene::getScene()->getDiphoneManager();
	if (file != "")
	{
		diphoneManager->loadDictionary("English", file);

		fl_alert("Dictionary loaded with %d words.", diphoneManager->getNumDictionaryWords("English"));
	}
}

std::string VisemeViewerWindow::translateWordsToPhonemes(const std::string& utterance)
{
	SmartBody::SBPhonemeManager* diphoneManager = SmartBody::SBScene::getScene()->getDiphoneManager();

	std::stringstream strstr;

	std::vector<std::string> tokens;
	vhcl::Tokenize(utterance, tokens, " .,?!\n\r\t");
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == "<sil>" ||
			tokens[i] == "<SIL>")
		{
			strstr << "sil ";
		}
		std::vector<std::string> phonemes = diphoneManager->getDictionaryWord("English", tokens[i]);
		for (size_t p = 0; p < phonemes.size(); p++)
		{
			strstr << phonemes[p] << " ";
		}
	}

	std::string phonemes = strstr.str();
	std::transform(phonemes.begin(), phonemes.end(), phonemes.begin(), ::tolower);

	return phonemes;
}


void VisemeViewerWindow::OnPhonemeTranslateCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	std::string utterance = viewer->_inputTranscription->value();

	std::string phonemes = VisemeViewerWindow::translateWordsToPhonemes(utterance);


	viewer->_inputPhonemes->value(phonemes.c_str());
}


void VisemeViewerWindow::OnSoundFileLoadCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	std::string curFile = viewer->_inputAudioFile->value();

	std::string startingDirectory = SmartBody::SBScene::getScene()->getMediaPath();
	if (curFile != "")
	{
		boost::filesystem::path p(curFile);
		std::string extension = boost::filesystem::extension(p);
		std::string basename = boost::filesystem::basename(p);
		boost::filesystem::path parentPath = p.parent_path();
		startingDirectory = parentPath.string();
	}
	std::string file = BaseWindow::chooseFile("Sound File", "*.wav", startingDirectory);
	if (file != "")
	{
		viewer->_inputSpeechFile->value(file.c_str());
	}
}

void VisemeViewerWindow::OnChangeLipSyncFolderCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	std::string curFile = viewer->_inputLipSyncFolder->value();

	std::string startingDirectory = SmartBody::SBScene::getScene()->getMediaPath();
	if (curFile != "")
	{
		boost::filesystem::path p(curFile);
		std::string extension = boost::filesystem::extension(p);
		std::string basename = boost::filesystem::basename(p);
		boost::filesystem::path parentPath = p.parent_path();
		startingDirectory = parentPath.string();
	}
	std::string file = BaseWindow::chooseDirectory("Lip sync output folder", startingDirectory);
	if (file != "")
	{
		viewer->_inputLipSyncFolder->value(file.c_str());
	}
}

void VisemeViewerWindow::OnGenerateLipSyncCB(Fl_Widget* widget, void* data)
{
	VisemeViewerWindow* viewer = (VisemeViewerWindow*) data;

	bool useBatchMode = viewer->_checkRunBatchMode->value();

	std::vector<boost::filesystem::path> filesToProcess;
	if (useBatchMode)
	{
		std::string batchFolder = viewer->_inputLipSyncFolder->value();
		if (batchFolder == "")
		{
			fl_alert("No batch processing folder specified.\nPlease enter a folder in the Batch Lip Sync Folder input\nthat contains both .wav and .txt files.");
			return;
		}
		LOG("Running forced alignment on folder %s", viewer->_inputLipSyncFolder->value());
		boost::filesystem::path someDir(viewer->_inputLipSyncFolder->value());
		boost::filesystem::directory_iterator end_iter;

		if ( boost::filesystem::exists(someDir) && 
			 boost::filesystem::is_directory(someDir))
		{

		  for( boost::filesystem::directory_iterator dir_iter(someDir) ; 
			   dir_iter != end_iter ; 
			   ++dir_iter)
		  {
			if (boost::filesystem::is_regular_file(dir_iter->status()) )
			{
				std::string fileExtension = boost::filesystem::extension((*dir_iter).path());

				std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::toupper);
				if (fileExtension == ".WAV")
				{
					filesToProcess.push_back((*dir_iter).path());
					LOG("Found sound file %s", (*dir_iter).path().string().c_str());
				}
			}
		  }
		}
	}
	else
	{
		std::string wavFile = viewer->_inputSpeechFile->value();
		if (wavFile == "")
		{
			fl_alert("No .wav files .\nPlease enter a .wav file in the Speech File input.");
			return;
		}
		std::string transcription = viewer->_inputTranscription->value();
		if (transcription == "")
		{
			fl_alert("No transcription.\nPlease enter a transcription of the .wav file in the Transcription input.\nMake sure to add <sil> to mark periods of silence in the recording.");
			return;
		}
		filesToProcess.push_back(viewer->_inputSpeechFile->value());
	}
	LOG("Running forced alignment on %d files.", filesToProcess.size());

	for (size_t f = 0; f < filesToProcess.size(); f++)
	{
		std::string extension = boost::filesystem::extension(filesToProcess[f]);
		std::string basename = boost::filesystem::basename(filesToProcess[f]);
		boost::filesystem::path wavPath = filesToProcess[f].parent_path();

		// create a transcription (.txt) file
		boost::filesystem::path transcriptionPath = wavPath;
		transcriptionPath /= basename + ".txt";
		if (useBatchMode)
		{
			// make sure the .txt file exists
			if (!boost::filesystem::exists(transcriptionPath) || 
				boost::filesystem::is_directory(transcriptionPath))
			{
				LOG("Transcription file %s does not exist, speech for file %s will not be processed.", 
					transcriptionPath.string().c_str(), filesToProcess[f].string().c_str());
				continue;
			}
		}
		else
		{
			ofstream mytxtfile;
			mytxtfile.open (transcriptionPath.string());
			mytxtfile << viewer->_inputTranscription->value();
			mytxtfile.close();
		}

		// create a .phones file
		boost::filesystem::path phonePath = wavPath;
		phonePath /= basename + ".phones";
		std::string transcription =  viewer->_inputTranscription->value();
		std::string phonemes = viewer->_inputPhonemes->value();
		if (useBatchMode)
		{
			std::stringstream strstr;
			std::string line;
			std::ifstream myfile (transcriptionPath.string().c_str());
			if (myfile.is_open())
			{
				while ( getline (myfile,line) )
				{
					strstr << line << " ";
				}
				myfile.close();
			}
			transcription = strstr.str();
			phonemes = VisemeViewerWindow::translateWordsToPhonemes(transcription);
		}
		
		ofstream myphonesfile;
		myphonesfile.open (phonePath.string());
		myphonesfile << phonemes;
		myphonesfile.close();

		// run the forced alignment command
		std::string lipSyncCommand = "";
		lipSyncCommand = SmartBody::SBScene::getSystemParameter("lipsynccommand");
		if (lipSyncCommand == "")
		{
			fl_alert("No lip sync command present in .smartbodysettings file.\nAdd to .smartbodysettings file: \nlipsynccommand=<command> <wavfile> <phones file> <transcriptionfile>"); 
			return;
		}
	
		std::string finalLipSyncCommand = vhcl::Replace(lipSyncCommand, "%1", filesToProcess[f].string());
		finalLipSyncCommand = vhcl::Replace(finalLipSyncCommand, "%2", phonePath.string());
		finalLipSyncCommand = vhcl::Replace(finalLipSyncCommand, "%3", transcriptionPath.string());
	
		LOG("Running lip sync with command: %s", finalLipSyncCommand.c_str());

		int ret = system(finalLipSyncCommand.c_str());
		if (ret == -1)
		{
			LOG("Lip sync command failed: %s", lipSyncCommand.c_str());
		}
	}
}
