
#include "vhcl.h"

#include "bonebus-c.h"

#include <string.h>

#include "bonebus.h"


using namespace bonebus;


bool BONEBUS_HandleExists( const BONEBUSHANDLE handle );


class BONEBUS_BoneBusServer : public BoneBusServer
{
public:
   BONEBUS_OnClientConnectFunc m_onClientConnectFunc;
   BONEBUS_OnClientDisconnectFunc m_onClientDisconnectFunc;
   BONEBUS_OnCreateCharacterFunc m_onCreateCharacterFunc;
   BONEBUS_OnDeleteCharacterFunc m_onDeleteCharacterFunc;
   BONEBUS_OnUpdateCharacterFunc m_onUpdateCharacterFunc;
   BONEBUS_OnSetCharacterPositionFunc m_onSetCharacterPositionFunc;
   BONEBUS_OnSetCharacterRotationFunc m_onSetCharacterRotationFunc;
   BONEBUS_OnBoneRotationsFunc m_onBoneRotationsFunc;
   BONEBUS_OnBonePositionsFunc m_onBonePositionsFunc;
   BONEBUS_OnSetCharacterVisemeFunc m_onSetCharacterVisemeFunc;
   BONEBUS_OnSetBoneIdFunc m_onSetBoneIdFunc;
   BONEBUS_OnSetVisemeIdFunc m_onSetVisemeIdFunc;
   BONEBUS_OnExecScriptFunc m_onExecScriptFunc;
   BONEBUS_OnPlaySoundFunc m_onPlaySoundFunc;
   BONEBUS_OnStopSoundFunc m_onStopSoundFunc;

   void BONEBUS_SetOnClientConnectCallback( BONEBUS_OnClientConnectFunc func ) { m_onClientConnectFunc = func; }
   void BONEBUS_SetOnCreateCharacterCallback( BONEBUS_OnCreateCharacterFunc func ) { m_onCreateCharacterFunc = func; }
   void BONEBUS_SetOnDeleteCharacterFunc( BONEBUS_OnDeleteCharacterFunc func ) { m_onDeleteCharacterFunc = func; }
   void BONEBUS_SetOnUpdateCharacterCallback( BONEBUS_OnUpdateCharacterFunc func ) { m_onUpdateCharacterFunc = func; }
   void BONEBUS_SetOnPlaySoundCallback( BONEBUS_OnPlaySoundFunc func ) { m_onPlaySoundFunc = func; }
   void BONEBUS_SetOnStopSoundCallback( BONEBUS_OnStopSoundFunc func ) { m_onStopSoundFunc = func; }
   void BONEBUS_SetOnClientDisconnectCallback( BONEBUS_OnClientDisconnectFunc func ) { m_onClientDisconnectFunc = func; }
   void BONEBUS_SetOnSetCharacterPositionFunc( BONEBUS_OnSetCharacterPositionFunc func ) { m_onSetCharacterPositionFunc = func; }
   void BONEBUS_SetOnSetCharacterRotationFunc( BONEBUS_OnSetCharacterRotationFunc func ) { m_onSetCharacterRotationFunc = func; }
   void BONEBUS_SetOnBoneRotationsFunc( BONEBUS_OnBoneRotationsFunc func ) { m_onBoneRotationsFunc = func; }
   void BONEBUS_SetOnBonePositionsFunc( BONEBUS_OnBonePositionsFunc func ) { m_onBonePositionsFunc = func; }
   void BONEBUS_SetOnBoneIdFunc( BONEBUS_OnSetBoneIdFunc func ) { m_onSetBoneIdFunc = func; }
   void BONEBUS_SetOnVisemeIdFunc( BONEBUS_OnSetVisemeIdFunc func ) { m_onSetVisemeIdFunc = func; }
   void BONEBUS_SetOnSetCharacterVisemeFunc( BONEBUS_OnSetCharacterVisemeFunc func ) { m_onSetCharacterVisemeFunc = func; }
   void BONEBUS_SetOnExecScriptFunc( BONEBUS_OnExecScriptFunc func ) { m_onExecScriptFunc = func; }


