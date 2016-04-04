#ifndef _BEHAVIORWINDOW_
#define _BEHAVIORWINDOW_

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Light_Button.H>
#include "nle/NonLinearEditor.h"
#include "nle/NonLinearEditorWidget.h"
#include "BehaviorEditorWidget.h"
#include "BehaviorBlock.h"
#include <bml/bml_speech.hpp>
#include <bml/bml_event.hpp>
#include <sbm/GenericViewer.h>

class BehaviorWindow : public Fl_Double_Window, public GenericViewer
{
	public:
		BehaviorWindow(int x, int y, int w, int h, char* name);
		~BehaviorWindow();

		static void OnRequest(BML::BmlRequest* request, void* data);

		virtual void label_viewer(std::string name);
		virtual void show_viewer();
		virtual void hide_viewer();
		virtual	void update_viewer();
		int handle(int event);
        void show();      
        void draw();
		void resize(int x, int y, int w, int h);
        
		void updateGUI();
        
		nle::EditorWidget* getEditorWidget();
		nle::Block* getSelectedBlock();
        nle::Track* getSelectedTrack();
		void updateBehaviors(BML::BmlRequest* request);

		void processMotionRequest(BML::MotionRequest* motionRequest, nle::NonLinearEditorModel* model, BML::BehaviorRequest* behavior, 
								  double triggerTime, BML::BehaviorSchedulerConstantSpeed* constantSpeedScheduler, 
								  std::map<std::string, double>& syncMap, std::vector<std::pair<RequestMark*, std::string> >& untimedMarks);
		void processControllerRequest(BML::MeControllerRequest* contrlllerRequest, nle::NonLinearEditorModel* model, BML::BehaviorRequest* behavior, 
									  double triggerTime, BML::BehaviorSchedulerConstantSpeed* constantSpeedScheduler, 
									  std::map<std::string, double>& syncMap, std::vector<std::pair<RequestMark*, std::string> >& untimedMarks);
		void processSpeechRequest(BML::SpeechRequest* speechRequest, nle::NonLinearEditorModel* model, BML::BehaviorRequest* behavior, 
									double triggerTime, BML::BehaviorSchedulerConstantSpeed* constantSpeedScheduler, 
									std::map<std::string, double>& syncMap, std::vector<std::pair<RequestMark*, std::string> >& untimedMarks);
		void processEventRequest(BML::EventRequest* eventRequest, nle::NonLinearEditorModel* model, BML::BehaviorRequest* behavior, 
									double triggerTime, BML::BehaviorSchedulerConstantSpeed* constantSpeedScheduler, 
									std::map<std::string, double>& syncMap, std::vector<std::pair<RequestMark*, std::string> >& untimedMarks);
		void processVisemeRequest(BML::VisemeRequest* eventRequest, nle::NonLinearEditorModel* model, BML::BehaviorRequest* behavior, 
									double triggerTime, BML::BehaviorSchedulerConstantSpeed* constantSpeedScheduler, 
									std::map<std::string, double>& syncMap, std::vector<std::pair<RequestMark*, std::string> >& untimedMarks);
	
		void adjustSyncPoints(BML::BehaviorRequest* behavior, nle::Block* block, std::map<std::string, double>& syncMap);
		
		static void ContextCB(Fl_Widget* widget, void* data);
		static void ClearCB(Fl_Widget* widget, void* data);
		static void ReplayCB(Fl_Widget* widget, void* data);

		BehaviorEditorWidget* nleWidget;


		Fl_Choice* choiceContexts;
		Fl_Button* buttonClear;
		Fl_Button* buttonReplay;
		Fl_Text_Editor* textXML;
		Fl_Text_Buffer* bufferXML;

		int contextCounter;
		std::string selectedContext;

		nle::NonLinearEditorModel* nleModel;
};

 class BehaviorViewerFactory : public GenericViewerFactory
 {
	public:
		BehaviorViewerFactory();

		virtual GenericViewer* create(int x, int y, int w, int h);
		virtual void destroy(GenericViewer* viewer);
 };
#endif
