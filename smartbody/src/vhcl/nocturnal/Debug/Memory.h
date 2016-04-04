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

#include "API.h"
//#include "Common/Types.h"

#include <string>

namespace nocturnal
{
namespace Debug
{
  //
  // Status Data
  //

#ifdef _WIN64
  typedef unsigned __int64    PointerSizedUInt;
  typedef __int64             PointerSizedInt;
#else
  typedef __w64 unsigned int  PointerSizedUInt;
  typedef __w64 int           PointerSizedInt;
#endif

  class MemoryStatus
  {
  public:
    PointerSizedUInt m_TotalReserve;
    PointerSizedUInt m_TotalCommit;
    PointerSizedUInt m_TotalFree;
    PointerSizedUInt m_LargestFree;

    MemoryStatus()
      : m_TotalReserve (0)
      , m_TotalCommit (0)
      , m_TotalFree (0)
      , m_LargestFree (0)
    {

    }
  };

  //
  // Profiler
  //

  class MemoryProfiler
  {
  private:
    static bool s_Enabled;
    static MemoryStatus s_Last;
    MemoryStatus m_Start;

  public:
    MemoryProfiler();
    ~MemoryProfiler();
    static void EnableProfiling(bool enable);
  };

  //
  // Query status
  //

  DEBUG_API void GetMemoryStatus(MemoryStatus* status);
  DEBUG_API void GetMemoryStatus(std::string& buffer);

  //
  // New Error Handling
  //

#if 0
  DEBUG_API void EnableNewErrorHandling(bool enable);
#endif

}
}
