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

#include "vhcl.h"


#if !defined(ANDROID_BUILD) && !defined(FLASH_BUILD)
//#if defined(WIN_BUILD)
//#if 1


#include "vhcl_audio.h"

#if defined(IPHONE_BUILD)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <AL/alut.h>
#elif defined(MAC_BUILD)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <AL/alut.h>
#elif defined(LINUX_BUILD)
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#else
#include "al.h"
#include "alc.h"
#include "alut.h"
#endif

#include "sndfile.h"


using namespace vhcl;


bool Audio::Open()
{
   //ALint error;
   ALCcontext * context;
   ALCdevice * device;

   //ALboolean bOffsetExt = AL_FALSE;
   //ALboolean bNewDistModels = AL_FALSE;
   //ALboolean bCaptureExt = AL_FALSE;

   // Init device/context

   // Open Preferred device
   device = alcOpenDevice( NULL );
   if ( device == NULL )
   {
      printf( "Failed to Initialize Open AL\n" );
      return false;
   }

   //Create context(s)
   context = alcCreateContext( device, NULL );
   if ( context == NULL )
   {
      printf( "Failed to initialize Open AL\n" );
      return false;
   }

   //Set active context
   alcGetError( device );
   alcMakeContextCurrent( context );
   if ( alcGetError( device ) != ALC_NO_ERROR )
   {
      printf( "Failed to Make Context Current\n" );
      return false;
   }

   // Clear Error Code
   alGetError();
   alcGetError( device );


   alutInitWithoutContext( NULL, NULL );


   // Set Listener defaults
   alListener3f( AL_POSITION, 0.0, 0.0, 0.0 );     // HandleALError("alListenerfv POSITION");
   alListener3f( AL_VELOCITY, 0.0, 0.0, 0.0 );     // HandleALError("alListenerfv VELOCITY");
   ALfloat listenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
   alListenerfv( AL_ORIENTATION, listenerOri );  // HandleALError("alListenerfv ORIENTATION");
   alListenerf( AL_GAIN, 1.0f );


   // Generate Buffers
//   alGenBuffers(NUM_BUFFERS, g_Buffers);
//   if ((error = alGetError()) != AL_NO_ERROR)
   {
//      DisplayALError("alGenBuffers :", error);
//      return false;
   }


   alGetError();
   for ( ;; )
   {
      ALuint uiSource;
      alGenSources( 1, &uiSource );
      if ( alGetError() != AL_NO_ERROR )
      {
         break;
      }

      m_channels.push_back( std::make_pair( uiSource, (Sound *)NULL ) );

      if ( m_channels.size() >= MAX_CHANNELS )
      {
         break;
      }
   }

   return true;
}


void Audio::Close()
{
   for ( size_t i = 0; i < m_channels.size(); i++ )
   {
      uint32_t source = m_channels[ i ].first;
      alSourceStop( source );
      alSourcei( source, AL_BUFFER, 0 );
      alDeleteSources( 1, &source );
   }

   m_channels.clear();

   for ( size_t i = 0; i < m_sounds.size(); i++ )
   {
      const uint32_t buffer = m_sounds[ i ]->GetBuffer();
      alDeleteBuffers( 1, &buffer );

      delete m_sounds[ i ];
   }

   m_sounds.clear();


   alutExit();


   ALCcontext * context = alcGetCurrentContext();

   //Get device for active context
   ALCdevice * device = alcGetContextsDevice( context );

   //Disable context
   alcMakeContextCurrent( NULL );

   //Release context(s)
   alcDestroyContext( context );

   //Close device
   alcCloseDevice( device );
}


void Audio::SetListenerPos( const float x, const float y, const float z )
{
   alListener3f( AL_POSITION, x, y, z );
}


void Audio::GetListenerPos( float & x, float & y, float & z )
{
   alGetListener3f( AL_POSITION, &x, &y, &z );
}


void Audio::SetListenerRot( const float targetx, const float targety, const float targetz, const float upx, const float upy, const float upz )
{
   float rot[] = { targetx, targety, targetz, upx, upy, upz };
   alListenerfv( AL_ORIENTATION, rot );
}


