#ifndef _RETARGETVIEWER_
#define _RETARGETVIEWER_

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Button.H>
#include <string>

class RetargetViewer : public Fl_Double_Window
{
	public:
		RetargetViewer(int x, int y, int w, int h, char* name);

		//void updateSkeletonList();
		//void updateCharacterList();
		int updateBehaviorSet();
		~RetargetViewer();
		void setCharacterName(const std::string& name);
		//void setSkeletonName(const std::string& name);
		const std::string& getCharacterName();
		//const std::string& getSkeletonName();
		void setShowButton(bool showButton);
		static void CharacterCB(Fl_Widget* widget, void* data);
		//static void SkeletonCB(Fl_Widget* widget, void* data);
		static void RetargetCB(Fl_Widget* widget, void* data);
		static void CancelCB(Fl_Widget* widget, void* data);
	protected:
		std::string _charName;
		//std::string _skelName;

		Fl_Choice* _choiceCharacters;
		Fl_Choice* _choiceSkeletons;
		Fl_Scroll* _scrollGroup;
		Fl_Button* _retargetButton;
		Fl_Button* _cancelButton;
		int behaviorSetCurY;
	public:
		Fl_Double_Window* rootWindow;
};
#endif
