#include "SBWindowListener.h"
#include <sb/SBScene.h>

SBWindowListener::SBWindowListener()
{
}

SBWindowListener::~SBWindowListener()
{
	SmartBody::SBScene::getScene()->removeSceneListener(this);
}

void SBWindowListener::OnCharacterCreate( const std::string & name, const std::string & objectClass )
{
}

void SBWindowListener::OnCharacterDelete( const std::string & name )
{
}

void SBWindowListener::OnCharacterUpdate( const std::string & name )
{
}
      
void SBWindowListener::OnPawnCreate( const std::string & name )
{
}

void SBWindowListener::OnPawnDelete( const std::string & name )
{
}

void SBWindowListener::OnSimulationStart()
{
}

void SBWindowListener::OnSimulationEnd()
{
}

void SBWindowListener::OnSimulationUpdate()
{
}

void SBWindowListener::OnObjectCreate( SmartBody::SBObject* object )
{
}

void SBWindowListener::OnObjectDelete( SmartBody::SBObject* object )
{
}

void SBWindowListener::windowShow()
{
	SmartBody::SBScene::getScene()->addSceneListener(this);
}

void SBWindowListener::windowHide()
{
	SmartBody::SBScene::getScene()->removeSceneListener(this);
}



