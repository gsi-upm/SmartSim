#ifndef _SBMWINDOW_H_
#define _SBMWINDOW_H_

#include <fltk/Window.h>
#include "CommandWindow.h"
#include "fltk_viewer.h"

class SBMWindow : public fltk::Window
{
	public:
		SBMWindow(int x, int y, int w, int h, char* name);
		~SBMWindow();

		CommandWindow* getCommandWindow();
		FltkViewer* getFltkViewer();

	private:
		CommandWindow* m_commandWindow;
		FltkViewer* m_fltkViewer;
};

#endif