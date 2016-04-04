/**************************************************
Copyright 2005 by Ari Shapiro and Petros Faloutsos

DANCE
Dynamic ANimation and Control Environment

 ***************************************************************
 ******General License Agreement and Lack of Warranty ***********
 ****************************************************************

This software is distributed for noncommercial use in the hope that it will 
be useful but WITHOUT ANY WARRANTY. The author(s) do not accept responsibility
to anyone for the consequences	of using it or for whether it serves any 
particular purpose or works at all. No warranty is made about the software 
or its performance. Commercial use is prohibited. 

Any plugin code written for DANCE belongs to the developer of that plugin,
who is free to license that code in any manner desired.

Content and code development by third parties (such as FLTK, Python, 
ImageMagick, ODE) may be governed by different licenses.
You may modify and distribute this software as long as you give credit 
to the original authors by including the following text in every file 
that is distributed: */

/*********************************************************
	Copyright 2005 by Ari Shapiro and Petros Faloutsos

	DANCE
	Dynamic ANimation and Control Environment
	-----------------------------------------
	AUTHOR:
		Ari Shapiro (ashapiro@cs.ucla.edu)
	ORIGINAL AUTHORS: 
		Victor Ng (victorng@dgp.toronto.edu)
		Petros Faloutsos (pfal@cs.ucla.edu)
	CONTRIBUTORS:
		Yong Cao (abingcao@cs.ucla.edu)
		Paco Abad (fjabad@dsic.upv.es)
**********************************************************/

#ifndef _COMMANDWINDOW_
#define _COMMANDWINDOW_


#include <FL/Fl_Slider.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_draw.H>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <sbm/GenericViewer.h>


class CommandWindow : public GenericViewer, public Fl_Double_Window, public vhcl::Log::Listener
{
public:
	CommandWindow(int, int, int, int, const char*);
	~CommandWindow();

	int width;
	int height;
	char printout[1024];
	void UpdateOutput(char *text, bool origCommand = false);
	void addHistoryItem(const char* item, int index);
	const char* getHistoryItem(int location, int index);
	void clearHistory(int index);
	void setMinOutputSize(int size);
	int getMinOutputSize();

	static CommandWindow* getCommandWindow(Fl_Widget* w);

	virtual void OnMessage( const std::string & message );

	Fl_Text_Buffer *textBuffer[2];
	Fl_Text_Editor *textEditor[2];
	Fl_Text_Buffer *DisplayTextBuffer;
	Fl_Text_Display *textDisplay;
	Fl_Menu_Bar* menubar;
	Fl_Tabs* tabGroup;
	Fl_Button* buttonExecute;

	bool isShowEvents;

	static void testCB();
	static void executecb(Fl_Widget* widget, void* data);
	static void upcb(int key, Fl_Text_Editor* te);
	static void entercb(int key, Fl_Text_Editor* te);
	static void downcb(int key, Fl_Text_Editor* te);
	static void tabcb(int key, Fl_Text_Editor* te);
	static void ctrlUcb(int key, Fl_Text_Editor* te);
	static void ctrlEntercb(int key, Fl_Text_Editor* te);

	static void clearcb(Fl_Widget* widget, void* data);
	static void clearhistorycb(Fl_Widget* widget, void* data);
	static void showeventscb(Fl_Widget* widget, void* data);

	static void FindFiles(char*, char*);

private:
	static const int MAXHISTORYSIZE = 10;

	void freeHistorySpace(int index);

	int historyCounter[2];	
	int historyLocation[2];
	std::vector<std::string> historyItems[2];
	int when;
	char curDir[256];
	int minOutputSize;
	

}
;


class CommandViewerFactory : public GenericViewerFactory
{
public:
	CommandViewerFactory();

	virtual GenericViewer* create(int x, int y, int w, int h);
	virtual void destroy(GenericViewer* viewer);
};
#endif


