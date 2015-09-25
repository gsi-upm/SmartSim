#ifndef SBOGRELISTENER_H
#define SBOGRELISTENER_H
#include "FLTKListener.h"
#include <sb/SBObserver.h>

class EmbeddedOgre;
class OgreListener : public FLTKListener
{
public:
	OgreListener(EmbeddedOgre* ogreInterface);
	~OgreListener(void);

	virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );
	virtual void OnCharacterDelete( const std::string & name );
	virtual void OnCharacterUpdate( const std::string & name );
	virtual void OnPawnCreate( const std::string & name );
	virtual void OnPawnDelete( const std::string & name );
	virtual void OnViseme( const std::string & name, const std::string & visemeName, const float weight, const float blendTime );
	virtual void OnChannel( const std::string & name, const std::string & channelName, const float value);
	virtual void OnSimulationStart();
	virtual void notify(SmartBody::SBSubject* subject);
protected:
	EmbeddedOgre* ogreInterface;	
};

#endif
