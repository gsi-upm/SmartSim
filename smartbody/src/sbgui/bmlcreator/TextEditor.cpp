#include "TextEditor.h"

TextEditorWindow::TextEditorWindow(int x, int y, int w, int h, char* name) : Fl_Double_Window(x, y, w, h, name)
{
	this->begin();
	int yDis = 25;
	textEditor = new Fl_Text_Editor(10, yDis, w - 20, 250, "");
	yDis += 250;
	textEditor->box(FL_UP_BOX);
	textEditor->textfont(FL_COURIER);
	
	textBuffer = new Fl_Text_Buffer();
	textEditor->buffer(textBuffer);

	yDis += 25;
	buttonOK = new Fl_Button(w - 100, yDis, 60, 25, "Confirm");
	buttonOK->callback(OnConfirmCB, this);
	this->end();
	this->resizable(textEditor);
	stringAttr = NULL;
}

TextEditorWindow::~TextEditorWindow()
{
	stringAttr = NULL;
}

void TextEditorWindow::setSBStringAttribute(SmartBody::StringAttribute* attr)
{
	stringAttr = attr;
	textBuffer->text((char*)stringAttr->getValue().c_str());
	this->label(stringAttr->getName().c_str());
}

void TextEditorWindow::OnConfirmCB(Fl_Widget* widget, void* data)
{
	TextEditorWindow* window = (TextEditorWindow*)data;
	std::string text = window->textBuffer->text();
	if (window->stringAttr != NULL)
		window->stringAttr->setValue(text);
	window->hide();
	delete window;
}
