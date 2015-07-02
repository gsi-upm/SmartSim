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

#ifndef VHMSG_TT_H
#define VHMSG_TT_H


#include "vhcl_public.h"

#include <string>
#include <map>


namespace vhmsg
{

static const int TTU_SUCCESS = 0;
static const int TTU_ERROR   = 1;


// ttu_set_client_callback
// * Sets the callback function to be used
// * Inputs: Memory address of some function
// * Output: None
void ttu_set_client_callback( void (*cb)( const char * op, const char * args, void * user_data ), void * user_data = NULL );
void ttu_set_client_callback_ws( void (*cb)( const wchar_t * op, const wchar_t * args, void * user_data ), void * user_data = NULL );
void ttu_set_client_callback_map( void (*cb)( const char * op, const char * args, const std::map< std::string, std::string > * messageMap, void * user_data ), void * user_data = NULL );
void ttu_set_client_callback_map_ws( void (*cb)( const wchar_t * op, const wchar_t * args, const std::map< std::wstring, std::wstring > * messageMap, void * user_data ), void * user_data = NULL );


// ttu_get_server
// * Retrieves the name of the server currently connected to
// * Inputs: None
// * Output: string name of the server currently connected to
const char * ttu_get_server();

// ttu_get_port
// * Retrieves the name of the port currently connected to
// * Inputs: None
// * Output: string name of the port currently connected to
const char * ttu_get_port();

// ttu_get_scope
// * Retrieves the name of the scope used to identify messages used in this connection
// * Inputs: None
// * Output: string name of the scope
const char * ttu_get_scope();

// ttu_open
// * Opens the ttu connection
// * Inputs: The server to connect to
// * Output: TTU_ERROR or TTU_SUCCESS 
int ttu_open( const char * hostname = NULL, const char * scope = NULL, const char * port = NULL );

// ttu_close
// * Closes the ttu connection
// * Inputs: None
// * Output: TTU_ERROR or TTU_SUCCESS
int ttu_close();

// ttu_is_open
bool ttu_is_open();

// ttu_set_bypass_mode
void ttu_set_bypass_mode( bool enable );

// ttu_register
// * Creates a new registration with opname as the name of the
// * operation to be executed if a message comes in
// * Inputs: Name of the operation to register
// * Output: TTU_ERROR or TTU_SUCCESS
int ttu_register( const char * opname );

// ttu_unregister
int ttu_unregister( const char * opname );

// ttu_notify1
// * Sends a notification
// * Inputs: Some string to be sent over vhmsg.  The first token
// * its the operations name, the rest are arguments to be sent
// * Output: TTU_ERROR or TTU_SUCCESS
int ttu_notify1( const char * msg );

// ttu_notify2
// * Sends a notification
// * Inputs: Some string to be sent over vhmsg.  The first token
// * its the operations name, the rest are arguments to be sent
// * Output: TTU_ERROR or TTU_SUCCESS
int ttu_notify2( const char * opname, const char * msg );

// ttu_poll
// * Processes all pending messages
// * Inputs: None
// * Output: TTU_ERROR or TTU_SUCCESS
int ttu_poll();

// ttu_wait
// * This function Sleeps until a messages arrives, and then processes all pending messages.  It then returns.
// * Inputs:
// * Output: TTU_ERROR or TTU_SUCCESS
int ttu_wait( const double waittime_secs );

// ttu_report_version
// * This function sends out a message with the version of the given component.  This is based on the given .ini file.
// * If file_name is NULL, it uses version.ini in the executable's directory.
// * Inputs:
// * Output: TTU_ERROR or TTU_SUCCESS
int ttu_report_version( const char * component, const char * subcomponent, const char * type, const char * file_name = NULL );


};

#endif  // VHMSG_TT_H
