// $Id: FluSimpleString.cpp,v 1.10 2006/02/23 12:59:20 jbryan Exp $

/***************************************************************
 *                FLU - FLTK Utility Widgets 
 *  Copyright (C) 2002 Ohio Supercomputer Center, Ohio State University
 *
 * This file and its content is protected by a software license.
 * You should have received a copy of this license with this file.
 * If not, please contact the Ohio Supercomputer Center immediately:
 * Attn: Jason Bryan Re: FLU 1224 Kinnear Rd, Columbus, Ohio 43212
 * 
 ***************************************************************/



#include "FluSimpleString.h"

#ifndef WIN32
#define _strdup strdup
#endif

FluSimpleString :: FluSimpleString()
{
  str = _strdup( "" );
}

FluSimpleString :: FluSimpleString( unsigned int len )
{
  if( len > 0 )
    {
      str = (char*)malloc( len );
      str[0] = '\0';
    }
  else
    str = _strdup( "" );
}

FluSimpleString :: FluSimpleString( const char *s )
{
  if( s )
    str = _strdup( s );
  else
    str = _strdup( "" );
}

FluSimpleString :: FluSimpleString( const FluSimpleString& s )
{
  str = _strdup( s.str );
}

FluSimpleString :: ~FluSimpleString()
{
  if(str) free(str);
}

void FluSimpleString :: copy( const char *s, unsigned int start, unsigned int len )
{
  if( len == 0 ) return;
  if( s == 0 ) return;
  if( start+len > strlen(s) ) return;
  if(str) free(str);
  str = (char*)malloc( len+1 );
  strncpy( str, s+start, len );
  str[len] = '\0';
}

int FluSimpleString :: compare( const FluSimpleString &s ) const
{
  return strcmp( str, s.str );
}

#define TOUPPER(c) (((c) >= 'a' && (c) <= 'z' ) ? (c)&0xDF : c)

int FluSimpleString :: casecompare( const FluSimpleString &s ) const
{
  //FluSimpleString s1(str), s2(s);
  //s1.upcase();
  //s2.upcase();
  //return strcmp( s1.str, s2.str );
  int i = 0, d;
  for(;;)
    {
      d = (int)TOUPPER(str[i]) - (int)TOUPPER(s.str[i]);
      if( d == 0 ) // same character
	{
	  if( str[i] == '\0' )
	    return 0; // same string
	  ++i;
	}
      else
	return d;
    }
}

void FluSimpleString :: upcase()
{
  int i = 0;
  while( str[i] )
    {
      str[i] = toupper( str[i] );
      ++i;
    }
}

void FluSimpleString :: downcase()
{
  int i = 0;
  while( str[i] )
    {
      str[i] = tolower( str[i] );
      ++i;
    }
}

int FluSimpleString :: find( char c ) const
{
  const char *i = strchr( str, c );
  if( !i )
    return -1;
  else
    return i-str;
}

int FluSimpleString :: rfind( char c ) const
{
  const char *i = strrchr( str, c );
  if( !i )
    return -1;
  else
    return i-str;
}

FluSimpleString FluSimpleString :: substr( int pos, int len ) const
{
  if( (pos+len) <= 0 || (pos+len) > size() )
    return FluSimpleString("");
  else
    {
      char *buf = (char*)malloc( len+1 );
      strncpy( buf, str+pos, len );
      buf[len] = '\0';
      FluSimpleString s = buf;
      free( buf );
      return s;
    }
}

FluSimpleString& FluSimpleString :: operator =( const char *s )
{ 
  char* tmp;
  if(s==0)
    tmp = _strdup("");
  else
    tmp = _strdup(s);
  if(str)
    free(str);
  str = tmp;
  return *this;
}

FluSimpleString& FluSimpleString :: operator +=( const char *s )
{
  if( s==0 )
    s = "";
  char *old = _strdup(str);
  int lold = strlen(old), ls = strlen(s);
  free(str);
  str = (char*)malloc( lold + ls + 1 );
  memcpy( str, old, lold );
  memcpy( str+lold, s, ls );
  str[lold+ls] = '\0';
  free(old);
  return *this;
}