void Audio::GetListenerRot( float & targetx, float & targety, float & targetz, float & upx, float & upy, float & upz )
{
   float rot[ 6 ];
   alGetListenerfv( AL_ORIENTATION, rot );
   targetx = rot[ 0 ];
   targety = rot[ 1 ];
   targetz = rot[ 2 ];
   upx = rot[ 3 ];
   upy = rot[ 4 ];
   upz = rot[ 5 ];
}


Sound * Audio::CreateSound( const std::string & fileName, const std::string & name )
{
   ALuint uiBuffer = alutCreateBufferFromFile( fileName.c_str() );

   Sound * s = new Sound( this, uiBuffer, name, fileName );
   m_sounds.push_back( s );

   return s;
}


Sound * Audio::CreateSoundLibSndFile( const std::string & fileName, const std::string & name )
{
   SF_INFO info = { 0 };
   SNDFILE * file = sf_open( fileName.c_str(), SFM_READ, &info );
   if ( file == NULL )
   {
      return NULL;
   }

   int channels   = info.channels;
   int sampleRate = info.samplerate;
   int numSamples = (int)info.frames * channels;  // frames is int64 here

   int16_t * data = new int16_t [ numSamples ];
   sf_read_short( file, data, numSamples );


   ALuint alBuffer;
   alGenBuffers( 1, &alBuffer );


   ALenum format;
   if ( channels == 1 )       format = AL_FORMAT_MONO16;
   else if ( channels == 2 )  format = AL_FORMAT_STEREO16;
   else                       format = 0;

   ALsizei size = numSamples * sizeof( int16_t );

   alBufferData( alBuffer, format, data, size, sampleRate );


   delete [] data;  data = NULL;
   sf_close( file );


   Sound * s = new Sound( this, alBuffer, name, fileName );
   m_sounds.push_back( s );

   return s;
}


void Audio::DestroySound( const std::string & name )
{
   std::string todelete = name;
   for ( size_t i = 0; i < m_sounds.size(); i++ )
   {
      if ( m_sounds[ i ]->GetName() == todelete )
      {
         ReleaseSoundFromChannel( m_sounds[ i ] );

         uint32_t buffer = m_sounds[ i ]->GetBuffer();
         alDeleteBuffers( 1, &buffer );

         delete m_sounds[ i ];

         m_sounds.erase( m_sounds.begin() + i );
      }
   }
}

Sound * Audio::FindSound( const std::string & name )
{
   for ( size_t i = 0; i < m_sounds.size(); i++ )
   {
      if ( m_sounds[ i ]->GetName() == name )
      {
         return m_sounds[ i ];
      }
   }

   return NULL;
}


void Audio::PauseAllSounds()
{
   for ( size_t i = 0; i < m_sounds.size(); i++ )
   {
      if ( m_sounds[ i ]->IsPlaying())
      {
         m_sounds[i]->Pause();
      }
   }
}


void Audio::UnpauseAllSounds()
{
   for ( size_t i = 0; i < m_sounds.size(); i++ )
   {
      if ( m_sounds[ i ]->IsPaused())
      {
         m_sounds[i]->Play();
      }
   }
}

void Audio::StopAllSounds()
{
   for ( size_t i = 0; i < m_sounds.size(); i++ )
   {
      if ( m_sounds[ i ]->IsPlaying())
      {
         m_sounds[i]->Stop();
      }
   }
}







void Audio::Update( const float frameTime )
{
}


bool Audio::AttachSoundToFreeChannel( Sound * sound )
{
   if ( sound == NULL )
      return false;

   if ( sound->GetChannel() != AL_NONE )
      return true;

   // find channel without sound attached
   for ( size_t i = 0; i < m_channels.size(); i++ )
   {
      if ( m_channels[ i ].second == NULL )
      {
         m_channels[ i ].second = sound;
         sound->SetChannel( m_channels[ i ].first );
         return true;
      }
   }

   // find sound that's stopped
   for ( size_t i = 0; i < m_channels.size(); i++ )
   {
      if ( m_channels[ i ].second->IsStopped() )
      {
         // TODO put m_channels[ i ].second in a queue to restart when channels free up

         m_channels[ i ].second->SetChannel( AL_NONE );

         m_channels[ i ].second = sound;
         sound->SetChannel( m_channels[ i ].first );
         return true;
      }
   }

   // TODO find sound with lower priority
   // TODO find sound with further distance

   return false;
}


