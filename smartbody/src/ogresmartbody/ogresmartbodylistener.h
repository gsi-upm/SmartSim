#ifndef _OGRESMARTBODYLISTENER_H_
#define _OGRESMARTBODYLISTENER_H_

#include <OGRE/Ogre.h>
#include "ogresmartbody.h"
#include <sb/SBSceneListener.h>

class OgreSmartBodyListener : public SmartBody::SBSceneListener
{
   public:
	   OgreSmartBodyListener(OgreSmartBody* osb);
	   ~OgreSmartBodyListener();

		virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );	
		virtual void OnCharacterDelete( const std::string & name );
		virtual void OnCharacterChanged( const std::string& name );		 
		virtual void OnLogMessage( const std::string & message );


	private:
		OgreSmartBody* ogreSB;
};

#endif

