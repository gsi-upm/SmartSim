////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2008 Insomniac Games
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the Insomniac Open License
// as published by Insomniac Games.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even an implied warranty. See the
// Insomniac Open License for more details.
//
// You should have received a copy of the Insomniac Open License
// along with this code; if not, write to the Insomniac Games
// 2255 N. Ontario St Suite 550, Burbank, CA 91504, or email
// nocturnal@insomniacgames.com, or visit
// http://nocturnal.insomniacgames.com.
////////////////////////////////////////////////////////////////////////////

#include "Mail.h"

#include <windows.h>

//#include "Common/Config.h"
//#include "Common/Version.h"


namespace nocturnal
{

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SetDefaultDestination()
//
//  written by: Geoff Evans
//
////////////////////////////////////////////////////////////////////////////////////////////////

static std::string g_DestinationAddress = "vhcrash@ict.usc.edu";

void Debug::SetDefaultDestination(const std::string& destination)
{
  g_DestinationAddress = destination;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SendDefaultSource()
//
//  written by: Geoff Evans
//
////////////////////////////////////////////////////////////////////////////////////////////////

static std::string g_SourceAddress = "vhcrash@ict.usc.edu";

void Debug::SetDefaultSource(const std::string& source)
{
  g_SourceAddress = source;
}


static std::string g_MailServer = "smtp.ict.usc.edu";

void Debug::SetDefaultServer(const std::string& server)
{
  g_MailServer = server;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SendMail()
//
//  written by: Geoff Evans
//
////////////////////////////////////////////////////////////////////////////////////////////////

bool Debug::SendMail(const std::string& subject, const std::string& text, const std::vector<std::string>& files, const std::string& destination, const std::string& source)
{
#if 0
  if (getenv( NOCTURNAL_STUDIO_PREFIX "DEBUG_DONT_EMAIL" ) != NULL)
  {
    return true;
  }
#endif


  std::string blatBody;
  std::string blatSubject;
  std::string blatFiles;
  std::string blatTo;
  std::string blatServer;
  std::string blatFrom;
  std::string blatSender;


  //
  // Message to send
  //

  if (text.empty())
  {
    blatSubject = subject + " (n/t)";
  }
  else
  {
    blatSubject = subject;
  }

  blatSubject = "-subject \"" + blatSubject + "\"";


  if (!text.empty())
  {
    std::string fixed;

    std::string::const_iterator itr = text.begin();
    std::string::const_iterator end = text.end();
    for ( ; itr != end; ++itr )
    {
      if (*itr == '\n')
      {
        fixed.append( "\r\n" );
      }
      else
      {
        fixed.append( itr, itr+1 );
      }
    }

    blatBody = "-body \"" + fixed + "\"";
  }


  std::vector<std::string>::const_iterator itr = files.begin();
  std::vector<std::string>::const_iterator end = files.end();
  for ( ; itr != end; ++itr )
  {
    if ( blatFiles.empty() )
    {
       blatFiles = std::string( "\"" ) + *itr + std::string( "\"" );
    }
    else
    {
       blatFiles += std::string( ",\"" ) + *itr + std::string( "\"" );
    }
  }

  if ( !blatFiles.empty() )
  {
     blatFiles = std::string( "-attach " ) + blatFiles;
  }


  //
  // Send
  //

  {
    typedef int (WINAPI * Blat_Send) ( const char * );
    Blat_Send  blat_Send = NULL;

    HMODULE hmod = LoadLibrary( "blat.dll" );
    if ( hmod != NULL )
    {
      blat_Send = (Blat_Send)GetProcAddress( hmod, "Send" );
      if ( blat_Send != NULL )
      {
        blatTo = destination.empty() ? g_DestinationAddress : destination;
        blatServer = g_MailServer;
        blatFrom   = source.empty() ? g_SourceAddress : source;
        blatSender = source.empty() ? g_SourceAddress : source;

        // blat -body "hello world" -subject "crash report" -to vhcrash@ict.usc.edu -attach testd.dmp -server smtp.ict.usc.edu -i vhcrash@ict.usc.edu -f vhcrash@ict.usc.edu
        std::string blat = "-q " +
                           blatBody + " " +
                           blatSubject + " " +
                           blatFiles +
                           " -to " + blatTo +
                           " -server " + blatServer +
                           " -i " + blatFrom +
                           " -f " + blatSender;

        //blat_Send( "-q -body \"hello world\" -subject \"crash report\" -to vhcrash@ict.usc.edu -attach testd.dmp -server smtp.ict.usc.edu -i vhcrash@ict.usc.edu -f vhcrash@ict.usc.edu" );
        int ret = blat_Send( blat.c_str() );

        return ret == 0;
      }
    }
  }

  return false;
}

}