void Audio::ReleaseSoundFromChannel( Sound * sound )
{
   sound->Stop();

   for ( size_t i = 0; i < m_channels.size(); i++ )
   {
      if ( m_channels[ i ].second == sound )
      {
         ASSERT( sound->GetChannel() == m_channels[ i ].first );

         m_channels[ i ].second = NULL;
      }
   }

   sound->SetChannel( AL_NONE );
}






Sound::Sound( Audio * audio, const uint32_t alBuffer, const std::string & name, const std::string & fileName )
{
   m_audio = audio;
   m_alBuffer = alBuffer;
   m_name = name;
   m_fileName = fileName;

   m_loop = false;
   m_x = 0;
   m_y = 0;
   m_z = 0;

   m_alSource = AL_NONE;
}


void Sound::Play()
{
   if ( IsPlaying() )
      return;

   if ( m_alSource == AL_NONE )
   {
      bool success = m_audio->AttachSoundToFreeChannel( this );
      if ( !success )
         return;
   }


   // FOR GUNSLINGER ONLY - for setting the hardware channel used by ARIA
   if ( !m_outputHardwareChannel.empty() )
   {
      int val = alIsExtensionPresent( (ALchar *)"AL_ICT_source_outputchannel" );
      if ( val == AL_TRUE )
      {
         ALuint AL_OUTPUTCHANNEL_ICT = alGetEnumValue( (const ALchar *)"AL_OUTPUTCHANNEL_ICT" );

         typedef ALint (*alGetOutputChannelICTFunc)( const ALubyte* channelName );

         alGetOutputChannelICTFunc alGetOutputChannelICT = (alGetOutputChannelICTFunc)alGetProcAddress( (const ALchar *)"ICT_IA_SR_Output_Channels" );

         //std::string channelString = vhcl::Format( "%d", m_outputHardwareChannel );

         ALint OUTPUT_CHANNEL = alGetOutputChannelICT( (const ALubyte *)m_outputHardwareChannel.c_str() );  //Should return 1

         if ( OUTPUT_CHANNEL != -1 )
         {
            alSourcei( m_alSource, AL_OUTPUTCHANNEL_ICT, OUTPUT_CHANNEL );
         }
      }
   }


   alSourcePlay( m_alSource );
}


void Sound::Pause()
{
   if ( m_alSource == AL_NONE )
      return;

   alSourcePause( m_alSource );
}


void Sound::Stop()
{
   if ( m_alSource == AL_NONE )
      return;

   alSourceStop( m_alSource );
}


bool Sound::IsPlaying() const
{
   if ( m_alSource == AL_NONE )
      return false;

   ALint state;
   alGetSourcei( m_alSource, AL_SOURCE_STATE, &state );

   return state == AL_PLAYING;
}


bool Sound::IsPaused() const
{
   if ( m_alSource == AL_NONE )
      return false;

   ALint state;
   alGetSourcei( m_alSource, AL_SOURCE_STATE, &state );

   return state == AL_PAUSED;
}


bool Sound::IsStopped() const
{
   if ( m_alSource == AL_NONE )
      return false;

   ALint state;
   alGetSourcei( m_alSource, AL_SOURCE_STATE, &state );

   return state == AL_STOPPED;
}


void Sound::SetLooping( const bool loop )
{
   m_loop = loop;

   if ( m_alSource != AL_NONE )
   {
      alSourcei( m_alSource, AL_LOOPING, m_loop );
   }
}


void Sound::SetPosition( const float x, const float y, const float z )
{
   m_x = x;
   m_y = y;
   m_z = z;

   if ( m_alSource != AL_NONE )
   {
      alSource3f( m_alSource, AL_POSITION, m_x, m_y, m_z );
   }
}


void Sound::SetChannel( const uint32_t alChannel )
{
   m_alSource = alChannel;

   if ( alChannel == AL_NONE )
   {
      return;
   }


   alSourcei( m_alSource, AL_BUFFER, m_alBuffer );

   alSourcei( m_alSource, AL_LOOPING, m_loop );
   alSource3f( m_alSource, AL_POSITION, m_x, m_y, m_z );


   alSourcef( m_alSource, AL_REFERENCE_DISTANCE, 1.0f );
   alSourcef( m_alSource, AL_ROLLOFF_FACTOR, 1.0f );
}


#else  // WIN_BUILD

int vhcl_audio_unused_ignore_xxx = 0;

#endif  // WIN32_BUILD
