#ifndef SBINTERFACELISTENER

#include <vector>
#include <sr/sr_vec.h>
#include <sb/SBObject.h>

class SBInterfaceListener
{
	public:
		SBInterfaceListener();
		~SBInterfaceListener();

		virtual void onStart();
		virtual bool onMouseClick(int x, int y, int button);
		virtual bool onMouseMove(int x, int y);
		virtual bool onMouseRelease(int x, int y, int button);
		virtual bool onMouseDrag(int x, int y);
		virtual bool onKeyboardPress(char c);
		virtual bool onKeyboardRelease(char c);
		virtual void onEnd();

};


class SBInterfaceManager
{
	public:
		SBInterfaceManager();
		~SBInterfaceManager();


		void addInterfaceListener(SBInterfaceListener* listener);
		void removeInterfaceListener(SBInterfaceListener* listener);
		std::vector<SBInterfaceListener*> getInterfaceListeners();

		SrVec convertScreenSpaceTo3D(int x, int y, SrVec ground, SrVec upVector);
		std::string getSelectedObject();

		static SBInterfaceManager* getInterfaceManager();

	protected:
		static SBInterfaceManager* _interfaceManager;
		std::vector<SBInterfaceListener*> _interfaceListeners;
};


#endif