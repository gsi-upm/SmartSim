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

#include "vhcl.h"
#include "CommandWindow.h"
#include <vector>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <FL/Fl.H>
#include <boost/algorithm/string/replace.hpp>
#include <sb/SBScene.h>


using std::string;


void updateDisplay(int pos, int nInserted, int nDeleted,int b, const char* d, void* v);


//creating FLTK multiline editor to connect to the interpreter
CommandWindow::CommandWindow(int x,int y,int w,int h, const char* s) : GenericViewer(x, y, w, h), Fl_Double_Window(x, y, w, h, s)
{
       curDir[0] = '\0';

    minOutputSize = 10000;
	this->begin();

	menubar = new Fl_Menu_Bar(0, 0, w, 25); 
	menubar->add("&Edit/&Clear", (const char*) NULL, clearcb, 0);
	menubar->add("&Edit/C&lear History", (const char*) NULL, clearhistorycb, 0);
	menubar->add("&Show/&Events", (const char*) NULL, showeventscb, 0);

	for (int c = 0; c < 2; c++)
	{
		historyCounter[c] = 0;
		historyLocation[c] = 0;
	}
	this->color(FL_LIGHT1);
	strcpy(printout,"");

	int yDis = 25;
	
	// output
	textDisplay = new Fl_Text_Display(10, yDis, w - 20, 250, "");
	yDis += 250;
	textDisplay->box(FL_UP_BOX);
	textDisplay->textfont(FL_COURIER);
#ifdef _MACOSX_
	textDisplay->textsize(10);
#else
	textDisplay->textsize(11);
#endif
	textDisplay->color(FL_LIGHT1);
	textDisplay->textcolor(FL_BLACK);
	textDisplay->redraw() ;

	DisplayTextBuffer = new Fl_Text_Buffer();
	DisplayTextBuffer->add_modify_callback(updateDisplay, this);
	textDisplay->buffer(DisplayTextBuffer);


	yDis += 25;

	buttonExecute = new Fl_Button(w - 100, yDis, 60, 25, "Run");
	buttonExecute->callback(executecb, this);

	yDis += 25;

	// command input
	tabGroup = new Fl_Tabs(10, yDis, w - 10, h - yDis);	
	tabGroup->color(FL_WHITE,FL_GRAY);
	tabGroup->begin();
	yDis += 30;
	for (int i = 0; i < 2; i++)
	{
		textEditor[i] = new Fl_Text_Editor(15, yDis, w - 20, h - yDis - 10);		
		textEditor[i]->selection_color(FL_DARK1);
		textEditor[i]->textfont(FL_COURIER);
	#ifdef _MACOSX_
		Fl_Text_Editor[i]->textsize(10);
	#else
		textEditor[i]->textsize(13);
	#endif
		textEditor[i]->box(FL_UP_BOX);
		textEditor[i]->textcolor(FL_BLACK);
		if (i == 1)
			textEditor[i]->color(FL_LIGHT2);

		textBuffer[i] = new Fl_Text_Buffer();
		textEditor[i]->buffer(textBuffer[i]); 

		textEditor[i]->add_key_binding(FL_Up, FL_TEXT_EDITOR_ANY_STATE, (Fl_Text_Editor::Key_Func) upcb);
		textEditor[i]->add_key_binding(FL_Down, FL_TEXT_EDITOR_ANY_STATE, (Fl_Text_Editor::Key_Func) downcb);

		textEditor[i]->add_key_binding(FL_Tab, FL_TEXT_EDITOR_ANY_STATE, (Fl_Text_Editor::Key_Func) tabcb);
		//textEditor[i]->add_key_binding(FL_Enter, FL_TEXT_EDITOR_ANY_STATE, (Fl_Text_Editor::Key_Func) entercb);
		textEditor[i]->add_key_binding('u', FL_CTRL | FL_SHIFT, (Fl_Text_Editor::Key_Func) ctrlUcb);
		textEditor[i]->add_key_binding('u', FL_CTRL, (Fl_Text_Editor::Key_Func) ctrlUcb);
		textEditor[i]->add_key_binding(FL_Enter,FL_CTRL, (Fl_Text_Editor::Key_Func) ctrlEntercb);
	}

	textEditor[0]->label("Sbm");
	textEditor[1]->label("Python");

	tabGroup->end();
	tabGroup->value(textEditor[1]); // Python command interface by default
	//tabGroup->value(textEditor[0]);   // Sbm command interface by default

	this->end();

	this->resizable(tabGroup);
	this->size_range(640, 480);

	isShowEvents = false;
	vhcl::Log::g_log.AddListener(this);
}

void CommandWindow::OnMessage( const std::string & message )
{
	UpdateOutput((char*) message.c_str());
}

CommandWindow::~CommandWindow()
{
//	delete DisplayTextBuffer;
	//delete EditorTextBuffer;
	delete textEditor[0];
	delete textEditor[1];
	/*Fl_Text_Buffer *DisplayTextBuffer;
	Fl_Text_Display *textDisplay;*/

	vhcl::Log::g_log.RemoveListener(this);
}

