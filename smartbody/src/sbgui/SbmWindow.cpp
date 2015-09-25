#include "SBMWindow.h"
#include <fltk/PackedGroup.h>

SBMWindow::SBMWindow(int x, int y, int w, int h, char* name) : Window(x, y, w, h)
{
	this->begin();
	fltk::PackedGroup* group = new fltk::PackedGroup(0, 0, w, h);
	group->begin();

	m_fltkViewer = new FltkViewer(0, 0, w, int(h * .6667), "Scene");

	group->end();
	group->resizable(m_fltkViewer);


	this->end();
	this->resizable(group);
	m_commandWindow = new CommandWindow(100, 100, w, h, "Commands");
	m_commandWindow->show();
}

SBMWindow::~SBMWindow()
{
	delete m_fltkViewer;
	//delete m_commandWindow; ?? deleted by log listener??
}

CommandWindow* SBMWindow::getCommandWindow()
{
	return m_commandWindow;
}

FltkViewer* SBMWindow::getFltkViewer()
{
	return m_fltkViewer;
}