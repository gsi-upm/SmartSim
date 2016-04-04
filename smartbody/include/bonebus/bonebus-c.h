#ifndef BONEBUS_C_H
#define BONEBUS_C_H

#include "vhcl_public.h"

// removed until bonebus is a dll itself
//#ifdef BONEBUS_C_EXPORTS
//#define BONEBUS_C_API __declspec(dllexport)
//#else
//#define BONEBUS_C_API __declspec(dllimport)
//#endif
#define BONEBUS_C_API

#if defined(WIN_BUILD)
#define STDCALL __stdcall
#else
#define STDCALL 
#endif


typedef intptr_t BONEBUSHANDLE;


#ifdef __cplusplus
extern "C" {
#endif 


struct BONEBUS_BulkBoneRotation
{
   int boneId;
   float rot_w;
   float rot_x;
   float rot_y;
   float rot_z;
};


struct BONEBUS_BulkBoneRotations
{
   int  packetId;
   int  time;
   int  charId;
   int  numBoneRotations;
   int  numBonePositions_unused;
   BONEBUS_BulkBoneRotation * bones;
};
static const int BONEBUS_BulkBoneRotationsHeaderSize = sizeof( int ) * 5;


struct BONEBUS_BulkBonePosition
{
   int boneId;
   float pos_x;
   float pos_y;
   float pos_z;
};

struct BONEBUS_BulkBonePositions
{
   int  packetId;
   int  time;
   int  charId;
   int  numBonePositions;
   BONEBUS_BulkBonePosition * bones;
};
static const int BONEBUS_BulkBonePositionsHeaderSize = sizeof( int ) * 4;


typedef void (STDCALL *BONEBUS_OnClientConnectFunc)( const char * clientName, void * userData);
typedef void (STDCALL *BONEBUS_OnClientDisconnectFunc)();
typedef void (STDCALL *BONEBUS_OnCreateCharacterFunc)( const int characterID, const char * characterType, const char * characterName, const int skeletonType, void * userData );
typedef void (STDCALL *BONEBUS_OnDeleteCharacterFunc)( const int characterID, void * userData );
typedef void (STDCALL *BONEBUS_OnUpdateCharacterFunc)( const int characterID, const char * characterType, const char * characterName, const int skeletonType, void * userData );
typedef void (STDCALL *BONEBUS_OnSetCharacterPositionFunc)( const int characterID, const float x, const float y, const float z, void * userData );
typedef void (STDCALL *BONEBUS_OnSetCharacterRotationFunc)( const int characterID, const float w, const float x, const float y, const float z, void * userData );
typedef void (STDCALL *BONEBUS_OnBoneRotationsFunc)( const BONEBUS_BulkBoneRotations * bulkBoneRotations, void * userData );
typedef void (STDCALL *BONEBUS_OnBonePositionsFunc)( const BONEBUS_BulkBonePositions * bulkBonePositions, void * userData );
typedef void (STDCALL *BONEBUS_OnSetCharacterVisemeFunc)( const int characterID, const int visemeId, const float weight, const float blendTime, void * userData );
typedef void (STDCALL *BONEBUS_OnSetBoneIdFunc)( const int characterID, const char * boneName, const int boneId, void * userData );
typedef void (STDCALL *BONEBUS_OnSetVisemeIdFunc)( const int characterID, const char * visemeName, const int visemeId, void * userData );
typedef void (STDCALL *BONEBUS_OnPlaySoundFunc)( const char * soundFile, const char * characterName, void * userData );
typedef void (STDCALL *BONEBUS_OnStopSoundFunc)( const char * soundFile, void * userData );
typedef void (STDCALL *BONEBUS_OnExecScriptFunc)( const char * command, void * userData );


BONEBUS_C_API BONEBUSHANDLE BONEBUS_CreateBoneBus();
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
                                 BONEBUS_OnExecScriptFunc onExecScriptFunc );

BONEBUS_C_API bool BONEBUS_Close( BONEBUSHANDLE handle );
BONEBUS_C_API bool BONEBUS_Update( BONEBUSHANDLE handle );


#ifdef __cplusplus
}
#endif

#endif  // BONEBUS_C_H
