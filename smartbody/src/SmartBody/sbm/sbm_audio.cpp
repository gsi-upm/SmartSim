/*
 *  sbm_audio.cpp - part of SmartBody-lib
 *  Copyright (C) 2008  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Ed Fast, USC
 */

// sbm_audio.cpp

#include "vhcl.h"

#include "sbm_audio.h"

#include "vhcl_audio.h"
#include <sstream>
#include <cstdlib>


vhcl::Audio * g_audio = NULL;


bool AUDIO_Init()
{
   if(g_audio)
      return false;

   g_audio = new vhcl::Audio();
   bool ret = g_audio->Open();
   return ret;
}


void AUDIO_Play( const char * audio_file )
{
   vhcl::Sound * sound = g_audio->CreateSoundLibSndFile( audio_file, audio_file );
   if ( sound )
   {
      //LOG("has sound, sound = %d",sound);
      sound->Play();
   }
}

void AUDIO_Stop( const char * audio_file )
{
   if (g_audio)
	   g_audio->DestroySound(audio_file);
}


void AUDIO_Close()
{
   if (!g_audio)
	   return;
   g_audio->Close();
   delete g_audio;
   g_audio = NULL;
}
