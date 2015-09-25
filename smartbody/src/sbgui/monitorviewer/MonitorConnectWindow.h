#ifndef _MONITOR_CONNECT_WINDOW_H_
#define _MONITOR_CONNECT_WINDOW_H_

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Input.H>

class MonitorConnectWindow : public Fl_Double_Window
{
public:
	MonitorConnectWindow(int x, int y, int w, int h, char* label, bool quickConnect);
	~MonitorConnectWindow();

	void loadProcesses();
	void show();
	void hide();

protected:
	Fl_Hold_Browser* _browserSBProcesses;
	Fl_Input* _inputServerName;
	Fl_Input* _inputPort;
	Fl_Input* _inputScope;
	Fl_Button* _buttonLoad;
	Fl_Button* _buttonOk;
	Fl_Button* _buttonCancel;
	Fl_Button* _buttonRefresh;

	static void OnConfirmCB(Fl_Widget* widget, void* data);
	static void OnCancelCB(Fl_Widget* widget, void* data);
	static void OnFefreshCB(Fl_Widget* widget, void* data);

};

#endif
