#ifndef _PARAM_ANIM_STATE_CREATOR_H_
#define _PARAM_ANIM_STATE_CREATOR_H_

#include <FL/Fl_Window.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include "ParamAnimStateEditor.h"


class PABlendCreator : public Fl_Window
{
	public:
		PABlendCreator(PABlendEditor* editor, int x, int y, int w, int h);
		~PABlendCreator();

		std::string getUniqueStateName(std::string prefix);
		void loadMotions();
		
		void setInfo(bool isCreateMode, const std::string& stateName);

		static void addMotion(Fl_Widget* widget, void* data);
		static void removeMotion(Fl_Widget* widget, void* data);
		static void createState(Fl_Widget* widget, void* data);
		static void cancelState(Fl_Widget* widget, void* data);

		Fl_Multi_Browser*		animationList;
		Fl_Multi_Browser*		stateAnimationList;
		Fl_Button*		animationAdd;
		Fl_Button*		animationRemove;
		Fl_Input*		inputStateName;
		Fl_Choice*		choiceStateType;
		PABlendEditor*	stateEditor;
		Fl_Button*		buttonCreateState;
		Fl_Button*		buttonCancelState;

		int xDis;
		int yDis;
		static int lastNameIndex;
		bool isCreateMode;


};


#endif
