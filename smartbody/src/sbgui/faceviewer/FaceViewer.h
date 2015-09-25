#ifndef _FACEVIEWER_H_
#define _FACEVIEWER_H_

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Value_Slider.H>
#include <sbm/GenericViewer.h>
#include <SBWindowListener.h>

class FaceViewer : public GenericViewer, public Fl_Double_Window, public SBWindowListener
{
	public:
		FaceViewer(int x, int y, int w, int h, char* name);
		~FaceViewer();

		virtual void show();
		virtual void hide();

		virtual void show_viewer();
		virtual void hide_viewer();

		virtual void updateGUI();

		virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );
		virtual void OnCharacterDelete( const std::string & name );
		virtual void OnCharacterUpdate( const std::string & name );
		virtual void OnSimulationUpdate();


		static void CharacterCB(Fl_Widget* widget, void* data);
		static void RefreshCB(Fl_Widget* widget, void* data);
		static void ResetCB(Fl_Widget* widget, void* data);
		static void FaceCB(Fl_Widget* widget, void* data);
		static void FaceWeightCB(Fl_Widget* widget, void* data);
		static void DefaultFaceCB(Fl_Widget* widget, void* data);
		static void ResetDefaultFaceCB(Fl_Widget* widget, void* data);
		
		Fl_Choice* choiceCharacters;
		Fl_Button* buttonRefresh;
		Fl_Button* buttonReset;
		Fl_Button* buttonDefaultFace;
		Fl_Button* buttonResetDefaultFace;
		Fl_Group* topGroup;
		Fl_Scroll* bottomGroup;
		std::vector<Fl_Value_Slider*> _sliders;
		std::vector<Fl_Value_Slider*> _weights;
};

class FaceViewerFactory : public GenericViewerFactory
{
	public:
		FaceViewerFactory();

		virtual GenericViewer* create(int x, int y, int w, int h);
		virtual void destroy(GenericViewer* viewer);
};


#endif