CommandWindow* CommandWindow::getCommandWindow(Fl_Widget* w)
{
	CommandWindow* commandWindow = NULL;
	Fl_Widget* widget = w;
	while (widget)
	{
		commandWindow = dynamic_cast<CommandWindow*>(widget);
		if (commandWindow)
			break;
		else
			widget = widget->parent();
	}

	return commandWindow;
}

void CommandWindow::entercb(int key, Fl_Text_Editor* editor) 
{
	SmartBody::SBScene* sbScene = SmartBody::SBScene::getScene();
	CommandWindow* commandWindow =  CommandWindow::getCommandWindow(editor);

	Fl_Text_Buffer* tempBuffer = editor->buffer();
	char* c = tempBuffer->text();
	int len = (int) strlen(c);

	string str = "-> ";
	if (editor == commandWindow->textEditor[1])
		str = "~ ";
	str.append(c);

	commandWindow->UpdateOutput((char*) str.c_str(), true);

	if (len == 0)
		return;

	if (editor == commandWindow->textEditor[1])
	{
		if (!sbScene->isRemoteMode())
		{
			sbScene->run(c);
		}
		else
		{
			string sendStr = "send sbm python ";
			sendStr.append(c);
			sbScene->command(sendStr);
		}
	}
	else
	{
		if (!sbScene->isRemoteMode())
		{
			sbScene->command(c);
		}
		else
		{
			string sendStr = "send sbm ";
			sendStr.append(c);
			sbScene->command(sendStr);
		}
	}

	int index = (editor == commandWindow->textEditor[0]? 0 : 1);
	commandWindow->addHistoryItem(c, index);

	tempBuffer->remove(0,len + 1);
}

void CommandWindow::upcb(int key, Fl_Text_Editor* editor) 
{
	CommandWindow* commandWindow = CommandWindow::getCommandWindow(editor);
	// only use up/down functionality if the cursor is at the very beginning
	int cursorPos = editor->insert_position();
	//if (editor->buffer()->length() > 0)
	if (cursorPos > 0)
		return;

	int index = editor == commandWindow->textEditor[0]? 0 : 1;
	commandWindow->historyLocation[index]--;
	if (commandWindow->historyLocation[index] < 0)
		commandWindow->historyLocation[index] = 0;
	const char* command = commandWindow->getHistoryItem(commandWindow->historyLocation[index], index);
	editor->buffer()->remove(0, editor->buffer()->length());
	editor->buffer()->insert(editor->buffer()->length(), command);
}

void CommandWindow::downcb(int key, Fl_Text_Editor* editor)
{
	CommandWindow* commandWindow = CommandWindow::getCommandWindow(editor);

	int cursorPos = editor->insert_position();
	if (cursorPos > 0)
		return;

	int index = editor == commandWindow->textEditor[0]? 0 : 1;
	commandWindow->historyLocation[index]++;
	if (commandWindow->historyLocation[index] > int(commandWindow->historyItems[index].size()))
		commandWindow->historyLocation[index] = commandWindow->historyItems[index].size();

	const char* command = commandWindow->getHistoryItem(commandWindow->historyLocation[index], index);
	editor->buffer()->remove(0, editor->buffer()->length());
	editor->buffer()->insert(editor->buffer()->length(), command);
}

void CommandWindow::clearcb(Fl_Widget* w, void* data)
{
	CommandWindow* commandWindow = CommandWindow::getCommandWindow(w);

	if (!commandWindow)
		return;
	Fl_Text_Display* display = commandWindow->textDisplay;
	display->buffer()->remove(0, display->buffer()->length());
}

void CommandWindow::executecb(Fl_Widget* w, void* data)
{
	CommandWindow* commandWindow = (CommandWindow*) data;

	CommandWindow::entercb(0, (Fl_Text_Editor*) commandWindow->tabGroup->value());
}

void CommandWindow::clearhistorycb(Fl_Widget* w, void* data)
{
	CommandWindow* commandWindow = CommandWindow::getCommandWindow(w);
	commandWindow->clearHistory(0);
	commandWindow->clearHistory(1);
}

void CommandWindow::showeventscb(Fl_Widget* widget, void* data)
{
	CommandWindow* commandWindow = CommandWindow::getCommandWindow(widget);
	commandWindow->isShowEvents = !commandWindow->isShowEvents;
}

