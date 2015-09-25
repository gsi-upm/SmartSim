#ifndef _FLTKLISTENER_H_
#define _FLTKLISTENER_H_

#include "sb/SBSceneListener.h"
#include "sb/SBObserver.h"

class FLTKListener : public SmartBody::SBSceneListener, public SmartBody::SBObserver
{
   public:
	  FLTKListener();
	  ~FLTKListener();

      virtual void OnCharacterCreate( const std::string & name, const std::string & objectClass );
      virtual void OnCharacterDelete( const std::string & name );
	  virtual void OnCharacterUpdate( const std::string & name );
      virtual void OnPawnCreate( const std::string & name );
      virtual void OnPawnDelete( const std::string & name );
      virtual void OnViseme( const std::string & name, const std::string & visemeName, const float weight, const float blendTime );
	  virtual void OnChannel( const std::string & name, const std::string & channelName, const float value);
	  virtual void OnLogMessage( const std::string & message );
	  virtual void OnEvent( const std::string & eventName, const std::string & eventParameters );

	  virtual void OnSimulationStart();
	  virtual void OnSimulationEnd();
	  virtual void OnSimulationUpdate();

	  virtual void notify(SmartBody::SBSubject* subject);

	  void setOtherListener(SBSceneListener* listener);
	protected:
	  SmartBody::SBSceneListener* otherListener;
};

#endif
