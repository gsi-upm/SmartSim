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

//#define _WIN32_WINNT    0x0501
//#define WINVER          0x0501
//#define _WIN32_WINDOWS  0x0500

#pragma warning(disable:4100)  // unreferenced formal parameter
#pragma warning(disable:4127)  // conditional expression is constant

#include "Symbols.h"
#include "Exception.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
//#define __out_xcount(x) // so so sweet
#include "dbghelp.h"

//#include "Common/Types.h"

//#define DEBUG_SYMBOLS

// disable the optimizer if debugging in release
#if defined(DEBUG_SYMBOLS) && defined(NDEBUG)
#pragma optimize("", off)
#endif


#include <map>
//typedef unsigned char         u8;
typedef unsigned int          u32;
typedef signed __int64        i64;
//typedef double                f64;


namespace nocturnal
{

// loaded flag
static bool g_Initialized = false;

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  EnumerateLoadedModulesProc()
//
//  written by: Geoff Evans
//
//  Callback loads symbol data from loaded dll into DbgHelp system, dumping error info
//
////////////////////////////////////////////////////////////////////////////////////////////////
#if API_VERSION_NUMBER <= 9   // dbghelp specific define
BOOL CALLBACK EnumerateLoadedModulesProc(PSTR name, DWORD64 base, ULONG size, PVOID data)
#else
BOOL CALLBACK EnumerateLoadedModulesProc(PCSTR name, DWORD64 base, ULONG size, PVOID data)
#endif
{
  char buf[512];

  if (SymLoadModule64(GetCurrentProcess(), 0, name, 0, base, size))
  {
    _snprintf(buf, sizeof(buf), "Debug loaded Symbols for module: %s (0x%08I64X, %u)\n", name, base, size);
    OutputDebugString(buf);

#ifdef DEBUG_SYMBOLS
    printf(buf);
#endif
  }
  else
  {
    _snprintf(buf, sizeof(buf), "Debug lailed to load Symbols for module: %s (%i)\n", name, GetLastError());
    OutputDebugString(buf);

#ifdef DEBUG_SYMBOLS
    printf(buf);
#endif
  }

  return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Initialize()
//
//  written by: Rob Wyatt
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool Debug::Initialize()
{
  if ( !g_Initialized )
  {
    char module[MAX_PATH];
    char drive[MAX_PATH];
    char path[MAX_PATH];
    char file[MAX_PATH];
    char ext[MAX_PATH];
    GetModuleFileName(0,module,MAX_PATH);
    _splitpath(module,drive,path,file,ext);

    char dir[MAX_PATH];
    strcpy(dir,drive);
    strcat(dir,path);

    DWORD options = SYMOPT_FAIL_CRITICAL_ERRORS |
                    SYMOPT_DEFERRED_LOADS |
                    SYMOPT_LOAD_LINES |
                    SYMOPT_UNDNAME;

    SymSetOptions(options);

    // initialize symbols (dbghelp.dll)
    if ( SymInitialize(GetCurrentProcess(), dir, options & SYMOPT_DEFERRED_LOADS) == 0 )
    {
      GetLastError();  // pop the value
      return false;
    }

    // if we are not deferred load, walk our loaded modules and load debug info
    if (!(options & SYMOPT_DEFERRED_LOADS))
    {
      // load debugging information
      EnumerateLoadedModules64(GetCurrentProcess(), &EnumerateLoadedModulesProc, NULL);
    }

    // success
    g_Initialized = true;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IsInitialized()
//
//  written by: Rob Wyatt
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool Debug::IsInitialized()
{
  return g_Initialized;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetSymbolInfo()
//
//  written by: Rob Wyatt
//
////////////////////////////////////////////////////////////////////////////////////////////////
std::string Debug::GetSymbolInfo(DWORD64 adr)
{
  Debug::Initialize();

  // module image name "reflect.dll"
  static char module[MAX_PATH];
  ZeroMemory(&module, sizeof(module));
  static char extension[MAX_PATH];
  ZeroMemory(&extension, sizeof(extension));

  // symbol name "Reflect::Class::AddSerializer + 0x16d"
  static char symbol[MAX_SYM_NAME+16];
  ZeroMemory(&symbol, sizeof(symbol));

  // source file name "typeinfo.cpp"
  static char filename[MAX_PATH];
  ZeroMemory(&filename, sizeof(filename));

  // line number in source "246"
//  DWORD line = 0xFFFFFFFF;

  // resulting line is worst case of all components
  static char result[sizeof(module) + sizeof(symbol) + sizeof(filename) + 64];
  ZeroMemory(&result, sizeof(result));


  //
  // Start by finding the module the address is in
  //

  IMAGEHLP_MODULE64 moduleInfo;
  ZeroMemory(&moduleInfo, sizeof(IMAGEHLP_MODULE64));
  moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
  //if (SymGetModuleInfo64(GetCurrentProcess(), adr, &moduleInfo))
  if ( 1 )
  {
    // success, copy the module info
    _splitpath(moduleInfo.ImageName, NULL, NULL, module, extension);
    strcat(module, extension);

    //
    // Now find symbol information
    //

    // displacement of the symbol
    DWORD64 disp;

    // okay, the name runs off the end of the structure, so setup a buffer and cast it
    ULONG64 buffer[(sizeof(SYMBOL_INFO) + MAX_SYM_NAME*sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64)];
    PSYMBOL_INFO symbolInfo = (PSYMBOL_INFO)buffer;
    symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbolInfo->MaxNameLen = MAX_SYM_NAME;

    if ( SymFromAddr(GetCurrentProcess(), adr, &disp, symbolInfo) != 0 )
    {
      // success, copy the symbol info
      sprintf(symbol, "%s + 0x%x", symbolInfo->Name, disp);

      //
      // Now find source line information
      //

      DWORD d;
      IMAGEHLP_LINE64 l;
      ZeroMemory(&l,sizeof(l));
      l.SizeOfStruct = sizeof(l);
      if ( SymGetLineFromAddr64(GetCurrentProcess(), adr, &d, &l) !=0 )
      {
        // success, copy the source file name
        strcpy(filename, l.FileName);
        static char ext[MAX_PATH];
        static char file[MAX_PATH];
        _splitpath(filename, NULL, NULL, file, ext);

        sprintf(result, "%s, %s : %s%s(%d)", module, symbol, file, ext, l.LineNumber);
        return result;
      }

      sprintf(result, "%s, %s", module, symbol);
      return result;
    }

    sprintf(result, "%s", module);
    return result;
  }
  else
  {
    GetLastError();  // pop the value
    return "Unknown";
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetStackTrace()
//
//  written by: Geoff Evans
//
//  Some techniques borrowed from Visual Leak Detector 1.9
//   (http://www.codeproject.com/tools/visualleakdetector.asp)
//
////////////////////////////////////////////////////////////////////////////////////////////////

bool Debug::GetStackTrace(V_StackFrame& trace, unsigned omitFrames)
{
#if 0
  CONTEXT context;

  volatile char *p = 0;
  __try
  {
    *p = 0;
  }
  __except(CopyMemory(&context, (GetExceptionInformation())->ContextRecord, sizeof(context)), EXCEPTION_EXECUTE_HANDLER)
  {

  }
#endif

  CONTEXT context;
  memset( &context, 0, sizeof(CONTEXT) );
  context.ContextFlags = CONTEXT_CONTROL;
  RtlCaptureContext( &context );

  //return GetStackTrace(&context, trace, omitFrames+1);
  return GetStackTrace(&context, trace, 0);
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetStackTrace()
//
//  written by: Rob Wyatt
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool Debug::GetStackTrace(LPCONTEXT context, V_StackFrame& stack, unsigned omitFrames)
{
  Debug::Initialize();

  if (omitFrames == 0)
  {
    // our our current location as the top of the stack
    stack.push_back( context->IPREG );
  }
  else
  {
    omitFrames--;
  }

  // this is for handling stack overflows
  std::map<i64, u32> visited;

  // setup the stack frame to use for traversal
  STACKFRAME64 frame;
  memset(&frame,0,sizeof(frame));
  frame.AddrPC.Offset = context->IPREG;
  frame.AddrPC.Segment = 0;
  frame.AddrPC.Mode = AddrModeFlat;
  frame.AddrStack.Offset = context->SPREG;
  frame.AddrStack.Segment = 0;
  frame.AddrStack.Mode = AddrModeFlat;
  frame.AddrFrame.Offset = context->BPREG;
  frame.AddrFrame.Segment = 0;
  frame.AddrFrame.Mode = AddrModeFlat;

  // make a copy here because StackWalk64 can modify the one you give it
  CONTEXT context_copy;
  memcpy(&context_copy, context, sizeof(context_copy));

  while(1)
  {
    if (!StackWalk64(IMAGE_FILE_ARCH,
                     GetCurrentProcess(),
                     GetCurrentThread(),
                     &frame,
                     &context_copy,
                     NULL,
                     SymFunctionTableAccess64,
                     SymGetModuleBase64,
                     NULL))
    {
      break;
    }

    if (frame.AddrReturn.Offset == 0x0 || frame.AddrReturn.Offset == 0xffffffffcccccccc)
    {
      break;
    }

    if (visited[ frame.AddrReturn.Offset ]++ >= 8192)
    {
      break;
    }

#ifdef DEBUG_SYMBOLS
    printf( "0x%08I64X - %s\n", frame.AddrReturn.Offset, GetSymbolInfo(frame.AddrReturn.Offset).c_str() );
#endif

    if (omitFrames == 0)
    {
      //stack.push_back( frame.AddrReturn.Offset );
      stack.push_back( frame.AddrPC.Offset );
    }
    else
    {
      omitFrames--;
    }
  }

  return !stack.empty();
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Translate()
//
//  written by: Geoff Evans
//
//  Converts a list of addresses into a string
//
////////////////////////////////////////////////////////////////////////////////////////////////
void Debug::Translate(const V_StackFrame& trace, std::string& buffer)
{
  V_StackFrame::const_iterator itr = trace.begin();
  V_StackFrame::const_iterator end = trace.end();
  for ( ; itr != end; ++itr )
  {
    Debug::Buffer(buffer, "0x%08I64X - %s\n", *itr, GetSymbolInfo(*itr).c_str() );
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FindException()
//
//  written by: Geoff Evans
//
//  Extracts a potential c++ exception pointer from a potential 64 bit address
//
////////////////////////////////////////////////////////////////////////////////////////////////
static std::exception* FindException(DWORD64 addr)
{
  std::exception* cppException = (std::exception*)addr;

  __try
  {
    // if its non-null
    if (cppException)
    {
      // this will explode if the address isn't really a c++ exception (std::exception)
      cppException->what();
    }

    // i guess we lived!
    return cppException;
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
    // uh oh, somebody is throwing register types or another root struct or class
    return NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetExceptionInfo()
//
//  written by: Geoff Evans
//
////////////////////////////////////////////////////////////////////////////////////////////////

std::string Debug::GetExceptionInfo(LPEXCEPTION_POINTERS info)
{
  bool cpp = false;
  std::string buffer;

  Buffer(buffer, "\nAn unhandled exception has occured\n");

  const char* ex_name = NULL;
  switch (info->ExceptionRecord->ExceptionCode)
  {
  case EXCEPTION_ACCESS_VIOLATION:
    ex_name = "EXCEPTION_ACCESS_VIOLATION";
    break;

  case EXCEPTION_BREAKPOINT:
    ex_name = "EXCEPTION_BREAKPOINT";
    break;

  case EXCEPTION_SINGLE_STEP:
    ex_name = "EXCEPTION_SINGLE_STEP";
    break;

  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    ex_name = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
    break;

  case EXCEPTION_FLT_DENORMAL_OPERAND:
    ex_name = "EXCEPTION_FLT_DENORMAL_OPERAND";
    break;

  case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    ex_name = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
    break;

  case EXCEPTION_FLT_INEXACT_RESULT:
    ex_name = "EXCEPTION_FLT_INEXACT_RESULT";
    break;

  case EXCEPTION_FLT_INVALID_OPERATION:
    ex_name = "EXCEPTION_FLT_INVALID_OPERATION";
    break;

  case EXCEPTION_FLT_OVERFLOW:
    ex_name = "EXCEPTION_FLT_OVERFLOW";
    break;

  case EXCEPTION_FLT_STACK_CHECK:
    ex_name = "EXCEPTION_FLT_STACK_CHECK";
    break;

  case EXCEPTION_FLT_UNDERFLOW:
    ex_name = "EXCEPTION_FLT_UNDERFLOW";
    break;

  case EXCEPTION_INT_DIVIDE_BY_ZERO:
    ex_name = "EXCEPTION_INT_DIVIDE_BY_ZERO";
    break;

  case EXCEPTION_INT_OVERFLOW:
    ex_name = "EXCEPTION_INT_OVERFLOW";
    break;

  case EXCEPTION_PRIV_INSTRUCTION:
    ex_name = "EXCEPTION_PRIV_INSTRUCTION";
    break;

  case EXCEPTION_IN_PAGE_ERROR:
    ex_name = "EXCEPTION_IN_PAGE_ERROR";
    break;

  case EXCEPTION_ILLEGAL_INSTRUCTION:
    ex_name = "EXCEPTION_ILLEGAL_INSTRUCTION";
    break;

  case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    ex_name = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
    break;

  case EXCEPTION_STACK_OVERFLOW:
    ex_name = "EXCEPTION_STACK_OVERFLOW";
    break;

  case EXCEPTION_INVALID_DISPOSITION:
    ex_name = "EXCEPTION_INVALID_DISPOSITION";
    break;

  case EXCEPTION_GUARD_PAGE:
    ex_name = "EXCEPTION_GUARD_PAGE";
    break;

  case EXCEPTION_INVALID_HANDLE:
    ex_name = "EXCEPTION_INVALID_HANDLE";
    break;

  case 0xC00002B5:
    ex_name = "Multiple floating point traps";
    break;

  default:
    break;
  }

  if (ex_name == NULL)
  {
    ex_name = "Unknown Exception";
  }

  if (info->ExceptionRecord->ExceptionCode == 0xE06D7363) // Microsoft C++ Exception code
  {
    cpp = true;

    const std::exception* cppException = FindException(info->ExceptionRecord->ExceptionInformation[1]);
    if (cppException)
    {
      const char* type = NULL;

      try
      {
        type = typeid(*cppException).name();
      }
      catch (const std::__non_rtti_object&)
      {

      }

      if (type)
      {
        Buffer(buffer, "\nC++ Exception, Type '%s'\n\nException Message:\n%s\n", type, cppException->what());
      }
      else
      {
        Buffer(buffer, "\nC++ Exception, Unknown Type\n\nException Message:\n%s\n", cppException->what());
      }
    }
    else
    {
      Buffer(buffer, "\nThrown object is not a std::exception\n");
    }

    info->ContextRecord->IPREG = (DWORD)info->ExceptionRecord->ExceptionInformation[2];
  }
  else
  {
    Buffer(buffer, "%s [%x]\n", ex_name, info->ExceptionRecord->ExceptionCode);

    if (info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
    {
      // We can get more info about this type of violation
      Buffer(buffer, "\nAttempt to %s address 0x%08X\n", (info->ExceptionRecord->ExceptionInformation[0]==1)?"write to":"read from", info->ExceptionRecord->ExceptionInformation[1]);
    }
  }

  if (!cpp)
  {
    Buffer(buffer, "\n");

    if (info->ContextRecord->ContextFlags & CONTEXT_CONTROL)
    {
      Buffer(buffer, "Control Registers:\n"
                     "  EIP = 0x%08X  ESP = 0x%08X\n"
                     "  EBP = 0x%08X  EFL = 0x%08X\n",
        info->ContextRecord->IPREG,
        info->ContextRecord->SPREG,
        info->ContextRecord->BPREG,
        info->ContextRecord->EFlags);
    }

    if (info->ContextRecord->ContextFlags & CONTEXT_INTEGER)
    {
      Buffer(buffer, "Integer Registers:\n"
                     "  EAX = 0x%08X  EBX = 0x%08X\n"
                     "  ECX = 0x%08X  EDX = 0x%08X\n"
                     "  ESI = 0x%08X  EDI = 0x%08X\n",
        info->ContextRecord->AXREG,
        info->ContextRecord->BXREG,
        info->ContextRecord->CXREG,
        info->ContextRecord->DXREG,
        info->ContextRecord->SIREG,
        info->ContextRecord->DIREG);
    }
  }

  Buffer(buffer, "\nCall Stack:\n");
  
  V_StackFrame trace;
  if ( GetStackTrace(info->ContextRecord, trace) )
  {
    Translate(trace, buffer);
  }
  else
  {
    Buffer(buffer, "No call stack info\n");
  }

  return buffer;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  WriteDump()
//
//  written by: Geoff Evans
//
////////////////////////////////////////////////////////////////////////////////////////////////

std::string Debug::WriteDump(LPEXCEPTION_POINTERS info, bool full)
{
  char module[MAX_PATH];
  char drive[MAX_PATH];
  char path[MAX_PATH];
  char file[MAX_PATH];
  char ext[MAX_PATH];
  GetModuleFileName(0,module,MAX_PATH);
  _splitpath(module,drive,path,file,ext);

  char dmp_file[MAX_PATH];
  strcpy(dmp_file,drive);
  strcat(dmp_file,path);
  strcat(dmp_file,file);
  strcat(dmp_file,".dmp");

  HANDLE dmp;
  dmp = CreateFile(dmp_file,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
  if (dmp!=INVALID_HANDLE_VALUE)
  {
    MINIDUMP_EXCEPTION_INFORMATION ex;
    ex.ClientPointers = true;
    ex.ExceptionPointers = info;
    ex.ThreadId = GetCurrentThreadId();

    _MINIDUMP_TYPE type;

    if (full)
    {
      type = MiniDumpWithFullMemory;
    }
    else
    {
      type = MiniDumpNormal;
    }

    // generate the minidump
    MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),dmp,type,&ex,0,0);

    // close the file
    CloseHandle(dmp);

    return dmp_file;
  }

  return "";
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Buffer()
//
//  written by: Geoff Evans
//
////////////////////////////////////////////////////////////////////////////////////////////////
void Debug::Buffer(std::string& buffer, const char* str, ...)
{
  static char buf[4096];

  va_list argptr;
  va_start(argptr, str);
  vsnprintf(buf, sizeof(buf), str, argptr);
  va_end(argptr);

  buffer += buf;
}

}
