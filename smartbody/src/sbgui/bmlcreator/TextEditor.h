#ifndef _TEXTEDITOR_H_
#define _TEXTEDITOR_H_

#include "sb/SBAttribute.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Button.H>
class TextEditorWindow : public Fl_Double_Window
{
public:
	TextEditorWindow(int x, int y, int w, int h, char* name);
	~TextEditorWindow();

	Fl_Text_Buffer *textBuffer;
	Fl_Text_Editor *textEditor;

	Fl_Button* buttonOK;

	SmartBody::StringAttribute* stringAttr;

	void setSBStringAttribute(SmartBody::StringAttribute* attr);
	static void OnConfirmCB(Fl_Widget* widget, void* data);
};

#endif