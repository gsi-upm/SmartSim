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
#include <vector>


namespace nocturnal
{
namespace Debug
{
  // destination address of submitted eMail
  DEBUG_API void SetDefaultDestination(const std::string& destination);

  // source address of submitted eMail
  DEBUG_API void SetDefaultSource(const std::string& source);

  DEBUG_API void SetDefaultServer(const std::string& server);

  // dispach Mail message to auto_tools_bugs
  DEBUG_API bool SendMail(const std::string& subject,
                          const std::string& text,
                          const std::vector<std::string>& files,
                          const std::string& destination = std::string (),
                          const std::string& source = std::string ());

  // handy for single files
  inline bool SendMail(const std::string& subject,
                       const std::string& text = std::string (),
                       const std::string& file = std::string (),
                       const std::string& destination = std::string (),
                       const std::string& source = std::string ())
  {
    std::vector<std::string> files;
    if (!file.empty())
    {
      files.push_back(file);
    }
    return SendMail(subject, text, files, destination, source);
  }
}
}
