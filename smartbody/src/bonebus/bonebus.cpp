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

#include "vhcl.h"

//#if defined(WIN_BUILD)
#if !defined(FLASH_BUILD)


#include "bonebus.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(WIN_BUILD)
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
//#include <sys/filio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netdb.h>
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s);
#define SOCKADDR struct sockaddr
#endif
#include <string>
#include <iostream>


using std::string;
using std::vector;
using namespace bonebus;


const int BONEBUS_PACKET_SIZE = 1024;

const double BoneBusCharacter::CACHE_REFRESH_TIME   = 9.0;  // seconds between refreshes
const double BoneBusCharacter::CACHE_REFRESH_JITTER = 1.0;  // to give the time between refreshes some randomness, so everything isn't updating at once

static double GetCacheRefreshTime() { return BoneBusCharacter::CACHE_REFRESH_TIME + ( ( (double)rand() / (double)RAND_MAX ) * BoneBusCharacter::CACHE_REFRESH_JITTER ); }  // return number between refresh and refresh + jitter


struct bonebus::BoneBusCharacterSocketPimpl
{
   SOCKET m_sockUDP;
   sockaddr_in m_toAddrUDP;
   SOCKET m_sockTCP;

   BoneBusCharacterSocketPimpl() {}
};


BoneBusCharacter::BoneBusCharacter( const bool useFaceBones )
{
   m_socketPimpl = new BoneBusCharacterSocketPimpl();

   m_numErrors = 0;
   m_time = 0;

   m_cacheQuatW = 0;
   m_cacheQuatX = 0;
   m_cacheQuatY = 0;
   m_cacheQuatZ = 0;
   m_cacheQuatHits = 0;
   m_cacheQuatTime = 0;
   m_cachePosX = 0;
   m_cachePosY = 0;
   m_cachePosZ = 0;
   m_cachePosHits = 0;
   m_cachePosTime = 0;

   m_nextBoneId = 0;
   m_nextVisemeId = 0;
}


BoneBusCharacter::~BoneBusCharacter()
{
   delete m_socketPimpl;
   m_socketPimpl = NULL;
}


void BoneBusCharacter::IncrementTime()
{
   m_time++;
}


void BoneBusCharacter::StartSendBoneRotations()
{
   m_bulkBoneRotations.time = m_time;
   m_bulkBoneRotations.numBoneRotations = 0;
}


