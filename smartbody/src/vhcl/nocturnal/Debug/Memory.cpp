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

#include "memory.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <new.h>
#include <sstream>

//#include "Console/Console.h"
//#include "Common/Exception.h"


using namespace nocturnal::Debug;

namespace nocturnal
{

bool MemoryProfiler::s_Enabled = false;
MemoryStatus MemoryProfiler::s_Last;

MemoryProfiler::MemoryProfiler()
{
  if (s_Enabled)
  {
    GetMemoryStatus(&m_Start);
  }
}

MemoryProfiler::~MemoryProfiler()
{
  if (s_Enabled)
  {
    MemoryStatus finish;
    GetMemoryStatus(&finish);
#if 0
    PointerSizedInt delta = finish.m_TotalCommit - s_Last.m_TotalCommit;
    PointerSizedInt scope = finish.m_TotalCommit - m_Start.m_TotalCommit;
    Console::Profile("Memory Commit: %u M bytes (Delta: %i K, %i K within scope)\n", (finish.m_TotalCommit) >> 20, (delta) >> 10, (scope) >> 10);
#endif
    s_Last = finish;
  }
}

void MemoryProfiler::EnableProfiling(bool enable)
{
  s_Enabled = enable;
}

void Debug::GetMemoryStatus(MemoryStatus* status)
{
  PointerSizedUInt base = 0;
  PointerSizedUInt res = 1;
  while (res)
  {
    MEMORY_BASIC_INFORMATION mem;
    res = VirtualQueryEx(GetCurrentProcess(), (void*)base, &mem, sizeof(mem));
    if (res)
    {
      if (mem.State & MEM_FREE)
      {
        status->m_TotalFree += mem.RegionSize;

        if (mem.RegionSize > status->m_LargestFree)
        {
          status->m_LargestFree = mem.RegionSize;
        }
      }
      else
      {
        if (mem.State & MEM_RESERVE)
        {
          status->m_TotalReserve += mem.RegionSize;
        }

        if (mem.State & MEM_COMMIT)
        {
          status->m_TotalCommit += mem.RegionSize;
        }
      }

      base += mem.RegionSize;
    }
  }
}

void Debug::GetMemoryStatus(std::string& buffer)
{
  MemoryStatus status;
  GetMemoryStatus(&status);

  try
  {
    std::ostringstream str;

    str << std::endl << "Memory:" << std::endl;
    str << "Total Reserved " << (status.m_TotalReserve>>10)   << "K bytes" << std::endl;
    str << "Total Commit   " << (status.m_TotalCommit>>10)    << "K bytes" << std::endl;
    str << "Total Free     " << (status.m_TotalFree>>10)      << "K bytes" << std::endl;
    str << "Largest Free   " << (status.m_LargestFree>>10)    << "K bytes" << std::endl;

    buffer += str.str();
  }
  catch (std::exception&)
  {

  }
}

#if 0
int NewError( size_t size )
{
  throw Nocturnal::Exception("Failed to allocate %d bytes", size);
}

void Debug::EnableNewErrorHandling(bool enable)
{
  if (enable)
  {
    _set_new_handler( &NewError );
  }
  else if (_query_new_handler() == &NewError)
  {
    _set_new_handler( NULL );
  }
}
#endif

}
