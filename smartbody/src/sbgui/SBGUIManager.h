#ifndef _SBGUIMANAGER_H_
#define _SBGUIMANAGER_H_

#include <string>
#include <map>
#include <vector>
#include <sr/sr_event.h>

namespace CEGUI
{
	class OpenGLRenderer;
}


class SBGUIManager // wrapper class for CEGUI library
{
	public:
		SBGUIManager();
		~SBGUIManager();
		static SBGUIManager* _singleton;
	public:
		static SBGUIManager& singleton() 
		{			
			return *singletonPtr();			
		}

		static SBGUIManager* singletonPtr() 
		{			
			if (!_singleton)
				_singleton = new SBGUIManager();
			return _singleton;			
		}

		static void destroy_singleton() {
			if( _singleton )
				delete _singleton;
			_singleton = NULL;
		}	

		void init();

		void resetGUI();
		void update();
		void resize(int w, int h); 
		int handleEvent(int eventID);		
	protected:		
		CEGUI::OpenGLRenderer* guiRenderer;
		bool initialized;
};
#endif
