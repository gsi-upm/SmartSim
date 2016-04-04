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

#define _WIN32_WINNT    0x0501
#define WINVER          0x0501
#define _WIN32_WINDOWS  0x0500

#pragma warning(disable:4100)  // unreferenced formal parameter

#include "Exception.h"
#include "Mail.h"
#include "Memory.h"

#if 0
#define WXUSINGDLL
#include <wx/wx.h>
#include <wx/choicdlg.h>
#include <wx/msw/private.h>
#endif

#include <windows.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <set>
#include <sys/stat.h>

#if 0
#include "Common/Assert.h"
#include "Common/Config.h"
#include "Common/Version.h"

#include "Windows/Console.h"
#include "Console/Console.h"
#endif

using namespace nocturnal::Debug;

namespace nocturnal
{

DWORD Debug::ExecuteHandler = EXCEPTION_EXECUTE_HANDLER;
//DWORD Debug::ContinueSearch = EXCEPTION_CONTINUE_SEARCH;
//DWORD Debug::ContinueExecution = EXCEPTION_CONTINUE_EXECUTION;

// has execution been interrupted (unhandled exception, or break signal)
bool g_ExceptionHandlerCalled = false;

#if 0
// the one and only abnormal termination event
TerminateSignature::Event g_TerminateEvent;
#endif

#if 0
// stub wx widgets code to make us work in a dll
class wxDLLApp : public wxApp
{
  bool OnInit()
  {
    return true;
  }
};
#endif

HINSTANCE g_Module = NULL;

bool g_wxInitialized = false;

#if 0
IMPLEMENT_APP_NO_MAIN(wxDLLApp);
#endif

#if 0
BOOL APIENTRY DllMain(HANDLE module, DWORD  reason, LPVOID /*reserved*/) 
{
  switch (reason)
  { 
  case DLL_PROCESS_ATTACH:
    {
      g_Module = (HINSTANCE)module;
      break;
    }

  case DLL_PROCESS_DETACH:
    {
      if (g_wxInitialized)
      {
        wxEntryCleanup();
      }
      break; 
    }
  }

  return TRUE;
}
#endif

const char* Debug::StructuredException::what() const
{
  if (m_StringInfo.empty())
  {
    m_StringInfo = Debug::GetExceptionInfo(m_Info);
    m_StringInfo.resize( m_StringInfo.size() - 1 );  // eat newline
  }

  return m_StringInfo.c_str();
}

void Debug::TranslateException(UINT code, LPEXCEPTION_POINTERS info)
{
  // we don't include the excption code in the C++ exception object because its already in the 
  //  exception record structure, just santity check that its really the case
#if 0
  NOC_ASSERT(code == info->ExceptionRecord->ExceptionCode);
#endif

  throw Debug::StructuredException(info);
}

bool TryExceptCommand::Attempt()
{
  __try
  {
    Execute();
  }
  __except( Debug::ProcessException( GetExceptionInformation() , Debug::ExecuteHandler, "Error" ) )
  {
    return false;
  }

  return true;
}

int Debug::GetExceptionBehavior()
{
#if 0
  if (getenv( NOCTURNAL_STUDIO_PREFIX "CRASH_DONT_BLOCK" ) != NULL)
  {
    // do not propagate the exception up to the system (avoid dialog)
    return EXCEPTION_EXECUTE_HANDLER;
  }
#endif

  // handle exception transparently
  return EXCEPTION_CONTINUE_SEARCH;
}

void DispatchReport(const std::string& category, const std::string& state, const std::string& report, LPEXCEPTION_POINTERS info = NULL)
{
#if 0
  if (getenv( NOCTURNAL_STUDIO_PREFIX "CRASH_DONT_EMAIL" ) != NULL)
  {
    return;
  }
#endif

#if 0
  Windows::Print(Windows::ConsoleColors::Red, stderr, "\nDispatching report...");
#endif


  //
  // Subject
  //

  char buf[MAX_PATH];
  GetModuleFileName(NULL, buf, MAX_PATH);

  std::string process_path = buf;
  size_t idx = process_path.find_last_of("\\");

  std::string process_file_name;
  if ( idx != std::string::npos )
    process_file_name = process_path.substr(idx+1);
  else
    process_file_name = process_path;

  GetEnvironmentVariable("COMPUTERNAME", buf, MAX_PATH);
  const std::string NOCTURNAL_VERSION_STRING = "";
  std::string subject = category + ": " + process_file_name + " (v" + NOCTURNAL_VERSION_STRING + ") " + " @ " + buf;


  //
  // Report text
  //

  std::string text;
  text += std::string ("Version:\t") + NOCTURNAL_VERSION_STRING + "\n";

  GetEnvironmentVariable("USERNAME", buf, MAX_PATH);
  text += std::string ("Username:\t") + buf + "\n";

  GetEnvironmentVariable("COMPUTERNAME", buf, MAX_PATH);
  text += std::string ("Computer:\t") + buf + "\n";

  text += std::string ("Command:\t") + GetCommandLine() + "\n";

#ifdef _DEBUG
  text += "Build:\tDebug\n";
#else
  text += "Build:\tRelease\n";
#endif


  //
  // Report Body
  //

  text += report;


  //
  // Memory Status
  //

  GetMemoryStatus(text);


  //
  // State
  //

  if (!state.empty())
  {
    text += "\nState:\n";
    text += state;
  }


  //
  // Environment
  //

  // Get a pointer to the environment block. 
  const char* env = (const char*)::GetEnvironmentStrings();

  // If the returned pointer is NULL, exit.
  if (env)
  {
    text += "\nEnvironment:\n";

    // Variable strings are separated by NULL byte, and the block is terminated by a NULL byte. 
    for (const char* var = (const char*)env; *var; var++) 
    {
      if (*var != '=')
      {
        text += " ";
        text += var;
        text += "\n";
      }

      while (*var)
      {
        var++;
      }
    }

    ::FreeEnvironmentStrings((char*)env);
  }


  //
  // Dump File
  //

  std::string file;

  if (info)
  {
#if 0
    bool dump = getenv( NOCTURNAL_STUDIO_PREFIX "CRASH_DUMP" ) != NULL;
    bool full = getenv( NOCTURNAL_STUDIO_PREFIX "CRASH_FULL_DUMP" ) != NULL;
#endif
    bool dump = true;
    bool full = false;
    
    if ( dump || full )
    {
      file = Debug::WriteDump(info, full);
    }
  }

  Debug::SendMail( subject, text, file );

#if 0
  Windows::Print(Windows::ConsoleColors::Red, stderr, " submitted\n");
#endif
}

void Debug::ProcessException(const std::exception& exception, const char* category, const char* state, bool print)
{
  std::string buffer;

  const char* type = NULL;

  try
  {
    type = typeid(exception).name();
  }
  catch (const std::bad_typeid&)
  {

  }

  if (type)
  {
    Buffer(buffer, "\nC++ Exception, Type '%s'\n\nException Message:\n%s\n", type, exception.what());
  }
  else
  {
    Buffer(buffer, "\nC++ Exception, Unknown Type\n\nException Message:\n%s\n", exception.what());
  }

  if (print)
  {
#if 0
    Windows::Print(Windows::ConsoleColors::Red, stderr, buffer.c_str());
#endif
  }

  DispatchReport(category, state, buffer);
}

DWORD Debug::ProcessException(LPEXCEPTION_POINTERS info, DWORD ret_code, const char* category, const char* state, bool print)
{
  std::string buffer;
  
  Debug::EnableTranslator<Debug::TranslateException> enable;

  try
  {
    buffer = GetExceptionInfo(info);
  }
  catch ( Debug::StructuredException& )
  {
#if 0
    Windows::Print(Windows::ConsoleColors::Red, stderr, "Exception occured in exception handler!\n");
#endif
  }

  if (!buffer.empty())
  {
    if (print)
    {
#if 0
      Windows::Print(Windows::ConsoleColors::Red, stderr, buffer.c_str());
#endif
    }

    DispatchReport(category, state, buffer, info);
  }

  return ret_code;
}

DWORD HandleTerminalException( LPEXCEPTION_POINTERS info )
{
  if ( g_ExceptionHandlerCalled )
  {
    // we are re-entrant, ignore this call
    return EXCEPTION_CONTINUE_SEARCH;
  }

  g_ExceptionHandlerCalled = true;

#if 0
  // reap worker processes
  g_TerminateEvent.Raise( TerminateArgs () );
#endif

  // submit report
#if 0
  return Debug::ProcessException(info, Debug::GetExceptionBehavior(), "Crash", Console::GetOutlineState().c_str(), true);
#endif
  return Debug::ProcessException(info, Debug::GetExceptionBehavior(), "Crash", "", true);
}

int Debug::HandleException( LPEXCEPTION_POINTERS info )
{
  if ( g_ExceptionHandlerCalled )
  {
    // we are re-entrant, ignore this call
    return EXCEPTION_CONTINUE_SEARCH;
  }

  g_ExceptionHandlerCalled = true;

  // handle breakpoint exceptions outside the debugger
  if (!::IsDebuggerPresent() && info->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
  {
#if 0
    // if we are not in a wxWidgets application then initialize an instance in this dll
    if (!wxTheApp && !g_wxInitialized)
    {
      // flag this instance for cleanup in process detach
      g_wxInitialized = true;

      // connect wx to our module instance
      wxSetInstance((HINSTANCE)g_Module);

      // do init
      int argc = 0;
      char** argv = NULL;
      wxEntryStart(argc, argv);

      // just in case someone puts logic in wxDLLApp::OnInit()
      wxTheApp->CallOnInit();
    }
#endif

    static std::set<intptr_t> disabled;
    static bool skipAll = false;
    bool skip = skipAll;

    // are we NOT skipping everything?
    if (!skipAll)
    {
      // have we disabled this break point?
      if (disabled.find(info->ContextRecord->IPREG) != disabled.end())
      {
        skip = true;
      }
      // we have NOT disabled this break point yet
      else
      {
#if 0
        // dump info to console
        Windows::Print(Windows::ConsoleColors::Red, stderr, "%s\n", Debug::GetExceptionInfo(info).c_str());
#endif

        // display result
        std::string message ("A break point was triggered in the application:\n\n");
        message += Debug::GetSymbolInfo( info->ContextRecord->IPREG );
        message += std::string ("\n\nWhat do you wish to do?");

#if 0
        const char* nothing = "Let the OS handle this as an exception";
        const char* thisOnce = "Skip this break point once";
        const char* thisDisable = "Skip this break point and disable it";
        const char* allDisable = "Skip all break points";

        wxArrayString choices;
        choices.Add(nothing);
        choices.Add(thisOnce);
        choices.Add(thisDisable);
        choices.Add(allDisable);
        wxString choice = ::wxGetSingleChoice( message.c_str(), "Break Point Triggered", choices );

        if (choice == nothing)
        {
          // we are not continuable, so unhook the top level filter
          SetUnhandledExceptionFilter( NULL );

          // this should let the OS prompt for the debugger
          return EXCEPTION_CONTINUE_SEARCH;
        }
        else if (choice == thisOnce)
        {
          skip = true;
        }
        else if (choice == thisDisable)
        {
          skip = true;
          disabled.insert(info->ContextRecord->IPREG);
        }
        else if (choice == allDisable)
        {
          skip = true;
          skipAll = true;
        }
#endif
      }
    }

    // allow re-entry
    g_ExceptionHandlerCalled = false;

    if (skipAll || skip)
    {
      // skip break instruction (move the ip ahead one byte)
      info->ContextRecord->IPREG += 1;

      // continue execution past the break instruction
      return EXCEPTION_CONTINUE_EXECUTION;
    }
    else
    {
      // fall through and let window's crash API run
      return EXCEPTION_CONTINUE_SEARCH;
    }
  }

  // we are not continuable, so unhook the top level filter
  SetUnhandledExceptionFilter( NULL );

  // release handler lock before calling terminal handler
  g_ExceptionHandlerCalled = false;

  // handle exception terminally and abort
  return HandleTerminalException( info );
}

void Debug::EnableExceptionHandling(bool enable)
{
  // handles an exception occuring in the process not handled by a user exception handler
  SetUnhandledExceptionFilter( enable ? (PTOP_LEVEL_EXCEPTION_FILTER)&HandleException : NULL );
}

#if 0
void Debug::AddTerminateListener( const TerminateSignature::Delegate& listener )
{
  g_TerminateEvent.Add( listener );
}

void Debug::RemoveTerminateListener( const TerminateSignature::Delegate& listener )
{
  g_TerminateEvent.Remove( listener );
}
#endif

}
