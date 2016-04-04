
#ifndef SMARTBODY_C_DLL_H
#define SMARTBODY_C_DLL_H

#include "vhcl_public.h"

#include "sb/SBTypes.h"
#include "sb/SBCharacterFrameData.h"


typedef intptr_t SBMHANDLE;


#ifdef __cplusplus
extern "C" {
#endif 


SBAPI SBMHANDLE SBM_CreateSBM();

SBAPI bool SBM_Init( SBMHANDLE sbmHandle, const char * pythonLibPath, bool logToFile );
SBAPI bool SBM_Shutdown( SBMHANDLE sbmHandle );

SBAPI bool SBM_Update( SBMHANDLE sbmHandle, double timeInSeconds );
SBAPI bool SBM_UpdateUsingDelta( SBMHANDLE sbmHandle, double deltaTimeInSeconds );  // same as SBM_Update(), but pass in a delta time since the prev call.
SBAPI bool SBM_ProcessVHMsgs( SBMHANDLE sbmHandle, const char * op, const char * args );

SBAPI bool SBM_InitCharacter( SBMHANDLE sbmHandle, const char * name, SBM_CharacterFrameDataMarshalFriendly * character );
SBAPI bool SBM_GetCharacter( SBMHANDLE sbmHandle, const char * name, SBM_CharacterFrameDataMarshalFriendly * character );
SBAPI bool SBM_ReleaseCharacter( SBM_CharacterFrameDataMarshalFriendly * character );

// used for polling events.  These must be called regularly, or else their queues will overflow
SBAPI bool SBM_IsCharacterCreated( SBMHANDLE sbmHandle, char * name, int maxNameLen, char * objectClass, int maxObjectClassLen );
SBAPI bool SBM_IsCharacterDeleted( SBMHANDLE sbmHandle, char * name, int maxNameLen );
SBAPI bool SBM_IsCharacterChanged( SBMHANDLE sbmHandle, char * name, int maxNameLen );
SBAPI bool SBM_IsVisemeSet( SBMHANDLE sbmHandle, char * name, int maxNameLen, char * visemeName, int maxVisemeNameLen, float * weight, float * blendTime );
SBAPI bool SBM_IsChannelSet( SBMHANDLE sbmHandle, char * name, int maxNameLen, char * channelName, int maxChannelNameLen, float * value );
SBAPI bool SBM_IsLogMessageWaiting( SBMHANDLE sbmHandle, char * logMessage, int maxLogMessageLen, int * logMessageType );
SBAPI bool SBM_IsBmlRequestWaiting( SBMHANDLE sbmHandle, char * charName, int maxNameLen, char * requestId, int maxRequestIdLength, char * bmlName, int maxBmlNameLength);
SBAPI void SBM_SendBmlReply(SBMHANDLE sbmHandle, const char * charName, const char * requestId, const char * utteranceId, const char * bmlText);

// python usage functions
SBAPI bool SBM_PythonCommandVoid( SBMHANDLE sbmHandle, const char * command );
SBAPI bool SBM_PythonCommandBool( SBMHANDLE sbmHandle, const char * command );
SBAPI int SBM_PythonCommandInt( SBMHANDLE sbmHandle, const char * command );
SBAPI float SBM_PythonCommandFloat( SBMHANDLE sbmHandle, const char * command );
SBAPI void SBM_PythonCommandString( SBMHANDLE sbmHandle, const char * command, char * output, int maxLen);

// DLL class accessors
SBAPI bool SBM_SBAssetManager_LoadSkeleton( SBMHANDLE sbmHandle, const void * data, int sizeBytes, const char * skeletonName );
SBAPI bool SBM_SBAssetManager_LoadMotion( SBMHANDLE sbmHandle, const void * data, int sizeBytes, const char * motionName );

SBAPI void SBM_SBDebuggerServer_SetID( SBMHANDLE sbmHandle, const char * id );
SBAPI void SBM_SBDebuggerServer_SetCameraValues( SBMHANDLE sbmHandle, double x, double y, double z, double rx, double ry, double rz, double rw, double fov, double aspect, double zNear, double zFar );
SBAPI void SBM_SBDebuggerServer_SetRendererIsRightHanded( SBMHANDLE sbmHandle, bool enabled );

SBAPI void SBM_SBMotion_AddChannel( SBMHANDLE sbmHandle, const char * motionName, const char * channelName, const char * channelType );
SBAPI void SBM_SBMotion_AddChannels( SBMHANDLE sbmHandle, const char * motionName, const char ** channelNames, const char ** channelTypes, int count );
SBAPI void SBM_SBMotion_AddFrame( SBMHANDLE sbmHandle, const char * motionName, float frameTime, const float * frameData, int numFrameData );
SBAPI void SBM_SBMotion_SetSyncPoint( SBMHANDLE sbmHandle, const char * motionName, const char * syncTag, double time );

SBAPI void SBM_SBJointMap_GetMapTarget( SBMHANDLE sbmHandle, const char * jointMap, const char * jointName, char * mappedJointName, int maxMappedJointName );

SBAPI void SBM_SBDiphoneManager_CreateDiphone(SBMHANDLE sbmHandle, const char * fromPhoneme, const char * toPhoneme, const char * name);
SBAPI void SBM_SBDiphone_AddKey(SBMHANDLE sbmHandle, const char * fromPhoneme, const char * toPhoneme, const char * name, const char * viseme, float time, float weight);

SBAPI void SBM_SBVHMsgManager_SetServer( SBMHANDLE sbmHandle, const char * server );
SBAPI void SBM_SBVHMsgManager_SetScope( SBMHANDLE sbmHandle, const char * scope );
SBAPI void SBM_SBVHMsgManager_SetPort( SBMHANDLE sbmHandle, const char * port );
SBAPI void SBM_SBVHMsgManager_SetEnable( SBMHANDLE sbmHandle, bool enable );


#ifdef __cplusplus
}
#endif

#endif  // SMARTBODY_C_DLL_H
