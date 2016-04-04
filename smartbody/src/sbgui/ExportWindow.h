#ifndef _EXPORTWINDOW_H_
#define _EXPORTWINDOW_H_

#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Radio_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <vector>
#include <string>

class ExportWindow : public Fl_Double_Window
{
	public:
		ExportWindow(int x, int y, int w, int h, char* name);
		~ExportWindow();

		static void ExportCB(Fl_Widget* widget, void* data);
		static void FileCB(Fl_Widget* widget, void* data);

		static void ExportAllCB(Fl_Widget* widget, void* data);
		static void ChangeExportTypeCB(Fl_Widget* widget, void* data);

		Fl_Check_Button* checkExportAsset;
		Fl_Choice* choiceExportType;
		std::vector<Fl_Check_Button*> checkExport;
		Fl_Input* inputFile;		
		Fl_Button* buttonFile;
		Fl_Button* buttonExport;
		Fl_Button* selectAllButton;
		Fl_Button* selectNoneButton;
		bool exportToFolder;
protected:
		void exportSceneScript(const std::string& preFix, std::vector<std::string>& aspects, bool separateScriptFile = false);
};
#endif