   BONEBUS_BoneBusServer()
   {
      m_onClientConnectFunc = NULL;
      m_onClientDisconnectFunc = NULL;
      m_onCreateCharacterFunc = NULL;
      m_onDeleteCharacterFunc = NULL;
      m_onUpdateCharacterFunc = NULL;
      m_onSetCharacterPositionFunc = NULL;
      m_onSetCharacterRotationFunc = NULL;
      m_onBoneRotationsFunc = NULL;
      m_onBonePositionsFunc = NULL;
      m_onSetCharacterVisemeFunc = NULL;
      m_onSetBoneIdFunc = NULL;
      m_onExecScriptFunc = NULL;
      m_onPlaySoundFunc = NULL;
      m_onStopSoundFunc = NULL;

      SetOnClientConnectCallback( BONEBUS_BoneBusServer::OnClientConnect, this );
      SetOnClientDisconnectCallback( BONEBUS_BoneBusServer::OnClientDisconnect, this );
      SetOnCreateCharacterFunc( BONEBUS_BoneBusServer::OnCreateCharacter, this );
      SetOnDeleteCharacterFunc( BONEBUS_BoneBusServer::OnDeleteCharacter, this );
      SetOnUpdateCharacterFunc( BONEBUS_BoneBusServer::OnCreateCharacter, this );
      SetOnSetCharacterPositionFunc( BONEBUS_BoneBusServer::SetCharacterPosition, this );
      SetOnSetCharacterRotationFunc( BONEBUS_BoneBusServer::SetCharacterRotation, this );
      SetOnBoneRotationsFunc( BONEBUS_BoneBusServer::OnBoneRotations, this );
      SetOnBonePositionsFunc( BONEBUS_BoneBusServer::OnBonePositions, this );
      SetOnSetCharacterVisemeFunc( BONEBUS_BoneBusServer::OnSetCharacterVisme, this );
      SetOnBoneIdFunc( BONEBUS_BoneBusServer::OnSetBoneId, this );
      SetOnVisemeIdFunc( BONEBUS_BoneBusServer::OnSetVisemeId, this );
      SetOnPlaySoundFunc( BONEBUS_BoneBusServer::OnPlaySound, this );
      SetOnStopSoundFunc( BONEBUS_BoneBusServer::OnStopSound, this );
      SetOnExecScriptFunc( BONEBUS_BoneBusServer::OnExecScript, this );
   }

   ~BONEBUS_BoneBusServer() {}


   static void OnClientConnect( const std::string & clientName, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onClientConnectFunc )
      {
         wrapper->m_onClientConnectFunc( clientName.c_str(), userData );
      }
   }

   static void OnCreateCharacter( const int characterID, const std::string & characterType, const std::string & characterName, const int skeletonType, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onCreateCharacterFunc )
      {
         wrapper->m_onCreateCharacterFunc( characterID, characterType.c_str(), characterName.c_str(), skeletonType, userData );
      }
   }

   static void OnClientDisconnect()
   {
      /*
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onClientDisconnectFunc )
      {
         wrapper->m_onClientDisconnectFunc();
      }
      */
   }

   static void OnDeleteCharacter( const int characterID, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onDeleteCharacterFunc )
      {
         wrapper->m_onDeleteCharacterFunc( characterID, userData );
      }
   }

   static void OnUpdateCharacter( const int characterID, const std::string & characterType, const std::string & characterName, const int skeletonType, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onUpdateCharacterFunc )
      {
         wrapper->m_onUpdateCharacterFunc( characterID, characterType.c_str(), characterName.c_str(), skeletonType, userData );
      }
   }

   static void SetCharacterPosition( const int characterID, const float x, const float y, const float z, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onSetCharacterPositionFunc )
      {
         wrapper->m_onSetCharacterPositionFunc( characterID, x, y, z, userData );
      }
   }

   static void SetCharacterRotation( const int characterID, const float w, const float x, const float y, const float z, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onSetCharacterRotationFunc )
      {
         wrapper->m_onSetCharacterRotationFunc( characterID, w, x, y, z, userData );
      }
   }

   static void OnBoneRotations( const BulkBoneRotations * bulkBoneRotations, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onBoneRotationsFunc )
      {
         BONEBUS_BulkBoneRotations rots;
         rots.packetId = bulkBoneRotations->packetId;
         rots.time = bulkBoneRotations->time;
         rots.charId = bulkBoneRotations->charId;
         rots.numBoneRotations = bulkBoneRotations->numBoneRotations;
         //rots.numBonePositions_unused = bulkBoneRotations->numBonePositions_unused;
         rots.bones = new BONEBUS_BulkBoneRotation[ bulkBoneRotations->numBoneRotations ];
         memcpy( rots.bones, bulkBoneRotations->bones, sizeof( BONEBUS_BulkBoneRotation ) * bulkBoneRotations->numBoneRotations );

         wrapper->m_onBoneRotationsFunc( &rots, userData );

         delete [] rots.bones;
      }
   }

   static void OnBonePositions( const BulkBonePositions * bulkBonePositions, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onBonePositionsFunc )
      {
         BONEBUS_BulkBonePositions positions;
         positions.packetId = bulkBonePositions->packetId;
         positions.time = bulkBonePositions->time;
         positions.charId = bulkBonePositions->charId;
         positions.numBonePositions = bulkBonePositions->numBonePositions;
         positions.bones = new BONEBUS_BulkBonePosition[ bulkBonePositions->numBonePositions ];
         memcpy( positions.bones, bulkBonePositions->bones, sizeof( BONEBUS_BulkBonePosition ) * bulkBonePositions->numBonePositions );

         wrapper->m_onBonePositionsFunc( &positions, userData );

         delete [] positions.bones;
      }
   }

   static void OnSetCharacterVisme( const int characterID, const int visemeId, const float weight, const float blendTime, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onSetCharacterVisemeFunc )
      {
         wrapper->m_onSetCharacterVisemeFunc( characterID, visemeId, weight, blendTime, userData );
      }
   }

   static void OnSetBoneId( const int characterID, const std::string boneName, const int id, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onSetBoneIdFunc )
      {
         wrapper->m_onSetBoneIdFunc( characterID, boneName.c_str(), id, userData );
      }
   }

   static void OnSetVisemeId( const int characterID, const std::string visemeName, const int id, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onSetVisemeIdFunc )
      {
         wrapper->m_onSetVisemeIdFunc( characterID, visemeName.c_str(), id, userData );
      }
   }

   static void OnPlaySound( const std::string & soundFile, const std::string & characterName, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onPlaySoundFunc )
      {
         wrapper->m_onPlaySoundFunc( soundFile.c_str(), characterName.c_str(), userData );
      }
   }

   static void OnStopSound( const std::string & soundFile, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onStopSoundFunc )
      {
         wrapper->m_onStopSoundFunc( soundFile.c_str(), userData );
      }
   }

   static void OnExecScript( const char * command, void * userData )
   {
      BONEBUS_BoneBusServer * wrapper = (BONEBUS_BoneBusServer *)userData;
      if ( wrapper->m_onExecScriptFunc )
      {
         wrapper->m_onExecScriptFunc( command, userData );
      }
   }
};


