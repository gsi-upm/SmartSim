#ifndef _CHARACTERCREATERWINDOW_H_
#define _CHARACTERCREATERWINDOW_H_

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <vector>
#include <sk/sk_skeleton.h>

class BaseWindow;

class CharacterCreatorWindow : public Fl_Double_Window
{
	public:
		CharacterCreatorWindow(int x, int y, int w, int h, char* name);
		~CharacterCreatorWindow();

		void setSkeletons(std::vector<std::string>& skeletonNames);
		static void CreateCB(Fl_Widget* w, void* data);

		Fl_Choice* choiceSkeletons;
		Fl_Input* inputName;
		Fl_Button* buttonCreate;
		int numCharacter;
};

class ResolutionWindow : public Fl_Double_Window
{
public:
	ResolutionWindow(int x, int y, int w, int h, char* name);
	~ResolutionWindow();
	
	static void SetCB(Fl_Widget* w, void* data);
	void setResolution();

	BaseWindow* baseWin;
	Fl_Input *inputXRes, *inputYRes;
	Fl_Button* buttonSet;
};


#endif
