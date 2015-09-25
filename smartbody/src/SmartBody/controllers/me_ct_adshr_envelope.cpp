/*
 *  me_ct_adshr_envelope.cpp - part of SmartBody-lib's Motion Engine
 *  Copyright (C) 2010  University of Southern California
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
 *      Andrew n marshall, USC
 */

#include "vhcl.h"
#include <controllers/me_ct_adshr_envelope.hpp>

#include <cstdlib>
#include <sstream>



std::string MeCtAdshrEnvelope::CONTROLLER_TYPE = "MeCtAdshrEnvelope";



MeCtAdshrEnvelope::MeCtAdshrEnvelope()
:	_base_level( 0.0 ),
	_amplitude( 1.0 ),
	_attack( 1.0 ),
	_decay( 0.0 ),
	_sustain( 1.0 ),
	_hold( 1.0 ),
	_release( 1.0 )
{}

const std::string& MeCtAdshrEnvelope::controller_type() const {
	return MeCtAdshrEnvelope::CONTROLLER_TYPE;
}

bool only_single_float_channels( SkChannelArray& channels ) {
	bool is_valid = true;

	const int size = channels.size();
	for( int i=0;is_valid && i<size; ++ i ) {
		is_valid &= ( channels.get( i ).size() == 1 );
	}

	return is_valid;
}

bool MeCtAdshrEnvelope::init(SbmPawn* pawn, SkChannelArray& channels) {
	if( !only_single_float_channels( channels ) )
		return false;  // unsupported channel set
	
	// Copied from MeCtRawWrite
	_channels.init();
	_channels.merge( channels );
//	_channels.compress();  // save memory
	_channels.rebuild_hash_table();

	const int size = _channels.size();
	_local_ch_to_buffer.size( size );
	int index = 0;
	for( int i=0; i<size; ++ i ) {
		_local_ch_to_buffer[i] = index;
		index += SkChannel::size( _channels.type(i) );
	}

	// Initialize superclass anyway
	MeController::init (pawn);

	return true;
}

void MeCtAdshrEnvelope::amplitude( float amplitude ) {
	_amplitude = amplitude;
}

void MeCtAdshrEnvelope::attack( float attack ) {
	_attack = ( attack>0 ? attack : 0 );
	inoutdt( _attack, _release );
}

void MeCtAdshrEnvelope::decay( float decay ) {
	_decay = ( decay>0 ? decay : 0 );
}

void MeCtAdshrEnvelope::sustain( float sustain ) {
	_sustain = sustain;
}

void MeCtAdshrEnvelope::hold( float hold ) {
	_hold = ( hold>0 ? hold : 0 );
}

void MeCtAdshrEnvelope::release( float release ) {
	_release = ( release>0 ? release : 0 );
	inoutdt( _attack, _release );
}

void MeCtAdshrEnvelope::envelope( float amplitude, float attack, float decay, float sustain, float hold, float release ) {
	this->amplitude( amplitude );
	this->attack( attack );
	this->decay( decay );
	this->sustain( sustain );
	this->hold( hold );
	this->release( release );
}

SkChannelArray& MeCtAdshrEnvelope::controller_channels() {
	return _channels;
}

double MeCtAdshrEnvelope::controller_duration() {
	return _attack + _decay + _hold + _release;
}

float interpolate( float a, float b, float alpha ) {
	return a*(1-alpha) + b*alpha;
}

bool MeCtAdshrEnvelope::controller_evaluate( double time, MeFrameData& frame ) {
	// Calculate the value to write
	// (could be optimized by precomputing durations realtive to start)
	float value;
	if( time < 0 || time > controller_duration() ) {
		// Beyond elevelope bounds
		value = _base_level;
	} else {
		float ftime = (float)time;
		if( ftime < _attack ) {
			// In attack
			value = interpolate( _base_level, _amplitude, ftime/_attack );
		} else {
			ftime -= _attack;
			if( ftime < _decay ) {
				// In decay
				value = interpolate( _amplitude, _sustain, ftime/_decay );
			} else {
				ftime -= _decay;
				if( ftime < _hold ) {
					// In hold
					value = _sustain;
				} else {
					// in release
					value = interpolate( _sustain, _base_level, (ftime-_hold)/_release );
				}
			}
		}
	}

	SkChannelArray& channels = controller_channels();
	SrBuffer<float>& frame_buffer = frame.buffer();
	const int size = channels.size();
	for( int i=0; i<size; ++i ) {          // i is the local channels[] index
		int context_ch = _toContextCh[i];  // frames.channels()[ index ]
		if( context_ch != -1 ) {           // Make sure channel exist in the context
#if DEBUG_CHANNELS   // Get a reference to the channel to inspect via debugger
			SkChannel::Type ch_type = channels.type( i );
			const char*     ch_name = (const char*)(channels.name( i ));
#endif
			int buff_index = _local_ch_to_buffer[i];   // Find the local buffer index
			int frame_buffer_index = frame.toBufferIndex( context_ch );  // find the matching context's buffer index

			frame_buffer[ frame_buffer_index ] = value;  // Assumes single value float channel
			frame.channelUpdated( context_ch );
		} // else ignore
	}

	return true;
}
