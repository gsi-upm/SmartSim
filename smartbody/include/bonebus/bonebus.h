/*
   This file is part of VHMsg written by Edward Fast at 
   University of Southern California's Institute for Creative Technologies.
   http://www.ict.usc.edu
   Copyright 2008 Edward Fast, University of Southern California

   VHMsg is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VHMsg is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with VHMsg.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BONEBUS_H
#define BONEBUS_H


#include "vhcl_public.h"

#include <string>
#include <vector>
#include <map>


namespace bonebus
{

struct BoneBusCharacterSocketPimpl;

class BoneBusCharacter
{
   private:
      struct BoneNameMapData
      {
         int   networkId;
         float cacheQuatW;  // cache the previous value to save bandwidth
         float cacheQuatX;
         float cacheQuatY;
         float cacheQuatZ;
         int   cacheQuatHits;   // keep track of how many times previous value is sent
         double cacheQuatTime;
         float cachePosX;
         float cachePosY;
         float cachePosZ;
         int   cachePosHits;   // keep track of how many times previous value is sent
         double cachePosTime;


         BoneNameMapData() {}
         BoneNameMapData( int networkId )
         {
            this->networkId  = networkId;
            this->cacheQuatW = 0;
            this->cacheQuatX = 0;
            this->cacheQuatY = 0;
            this->cacheQuatZ = 0;
            this->cacheQuatHits = 0;
            this->cacheQuatTime = 0;
            this->cachePosX  = 0;
            this->cachePosY  = 0;
            this->cachePosZ  = 0;
            this->cachePosHits  = 0;
            this->cachePosTime = 0;
         }
      };



      typedef std::map<const std::string, BoneNameMapData>  BoneNameMap;
      typedef std::map<const std::string, BoneNameMapData>::iterator  BoneNameMapIter;
      typedef std::map<const std::string, BoneNameMapData>::const_iterator  BoneNameMapConstIter;



      typedef std::map<const std::string, int>  VisemeNameMap;


      struct BulkBoneRotation
      {
         int boneId;
         float rot_w;
         float rot_x;
         float rot_y;
         float rot_z;
      };

      struct BulkBoneRotations
      {
         int packetId;
         int time;
         int charId;
         int numBoneRotations;
         BulkBoneRotation bones[ 1000 ];
      };

      static const int BulkBoneRotationsHeaderSize = sizeof( int ) * 4;


      struct BulkBonePosition
      {
         int boneId;
         float pos_x;
         float pos_y;
         float pos_z;
      };

      struct BulkBonePositions
      {
         int  packetId;
         int  time;
         int  charId;
         int  numBonePositions;
         BulkBonePosition bones[ 1000 ];
      };

      static const int BulkBonePositionsHeaderSize = sizeof( int ) * 4;


      struct VisemeBlendData
      {
         int packetId;
         int charId;
         int visemeId;
         float weight;
         float blendTime;
      };

      struct BulkGeneralParam
      {
         int nameId;
         int paramSize;
         int paramIndex;
         float paramValue;   // value of a param at a given index inside a param array
      };

      struct BulkGeneralParams
      {
         int packetId;
         int charId;
         int numGeneralParams;
         BulkGeneralParam params[ 1000 ];
     };
     static const int BulkGeneralParamsHeaderSize = sizeof( int ) * 3;

   public:
      static const double CACHE_REFRESH_TIME;
      static const double CACHE_REFRESH_JITTER;


   //private:
   public:
      std::string   m_name;
      std::string   m_objectClass;
      int           m_charId;
      BoneNameMap   m_boneNameMap;
      VisemeNameMap m_visemeNameMap;
      int           m_time;
      int           m_nextBoneId;
      int           m_nextVisemeId;

      BulkBoneRotations  m_bulkBoneRotations;
      BulkBonePositions  m_bulkBonePositions;
      BulkGeneralParams  m_bulkGeneralParams;

      float m_cacheQuatW;  // cache the previous value to save bandwidth
      float m_cacheQuatX;
      float m_cacheQuatY;
      float m_cacheQuatZ;
      int   m_cacheQuatHits;   // keep track of how many times previous value is sent
      double m_cacheQuatTime;
      float m_cachePosX;
      float m_cachePosY;
      float m_cachePosZ;
      int   m_cachePosHits;   // keep track of how many times previous value is sent
      double m_cachePosTime;
      int m_numErrors;

      BoneBusCharacterSocketPimpl * m_socketPimpl;


   public:
      BoneBusCharacter( const bool useFaceBones );
      virtual ~BoneBusCharacter();

      void StartSendBoneRotations();
      void EndSendBoneRotations();
      void AddBoneRotation( const char * boneName, const float w, const float x, const float y, const float z, const double time );

      void StartSendBonePositions();
      void EndSendBonePositions();
      void AddBonePosition( const char * boneName, const float x, const float y, const float z, const double time );

      void StartSendGeneralParameters();
      void EndSendGeneralParameters();
      void AddGeneralParameters( const int paramNameId, const int size, const float x, const int index, const double time );

      void SetViseme( const char * viseme, const float weight, const float blendTime );

      void SetPosition( const float x, const float y, const float z, const double time );
      void SetRotation( const float w, const float x, const float y, const float z, const double time );
      void SetParams( const char * paramName, const int paramNameId );
      void SetBoneId( const char * boneName, int id );
      void SetVisemeId( const char * visemeName, int id );

      void IncrementTime();
      int GetNumErrors();
};


struct BoneBusClientSocketPimpl;

class BoneBusClient
{
   private:
      struct ExecScriptData
      {
         int packetID;
         char command[ 256 ];
      };

   private:
      static const int NETWORK_PORT_UDP = 15100;  // according to Kumar, ICT (unofficially) uses a range in the 15000s (handy unoffical port list: http://www.iana.org/assignments/port-numbers)
      static const int NETWORK_PORT_TCP = 15102;


   private:
      bool m_wsaStartupCalled;
      BoneBusClientSocketPimpl * m_socketPimpl;

      typedef std::map<const std::string, int>  CharacterNameMap;
      CharacterNameMap m_characterNameMap;

      typedef std::map<int, BoneBusCharacter *>  CharacterDataMap;
      CharacterDataMap m_characterDataMap;

   public:
      BoneBusClient();
      virtual ~BoneBusClient();

      bool OpenConnection( const char * server );
      bool CloseConnection();

      bool IsOpen();

      bool Update();


      BoneBusCharacter * CreateCharacter( const char * charName, const char * objectClass, const bool useFaceBones );
      void UpdateAllCharacters();
      void UpdateCharacter( const char * charName );
      bool DeleteCharacter( BoneBusCharacter * character );
      BoneBusCharacter * FindCharacter( const int charID );
      BoneBusCharacter * FindCharacterByName( const char * name );
      int GetNumCharacters();

      void SetCameraPosition( const float x, const float y, const float z );
      void SetCameraRotation( const float w, const float x, const float y, const float z );


      void ExecScript( const char * command );
      void SendPlaySound( const char * soundFile, const char * charName = NULL );
      void SendStopSound( const char * soundFile );


      void SendSpeakText( const int msgNumber, const char * agent, const char * text ); // [BMLR] Sends text to speak
      void SendCreatePawn( const char * name, const double locx, const double locy, const double locz ); // [BMLR] Sends create pawn

      std::vector<std::string> GetCommand(); // [BMLR] Get command from renderer and external programs

   protected:
      static unsigned int HashDJB2( const char * str );
      static bool FindID( const CharacterNameMap & map, const int id );
};


struct BulkBoneRotation
{
   int boneId;
   float rot_w;
   float rot_x;
   float rot_y;
   float rot_z;
};

struct BulkBoneRotations
{
   int  packetId;
   int  time;
   int  charId;
   int  numBoneRotations;
   BulkBoneRotation bones[ 1000 ];
};
static const int BulkBoneRotationsHeaderSize = sizeof( int ) * 4;


struct BulkBonePosition
{
   int boneId;
   float pos_x;
   float pos_y;
   float pos_z;
};

struct BulkBonePositions
{
   int  packetId;
   int  time;
   int  charId;
   int  numBonePositions;
   BulkBonePosition bones[ 1000 ];
};
static const int BulkBonePositionsHeaderSize = sizeof( int ) * 4;

struct BulkGeneralParam
{
   int nameId;
   int paramSize;
   int paramIndex;
   float paramValue;  // value of a param at a given index inside a param array
};

struct BulkGeneralParams
{
   int packetId;
   int time;
   int charId;
   int numGeneralParams;
   BulkGeneralParam params[ 1000 ];
};
static const int BulkGeneralParamsHeaderSize = sizeof( int ) * 4;


struct BoneBusServerSocketPimpl;

class BoneBusServer
{
   private:
      static const int NETWORK_PORT_UDP = 15100;  // according to Kumar, ICT (unofficially) uses a range in the 15000s (handy unoffical port list: http://www.iana.org/assignments/port-numbers)
      static const int NETWORK_PORT_TCP = 15102;


   public:
      struct ExecScriptData
      {
         int packetID;
         char command[ 256 ];
      };


   public:
      typedef void (*OnClientConnectFunc)( const std::string & clientName, void * userData );
      typedef void (*OnClientDisconnectFunc)();
      typedef void (*OnCreateCharacterFunc)( const int characterID, const std::string & characterType, const std::string & characterName, const int skeletonType, void * userData );
      typedef void (*OnDeleteCharacterFunc)( const int characterID, void * userData );
      typedef void (*OnUpdateCharacterFunc)( const int characterID, const std::string & characterType, const std::string & characterName, const int skeletonType, void * userData );
      typedef void (*OnSetCharacterPositionFunc)( const int characterID, const float x, const float y, const float z, void * userData );
      typedef void (*OnSetCharacterRotationFunc)( const int characterID, const float w, const float x, const float y, const float z, void * userData );
      typedef void (*OnBoneRotationsFunc)( const BulkBoneRotations * bulkBoneRotations, void * userData );
      typedef void (*OnBonePositionsFunc)( const BulkBonePositions * bulkBonePositions, void * userData );
      typedef void (*OnSetCharacterVisemeFunc)( const int characterID, const int visemeId, const float weight, const float blendTime, void * userData );
      typedef void (*OnSetBoneIdFunc)( const int characterID, const std::string boneName, const int id, void * userData );
      typedef void (*OnSetVisemeIdFunc)( const int characterID, const std::string visemeName, const int id, void * userData );
      typedef void (*OnPlaySoundFunc)( const std::string & soundFile, const std::string & characterName, void * userData );
      typedef void (*OnStopSoundFunc)( const std::string & soundFile, void * userData );
      typedef void (*OnExecScriptFunc)( const char * command, void * userData );

      typedef void (*OnSetCharacterParamFunc)( const char * name, const int name_id, void * userData );
      typedef void (*OnGeneralParamFunc)( const BulkGeneralParams * bulkGeneralParams, void * userData );


   private:
      bool m_wsaStartupCalled;

      BoneBusServerSocketPimpl * m_socketPimpl;

      OnClientConnectFunc m_onClientConnectFunc;
      void * m_onClientConnectUserData;
      OnClientDisconnectFunc m_onClientDisconnectFunc;
      void * m_onClientDisconnectUserData;
      OnCreateCharacterFunc m_onCreateCharacterFunc;
      void * m_onCreateCharacterUserData;
      OnUpdateCharacterFunc m_onUpdateCharacterFunc;
      void * m_onUpdateCharacterUserData;
      OnDeleteCharacterFunc m_onDeleteCharacterFunc;
      void * m_onDeleteCharacterUserData;
      OnSetCharacterPositionFunc m_onSetCharacterPositionFunc;
      void * m_onSetCharacterPositionUserData;
      OnSetCharacterRotationFunc m_onSetCharacterRotationFunc;
      void * m_onSetCharacterRotationUserData;
      OnBoneRotationsFunc m_onBoneRotationsFunc;
      void * m_onBoneRotationsUserData;
      OnBonePositionsFunc m_onBonePositionsFunc;
      void * m_onBonePositionsUserData;
      OnSetCharacterVisemeFunc m_onSetCharacterVisemeFunc;
      void * m_onSetCharacterVisemeUserData;
      OnSetBoneIdFunc m_onSetBoneIdFunc;
      void * m_onSetBoneIdUserData;
      OnSetVisemeIdFunc m_onSetVisemeIdFunc;
      void * m_onSetVisemeIdUserData;
      OnPlaySoundFunc m_onPlaySoundFunc;
      void * m_onPlaySoundUserData;
      OnStopSoundFunc m_onStopSoundFunc;
      void * m_onStopSoundUserData;
      OnExecScriptFunc m_onExecScriptFunc;
      void * m_onExecScriptUserData;
      OnSetCharacterParamFunc m_onSetCharacterParamFunc;
      void * m_onSetCharacterParamUserData;
      OnGeneralParamFunc m_onGeneralParamFunc;
      void * m_onGeneralParamUserData;

      std::vector<int> m_lastBonePosition;
      std::vector<int> m_lastBoneRotation;

   public:
      BoneBusServer();
      virtual ~BoneBusServer();

      bool OpenConnection();
      bool CloseConnection();

      bool IsOpen();

      bool Update();

      void SetOnClientConnectCallback( OnClientConnectFunc func, void * userData = NULL ) { m_onClientConnectFunc = func; m_onClientConnectUserData = userData; }
      void SetOnClientDisconnectCallback( OnClientDisconnectFunc func, void * userData = NULL ) { m_onClientDisconnectFunc = func; m_onClientDisconnectUserData = userData; }
      void SetOnCreateCharacterFunc( OnCreateCharacterFunc func, void * userData = NULL ) { m_onCreateCharacterFunc = func; m_onCreateCharacterUserData = userData; }
      void SetOnDeleteCharacterFunc( OnDeleteCharacterFunc func, void * userData = NULL ) { m_onDeleteCharacterFunc = func; m_onDeleteCharacterUserData = userData; }
      void SetOnUpdateCharacterFunc( OnUpdateCharacterFunc func, void * userData = NULL ) { m_onUpdateCharacterFunc = func; m_onUpdateCharacterUserData = userData; }
      void SetOnSetCharacterPositionFunc( OnSetCharacterPositionFunc func, void * userData = NULL ) { m_onSetCharacterPositionFunc = func; m_onSetCharacterPositionUserData = userData; }
      void SetOnSetCharacterRotationFunc( OnSetCharacterRotationFunc func, void * userData = NULL ) { m_onSetCharacterRotationFunc = func; m_onSetCharacterRotationUserData = userData; }

      void SetOnBoneRotationsFunc( OnBoneRotationsFunc func, void * userData = NULL ) { m_onBoneRotationsFunc = func; m_onBoneRotationsUserData = userData; }
      void SetOnBonePositionsFunc( OnBonePositionsFunc func, void * userData = NULL ) { m_onBonePositionsFunc = func; m_onBonePositionsUserData = userData; }

      void SetOnSetCharacterVisemeFunc( OnSetCharacterVisemeFunc func, void * userData = NULL ) { m_onSetCharacterVisemeFunc = func; m_onSetCharacterVisemeUserData = userData; }
      void SetOnBoneIdFunc( OnSetBoneIdFunc func, void * userData = NULL ) { m_onSetBoneIdFunc = func; m_onSetBoneIdUserData = userData; }
      void SetOnVisemeIdFunc( OnSetVisemeIdFunc func, void * userData = NULL ) { m_onSetVisemeIdFunc = func; m_onSetVisemeIdUserData = userData; }

      void SetOnPlaySoundFunc( OnPlaySoundFunc func, void * userData = NULL ) { m_onPlaySoundFunc = func; m_onPlaySoundUserData = userData; }
      void SetOnStopSoundFunc( OnStopSoundFunc func, void * userData = NULL ) { m_onStopSoundFunc = func; m_onStopSoundUserData = userData; }

      void SetOnExecScriptFunc( OnExecScriptFunc func, void * userData = NULL ) { m_onExecScriptFunc = func; m_onExecScriptUserData = userData; }

      void SetOnSetCharacterParamFunc( OnSetCharacterParamFunc func, void * userData = NULL ) { m_onSetCharacterParamFunc = func; m_onSetCharacterParamUserData = userData; }
      void SetOnGeneralParamFunc( OnGeneralParamFunc func, void *userData = NULL ) { m_onGeneralParamFunc = func; m_onGeneralParamUserData = userData; }
};

};


#endif  // BONEBUS_H
