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

#ifndef VHCL_AUDIO_H
#define VHCL_AUDIO_H


#if !defined(ANDROID_BUILD) && !defined(FLASH_BUILD)
//#if defined(WIN_BUILD)
//#if 1


// Requires linking with the following libs:
//   lib\vhcl\openal\libs\Win32\OpenAL32.lib
//   lib\vhcl\openal\libs\Win32\alut.lib
//   lib\vhcl\libsndfile-1_0_17\libsndfile-1.lib
//
// Requires the following .dlls:
//   lib\vhcl\openal\libs\Win32\OpenAL32.dll
//   lib\vhcl\openal\libs\Win32\alut.dll
//   lib\vhcl\libsndfile-1_0_17\libsndfile-1.dll


namespace vhcl
{

class Audio;


class Sound
{
   private:
      Audio * m_audio;
      uint32_t m_alBuffer;
      std::string m_name;
      std::string m_fileName;

      bool  m_loop;
      float m_x;
      float m_y;
      float m_z;

      uint32_t m_alSource;


      // FOR GUNSLINGER ONLY - for setting the hardware channel used by ARIA
      std::string m_outputHardwareChannel;


      // TODO auto release flag
      // TODO clone flag

   public:
      Sound( Audio * audio, const uint32_t alBuffer, const std::string & name, const std::string & fileName );

      void Play();
      void Pause();
      void Stop();

      bool IsPlaying() const;
      bool IsPaused() const;
      bool IsStopped() const;


      void SetLooping( const bool loop );
      void SetPosition( const float x, const float y, const float z );


      const std::string & GetName() const { return m_name; }
      uint32_t GetBuffer() const { return m_alBuffer; }


      uint32_t GetChannel() const { return m_alSource; }
      void SetChannel( const uint32_t alChannel );


      // FOR GUNSLINGER ONLY - for setting the hardware channel used by ARIA
      void SetHardwareChannel( const std::string & channel){ m_outputHardwareChannel = channel; }
      const std::string & GetHardwareChannel() const { return m_outputHardwareChannel; }
};


class Audio
{
   private:
      static const int MAX_CHANNELS = 256;

   private:
      std::vector< std::pair< uint32_t, Sound * > > m_channels;
      std::vector< Sound * > m_sounds;

   public:
      Audio() {}
      ~Audio() { Close(); }

      bool Open();
      void Close();

      void SetListenerPos( const float x, const float y, const float z );
      void GetListenerPos( float & x, float & y, float & z );

      void SetListenerRot( const float targetx, const float targety, const float targetz, const float upx, const float upy, const float upz );
      void GetListenerRot( float & targetx, float & targety, float & targetz, float & upx, float & upy, float & upz );

      Sound * CreateSound( const std::string & fileName, const std::string & name );
      Sound * CreateSoundLibSndFile( const std::string & fileName, const std::string & name );
      // Sound * CloneSound( const std::string & name );
      void DestroySound( const std::string & name );
      Sound * FindSound( const std::string & name );
      void PauseAllSounds();
      void UnpauseAllSounds();
      void StopAllSounds();


      void Update( const float frameTime );


      bool AttachSoundToFreeChannel( Sound * sound );
      void ReleaseSoundFromChannel( Sound * sound );
};

};



#else  // WIN_BUILD



namespace vhcl
{

class Audio;


class Sound
{
   private:
      std::string temp;

   public:
      Sound( Audio * audio, const uint32_t alBuffer, const std::string & name, const std::string & fileName ) {}

      void Play() {}
      void Pause() {}
      void Stop() {}

      bool IsPlaying() const { return false; }
      bool IsPaused() const { return false; }
      bool IsStopped() const { return true; }


      void SetLooping( const bool loop ) {}
      void SetPosition( const float x, const float y, const float z ) {}


      const std::string & GetName() const { return temp; }
      uint32_t GetBuffer() const { return 42; }


      uint32_t GetChannel() const { return 42; }
      void SetChannel( const uint32_t alChannel ) {}


      // FOR GUNSLINGER ONLY - for setting the hardware channel used by ARIA
      void SetHardwareChannel( const std::string & channel){}
      const std::string & GetHardwareChannel() const { return temp; }
};


class Audio
{
   private:
      static const int MAX_CHANNELS = 256;

   private:
      std::vector< std::pair< uint32_t, Sound * > > m_channels;
      std::vector< Sound * > m_sounds;

   public:
      Audio() {}
      ~Audio() { Close(); }

      bool Open() { return true; }
      void Close() {}

      void SetListenerPos( const float x, const float y, const float z ) {}
      void GetListenerPos( float & x, float & y, float & z ) {}

      void SetListenerRot( const float rx, const float ry, const float rz, const float ux, const float uy, const float uz ) {}
      void GetListenerRot( float & rx, float & ry, float & rz, float & ux, float & uy, float & uz ) {}

      Sound * CreateSound( const std::string & fileName, const std::string & name ) { return NULL; }
      Sound * CreateSoundLibSndFile( const std::string & fileName, const std::string & name ) { return NULL; }
      // Sound * CloneSound( const std::string & name );
      void DestroySound( const std::string & name ) {}
      Sound * FindSound( const std::string & name ) { return NULL; }

      void Update( const float frameTime ) {}


      bool AttachSoundToFreeChannel( Sound * sound ) { return true; }
      void ReleaseSoundFromChannel( Sound * sound ) {}
};

};



#endif  // WIN_BUILD


#endif  // VHCL_AUDIO_H
