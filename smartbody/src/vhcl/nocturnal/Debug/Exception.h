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

#pragma once

#include <string>
#include <vector>
#include <exception>

#include "API.h"
#include "Symbols.h"

//#include "Common/Exception.h"
//#include "Common/Automation/Event.h"

//#include "Console/Console.h"

namespace nocturnal
{
namespace Debug
{
  //
  // StackTraceException captures the stack trace in its constructor
  //

#if 0
  class StackTraceException : public Nocturnal::Exception
  {
  private:
    mutable bool m_Translated;
    V_StackFrame m_StackTrace;
    std::string m_State;

  public:
    StackTraceException( const char *msgFormat, ... )
      : m_Translated (false)
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );

      m_State = Console::GetOutlineState();

      Debug::GetStackTrace( m_StackTrace, 1 );
    }

    virtual const char* what() const
    {
      if (!m_Translated)
      {
        m_Translated = true;
        m_Message.append("\n");

        if (!m_State.empty())
        {
          m_Message.append("\nState:\n");
          m_Message.append( m_State );
        }

        m_Message.append("\nThrow Stack:\n");
        Debug::Translate( m_StackTrace, m_Message );

        m_Message.resize( m_Message.size() - 1 );  // eat newline
      }

      return __super::what();
    }
  };
#endif


  //
  // Helper for handling SEH Exceptions in C++ catch statements
  //

  class StructuredException : public std::exception
  {
  private:
    LPEXCEPTION_POINTERS m_Info;
    mutable std::string m_StringInfo;

  public:
    explicit StructuredException(LPEXCEPTION_POINTERS info)
      : m_Info (info)
    {

    }

    //virtual const char* what() const override;
    virtual const char* what() const;
  };

  //
  // Stack based enabler to ensure SEH translator setup/restoration
  //  this technically doesn't HAVE to be a template, but if it wasn't it would generate a compiler warning in code files
  //  that are compiled without /EHa, even IF they don't actually instantiate it.  Since the code for templates is generated
  //  only upon instantiation this delays that compiler warning only to code files that use this /EHa-dependent features of Debug
  //

  template<_se_translator_function translator>
  class EnableTranslator
  {
  private:
    _se_translator_function m_Previous;

  public:
    EnableTranslator()
    {
      m_Previous = _set_se_translator(&translator);
    }

    ~EnableTranslator()
    {
      _set_se_translator(m_Previous);
    }
  };

  // translates an SEH exception into a StructuredException when compiling with /EHa
  DEBUG_API void TranslateException(UINT code, LPEXCEPTION_POINTERS info);

  //
  // Alternatively, subclassing this command class will better allow mixing of SEH and C++ objects w/ constructor/destructor functions
  //  to use it just initialize references to stack variables in the derived constructor
  //  and use them in your Execute function implementation
  //

  class DEBUG_API TryExceptCommand
  {
  public:
    bool Attempt();

  protected:
    virtual void Execute() = 0;
  };

  //
  // Exception handling and reporting API
  //

  // some constants
  extern DEBUG_API DWORD ExecuteHandler;    // execute the __except statement for this __try
  extern DEBUG_API DWORD ContinueSearch;    // continue up the stack and look for another __except to handle this exception
  extern DEBUG_API DWORD ContinueExecution; // continue execution at the exception point (ex. after you changed some instructions or mapped some memory)

  // get environment-driven handling behavior
  DEBUG_API int GetExceptionBehavior();

  // prepare and dispatch a report via email for a C++ exception, this uses the current execution state, not the state at which the exception was thrown
  DEBUG_API void ProcessException( const std::exception& ex,
                                   const char* category = "Exception",
                                   const char* state = "",
                                   bool print = false );

  // prepare and dispatch a report via email for an SEH exception such as divide by zero, page fault from a invalid memory access, or even breakpoint instructions
  DEBUG_API DWORD ProcessException( LPEXCEPTION_POINTERS info,
                                    DWORD ret_code = GetExceptionBehavior(),
                                    const char* category = "Crash",
                                    const char* state = "",
                                    bool print = false );

  // this is the main entry point for handling SEH Exceptions that are potentially continuable (use in your top level __except)
  DEBUG_API int HandleException( LPEXCEPTION_POINTERS info );


  //
  // Initialize Handling
  //

  // installs unhandled exception filter into the C-runtime
  DEBUG_API void EnableExceptionHandling(bool enable);


  //
  // Events for attaching listeners to events
  //

  struct DEBUG_API TerminateArgs { };
  //typedef Nocturnal::Signature<void, const TerminateArgs&> TerminateSignature;
  //DEBUG_API void AddTerminateListener( const TerminateSignature::Delegate& listener );
  //DEBUG_API void RemoveTerminateListener( const TerminateSignature::Delegate& listener );
}
}
