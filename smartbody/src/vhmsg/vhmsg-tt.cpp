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

#include "vhmsg-tt.h"

#include <string.h>

#ifdef WIN_BUILD
#include <windows.h>
#endif

#include "vhmsg.h"


// choose to hide/show debug messages
#ifdef DEBUG_BUILD
//#define Log  LOG
#define Log
#else
#define Log
#endif


using std::string;
using std::wstring;
using std::map;


namespace vhmsg
{

// This is used to set whatever the client callback will be for tt_utils
static void (*g_clientCallback)( const char * op, const char * args, void * user_data );
static void * g_clientCallbackUserData = NULL;
static void (*g_clientCallback_ws)( const wchar_t * op, const wchar_t * args, void * user_data );
static void * g_clientCallbackUserData_ws = NULL;
static void (*g_clientCallbackMap)( const char * op, const char * args, const std::map< string, string > * messageMap, void * user_data );
static void * g_clientCallbackMapUserData = NULL;
static void (*g_clientCallbackMap_ws)( const wchar_t * op, const wchar_t * args, const std::map< wstring, wstring > * messageMap, void * user_data );
static void * g_clientCallbackMapUserData_ws = NULL;
static bool g_useMapCallback = false;
static bool g_useWideStrings = false;

static Client * g_vhmsg = NULL;

static string g_lastUsedHostname = "localhost";
static string g_lastUsedPort = "61616";
static string g_lastUsedScope = "DEFAULT_SCOPE";


class TTUListener : public Listener
{
   public:
      virtual void OnMessage( const Message & message )
      {
         const string & m = message.GetString();
         string name;
         string value;

         size_t index = m.find_first_of( " " );
         if ( index == string::npos )
         {
            name  = m;
            value = "";
         }
         else
         {
            name  = m.substr( 0, index );
            value = m.substr( index + 1 );
         }

         //printf( "'%s' - '%s' '%s'\n", m.c_str(), name.c_str(), value.c_str() );

         if ( g_useMapCallback )
         {
            if ( g_useWideStrings )
            {
               const map< wstring, wstring > & messageMap = message.GetWMap();

               if ( g_clientCallbackMap_ws )
               {
                  g_clientCallbackMap_ws( (const wchar_t *)name.c_str(), (const wchar_t *)value.c_str(), &messageMap, g_clientCallbackMapUserData_ws );
               }
            }
            else
            {
               const map< string, string > & messageMap = message.GetMap();

               if ( g_clientCallbackMap )
               {
                  g_clientCallbackMap( name.c_str(), value.c_str(), &messageMap, g_clientCallbackMapUserData );
               }
            }
         }
         else
         {
            if ( g_useWideStrings )
            {
               if ( g_clientCallback_ws )
               {
                  g_clientCallback_ws( (const wchar_t *)name.c_str(), (const wchar_t *)value.c_str(), g_clientCallbackUserData_ws );
               }
            }
            else
            {
               if ( g_clientCallback )
               {
                  g_clientCallback( name.c_str(), value.c_str(), g_clientCallbackUserData );
               }
            }
         }
      }
};

static TTUListener g_ttuListener;


void ttu_set_client_callback( void (*cb)( const char * op, const char * args, void * user_data), void * user_data )
{
   g_clientCallback = cb;
   g_clientCallbackUserData = user_data;
   g_useMapCallback = false;
   g_useWideStrings = false;
}


void ttu_set_client_callback_ws( void (*cb)( const wchar_t * op, const wchar_t * args, void * user_data), void * user_data )
{
   g_clientCallback_ws = cb;
   g_clientCallbackUserData_ws = user_data;
   g_useMapCallback = false;
   g_useWideStrings = true;
}


void ttu_set_client_callback_map( void (*cb)( const char * op, const char * args, const std::map< std::string, std::string > * messageMap, void * user_data ), void * user_data )
{
   g_clientCallbackMap = cb;
   g_clientCallbackMapUserData = user_data;
   g_useMapCallback = true;
   g_useWideStrings = false;
}


void ttu_set_client_callback_map_ws( void (*cb)( const wchar_t * op, const wchar_t * args, const std::map< std::wstring, std::wstring > * messageMap, void * user_data ), void * user_data )
{
   g_clientCallbackMap_ws = cb;
   g_clientCallbackMapUserData_ws = user_data;
   g_useMapCallback = true;
   g_useWideStrings = true;
}


const char * ttu_get_server()
{
   if ( g_vhmsg )
      return g_vhmsg->GetServer().c_str();
   else
      return g_lastUsedHostname.c_str();
}

const char * ttu_get_port()
{
   if ( g_vhmsg )
      return g_vhmsg->GetPort().c_str();
   else
      return g_lastUsedPort.c_str();
}


const char * ttu_get_scope()
{
   if ( g_vhmsg )
      return g_vhmsg->GetScope().c_str();
   else
      return g_lastUsedScope.c_str();
}


int ttu_open( const char * hostname, const char * scope,  const char * port )
{
   g_vhmsg = new Client();

   if ( scope != NULL && strcmp( scope, "" ) != 0 )
   {
      g_vhmsg->SetScope( scope );
   }

   bool ret;
   if ( hostname != NULL && strcmp( hostname, "" ) != 0 )
   {
      if ( port != NULL && strcmp( port, "" ) != 0 )
         ret = g_vhmsg->OpenConnection( hostname, port );
      else
         ret = g_vhmsg->OpenConnection( hostname );
   }
   else
   {
      ret = g_vhmsg->OpenConnection();
   }

   g_lastUsedHostname = g_vhmsg->GetServer();
   g_lastUsedPort = g_vhmsg->GetPort();
   g_lastUsedScope = g_vhmsg->GetScope();

   if ( !ret )
   {
      delete g_vhmsg;
      g_vhmsg = NULL;

      Log( "tt_utils: Error: Connection to server failure" );
      return TTU_ERROR;
   }

#ifdef DEBUG_BUILD
   Log( "tt_utils: Myhost: %s - %s", g_vhmsg->GetServer().c_str(), g_vhmsg->GetScope().c_str() );
#endif

   g_vhmsg->EnablePollingMethod();
   g_vhmsg->SetListener( &g_ttuListener );

#ifdef DEBUG_BUILD
   Log( "tt_utils: Connection %x successful", (int)g_vhmsg );
#endif

   return TTU_SUCCESS;
}


int ttu_close()
{
   if ( g_vhmsg )
      g_vhmsg->CloseConnection();

   delete g_vhmsg;
   g_vhmsg = NULL;

   return TTU_SUCCESS;
}


bool ttu_is_open()
{
   if ( g_vhmsg )
   {
      return g_vhmsg->IsOpen();
   }

   return false;
}


void ttu_set_bypass_mode( bool enable )
{
   // This is an explicit bypass of the VHMsg message protocol to help support non-vhmsg clients.
   // Default is false.
   // Bypass mode doesn't require ELVISH_SCOPE and MESSAGE_PREFIX ActiveMQ properties to be set
   // All ActiveMQ messages will be received.
   // A single ttu_register() call will be required, but it's parameter will be ignored.
   // Also, this function must be called first before ttu_register()

   if ( !g_vhmsg )
   {
      Log( "ttu_set_bypass_mode(): Error: Not connected" );
   }

   g_vhmsg->SetBypassMode( enable );
}


int ttu_register( const char * opname )
{
   if ( g_vhmsg == NULL )
   {
      Log( "tt_utils: Error: Not connected, cannot subscribe" );
      return TTU_ERROR;
   }

   bool ret = g_vhmsg->Subscribe( opname );
   if ( !ret )
   {
      Log( "tt_utils: Error: Cannot subscribe to %s", opname );
      return TTU_ERROR;
   }

   //Log( "tt_utils: '%s'", opname.c_str() );

   return TTU_SUCCESS;
}


int ttu_unregister( const char * opname )
{
   if ( g_vhmsg == NULL )
   {
      Log( "tt_utils: Error: Not connected, cannot subscribe" );
      return TTU_ERROR;
   }

   bool ret = g_vhmsg->Unsubscribe( opname );
   if ( !ret )
   {
      Log( "tt_utils: Error: Cannot unsubscribe to %s", opname );
      return TTU_ERROR;
   }

   //Log( "tt_utils: '%s'", opname.c_str() );

   return TTU_SUCCESS;
}


int ttu_notify1( const char * msg )
{
   if ( g_vhmsg == NULL )
   {
      Log( "tt_utils: Error: Not connected, cannot send notification" );
      return TTU_ERROR;
   }

#ifdef DEBUG_BUILD
   Log( "tt_utils: Attempt ttu_notify1: '%s'", msg );
#endif

   g_vhmsg->Send( msg );

   return TTU_SUCCESS;
}


int ttu_notify2( const char * opname, const char * msg )
{
   if ( g_vhmsg == NULL )
   {
      Log( "tt_utils: Error: Not connected, cannot send notification" );
      return TTU_ERROR;
   }

#ifdef DEBUG_BUILD
   Log( "tt_utils: Attempt ttu_notify2: '%s' '%s'", opname, msg );
#endif

   g_vhmsg->Send( opname, msg );

   return TTU_SUCCESS;
}


int ttu_poll()
{
   if ( !g_vhmsg )
      return TTU_ERROR;

   g_vhmsg->Poll();

   return TTU_SUCCESS;
}


int ttu_wait( const double waittime_secs )
{
   if ( !g_vhmsg )
      return TTU_ERROR;

   g_vhmsg->WaitAndPoll( waittime_secs );

   return TTU_SUCCESS;
}


int ttu_report_version( const char * component, const char * subcomponent, const char * type, const char * file_name )
{
// TODO: linux
#if defined(WIN_BUILD)

   string sComponent;
   string sSubcomponent;
   string sType;

   if ( component )
      sComponent = component;

   if ( subcomponent )
      sSubcomponent = subcomponent;

   if ( type )
      sType = type;


   string versionFile;

   if ( file_name == NULL )
   {
      char fullExePath[ 1024 ];
      GetModuleFileName( NULL, fullExePath, 1024 );

      string path;
      string file;
      vhcl::StripPath( string( fullExePath ), path, file );

      versionFile = path + "\\" + "version.ini";
   }
   else
   {
      versionFile = file_name;
   }

   string defaultVersion = "0";

   char version[ 1024 ];
   GetPrivateProfileString( "version", "version", defaultVersion.c_str(), version, 1024, versionFile.c_str() );

   //OutputDebugString( vhcl::Format( "%s\n", fullExePath ).c_str() );
   //OutputDebugString( vhcl::Format( "%s %s\n", path.c_str(), file.c_str() ).c_str() );
   //OutputDebugString( vhcl::Format( "version: %s\n", version ).c_str() );

   ttu_notify2( "vrVersion", vhcl::Format( "%s %s %s %s", sComponent.c_str(), sSubcomponent.c_str(), sType.c_str(), version ).c_str() );

   return TTU_SUCCESS;

#else

   return TTU_SUCCESS;

#endif
}

};  // namespace vhmsg
