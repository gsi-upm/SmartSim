#ifndef _BMLCREATORWINDOW_H_
#define _BMLCREATORWINDOW_H_

#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Text_Editor.H>
#include "bml/BMLObject.h"

class BMLCreatorWindow : public Fl_Double_Window, public SmartBody::SBObserver
{
	public:
		BMLCreatorWindow(int, int, int, int, const char*);
		~BMLCreatorWindow();

		virtual void notify(SmartBody::SBSubject* subject);
		void updateBMLBuffer();

		static void RunBMLCB(Fl_Widget* w, void *data);
		static void RefreshCharactersCB(Fl_Widget* w, void *data);
		static void ResetBMLCB(Fl_Widget* w, void *data);
		static void ChooseCharactersCB(Fl_Widget* w, void *data);

	protected:
		std::vector<BMLObject*> _bmlObjects;
		Fl_Text_Editor* _editor;
		Fl_Choice* _choiceCharacters;
		std::string _curCharacter;
		std::string _curBML;

};
#endif
