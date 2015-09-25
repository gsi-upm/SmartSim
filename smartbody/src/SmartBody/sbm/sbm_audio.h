/*
 *  sbm_audio.h - part of SmartBody-lib
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


#ifndef __SBM_AUDIO_H
#define __SBM_AUDIO_H

#include <sb/SBTypes.h>

SBAPI bool AUDIO_Init();
void AUDIO_Play( const char * audio_file );
void AUDIO_Stop( const char * audio_file );
SBAPI void AUDIO_Close();


#endif // __SBM_AUDIO_H
