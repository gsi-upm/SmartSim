/*
 *  PanimationWindow.h - part of SmartBody-lib's Test Suite
 *  Copyright (C) 2009  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Yuyu Xu, USC
 */

#ifndef _PANIMATION_WINDOW_H_
#define _PANIMATION_WINDOW_H_

#include <FL/Fl_Slider.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_ask.H>
#include <sbm/GenericViewer.h>
#include <controllers/me_ct_param_animation_utilities.h>
#include <sb/PABlend.h>
#include <map>
#include <sb/SBSceneListener.h>

const static int yDis = 10;
const static int xDis = 10;


class PABlendEditor;
class PATransitionEditor;
class PATransitionEditor2;
class PAScriptEditor;
class PARunTimeEditor;
class VisualizationView;

namespace nle {
	class NonLinearEditorModel;
	class Block;
	class Mark;
	class Track;
}

namespace SmartBody {
	class SBCharacter;
}

class PanimationWindow;

class PanimationWindowListener : public SmartBody::SBSceneListener
{
	public:
		PanimationWindowListener(PanimationWindow* window);

		virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );
		virtual void OnCharacterDelete( const std::string & name );
		virtual void OnCharacterUpdate( const std::string & name );
      
		virtual void OnPawnCreate( const std::string & name );
		virtual void OnPawnDelete( const std::string & name );

		virtual void OnSimulationStart();
		virtual void OnSimulationEnd();
		virtual void OnSimulationUpdate();
	
	private:
		PanimationWindow* _window;

};




class PanimationWindow : public Fl_Double_Window, public GenericViewer
{
	public:
		PanimationWindow(int x, int y, int w, int h, char* name);
		~PanimationWindow();

		virtual void label_viewer(std::string name);
		virtual void show_viewer();
		virtual void hide_viewer();
		virtual void update_viewer();
		virtual void draw();
        virtual void show(); 
		virtual void hide();

		void setCurrentCharacterName(const std::string& name);
		const std::string& getCurrentCharacterName();
		SmartBody::SBCharacter* getCurrentCharacter();
		bool checkCommand(std::string command);
		static void execCmd(PanimationWindow* window, std::string command, double tOffset = 0.0);
		std::vector<std::string> tokenize(const std::string& str,const std::string& delimiters);
		void addTimeMark(nle::NonLinearEditorModel* model, bool selective = false);
		void removeTimeMark(nle::NonLinearEditorModel* model); 
		void addTimeMarkToBlock(nle::Block* block, double t);

		static void loadCharacters(Fl_Choice* characterList);
		static void refreshUI(Fl_Widget* widget, void* data);
		void getSelectedMarkInfo(nle::NonLinearEditorModel* model, std::string& blockName, double& time);
		static void reset(Fl_Widget* widget, void* data);
		static PanimationWindow* getPAnimationWindow(Fl_Widget* w);

		// transition editor functions
		static void changeTransitionEditorMode(Fl_Widget* widget, void* data);
		static void changeStateList1(Fl_Widget* widget, void* data);
		static void changeStateList2(Fl_Widget* widget, void* data);
		static void changeAnimForTransition(Fl_Widget* widget, void* data);
		static void addTransitionTimeMark(Fl_Widget* widget, void* data);
		static void removeTransitionTimeMark(Fl_Widget* widget, void* data);
		static void updateTransitionTimeMark(Fl_Widget* widget, void* data);
		static void loadTransitions(Fl_Choice* transitionList);
		static void createNewTransition(Fl_Widget* widget, void* data);
		static void changeTransitionList(Fl_Widget* widget, void* data);
		static void changeTabGroup(Fl_Widget* widget, void* data);

	public:
		std::string lastCommand;
		std::string _currentCharacterName;
	
		Fl_Tabs*		tabGroup;
		//PATransitionEditor* transitionEditor;
		PATransitionEditor2* transitionEditor2;
		PABlendEditor*		stateEditor;
		PAScriptEditor*		scriptEditor;
		PARunTimeEditor*	runTimeEditor;
		VisualizationView*  visView;

		Fl_Choice*		characterList;
		Fl_Button*		refresh;
		Fl_Button*		resetCharacter;
		PanimationWindowListener* _listener;
};

 class PanimationViewerFactory : public GenericViewerFactory
 {
	public:
		PanimationViewerFactory();

		virtual GenericViewer* create(int x, int y, int w, int h);
		virtual void destroy(GenericViewer* viewer);
 };
#endif
