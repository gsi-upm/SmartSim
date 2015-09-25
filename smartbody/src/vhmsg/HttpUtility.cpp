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


// This code was taken from the Mono sources, and converted from C# to C++.
// Mono's copywrite notice is below:


// System.Web.HttpUtility
//
// Authors:
//   Patrik Torstensson (Patrik.Torstensson@labs2.com)
//   Wictor Wilén (decode/encode functions) (wictor@ibizkit.se)
//   Tim Coleman (tim@timcoleman.com)
//   Gonzalo Paniagua Javier (gonzalo@ximian.com)
//
// Copyright (C) 2005 Novell, Inc (http://www.novell.com)
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//


#include "vhcl.h"

#include "HttpUtility.h"


using std::string;
using std::wstring;
using std::vector;


int GetInt( const unsigned char b );
int GetChar( const string & str, const int offset, const int length );
wstring GetChars( const vector< unsigned char > & bytes, const int enc );
string GetChars_Byte( const vector< unsigned char > & bytes, const int enc );
bool NotEncoded( const wchar_t c );
void UrlEncodeChar( const wchar_t c, wstring & result, const bool isUnicode );


static const char * hexChars = "0123456789abcdef";


wstring HtppUtility_UrlDecode( const string & s )
{
   size_t len = s.length();

   int xchar;
   wstring output;
   vector< unsigned char > bytes;

   int e = 0;

   for ( size_t i = 0; i < len; i++ )
   {
      if ( s[ i ] == '%' && i + 2 < len && s[ i + 1 ] != '%' )
      {
         if ( s[ i + 1 ] == 'u' && i + 5 < len )
         {
            if ( bytes.size() > 0 )
            {
               output.append( GetChars( bytes, e ) );
               bytes.clear();
            }

            xchar = GetChar( s, (int)i + 2, 4 );
            if ( xchar != -1 )
            {
               output.push_back( (wchar_t)xchar );
               i += 5;
            }
            else
            {
               output.push_back( '%' );
            }
         }
         else if ( ( xchar = GetChar( s, (int)i + 1, 2 ) ) != -1 )
         {
            bytes.push_back( (unsigned char)xchar );
            i += 2;
         }
         else
         {
            output.push_back( '%' );
         }

         continue;
      }

      if ( bytes.size() > 0 )
      {
         output.append( GetChars( bytes, e ) );
         bytes.clear();
      }

      if ( s[ i ] == '+' )
      {
         output.push_back( ' ' );
      }
      else
      {
         output.push_back( s[ i ] );
      }
   }

   if ( bytes.size() > 0 )
   {
      output.append( GetChars( bytes, e ) );
   }

   bytes.clear();
   return output;
}


wstring HtppUtility_UrlEncode( const wstring & s )
{
   wstring result;

   size_t len = s.length();
   for ( size_t i = 0; i < len; i++ )
   {
      UrlEncodeChar( s[ i ], result, false );
   }

   return result;
}


string HtppUtility_UrlDecode_ByteChar( const string & s )
{
   size_t len = s.length();

   int xchar;
   string output;
   vector< unsigned char > bytes;

   int e = 0;

   for ( size_t i = 0; i < len; i++ )
   {
      if ( s[ i ] == '%' && i + 2 < len && s[ i + 1 ] != '%' )
      {
         if ( s[ i + 1 ] == 'u' && i + 5 < len )
         {
            if ( bytes.size() > 0 )
            {
               output.append( GetChars_Byte( bytes, e ) );
               bytes.clear();
            }

            xchar = GetChar( s, (int)i + 2, 4 );
            if ( xchar != -1 )
            {
               output.push_back( (char)(wchar_t)xchar );
               i += 5;
            }
            else
            {
               output.push_back( '%' );
            }
         }
         else if ( ( xchar = GetChar( s, (int)i + 1, 2 ) ) != -1 )
         {
            bytes.push_back( (unsigned char)xchar );
            i += 2;
         }
         else
         {
            output.push_back( '%' );
         }

         continue;
      }

      if ( bytes.size() > 0 )
      {
         output.append( GetChars_Byte( bytes, e ) );
         bytes.clear();
      }

      if ( s[ i ] == '+' )
      {
         output.push_back( ' ' );
      }
      else
      {
         output.push_back( s[ i ] );	
      }
   }

   if ( bytes.size() > 0 )
   {
      output.append( GetChars_Byte( bytes, e ) );
   }

   bytes.clear();
   return output;
}


string HtppUtility_UrlEncode_ByteChar( const string & s )
{
   return s;
}


int GetInt( const unsigned char b )
{
   char c = (char)b;
   if ( c >= '0' && c <= '9' )
      return c - '0';

   if ( c >= 'a' && c <= 'f' )
      return c - 'a' + 10;

   if ( c >= 'A' && c <= 'F' )
      return c - 'A' + 10;

   return -1;
}


int GetChar( const string & str, const int offset, const int length )
{
   int val = 0;
   int end = length + offset;
   for ( int i = offset; i < end; i++ )
   {
      wchar_t c = str[ i ];
      if ( c > 127 )
         return -1;

      int current = GetInt( (unsigned char)c );
      if ( current == -1 )
         return -1;

      val = ( val << 4 ) + current;
   }

   return val;
}


wstring GetChars( const vector< unsigned char > & bytes, const int enc )
{
   string s;
   for ( size_t i = 0; i < bytes.size(); i++ )
   {
      s += bytes[ i ];
   }

   return vhcl::Utf8ToWString( s.c_str() );
}


string GetChars_Byte( const vector< unsigned char > & bytes, const int enc )
{
   string s;
   for ( size_t i = 0; i < bytes.size(); i++ )
   {
      s += bytes[ i ];
   }

   return s;
}


bool NotEncoded( const wchar_t c )
{
   return ( c == '!' || c == '\'' || c == '(' || c == ')' || c == '*' || c == '-' || c == '.' || c == '_' );
}


void UrlEncodeChar( const wchar_t c, wstring & result, const bool isUnicode )
{
   if ( c > 255 )
   {
      int idx;
      int i = (int)c;

      result.push_back( '%' );
      result.push_back( 'u' );
      idx = i >> 12;
      result.push_back( hexChars[ idx ] );
      idx = ( i >> 8 ) & 0x0F;
      result.push_back( hexChars[ idx ] );
      idx = ( i >> 4 ) & 0x0F;
      result.push_back( hexChars[ idx ] );
      idx = i & 0x0F;
      result.push_back( hexChars[ idx ] );
      return;
   }

   if ( c > ' ' && NotEncoded( c ) )
   {
      result.push_back( c );
      return;
   }

   if ( c == ' ' )
   {
      result.push_back( '+' );
      return;
   }

   if ( ( c < '0' ) ||
      (   c < 'A' && c > '9' ) ||
      (   c > 'Z' && c < 'a' ) ||
      (   c > 'z' ) )
   {
      if ( isUnicode && c > 127 )
      {
         result.push_back( '%' );
         result.push_back( 'u' );
         result.push_back( '0' );
         result.push_back( '0' );
      }
      else
      {
         result.push_back( '%' );
      }

      int idx = ( (int)c ) >> 4;
      result.push_back( hexChars[ idx ] );
      idx = ( (int) c ) & 0x0F;
      result.push_back( hexChars[ idx ] );
   }
   else
   {
      result.push_back( c );
   }
}