void CommandWindow::tabcb(int key, Fl_Text_Editor* editor)
{
	editor->insert("\t");

	if (true) return;

	Fl_Text_Buffer* tempBuffer = editor->buffer();
	const char* c = tempBuffer->text();

	//get the last word
	char lastword[256] = "";
	bool singlequote = false;
	bool doublequote = false;
	bool word = false;
	int cursorPos = editor->insert_position();
	int j = 0;

	for (int i = 0; i < cursorPos; i++)
	{
		switch(c[i])
		{
		case ' ':
			if (doublequote || singlequote)
			{
				lastword[j++] = c[i];
			}
			else
			{
				word = true;
			}
			break;
		case '\\':
			if (singlequote)
			{
				lastword[j++] = c[i];
			}
			else
			{
				lastword[j++] = c[i++];
				lastword[j++] = c[i];
			}
			break;
		case '\'':
			lastword[j++] = c[i];
			if (!doublequote) 
				singlequote = !singlequote;
			break;
		case '\"':
			lastword[j++] = c[i];
			if (!singlequote) 
				doublequote = !doublequote;
			break;
		default:
			lastword[j++] = c[i];
			break;
		}

		if (word)
		{
			j = 0;
			word = false;
		}
	}
	lastword[j] = '\0';
}

void CommandWindow::ctrlUcb(int key, Fl_Text_Editor* editor)
{
	Fl_Text_Buffer* tempBuffer = editor->buffer();

	tempBuffer->remove(0, editor->insert_position());
}


void CommandWindow::ctrlEntercb(int key, Fl_Text_Editor* editor)
{
	CommandWindow::entercb(key, editor);
}

void CommandWindow::UpdateOutput(char *text, bool origCommand)
{
	if(textDisplay == NULL) return;

	// if the display is approaching capacity, then keep the last minOutputSize characters
	int bufferLength = textDisplay->buffer()->length();
	if (bufferLength > getMinOutputSize() * 100)
	{
		textDisplay->buffer()->remove(0, bufferLength -  getMinOutputSize());
	}

/*	textDisplay->highlightdata(StyleBuffer, styletable,
								sizeof(styletable) / sizeof(styletable[0]),
								'A', styleunfinishedcb, 0);
	char *style = new char[strlen(text) + 1];
	memset(style, 'A', strlen(text));

*/


	std::string textString = text;
	// substitute any final \r\n for \n
	boost::replace_all(textString, "\r\n", "\n");

	if (true) //length <= 79)
	{
		// find the end of the buffer
		textDisplay->insert_position(9999999); // move to the last position

		if (origCommand)
		{
		    textDisplay->insert(textString.c_str());
			textDisplay->insert("\n");
		}
		else
		{
			// text is less than the width of the command window, just insert it
			textDisplay->insert(textString.c_str());
		}
	}
	else
	{
		// text size is greater than width of window - scroll to fit window
		int size =  (int) textString.length();
		while (size > 0)
		{
			// check for \n 
			int loc = (int) textString.find('\n');
			if (loc == -1 || loc > 79)
			{
				int max = 79;
				if (size < max)
					max = size;
				string g  = textString.substr(0, max).c_str() ;
				textDisplay->insert(textString.substr(0, max).c_str());
				textDisplay->insert("\n");
				if (size > max)
					textString = textString.substr(80);
				else
					textString = "";
			}
			else if (loc == 0)
			{
				textString = textString.substr(1);
			}
			else
			{
				textDisplay->insert(textString.substr(0, loc).c_str());
				textDisplay->insert("\n");
				textString = textString.substr(loc + 1);
			}
			size =  (int) textString.length();
		}

	}

}

void updateDisplay(int pos, int nInserted, int nDeleted, int b, const char* d, void* v)
{
	CommandWindow* commandWindow = static_cast<CommandWindow*>(v);
	
	commandWindow->textDisplay->show_insert_position();
}

void CommandWindow::addHistoryItem(const char* item, int index)
{
	if (this->historyItems[index].size() > MAXHISTORYSIZE - 1)
	{
		freeHistorySpace(index);
	}
	this->historyItems[index].push_back(item);
	this->historyLocation[index] = this->historyItems[index].size();
}

const char* CommandWindow::getHistoryItem(int location, int index)
{
	if ((unsigned int) location < this->historyItems[index].size() && location >= 0)
		return (char*) this->historyItems[index][location].c_str();
	else
		return "";
}

void CommandWindow::clearHistory(int index)
{
	historyItems[index].clear();
	historyCounter[index] = 0;
	historyLocation[index] = 0;
}

void CommandWindow::freeHistorySpace(int index)
{
	int halfHistorySize = MAXHISTORYSIZE / 2;
	// shift history items down

	// delete the old values
	int count = 0;
	while (count < halfHistorySize)
	{
		historyItems[index].erase(historyItems[index].begin());
		count++;
	}

	if (historyLocation[index] < count)
	{
		historyLocation[index] = 0;
	}
	else
	{
		historyLocation[index] -= count;
	}
}

void CommandWindow::setMinOutputSize(int size)
{
	minOutputSize = size;
}

int CommandWindow::getMinOutputSize()
{
	return minOutputSize;
}

CommandViewerFactory::CommandViewerFactory()
{
}

GenericViewer* CommandViewerFactory::create(int x, int y, int w, int h)
{
	CommandWindow* commandWindow = new CommandWindow(x, y, w, h, "Command");
	return commandWindow;
}

void CommandViewerFactory::destroy(GenericViewer* viewer)
{
	delete viewer;
}

