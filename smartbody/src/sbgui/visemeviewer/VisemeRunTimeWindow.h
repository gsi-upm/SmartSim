#ifndef _VisemeRunTimeWindow_H_
#define _VisemeRunTimeWindow_H_

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Value_Slider.H>
#include <map>
#include <vector>
#include <bml/bml.hpp>

class VisemeRTCurveViewer;
class RunTimeCurveData
{
public:
	struct CurveData
	{
		std::string id;
		std::vector<float> data;
		int groupId;
		std::string curveInfo;
		bool display;
		bool highlight;
		bool inlist;
	};
	std::vector<CurveData> _curves;
	std::vector<std::string> _phonemeNames;
	std::vector<double> _phonemeTimes;

public:
	void insertData(const std::string& id, const std::vector<float>& curve, int group = 0, const std::string info = "");
	void insertPhoneme(const std::string& name, double t);
	int getCurveDataId(const std::string& id, int group);
	void clear();
};	// Data containing all the curves

class VisemeRunTimeWindow : public Fl_Double_Window
{
public:
	VisemeRunTimeWindow(int x, int y, int w, int h, char* name);
	~VisemeRunTimeWindow();

	void show();
	void hide();
	void draw();

	void retrievingCurveData(BML::BmlRequest* request);
	void refreshChannel();
	const std::string constructDisplayName(const std::string& id, int group);
	void parseDisplayName(const std::string& name, std::string& id, int& group);
	bool matchingPattern(std::string input, std::string pattern, int caseSensitive = 0);

public:
	Fl_Input* _inputCurveFilter;
	Fl_Input* _inputCurveDisplayedFilter;
	Fl_Multi_Browser* _browserCurveList;
	Fl_Multi_Browser* _browserCurveDisplay;
	Fl_Button* _buttonAdd;
	Fl_Button* _buttonRemove;
	Fl_Button* _buttonResetCamera;
	Fl_Value_Slider* _sliderZoomRes;
	VisemeRTCurveViewer* _rtCurveViewer;
	RunTimeCurveData* _data;

public:
	static void OnResetCameraCB(Fl_Widget* widget, void* data);
	static void OnChangeZoomResolutionCB(Fl_Widget* widget, void* data);
	static void OnAddDisplayCurve(Fl_Widget* widget, void* data);
	static void OnDelDisplayCurve(Fl_Widget* widget, void* data);
	static void OnSelectDisplay(Fl_Widget* widget, void* data);
	static void OnChangeFilter(Fl_Widget* widget, void* data);
};

#endif