void BoneBusCharacter::EndSendBoneRotations()
{
   if ( m_bulkBoneRotations.numBoneRotations <= 0 )
   {
      return;
   }

   int packetSize = BulkBoneRotationsHeaderSize + ( sizeof( BulkBoneRotation ) * m_bulkBoneRotations.numBoneRotations );

   //printf( "Rotation Packet size: %d\n", packetSize );

   int bytesSent;
   bytesSent = sendto( m_socketPimpl->m_sockUDP, (const char *)&m_bulkBoneRotations, packetSize, 0, (SOCKADDR*)&m_socketPimpl->m_toAddrUDP, sizeof( m_socketPimpl->m_toAddrUDP ) );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      m_numErrors++;
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


void BoneBusCharacter::AddBoneRotation( const char * boneName, const float w, const float x, const float y, const float z, const double time )
{
   int packetSize = BulkBoneRotationsHeaderSize + ( sizeof( BulkBoneRotation ) * m_bulkBoneRotations.numBoneRotations );

   if ( packetSize > BONEBUS_PACKET_SIZE )
   {
      //return;
      EndSendBoneRotations();
      StartSendBoneRotations();
      packetSize = BulkBoneRotationsHeaderSize + ( sizeof( BulkBoneRotation ) * m_bulkBoneRotations.numBoneRotations );
   }

   BoneNameMapIter iter = m_boneNameMap.find( std::string(boneName));

   bool boneIdSent = false;
   if ( iter == m_boneNameMap.end() )
   {
      // send the bone id information
      SetBoneId(boneName, m_nextBoneId);
      // add the bone information to the bone map
      BoneNameMapData data(m_nextBoneId);
      m_nextBoneId++;
      m_boneNameMap[std::string(boneName)] = data;
      iter = m_boneNameMap.find( boneName );
      boneIdSent = true;
   }

   BoneNameMapData & data = iter->second;


   bool cacheHit = false;

   // check to see if we've sent this data before
   // if we have, send it again to account for packet loss
   // and then don't send it again until the bone has moved
   if ( data.cacheQuatW == w &&
         data.cacheQuatX == x &&
         data.cacheQuatY == y &&
         data.cacheQuatZ == z )
   {
      data.cacheQuatHits++;

      if ( data.cacheQuatHits > 3 )
      {
         cacheHit = true;
      }
   }
   else
   {
      data.cacheQuatW = w;
      data.cacheQuatX = x;
      data.cacheQuatY = y;
      data.cacheQuatZ = z;
      data.cacheQuatHits = 0;
      data.cacheQuatTime = time + GetCacheRefreshTime();
   }

   if ( cacheHit )
   {
      if ( time > data.cacheQuatTime )
      {
         data.cacheQuatTime = time + GetCacheRefreshTime();
         cacheHit = false;
      }
   }

   if ( cacheHit || boneIdSent )
   {
      return;
   }


   BulkBoneRotation * item = &m_bulkBoneRotations.bones[ m_bulkBoneRotations.numBoneRotations ];

   item->boneId = data.networkId;
   item->rot_w  = w;
   item->rot_x  = x;
   item->rot_y  = y;
   item->rot_z  = z;

   m_bulkBoneRotations.numBoneRotations++;
}


void BoneBusCharacter::StartSendGeneralParameters()
{
   m_bulkGeneralParams.numGeneralParams = 0;
}


void BoneBusCharacter::AddGeneralParameters( const int paramNameId, const int size, const float x, const int id, const double time )
{
   int packetSize = BulkGeneralParamsHeaderSize + ( sizeof( BulkGeneralParam ) * m_bulkGeneralParams.numGeneralParams );

   if ( packetSize > BONEBUS_PACKET_SIZE )
   {
      EndSendGeneralParameters();
      StartSendGeneralParameters();
      packetSize = BulkGeneralParamsHeaderSize + ( sizeof( BulkGeneralParam ) * m_bulkGeneralParams.numGeneralParams );
   }

   BulkGeneralParam * item = &m_bulkGeneralParams.params[ m_bulkGeneralParams.numGeneralParams ];

   item->nameId = paramNameId;
   item->paramIndex = id;
   item->paramSize = size;
   item->paramValue = x;

   m_bulkGeneralParams.numGeneralParams++;
}


void BoneBusCharacter::EndSendGeneralParameters()
{
   if ( m_bulkGeneralParams.numGeneralParams <= 0 )
   {
      return;
   }

   int packetSize = BulkGeneralParamsHeaderSize + ( sizeof( BulkGeneralParam ) * m_bulkGeneralParams.numGeneralParams );

   int bytesSent;
   bytesSent = sendto( m_socketPimpl->m_sockUDP, (const char *)&m_bulkGeneralParams, packetSize, 0, (SOCKADDR*)&m_socketPimpl->m_toAddrUDP, sizeof( m_socketPimpl->m_toAddrUDP ) );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      m_numErrors++;
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


void BoneBusCharacter::StartSendBonePositions()
{
   m_bulkBonePositions.time             = m_time;
   m_bulkBonePositions.charId           = m_charId;
   m_bulkBonePositions.numBonePositions = 0;
}


void BoneBusCharacter::EndSendBonePositions()
{
   if ( m_bulkBonePositions.numBonePositions <= 0 )
   {
      return;
   }

   int packetSize = BulkBonePositionsHeaderSize + ( sizeof( BulkBonePosition ) * m_bulkBonePositions.numBonePositions );

   //printf( "Position Packet size: %d\n", packetSize );

   int bytesSent;
   bytesSent = sendto( m_socketPimpl->m_sockUDP, (const char *)&m_bulkBonePositions, packetSize, 0, (SOCKADDR*)&m_socketPimpl->m_toAddrUDP, sizeof( m_socketPimpl->m_toAddrUDP ) );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif


      printf( "socket error: %d\n", errnum );
      m_numErrors++;
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


void BoneBusCharacter::AddBonePosition( const char * boneName, const float x, const float y, const float z, const double time )
{
   int packetSize = BulkBonePositionsHeaderSize + ( sizeof( BulkBonePosition ) * m_bulkBonePositions.numBonePositions );

   if ( packetSize > BONEBUS_PACKET_SIZE )
   {
      //return;
      EndSendBonePositions();
      StartSendBonePositions();
      packetSize = BulkBonePositionsHeaderSize + ( sizeof( BulkBonePosition ) * m_bulkBonePositions.numBonePositions );
   }


   BoneNameMapIter iter = m_boneNameMap.find(  std::string(boneName) );

   bool boneIdSent = false;
   if ( iter == m_boneNameMap.end() )
   {
      // send the bone id information
      SetBoneId(boneName, m_nextBoneId);
      // add the bone information to the bone map
      BoneNameMapData data(m_nextBoneId);
      m_nextBoneId++;
      m_boneNameMap[std::string(boneName)] = data;
      iter = m_boneNameMap.find( boneName );
      boneIdSent = true;
   }

   BoneNameMapData & data = iter->second;


   bool cacheHit = false;

   // check to see if we've sent this data before
   // if we have, send it again to account for packet loss
   // and then don't send it again until the bone has moved
   if ( data.cachePosX == x &&
        data.cachePosY == y &&
        data.cachePosZ == z )
   {
      data.cachePosHits++;

      if ( data.cachePosHits > 3 )
      {
         cacheHit = true;
      }
   }
   else
   {
      data.cachePosX = x;
      data.cachePosY = y;
      data.cachePosZ = z;
      data.cachePosHits = 0;
      data.cachePosTime = time + GetCacheRefreshTime();
   }

   if ( cacheHit )
   {
      if ( time > data.cachePosTime )
      {
         data.cachePosTime = time + GetCacheRefreshTime();
         cacheHit = false;
      }
   }

   if ( cacheHit || boneIdSent)
   {
      return;
   }


   BulkBonePosition * item = &m_bulkBonePositions.bones[ m_bulkBonePositions.numBonePositions ];

   item->boneId = data.networkId;
   item->pos_x  = x;
   item->pos_y  = y;
   item->pos_z  = z;

   m_bulkBonePositions.numBonePositions++;
}


void BoneBusCharacter::SetParams( const char * paramName, const int paramNameId )
{
   char b[ 50 ];
   int  blen;
   sprintf( b, "SetParamNameMap|%d|%s|%d;", m_time, paramName, paramNameId );
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      m_numErrors++;
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


void BoneBusCharacter::SetViseme( const char * viseme, const float weight, const float blendTime )
{
   std::map<const std::string, int>::iterator iter = m_visemeNameMap.find( viseme );
   if (iter  == m_visemeNameMap.end() )
   {
      // send the viseme id information
      SetVisemeId(viseme, m_nextVisemeId);
      // add the viseme information to the viseme map
      m_visemeNameMap[std::string(viseme)] = m_nextVisemeId;
      m_nextVisemeId++;
      iter = m_visemeNameMap.find( viseme );
   }


   char b[ 512 ];
   int  blen;
   sprintf( b, "SetActorViseme|%d|%d|%f|%f;", m_charId, (*iter).second, weight, blendTime );
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      m_numErrors++;
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}

void BoneBusCharacter::SetBoneId( const char * name, const int id )
{
   char b[ 512 ];
   int  blen;
   sprintf( b, "SetBoneId|%d|%s|%d;",  m_charId, name, id );
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      m_numErrors++;
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}

void BoneBusCharacter::SetVisemeId( const char * visemeName, const int id )
{
   char b[ 512 ];
   int  blen;
   sprintf( b, "SetVisemeId|%d|%s|%d;",  m_charId, visemeName, id );
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      m_numErrors++;
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


void BoneBusCharacter::SetPosition( const float x, const float y, const float z, const double time )
{
   bool cacheHit = false;

   // check to see if we've sent this data before
   // if we have, send it again to account for packet loss
   // and then don't send it again until the bone has moved
   if ( m_cachePosX == x &&
        m_cachePosY == y &&
        m_cachePosZ == z )
   {
      m_cachePosHits++;

      if ( m_cachePosHits > 3 )
      {
         cacheHit = true;
      }
   }
   else
   {
      m_cachePosX = x;
      m_cachePosY = y;
      m_cachePosZ = z;
      m_cachePosHits = 0;
      m_cachePosTime = time + GetCacheRefreshTime();
   }

   if ( cacheHit )
   {
      if ( time > m_cachePosTime )
      {
         m_cachePosTime = time + GetCacheRefreshTime();
         cacheHit = false;
      }
   }

   if ( cacheHit )
   {
      return;
   }


   //printf( "Calling 'SetActorPos %d %f %f %f'\n", m_charId, x, y, z );

   char b[ 512 ];
   int  blen;
   sprintf( b, "SetActorPos|%d|%f|%f|%f;", m_charId, x, y, z );
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      m_numErrors++;
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


void BoneBusCharacter::SetRotation( const float w, const float x, const float y, const float z, const double time )
{
   bool cacheHit = false;

   // check to see if we've sent this data before
   // if we have, send it again to account for packet loss
   // and then don't send it again until the bone has moved
   if ( m_cacheQuatW == w &&
        m_cacheQuatX == x &&
        m_cacheQuatY == y &&
        m_cacheQuatZ == z )
   {
      m_cacheQuatHits++;

      if ( m_cacheQuatHits > 3 )
      {
         cacheHit = true;
      }
   }
   else
   {
      m_cacheQuatW = w;
      m_cacheQuatX = x;
      m_cacheQuatY = y;
      m_cacheQuatZ = z;
      m_cacheQuatHits = 0;
      m_cacheQuatTime = time + GetCacheRefreshTime();
   }

   if ( cacheHit )
   {
      if ( time > m_cacheQuatTime )
      {
         m_cacheQuatTime = time + GetCacheRefreshTime();
         cacheHit = false;
      }
   }

   if ( cacheHit )
   {
      return;
   }


   //printf( "Calling 'SetActorRot %d %f %f %f %f'\n", m_charId, w, x, y, z );

   char b[ 512 ];
   int  blen;
   sprintf( b, "SetActorRot|%d|%f|%f|%f|%f;", m_charId, w, x, y, z );
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      m_numErrors++;
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


int BoneBusCharacter::GetNumErrors()
{
   return m_numErrors;
}

struct bonebus::BoneBusClientSocketPimpl
{
   SOCKET m_sockUDP;
   sockaddr_in m_toAddrUDP;
   SOCKET m_sockTCP;
   sockaddr_in m_toAddrTCP;
};



BoneBusClient::BoneBusClient()
{
   m_socketPimpl = new BoneBusClientSocketPimpl();

   m_wsaStartupCalled = false;
#if defined(WIN_BUILD)
   m_socketPimpl->m_sockUDP = NULL;
   m_socketPimpl->m_sockTCP = NULL;
#else
   m_socketPimpl->m_sockUDP = -1;
   m_socketPimpl->m_sockTCP = -1;
#endif

}


BoneBusClient::~BoneBusClient()
{
   CloseConnection();

   delete m_socketPimpl;
   m_socketPimpl = NULL;
}


bool BoneBusClient::OpenConnection( const char * server )
{
   // TODO: cleanup udp if tcp error

   if ( strcmp( server, "" ) == 0 )
      return false;

#if defined(WIN_BUILD)
   WSADATA wsaData;
   int err = WSAStartup( MAKEWORD(2,2), &wsaData );
   if ( err != 0 )
   {
      printf( "WSAStartup failed. Code: %d\n", err );
      return false;
   }
#endif

   m_wsaStartupCalled = true;

   m_socketPimpl->m_sockUDP = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
   if ( m_socketPimpl->m_sockUDP == INVALID_SOCKET )
   {
      printf( "Couldn't create socket.\n" );
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
#if defined(WIN_BUILD)
      m_socketPimpl->m_sockUDP = NULL;
      WSACleanup();
#else
      m_socketPimpl->m_sockUDP = -1;
#endif
      m_wsaStartupCalled = false;
      return false;
   }


   // see if we're specifying a host by name or by number
   if ( isalpha( server[ 0 ] ) )
   {
      hostent * host = gethostbyname( server );
      if ( host == NULL )
      {
         printf( "gethostbyname() failed.\n" );
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
         printf( "socket error: %d\n", errnum );
         closesocket( m_socketPimpl->m_sockUDP );
#if defined(WIN_BUILD)
         m_socketPimpl->m_sockUDP = NULL;
         WSACleanup();
#else
         m_socketPimpl->m_sockUDP = -1;
#endif
         m_wsaStartupCalled = false;
         return false;
      }

      m_socketPimpl->m_toAddrUDP.sin_family = AF_INET;
      m_socketPimpl->m_toAddrUDP.sin_addr = *( (in_addr *)host->h_addr );
      m_socketPimpl->m_toAddrUDP.sin_port = htons( NETWORK_PORT_UDP );
   }
   else
   {
      m_socketPimpl->m_toAddrUDP.sin_family = AF_INET;
      m_socketPimpl->m_toAddrUDP.sin_addr.s_addr = inet_addr( server );
      m_socketPimpl->m_toAddrUDP.sin_port = htons( NETWORK_PORT_UDP );
   }


   m_socketPimpl->m_sockTCP = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
   if ( m_socketPimpl->m_sockTCP == INVALID_SOCKET )
   {
      printf( "Couldn't create socket2.\n" );
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
#if defined(WIN_BUILD)
      m_socketPimpl->m_sockTCP = NULL;
      WSACleanup();
#else
      m_socketPimpl->m_sockTCP = -1;
#endif
      m_wsaStartupCalled = false;
      return false;
   }


   // see if we're specifying a host by name or by number
   if ( isalpha( server[ 0 ] ) )
   {
      hostent * host = gethostbyname( server );
      if ( host == NULL )
      {
         printf( "gethostbyname() failed.\n" );
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
         printf( "socket error: %d\n", errnum );
         closesocket( m_socketPimpl->m_sockTCP );
#if defined(WIN_BUILD)
         m_socketPimpl->m_sockTCP = NULL;
	 WSACleanup();
#else
         m_socketPimpl->m_sockTCP = -1;
#endif
         m_wsaStartupCalled = false;
         return false;
      }

      m_socketPimpl->m_toAddrTCP.sin_family = AF_INET;
      m_socketPimpl->m_toAddrTCP.sin_addr = *( (in_addr *)host->h_addr );
      m_socketPimpl->m_toAddrTCP.sin_port = htons( NETWORK_PORT_TCP );
   }
   else
   {
      m_socketPimpl->m_toAddrTCP.sin_family = AF_INET;
      m_socketPimpl->m_toAddrTCP.sin_addr.s_addr = inet_addr( server );
      m_socketPimpl->m_toAddrTCP.sin_port = htons( NETWORK_PORT_TCP );
   }


   {
      int ret;
      ret = connect( m_socketPimpl->m_sockTCP, (SOCKADDR*)&m_socketPimpl->m_toAddrTCP, sizeof( m_socketPimpl->m_toAddrTCP ) );
      if ( ret < 0 )
      {
         printf( "connect() failed.\n" );
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
         printf( "socket error: %d\n", errnum );
         closesocket( m_socketPimpl->m_sockTCP );
#if defined(WIN_BUILD)
         m_socketPimpl->m_sockTCP = NULL;
	 WSACleanup();
#else
         m_socketPimpl->m_sockTCP = -1;
#endif
         m_wsaStartupCalled = false;
         return false;
      }
   }

   return true;
}


bool BoneBusClient::CloseConnection()
{
   if ( m_socketPimpl->m_sockUDP )
   {
      closesocket( m_socketPimpl->m_sockUDP );
#if defined(WIN_BUILD)
      m_socketPimpl->m_sockUDP = NULL;
#else
      m_socketPimpl->m_sockUDP = -1;
#endif
   }

   if ( m_socketPimpl->m_sockTCP )
   {
      closesocket( m_socketPimpl->m_sockTCP );
#if defined(WIN_BUILD)
      m_socketPimpl->m_sockTCP = NULL;
#else
      m_socketPimpl->m_sockTCP = -1;
#endif
   }


   if ( m_wsaStartupCalled )
   {
#if defined(WIN_BUILD)
      WSACleanup();
#endif
      m_wsaStartupCalled = false;
   }

   return true;
}


bool BoneBusClient::IsOpen()
{
#if defined(WIN_BUILD)
   return ( m_socketPimpl->m_sockUDP != NULL ) && ( m_socketPimpl->m_sockTCP != NULL );
#else
   return ( m_socketPimpl->m_sockUDP != -1) && ( m_socketPimpl->m_sockTCP != -1);

#endif
}


bool BoneBusClient::Update()
{
   return true;
}


BoneBusCharacter * BoneBusClient::CreateCharacter( const char * charName, const char * objectClass, const bool useFaceBones )
{
   if ( !IsOpen() )
   {
      return NULL;
   }

   // TODO:  This creates a unique_id based on the hash of charName + objectClass
   //        If that hash is in use, iterate until we come to a hash that isn't used.
   //        This will suffice until the bonebus is rewritten.

   if ( m_characterNameMap.find( charName ) == m_characterNameMap.end() )
   {
      string hashName = (string)charName + " " + (string)objectClass;
      int hash = abs( (int)HashDJB2( hashName.c_str() ) );

      while ( FindID( m_characterNameMap, hash ) == true )
      {
         hash++;
      }

      m_characterNameMap[ charName ] = hash;
   }

   BoneBusCharacter * character = new BoneBusCharacter( useFaceBones );

   character->m_name   = charName;
   character->m_objectClass = objectClass;
   character->m_charId = m_characterNameMap[ charName ];
   character->m_socketPimpl->m_sockUDP = m_socketPimpl->m_sockUDP;
   character->m_socketPimpl->m_toAddrUDP = m_socketPimpl->m_toAddrUDP;
   character->m_socketPimpl->m_sockTCP = m_socketPimpl->m_sockTCP;

   //NetworkMapBoneNames( character->m_boneNameMap, useFaceBones );
   //
   //if ( !useFaceBones )
   //   NetworkMapVisemeNames( character->m_visemeNameMap );

   character->m_bulkBoneRotations.packetId = 0x10;
   character->m_bulkBoneRotations.charId   = character->m_charId;
   character->m_bulkBoneRotations.numBoneRotations = 0;
   memset( character->m_bulkBoneRotations.bones, 0, sizeof(character->m_bulkBoneRotations.bones) );

   character->m_bulkBonePositions.packetId = 0x11;
   character->m_bulkBonePositions.charId   = character->m_charId;
   character->m_bulkBonePositions.numBonePositions = 0;
   memset( character->m_bulkBonePositions.bones, 0, sizeof(character->m_bulkBonePositions.bones) );

   character->m_bulkGeneralParams.packetId=0x12;
   character->m_bulkGeneralParams.charId = character->m_charId;
   character->m_bulkGeneralParams.numGeneralParams=0;
   memset( character->m_bulkGeneralParams.params, 0, sizeof(character->m_bulkGeneralParams.params) );

   character->m_cachePosX = 0;
   character->m_cachePosY = 0;
   character->m_cachePosZ = 0;
   character->m_cachePosHits = 0;
   character->m_cacheQuatW = 1;
   character->m_cacheQuatX = 0;
   character->m_cacheQuatY = 0;
   character->m_cacheQuatZ = 0;
   character->m_cacheQuatHits = 0;

#if defined(WIN_BUILD)
   if ( m_socketPimpl->m_sockTCP != NULL )
#else
   if ( m_socketPimpl->m_sockTCP != -1)
#endif
   {
      printf( "Calling 'CreateActor %d %s %s %d'\n", character->m_charId, objectClass, charName, 1 );

      char b[ 512 ];
      int  blen;
      sprintf( b, "CreateActor|%d|%s|%s|%d;", character->m_charId, objectClass, charName, 1 );
      blen = (int)strlen( b );

      int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
      if ( bytesSent < 0 )
      {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
         printf( "socket error: %d\n", errnum );
         //fprintf( fp, "socket error: %d\n", errnum );
      }
      if ( bytesSent > 0 )
      {
         //printf( "send: %ld\n", bytesSent );
         //fprintf( fp, "send: %ld\n", bytesSent );
      }
   }


   m_characterDataMap[ character->m_charId ] = character;

   return character;
}


void BoneBusClient::UpdateAllCharacters()
{
   if (!IsOpen())
      return;

   for (CharacterNameMap::iterator iter = m_characterNameMap.begin();
       iter != m_characterNameMap.end();
       iter++)
   {
      std::string charName = (*iter).first;
      UpdateCharacter(charName.c_str());
   }
}


void BoneBusClient::UpdateCharacter( const char * charName )
{
   if ( IsOpen() )
   {
      CharacterNameMap::iterator iter = m_characterNameMap.find(charName);
      if (iter == m_characterNameMap.end())
         return;

      int charId = (*iter).second;
      CharacterDataMap::iterator iter2 = m_characterDataMap.find(charId);
      if (iter2 == m_characterDataMap.end())
         return;

      BoneBusCharacter * character = (*iter2).second;

      char b[ 512 ];
      int  blen;
      sprintf( b, "UpdateActor|%d|%s|%s|%d;", character->m_charId, character->m_objectClass.c_str(), charName, 1 );
      blen = (int)strlen( b );

      int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
      if ( bytesSent < 0 )
      {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
         printf( "socket error: %d\n", errnum );
         //fprintf( fp, "socket error: %d\n", errnum );
      }
      if ( bytesSent > 0 )
      {
         //printf( "send: %ld\n", bytesSent );
         //fprintf( fp, "send: %ld\n", bytesSent );
      }
   }
}


bool BoneBusClient::DeleteCharacter( BoneBusCharacter * character )
{
   if ( IsOpen() )
   {
      printf( "Calling 'DeleteActor %d'\n", character->m_charId );

      char b[ 512 ];
      int  blen;
      sprintf( b, "DeleteActor|%d;", character->m_charId );
      blen = (int)strlen( b );

      int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
      if ( bytesSent < 0 )
      {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
         printf( "socket error: %d\n", errnum );
         //fprintf( fp, "socket error: %d\n", errnum );
      }
      if ( bytesSent > 0 )
      {
         //printf( "send: %ld\n", bytesSent );
         //fprintf( fp, "send: %ld\n", bytesSent );
      }
   }


   m_characterDataMap.erase( character->m_charId );
   m_characterNameMap.erase( character->m_name );

   delete character;
   character = NULL;

   return true;
}


BoneBusCharacter * BoneBusClient::FindCharacter( const int charID )
{
   return m_characterDataMap[ charID ];
}


BoneBusCharacter * BoneBusClient::FindCharacterByName( const char * name )
{
   CharacterDataMap::const_iterator it;
   for ( it = m_characterDataMap.begin(); it != m_characterDataMap.end(); it++ )
   {
      if ( (*it).second->m_name == (string)name )
      {
         return (*it).second;
      }
   }

   return NULL;
}


int BoneBusClient::GetNumCharacters( )
{
   return m_characterDataMap.size();
}


void BoneBusClient::SetCameraPosition( const float x, const float y, const float z )
{
   if ( !IsOpen() )
   {
      return;
   }

   char b[ 512 ];
   int  blen;
   sprintf( b, "SetActorPos|%d|%f|%f|%f;", -1, x, y, z );
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


void BoneBusClient::SetCameraRotation( const float w, const float x, const float y, const float z )
{
   if ( !IsOpen() )
   {
      return;
   }

   char b[ 512 ];
   int  blen;
   sprintf( b, "SetActorRot|%d|%f|%f|%f|%f;", -1, w, x, y, z );
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


void BoneBusClient::ExecScript( const char * command )
{
   if ( !IsOpen() )
   {
      return;
   }

   ExecScriptData data;
   data.packetID = 0x04;
   strcpy( data.command, command );  // watch buffer length!

   int bytesSent;
   bytesSent = sendto( m_socketPimpl->m_sockUDP, (const char *)&data, sizeof( data ), 0, (SOCKADDR*)&m_socketPimpl->m_toAddrUDP, sizeof( m_socketPimpl->m_toAddrUDP ) );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


void BoneBusClient::SendPlaySound( const char * soundFile, const char * charName )
{
   if ( !IsOpen() )
   {
      return;
   }

   char b[512];
   int  blen;

   //printf( "Calling 'PlaySoundDynamic %s %d'\n", soundFile, charId );

   sprintf( b, "PlaySoundDynamic|%s|%s;", soundFile, charName );
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


void BoneBusClient::SendStopSound( const char * soundFile )
{
   if ( !IsOpen() )
   {
      return;
   }

   char b[512];
   int  blen;

   //printf( "Calling 'StopSoundDynamic %s'\n", filename );

   sprintf( b, "StopSoundDynamic|%s;", soundFile);
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      //fprintf( fp, "socket error: %d\n", errnum );
   }
   if ( bytesSent > 0 )
   {
      //printf( "send: %ld\n", bytesSent );
      //fprintf( fp, "send: %ld\n", bytesSent );
   }
}


// [BMLR] Get command from renderer and external programs
vector<string> BoneBusClient::GetCommand()
{
   // EDF - re-do to make this more generic.  support message types, etc

   string result = "";
#if defined(WIN_BUILD)
   if ( m_socketPimpl->m_sockTCP != NULL )
#else
   if ( m_socketPimpl->m_sockTCP != -1)
#endif
   {
      fd_set SocketSet;
      FD_ZERO( &SocketSet );
      FD_SET( m_socketPimpl->m_sockTCP, &SocketSet );
#if defined(WIN_BUILD)
      TIMEVAL SelectTime = { 0, 0 };
#else
      timeval SelectTime;
#endif
      int error = select( m_socketPimpl->m_sockTCP + 1, &SocketSet, 0, 0, &SelectTime );
      if ( error > 0 )
      {
         char b[ 1024 ]; // Todo: use a while loop instead
         int bytesRecv = recv( m_socketPimpl->m_sockTCP, b, sizeof(b), 0 );
         if ( bytesRecv > 0 )
         {
            result.append( b );
            result = result.substr( 0, bytesRecv );
         }
      }
   }

   vector<string> results;

   if ( result.length() > 0 )
   {
      vhcl::Tokenize( result, results, "\r\r\r" ); // we use \r\r\r as a separator between commands :)
   }

   return results;
}


#pragma warning( push )
#pragma warning( disable : 4706 )
unsigned int BoneBusClient::HashDJB2( const char * str )
{
   unsigned int hash = 5381;
   int c;

   while ( ( c = *str++ ) )
      hash = ( ( hash << 5 ) + hash ) + c;   // hash * 33 + c

   return hash;
}
#pragma warning( pop )


bool BoneBusClient::FindID( const CharacterNameMap & map, const int id )
{
   for ( CharacterNameMap::const_iterator it = map.begin(); it != map.end(); it++ )
   {
      if ( it->second == id )
      {
         return true;
      }
   }

   return false;
}


// [BMLR] Send text to speak to the renderer
void BoneBusClient::SendSpeakText( const int msgNumber, const char * agent, const char * text )
{
   if ( !IsOpen() )
   {
      return;
   }

   char b[ 512 ];
   int  blen;

   sprintf( b, "SpeakText|%d|%s|%s;", msgNumber, agent, text );
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
   }
}


// [BMLR] Sends create pawn requests to renderer
void BoneBusClient::SendCreatePawn( const char * name, const double locx, const double locy, const double locz )
{
   if ( !IsOpen() )
   {
      return;
   }

   char b[ 1024 ];
   int  blen;

   sprintf( b, "CreatePawn|%s|%f|%f|%f;", name, locx, locy, locz );
   blen = (int)strlen( b );

   int bytesSent = send( m_socketPimpl->m_sockTCP, (const char *)b, blen, 0 );
   if ( bytesSent < 0 )
   {
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
   }
}



struct bonebus::BoneBusServerSocketPimpl
{
   SOCKET m_sockUDP;
   sockaddr_in m_addrUDP;
   SOCKET m_sockTCP;
   sockaddr_in m_addrTCP;
   std::vector< SOCKET > m_sockConnectionsTCP;
};


BoneBusServer::BoneBusServer()
{
   m_socketPimpl = new BoneBusServerSocketPimpl();

#if defined(WIN_BUILD)
   m_socketPimpl->m_sockUDP = NULL;
   m_socketPimpl->m_sockTCP = NULL;
#else
   m_socketPimpl->m_sockUDP = -1;
   m_socketPimpl->m_sockTCP = -1;
#endif

   m_wsaStartupCalled = false;
   m_onClientConnectFunc = NULL;
   m_onClientConnectUserData = NULL;
   m_onClientDisconnectFunc = NULL;
   m_onClientDisconnectUserData = NULL;
   m_onCreateCharacterFunc = NULL;
   m_onCreateCharacterUserData = NULL;
   m_onDeleteCharacterFunc = NULL;
   m_onDeleteCharacterUserData = NULL;
   m_onUpdateCharacterFunc = NULL;
   m_onUpdateCharacterUserData = NULL;
   m_onSetCharacterPositionFunc = NULL;
   m_onSetCharacterPositionUserData = NULL;
   m_onSetCharacterRotationFunc = NULL;
   m_onSetCharacterRotationUserData = NULL;
   m_onBoneRotationsFunc = NULL;
   m_onBoneRotationsUserData = NULL;
   m_onBonePositionsFunc = NULL;
   m_onBonePositionsUserData = NULL;
   m_onSetCharacterVisemeFunc = NULL;
   m_onSetCharacterVisemeUserData = NULL;
   m_onPlaySoundFunc = NULL;
   m_onPlaySoundUserData = NULL;
   m_onStopSoundFunc = NULL;
   m_onStopSoundUserData = NULL;
   m_onExecScriptFunc = NULL;
   m_onExecScriptUserData = NULL;
   m_onGeneralParamFunc = NULL;
   m_onGeneralParamUserData = NULL;
}


BoneBusServer::~BoneBusServer()
{
   CloseConnection();

   delete m_socketPimpl;
   m_socketPimpl = NULL;
}


bool BoneBusServer::OpenConnection()
{
   // TODO: cleanup udp if tcp error

#if defined(WIN_BUILD)
   WSADATA wsaData;
   int err = WSAStartup( MAKEWORD(2,2), &wsaData );
   if ( err != 0 )
   {
      printf( "WSAStartup failed. Code: %d\n", err );
      return false;
   }
#endif

   m_wsaStartupCalled = true;


   m_socketPimpl->m_sockUDP = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
   if ( m_socketPimpl->m_sockUDP == INVALID_SOCKET )
   {
      printf( "Couldn't create socket.\n" );
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
#if defined(WIN_BUILD)
      m_socketPimpl->m_sockUDP = NULL;
      WSACleanup();
#else
      m_socketPimpl->m_sockUDP = -1;
#endif
      m_wsaStartupCalled = false;
      return false;
   }


   // Bind
   m_socketPimpl->m_addrUDP.sin_addr.s_addr = INADDR_ANY;
   m_socketPimpl->m_addrUDP.sin_family = AF_INET;
   m_socketPimpl->m_addrUDP.sin_port = htons( NETWORK_PORT_UDP );
   memset( m_socketPimpl->m_addrUDP.sin_zero, 0, sizeof( m_socketPimpl->m_addrUDP.sin_zero ) );

   if ( bind( m_socketPimpl->m_sockUDP, (const sockaddr *)&m_socketPimpl->m_addrUDP, sizeof( m_socketPimpl->m_addrUDP ) ) == SOCKET_ERROR )
   {
      printf( "bind() failed.\n" );
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      closesocket( m_socketPimpl->m_sockUDP );
#if defined(WIN_BUILD)
      m_socketPimpl->m_sockUDP = NULL;
      WSACleanup();
#else
      m_socketPimpl->m_sockUDP = -1;
#endif
      m_wsaStartupCalled = false;
      return false;
   }


   u_long nonBlocking = 1;
#if defined(WIN_BUILD)
   ioctlsocket( m_socketPimpl->m_sockUDP, FIONBIO, &nonBlocking );
#else
   ioctl( m_socketPimpl->m_sockUDP, FIONBIO, &nonBlocking );
#endif



   m_socketPimpl->m_sockTCP = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
   if ( m_socketPimpl->m_sockTCP == INVALID_SOCKET )
   {
      printf( "Couldn't create socket tcp.\n" );
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
#if defined(WIN_BUILD)
      m_socketPimpl->m_sockTCP = NULL;
      WSACleanup();
#else
      m_socketPimpl->m_sockTCP = -1;
#endif
      m_wsaStartupCalled = false;
      return false;
   }


   int reuseAddr = 1;
   setsockopt( m_socketPimpl->m_sockTCP, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseAddr, sizeof( int ) );


   m_socketPimpl->m_addrTCP.sin_family      = AF_INET;
   m_socketPimpl->m_addrTCP.sin_addr.s_addr = INADDR_ANY;
   m_socketPimpl->m_addrTCP.sin_port        = htons( NETWORK_PORT_TCP );
   memset( m_socketPimpl->m_addrTCP.sin_zero, 0, sizeof( m_socketPimpl->m_addrTCP.sin_zero ) );

   if ( bind( m_socketPimpl->m_sockTCP, (sockaddr *)&m_socketPimpl->m_addrTCP, sizeof( m_socketPimpl->m_addrTCP ) ) == SOCKET_ERROR )
   {
      printf( "bind() failed.\n" );
#if defined(WIN_BUILD)
      int errnum = WSAGetLastError();
#else
      int errnum = errno;
#endif
      printf( "socket error: %d\n", errnum );
      closesocket( m_socketPimpl->m_sockTCP );
#if defined(WIN_BUILD)
      m_socketPimpl->m_sockTCP = NULL;
      WSACleanup();
#else
      m_socketPimpl->m_sockTCP = -1;
#endif
      m_wsaStartupCalled = false;
      return false;
   }


   {
   u_long nonBlocking = 1;
#if defined(WIN_BUILD)
   ioctlsocket( m_socketPimpl->m_sockTCP, FIONBIO, &nonBlocking );
#else
   ioctl( m_socketPimpl->m_sockTCP, FIONBIO, &nonBlocking );
#endif
   }


   listen( m_socketPimpl->m_sockTCP, 10 );


   return true;
}


bool BoneBusServer::CloseConnection()
{
   if ( m_socketPimpl->m_sockUDP )
   {
      closesocket( m_socketPimpl->m_sockUDP );
#if defined(WIN_BUILD)
      m_socketPimpl->m_sockUDP = NULL;
#else
      m_socketPimpl->m_sockUDP = -1;
#endif
   }

   if ( m_socketPimpl->m_sockTCP )
   {
      closesocket( m_socketPimpl->m_sockTCP );
#if defined(WIN_BUILD)
      m_socketPimpl->m_sockTCP = NULL;
#else
      m_socketPimpl->m_sockTCP = -1;
#endif
   }


   if ( m_wsaStartupCalled )
   {
#if defined(WIN_BUILD)
      WSACleanup();
#endif
      m_wsaStartupCalled = false;
   }

   return true;
}


bool BoneBusServer::IsOpen()
{
#if defined(WIN_BUILD)
   return ( m_socketPimpl->m_sockUDP != NULL ) && ( m_socketPimpl->m_sockTCP != NULL );
#else
   return ( m_socketPimpl->m_sockUDP != -1) && ( m_socketPimpl->m_sockTCP != -1);
#endif
}


bool BoneBusServer::Update()
{
   if ( !IsOpen() )
   {
      return false;
   }

   {
      fd_set readfds;
      FD_ZERO( &readfds );
      FD_SET( m_socketPimpl->m_sockTCP, &readfds );
      timeval timeout = { 0, 0 };  // return immediately
      int error = select( 0, &readfds, 0, 0, &timeout );   // 1st parameter ignored by winsock
      if ( error == SOCKET_ERROR )
      {
         printf( "TCP - Error checking status\n" );
      }
      else if ( error != 0 )
      {
         int newSock;
         sockaddr_in newToAddr;

#if defined(WIN_BUILD)
         int i = sizeof( sockaddr_in );
#else
         socklen_t i = sizeof( sockaddr_in );
#endif
         newSock = (int)accept( m_socketPimpl->m_sockTCP, (sockaddr *)&newToAddr, &i );

         u_long nonBlocking = 1;
#if defined(WIN_BUILD)
         ioctlsocket( newSock, FIONBIO, &nonBlocking );
#else
         ioctl( newSock, FIONBIO, &nonBlocking );
#endif

         m_socketPimpl->m_sockConnectionsTCP.push_back( newSock );

         //printf( "New Connection!\n" );

         if ( m_onClientConnectFunc )
         {
            string clientIP = inet_ntoa( newToAddr.sin_addr );
            m_onClientConnectFunc( clientIP, m_onClientConnectUserData );
         }
      }
   }


   for ( int i = 0; i < (int)m_socketPimpl->m_sockConnectionsTCP.size(); i++ )
   {
      int tcpDataPending;

      SOCKET s = m_socketPimpl->m_sockConnectionsTCP[ i ];

      fd_set readfds;
      FD_ZERO( &readfds );
      FD_SET( s, &readfds );
      timeval timeout = { 0, 0 };  // return immediately
      int error = select( 0, &readfds, 0, 0, &timeout );   // 1st parameter ignored by winsock
      if ( error == SOCKET_ERROR )
      {
         //printf( "TCP - Error checking status\n" );
         tcpDataPending = 0;
      }
      else if ( error == 0 )
      {
         tcpDataPending = 0;
      }
      else
      {
         tcpDataPending = 1;
      }


      std::string overflowData = "";
      while ( tcpDataPending )
      {
         tcpDataPending = 0;

         char str[ 1000 ];
         memset( str, 0, sizeof( char ) * 1000 );

         int bytesReceived = recv( (SOCKET)s, str, sizeof( str ) - 1, 0 );
         if ( bytesReceived > 0 )
         {
            str[ bytesReceived ] = 0;

            string recvStr = overflowData + str;

            //OutputDebugString( string( recvStr + "\n" ).c_str() );


            int lastFullCommandIndex = -1;
            if (recvStr.size() > 0 && recvStr[recvStr.size() - 1] != ';')
            {
               // retain all the characters up to the last semicolon
               for (int x = recvStr.size() - 1; x >= 0; x--)
               {
                  if (recvStr[x] == ';')
                  {
                     lastFullCommandIndex = x;
                     break;
                  }

                  if (x == 0)
                     lastFullCommandIndex = -1;
               }

               if (lastFullCommandIndex >= 0)
               {
                  overflowData = recvStr.substr(lastFullCommandIndex + 1);
                  recvStr = recvStr.substr(0, lastFullCommandIndex + 1);
                  tcpDataPending = 1;
               }
            }

            vector< string > tokens;
            vhcl::Tokenize( recvStr, tokens, ";" );

            for ( int t = 0; t < (int)tokens.size(); t++ )
            {
               vector< string > msgTokens;
               vhcl::Tokenize( tokens[ t ], msgTokens, "|" );

               if ( msgTokens.size() > 0 )
               {
                  if ( msgTokens[ 0 ] == "CreateActor" )
                  {
                     if ( msgTokens.size() > 4 )
                     {
                        string charIdStr = msgTokens[ 1 ];
                        int charId       = atoi( charIdStr.c_str() );
                        string uClass    = msgTokens[ 2 ];
                        string name      = msgTokens[ 3 ];
                        int skeletonType = atoi( msgTokens[ 4 ].c_str() );

                        if ( m_onCreateCharacterFunc )
                        {
                           m_onCreateCharacterFunc( charId, uClass, name, skeletonType, m_onClientConnectUserData );
                        }
                     }
                  }
                  else if ( msgTokens[ 0 ] == "DeleteActor" )
                  {
                     if ( msgTokens.size() > 1 )
                     {
                        string charIdStr = msgTokens[ 1 ];
                        int charId       = atoi( charIdStr.c_str() );

                        if ( m_onDeleteCharacterFunc )
                        {
                           m_onDeleteCharacterFunc( charId, m_onDeleteCharacterUserData );
                        }
                     }
                  }
                  else if ( msgTokens[ 0 ] == "SetParamNameMap" )
                  {
                     if ( msgTokens.size() ==3 )
                     {
                        string charIdStr = msgTokens[ 1 ];
                        const char * paramName = charIdStr.c_str();
                        int paramNameId = atoi ( msgTokens[ 2 ].c_str() );
                        if ( m_onSetCharacterParamFunc )
                        {
                           m_onSetCharacterParamFunc( paramName, paramNameId, m_onSetCharacterParamUserData );
                        }
                     }
                  }
                  else if ( msgTokens[ 0 ] == "SetActorPos" )
                  {
                     if ( msgTokens.size() > 4 )
                     {
                        string charIdStr = msgTokens[ 1 ];
                        int charId       = atoi( charIdStr.c_str() );
                        float x          = (float)atof( msgTokens[ 2 ].c_str() );
                        float y          = (float)atof( msgTokens[ 3 ].c_str() );
                        float z          = (float)atof( msgTokens[ 4 ].c_str() );

                        if ( m_onSetCharacterPositionFunc )
                        {
                           m_onSetCharacterPositionFunc( charId, x, y, z, m_onSetCharacterPositionUserData );
                        }
                     }
                  }
                  else if ( msgTokens[ 0 ] == "SetActorRot" )
                  {
                     if ( msgTokens.size() > 5 )
                     {
                        string charIdStr = msgTokens[ 1 ];
                        int charId       = atoi( charIdStr.c_str() );
                        float w          = (float)atof( msgTokens[ 2 ].c_str() );
                        float x          = (float)atof( msgTokens[ 3 ].c_str() );
                        float y          = (float)atof( msgTokens[ 4 ].c_str() );
                        float z          = (float)atof( msgTokens[ 5 ].c_str() );

                        if ( m_onSetCharacterRotationFunc )
                        {
                           m_onSetCharacterRotationFunc( charId, w, x, y, z, m_onSetCharacterRotationUserData );
                        }
                     }
                  }
                  else if ( msgTokens[ 0 ] == "SetActorViseme" )
                  {
                     if ( msgTokens.size() > 4 )
                     {
                        string charIdStr = msgTokens[ 1 ];
                        int charId       = atoi( charIdStr.c_str() );
                        string visemeStr = msgTokens[ 2 ];
                        int visemeId     = atoi( visemeStr.c_str() );
                        float weight     = (float)atof( msgTokens[ 3 ].c_str() );
                        float blendTime  = (float)atof( msgTokens[ 4 ].c_str() );

                        if ( m_onSetCharacterVisemeFunc )
                        {
                           m_onSetCharacterVisemeFunc( charId, visemeId, weight, blendTime, m_onSetCharacterVisemeUserData );
                        }
                     }
                  }
                  else if ( msgTokens[ 0 ] == "SetBoneId" )
                  {
                     if ( msgTokens.size() > 3 )
                     {
                        string charIdStr = msgTokens[ 1 ];
                        int charId       = atoi( charIdStr.c_str() );
                        string boneName = msgTokens[ 2 ];
                        int boneId       = atoi( msgTokens[ 3 ].c_str() );

                        if ( m_onSetBoneIdFunc )
                        {
                           m_onSetBoneIdFunc( charId, boneName, boneId, m_onSetBoneIdUserData );
                        }
                     }
                  }
                  else if ( msgTokens[ 0 ] == "SetVisemeId" )
                  {
                     if ( msgTokens.size() > 3 )
                     {
                        string charIdStr = msgTokens[ 1 ];
                        int charId       = atoi( charIdStr.c_str() );
                        string visemeName = msgTokens[ 2 ];
                        int visemeId       = atoi( msgTokens[ 3 ].c_str() );

                        if ( m_onSetVisemeIdFunc )
                        {
                           m_onSetVisemeIdFunc( charId, visemeName, visemeId, m_onSetVisemeIdUserData );
                        }
                     }
                  }
                  else if ( msgTokens[ 0 ] == "PlaySoundDynamic" )
                  {
                     if ( msgTokens.size() > 2 )
                     {
                        string soundFile = msgTokens[ 1 ];
                        string charName  = msgTokens[ 2 ];

                        if ( m_onPlaySoundFunc )
                        {
                           m_onPlaySoundFunc( soundFile, charName, m_onPlaySoundUserData );
                        }
                     }
                  }
                  else if ( msgTokens[ 0 ] == "StopSoundDynamic" )
                  {
                     if ( msgTokens.size() > 1 )
                     {
                        string soundFile = msgTokens[ 1 ];

                        if ( m_onStopSoundFunc )
                        {
                           m_onStopSoundFunc( soundFile, m_onStopSoundUserData );
                        }
                     }
                  }
                  else if ( msgTokens[ 0 ] == "UpdateActor" )
                  {
                     if ( msgTokens.size() > 4 )
                     {
                        string charIdStr = msgTokens[ 1 ];
                        int charId       = atoi( charIdStr.c_str() );
                        string uClass    = msgTokens[ 2 ];
                        string name      = msgTokens[ 3 ];
                        int skeletonType = atoi( msgTokens[ 4 ].c_str() );

                        if ( m_onUpdateCharacterFunc )
                        {
                           m_onUpdateCharacterFunc( charId, uClass, name, skeletonType, m_onClientConnectUserData );
                        }
                     }
                  }
               }
            }
         }
      }
   }


   int udpDataPending;

   fd_set readfds;
   FD_ZERO( &readfds );
   FD_SET( m_socketPimpl->m_sockUDP, &readfds );
   timeval timeout = { 0, 0 };  // return immediately
   int error = select( 0, &readfds, 0, 0, &timeout );   // 1st parameter ignored by winsock
   if ( error == SOCKET_ERROR )
   {
      //printf( "TCP - Error checking status\n" );
      udpDataPending = 0;
   }
   else if ( error == 0 )
   {
      udpDataPending = 0;
   }
   else
   {
      udpDataPending = 1;
   }


   int dataReceived = 0;

   char buffer[ BONEBUS_PACKET_SIZE * 2 ] = {0};
   int bytesReceived = 1;

   while ( bytesReceived > 0 )
   {
      bytesReceived = 0;
      dataReceived = 0;

      if ( udpDataPending )
      {
         sockaddr_in fromAddr;
#if defined(WIN_BUILD)
         int fromAddrSize = sizeof(sockaddr_in);
#else
         socklen_t fromAddrSize = sizeof(sockaddr_in);
#endif

         bytesReceived = recvfrom( m_socketPimpl->m_sockUDP, buffer, ( BONEBUS_PACKET_SIZE * 2 - 1 ) * sizeof(char), 0, (sockaddr *)&fromAddr, &fromAddrSize );
         if ( bytesReceived < 0 )
         {
            //int error = WSAGetLastError();
            //printf( "%d\n", error );
            //Warning( "Could not receive datagram\n" );
            dataReceived = 0;
         }
         else
         {
            dataReceived = 1;
         }
      }


      if ( dataReceived && ( bytesReceived > 3 ) )
      {
         int opcode = ((int *)buffer)[ 0 ];

         if ( opcode == 0x10 )
         {
            //printf( "BULK_BONE_DATA\n" );

            BulkBoneRotations * bulkBoneRotations;

            bulkBoneRotations = (BulkBoneRotations *)buffer;

            if ( m_onBoneRotationsFunc )
            {
               m_onBoneRotationsFunc( bulkBoneRotations, m_onBoneRotationsUserData );
            }
         }
         else if ( opcode == 0x11 )
         {
            //printf( "BULK_POSITION_DATA\n" );

            BulkBonePositions * bulkBonePositions;

            bulkBonePositions = (BulkBonePositions *)buffer;

            if ( m_onBonePositionsFunc )
            {
               m_onBonePositionsFunc( bulkBonePositions, m_onBonePositionsUserData );
            }
         }
         else if ( opcode == 0x04 )
         {
            //printf( "ExecScriptData\n" );

            ExecScriptData * execScriptData;

            execScriptData = (ExecScriptData *)buffer;

            if ( m_onExecScriptFunc )
            {
               m_onExecScriptFunc( execScriptData->command, m_onExecScriptUserData );
            }
         }
         else if ( opcode == 0x12)
         {
            BulkGeneralParams * bulkGeneralParams;
            bulkGeneralParams = (BulkGeneralParams *)buffer;
            if ( m_onGeneralParamFunc )
            {
               m_onGeneralParamFunc( bulkGeneralParams, m_onGeneralParamUserData );
            }
         }
         else
         {
            // opcode not recognized
         }
      }
   }

   return true;
}


#else  // WIN_BUILD

#include "bonebus.h"

#include <string>

using std::string;
using std::vector;
using namespace bonebus;

// TODO: linux

BoneBusCharacter::BoneBusCharacter( const bool useFaceBones ) {}
BoneBusCharacter::~BoneBusCharacter() {}
void BoneBusCharacter::StartSendBoneRotations() {}
void BoneBusCharacter::EndSendBoneRotations() {}
void BoneBusCharacter::AddBoneRotation( const char * boneName, const float w, const float x, const float y, const float z, const double time ) {}
void BoneBusCharacter::StartSendGeneralParameters() {}
void BoneBusCharacter::AddGeneralParameters( const int paramNameId, const int size, const float x, const int id, const double time ) {}
void BoneBusCharacter::EndSendGeneralParameters() {}
void BoneBusCharacter::StartSendBonePositions() {}
void BoneBusCharacter::EndSendBonePositions() {}
void BoneBusCharacter::AddBonePosition( const char * boneName, const float x, const float y, const float z, const double time ) {}
void BoneBusCharacter::SetParams( const char * paramName, const int paramNameId ) {}
void BoneBusCharacter::SetViseme( const char * viseme, const float weight, const float blendTime ) {}
void BoneBusCharacter::SetPosition( const float x, const float y, const float z, const double time ) {}
void BoneBusCharacter::SetRotation( const float w, const float x, const float y, const float z, const double time ) {}
int BoneBusCharacter::GetNumErrors() { return 0; }
void BoneBusCharacter::IncrementTime () {}

BoneBusClient::BoneBusClient() {}
BoneBusClient::~BoneBusClient() {}
bool BoneBusClient::OpenConnection( const char * server ) { return true; }
bool BoneBusClient::CloseConnection() { return true; }
bool BoneBusClient::IsOpen() { return true; }
bool BoneBusClient::Update() { return true; }
int BoneBusClient::GetNumCharacters() { return 0; }
BoneBusCharacter * BoneBusClient::CreateCharacter( const char * charName, const char * objectClass, const bool useFaceBones ) { return NULL; }
void BoneBusClient::UpdateAllCharacters() {};
bool BoneBusClient::DeleteCharacter( BoneBusCharacter * character ) { return true; }
BoneBusCharacter * BoneBusClient::FindCharacter( const int charID ) { return NULL; }
BoneBusCharacter * BoneBusClient::FindCharacterByName( const char * name ) { return NULL; }
void BoneBusClient::SetCameraPosition( const float x, const float y, const float z ) {}
void BoneBusClient::SetCameraRotation( const float w, const float x, const float y, const float z ) {}
void BoneBusClient::ExecScript( const char * command ) {}
void BoneBusClient::SendPlaySound( const char * soundFile, const char * charName ) {}
void BoneBusClient::SendStopSound( const char * soundFile ) {}
vector<string> BoneBusClient::GetCommand()
{
   vector<string> results;
   return results;
}

unsigned int BoneBusClient::HashDJB2( const char * str )
{
   unsigned int hash = 5381;
   int c;

   while ( c = *str++ )
      hash = ( ( hash << 5 ) + hash ) + c;   // hash * 33 + c

   return hash;
}


bool BoneBusClient::FindID( const CharacterNameMap & map, const int id )
{
   for ( CharacterNameMap::const_iterator it = map.begin(); it != map.end(); it++ )
   {
      if ( it->second == id )
      {
         return true;
      }
   }

   return false;
}


void BoneBusClient::SendSpeakText( const int msgNumber, const char * agent, const char * text ) {}
void BoneBusClient::SendCreatePawn( const char * name, const double locx, const double locy, const double locz ) {}


BoneBusServer::BoneBusServer() {}
BoneBusServer::~BoneBusServer() {}
bool BoneBusServer::OpenConnection() { return true; }
bool BoneBusServer::CloseConnection() { return true; }
bool BoneBusServer::IsOpen() { return true; }
bool BoneBusServer::Update() { return true; }


#endif  // WIN_BUILD

