#ifndef VisemeViewerWindow_h
#define VisemeViewerWindow_h

#include <vector>
#include <map>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>

#include <sb/SBCharacter.h>
#include <sb/SBPhoneme.h>
#include <sb/SBSceneListener.h>
#include <bml/bml.hpp>
#include <SBWindowListener.h>


class VisemeCurveEditor;
class VisemeRunTimeWindow;
class VisemeViewerWindow;
class ImageSequenceViewer;

class VisemeViewerWindow : public Fl_Double_Window, SBWindowListener {
public:
	VisemeViewerWindow(int x, int y, int w, int h, char* name);
	~VisemeViewerWindow();

	void refreshData();
	bool isPlayingViseme();
	float getSliderValue();
	void drawNames();
	void show();
	void hide();
	void update();
	void selectViseme(int id);

	void OnSimulationUpdate();
	void OnCharacterUpdate( const std::string & name );
	void OnCharacterDelete( const std::string & name );
	void OnCharacterCreate( const std::string & name, const std::string & objectClass );


	static Fl_Menu_Item menu_[];

	Fl_Choice * _choiceCharacter;
	Fl_Hold_Browser *_browserPhoneme[2];
	Fl_Multi_Browser *_browserViseme;
	Fl_Hold_Browser* _browserDiphone;
	Fl_Hold_Browser* _browserSinglePhoneme;
	Fl_Value_Slider *_sliderCurveAnimation;
	Fl_Button *_buttonPlay;
	Fl_Check_Button* _checkEnableScrub;
	
	Fl_Input* _inputPlayTime;
	Fl_Button* _buttonPlayDialog;
	Fl_Button* _buttonPlayAudioFile;
	Fl_Button* _buttonSetCharacterAudioFolder;
	Fl_Button* _buttonReset;
	Fl_Button* _buttonShowStats;
	Fl_Button* _buttonNormalize;
	Fl_Button* _buttonRefreshCharacter;
	Fl_Button* _buttonDump;
	Fl_Input* _inputUtterance;
	Fl_Input* _inputAudioFile;
	Fl_Input* _inputPhonemes;
	Fl_Button* _buttonRunTimeCurves;
	Fl_Button* _buttonPlayImageSequence;
	Fl_Choice* _choiceAudioFile;
	Fl_Button* _buttonDictionaryFile;
	Fl_Input* _inputLipSyncFolder;
	Fl_Button* _buttonLipSyncFolder;
	Fl_Button* _buttonGenerateLipSync;
	Fl_Check_Button* _checkRunBatchMode;

	VisemeRunTimeWindow* _windowVisemeRunTime;
	ImageSequenceViewer* _imageSequenceViewer;

	Fl_Button* _buttonSpeechFile;
	Fl_Input* _inputSpeechFile;
	Fl_Input* _inputTranscription;
	Fl_Button* _buttonGeneratePhonemes;

	Fl_Check_Button* _checkStats;
	VisemeCurveEditor * _curveEditor;
	bool _phonemesSelected[2];
	std::string _lastUtterance;
	bool _gatherStats;
	std::map<std::string, int> _diphoneStats;
	bool _useRemote;

	void draw();
	bool loadData();
	void loadAudioFiles();
	void selectViseme(const char * phoneme1, const char * phoneme2);
	void selectPhonemes(const char * viseme);
	void updateViseme();
	void initializeVisemes();
	void resetViseme();
	void enforceNamingConvention(char * c_str);
	void setUseRemote(bool val);
	bool getUseRemote();
	

	SmartBody::SBCharacter* getCurrentCharacter();
	std::string getCurrentCharacterName();
	SmartBody::SBDiphone* getCurrentDiphone();

	static std::string translateWordsToPhonemes(const std::string& utterance);

	static void OnDiphoneSelectCB(Fl_Widget* widget, void* data);
	static void OnCharacterSelectCB(Fl_Widget* widget, void* data);
	static void OnPhoneme1SelectCB(Fl_Widget* widget, void* data);
	static void OnPhoneme2SelectCB(Fl_Widget* widget, void* data);
	static void OnVisemeSelectCB(Fl_Widget* widget, void* data);
	static void OnSliderSelectCB(Fl_Widget* widget, void* data);
	static void OnPlayCB(Fl_Widget* widget, void* data);
	static void OnEnableScrub(Fl_Widget* widget, void* data);
	static void OnPlayDialogCB(Fl_Widget* widget, void* data);
	static void OnPlayAudioFileCB(Fl_Widget* widget, void* data);
	static void OnAudioFileSelectCB(Fl_Widget* widget, void* data);
	static void OnSaveCB(Fl_Widget* widget, void* data);
	static void OnLoadCB(Fl_Widget* widget, void* data);
	static void OnBmlRequestCB(BML::BmlRequest* request, void* data);
	static void OnShowStatsCB(Fl_Widget* widget, void* data);
	static void OnStatsResetCB(Fl_Widget* widget, void* data);
	static void OnGatherStatsCB(Fl_Widget* widget, void* data);
	static void OnCharacterRefreshCB(Fl_Widget* widget, void* data);
	static void OnDumpCB(Fl_Widget* widget, void* data);
	static void OnNormalizeCB(Fl_Widget* widget, void* data);
	static void OnRunTimeCurvesCB(Fl_Widget* widget, void* data);
	static void OnSinglePhonemeSelectCB(Fl_Widget* widget, void* data);
	static void OnPlayImageSequence(Fl_Widget* widget, void* data);

	static void OnPhonemeTranslateCB(Fl_Widget* widget, void* data);
	static void OnSoundFileLoadCB(Fl_Widget* widget, void* data);
	static void OnDictionaryLoadCB(Fl_Widget* widget, void* data);
	static void OnChangeLipSyncFolderCB(Fl_Widget* widget, void* data);
	static void OnGenerateLipSyncCB(Fl_Widget* widget, void* data);

	static void OnPhonemeAlignmentCommandCB(Fl_Widget* widget, void* data);

};
#endif
