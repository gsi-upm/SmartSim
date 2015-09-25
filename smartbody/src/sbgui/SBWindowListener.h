#ifndef _SBWINDOWLISTENER_
#define _SBWINDOWLISTENER_

#include <sb/SBSceneListener.h>

class SBWindowListener : SmartBody::SBSceneListener
{
	public:
		SBWindowListener();
		~SBWindowListener();

		virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );
		virtual void OnCharacterDelete( const std::string & name );
		virtual void OnCharacterUpdate( const std::string & name );
      
		virtual void OnPawnCreate( const std::string & name );
		virtual void OnPawnDelete( const std::string & name );

		virtual void OnObjectCreate( SmartBody::SBObject* object );
		virtual void OnObjectDelete( SmartBody::SBObject* object );

		virtual void OnSimulationStart();
		virtual void OnSimulationEnd();
		virtual void OnSimulationUpdate();

		virtual void windowShow();
		virtual void windowHide();

};


#endif