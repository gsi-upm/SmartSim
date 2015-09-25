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

#ifndef HTTPUTILITY_H
#define HTTPUTILITY_H


#include "vhcl_public.h"


std::wstring HtppUtility_UrlDecode( const std::string & s );
std::wstring HtppUtility_UrlEncode( const std::wstring & s );
std::string HtppUtility_UrlDecode_ByteChar( const std::string & s );
std::string HtppUtility_UrlEncode_ByteChar( const std::string & s );


#endif  // HTTPUTILITY_H