std::map< BONEBUSHANDLE, BONEBUS_BoneBusServer * > g_boneBusInstances;
int g_handleId = 0;


BONEBUS_C_API BONEBUSHANDLE BONEBUS_CreateBoneBus()
{
   g_handleId++;
   g_boneBusInstances[ g_handleId ] = new BONEBUS_BoneBusServer();
   return g_handleId;
}


BONEBUS_C_API bool BONEBUS_Open( BONEBUSHANDLE handle,
                                 BONEBUS_OnClientConnectFunc onClientConnectFunc,
                                 BONEBUS_OnClientDisconnectFunc onClientDisconnectFunc,
                                 BONEBUS_OnCreateCharacterFunc onCreateCharacterFunc,
                                 BONEBUS_OnDeleteCharacterFunc onDeleteCharacterFunc,
                                 BONEBUS_OnUpdateCharacterFunc onUpdateCharacterFunc,
                                 BONEBUS_OnSetCharacterPositionFunc onSetCharacterPositionFunc,
                                 BONEBUS_OnSetCharacterRotationFunc onSetCharacterRotationFunc,
                                 BONEBUS_OnBoneRotationsFunc onBoneRotationsFunc,
                                 BONEBUS_OnBonePositionsFunc onBonePositionsFunc,
                                 BONEBUS_OnSetCharacterVisemeFunc onSetCharacterVisemeFunc,
                                 BONEBUS_OnSetBoneIdFunc onSetBoneIdFunc,
                                 BONEBUS_OnSetVisemeIdFunc onSetVisemeIdFunc,
                                 BONEBUS_OnPlaySoundFunc onPlaySoundFunc,
                                 BONEBUS_OnStopSoundFunc onStopSoundFunc,
                                 BONEBUS_OnExecScriptFunc onExecScriptFunc )
{
   if ( !BONEBUS_HandleExists( handle ) )
   {
      return false;
   }


   g_boneBusInstances[ handle ]->BONEBUS_SetOnClientConnectCallback( onClientConnectFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnClientDisconnectCallback( onClientDisconnectFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnCreateCharacterCallback( onCreateCharacterFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnDeleteCharacterFunc( onDeleteCharacterFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnUpdateCharacterCallback( onUpdateCharacterFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnSetCharacterPositionFunc( onSetCharacterPositionFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnSetCharacterRotationFunc( onSetCharacterRotationFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnBoneRotationsFunc( onBoneRotationsFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnBonePositionsFunc( onBonePositionsFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnSetCharacterVisemeFunc( onSetCharacterVisemeFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnBoneIdFunc( onSetBoneIdFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnVisemeIdFunc( onSetVisemeIdFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnPlaySoundCallback( onPlaySoundFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnStopSoundCallback( onStopSoundFunc );
   g_boneBusInstances[ handle ]->BONEBUS_SetOnExecScriptFunc( onExecScriptFunc );

   return g_boneBusInstances[ handle ]->OpenConnection();
}


BONEBUS_C_API bool BONEBUS_Close( BONEBUSHANDLE handle )
{
   if ( !BONEBUS_HandleExists( handle ) )
   {
      return false;
   }

   std::map< BONEBUSHANDLE, BONEBUS_BoneBusServer * >::iterator it = g_boneBusInstances.find( handle );
   BoneBusServer * boneBus = g_boneBusInstances[ handle ];
   g_boneBusInstances.erase( it );
   bool ret = boneBus->CloseConnection();
   delete boneBus;
   return ret;
}


BONEBUS_C_API bool BONEBUS_Update( BONEBUSHANDLE handle )
{
   if ( !BONEBUS_HandleExists( handle ) )
   {
      return false;
   }

   return g_boneBusInstances[ handle ]->Update();
}


bool BONEBUS_HandleExists( const BONEBUSHANDLE handle )
{
   return g_boneBusInstances.find( handle ) != g_boneBusInstances.end();
}
