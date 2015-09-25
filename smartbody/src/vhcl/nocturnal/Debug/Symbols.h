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

#include <vector>

#include "API.h"

#if defined(_M_IX86)
# define IMAGE_FILE_ARCH IMAGE_FILE_MACHINE_I386
# define AXREG Eax
# define BXREG Ebx
# define CXREG Ecx
# define DXREG Edx
# define SIREG Esi
# define DIREG Edi
# define BPREG Ebp
# define IPREG Eip
# define SPREG Esp
#elif defined(_M_X64)
# define IMAGE_FILE_ARCH IMAGE_FILE_MACHINE_AMD64
# define AXREG Rax
# define BXREG Rbx
# define CXREG Rcx
# define DXREG Rdx
# define SIREG Rsi
# define DIREG Rdi
# define BPREG Rbp
# define IPREG Rip
# define SPREG Rsp
#else
# error Machine type not supported
#endif // _M_IX86

struct _CONTEXT;
typedef _CONTEXT CONTEXT;
typedef CONTEXT* PCONTEXT;
typedef PCONTEXT LPCONTEXT;

struct _EXCEPTION_POINTERS;
typedef _EXCEPTION_POINTERS EXCEPTION_POINTERS;
typedef EXCEPTION_POINTERS* PEXCEPTION_POINTERS;
typedef PEXCEPTION_POINTERS LPEXCEPTION_POINTERS;

typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef unsigned __int64 DWORD64;

namespace nocturnal
{
namespace Debug
{
  // how we treat stacks
  typedef std::vector<DWORD64> V_StackFrame;

  // Inits the symbol system and loads any asscoiated symbol files, called from win32_main.
  DEBUG_API bool Initialize();

  // Returns true if the Symbols are loaded, this is basically the same as the return code to Initialize()
  DEBUG_API bool IsInitialized();

  // Get the string representation of a symbol
  DEBUG_API std::string GetSymbolInfo(DWORD64 adr);
 
  // Captures the caller's stack to a stack trace
  DEBUG_API bool GetStackTrace(V_StackFrame& trace, unsigned omitFrames = 0);

  // Gets the stack addresses to a vector (stack trace)
  DEBUG_API bool GetStackTrace(LPCONTEXT context, V_StackFrame& stack, unsigned omitFrames = 0);

  // Convert a list of addresses into a stack string
  DEBUG_API void Translate(const V_StackFrame& trace, std::string& buffer);

  // Walks the stack and converts it to a string buffer
  DEBUG_API bool GetCallStack(LPCONTEXT context, std::string& buffer);

  // Get full exception information string report
  DEBUG_API std::string GetExceptionInfo(LPEXCEPTION_POINTERS info);

  // Generate dump file for this exception
  DEBUG_API std::string WriteDump(LPEXCEPTION_POINTERS info, bool full);

  // Buffer to a string what we throw at it with va_args
  DEBUG_API void Buffer(std::string& buffer, const char* format,...);
}
}
