#ifndef _FACESHIFTVIEWER_H_
#define _FACESHIFTVIEWER_H_

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Value_Slider.H>
#include <sbm/GenericViewer.h>
#include <SBWindowListener.h>

class FaceShiftViewer : public GenericViewer, public Fl_Double_Window, public SBWindowListener
{
	public:
		FaceShiftViewer(int x, int y, int w, int h, char* name);
		~FaceShiftViewer();

		virtual void show();
		virtual void hide();

		virtual void show_viewer();
		virtual void hide_viewer();

		virtual void updateGUI();

		//virtual void OnSimulationUpdate();
	
		Fl_Group* topGroup;
		Fl_Scroll* bottomGroup;
		std::vector<Fl_Value_Slider*> _sliders;
		std::vector<Fl_Value_Slider*> _weights;
};

class FaceShiftViewerFactory : public GenericViewerFactory
{
	public:
		FaceShiftViewerFactory();

		virtual GenericViewer* create(int x, int y, int w, int h);
		virtual void destroy(GenericViewer* viewer);
};


#endif
