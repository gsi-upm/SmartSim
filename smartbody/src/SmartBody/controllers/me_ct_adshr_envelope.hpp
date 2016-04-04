/*
 *  me_ct_adshr_envelope.hpp - part of SmartBody-lib's Motion Engine
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

#ifndef ME_CT_ADSHR_ENVELOPE_HPP
#define ME_CT_ADSHR_ENVELOPE_HPP

#include <sb/SBController.h>


/**
 *  Write a value computed by an attack/decay/sustain/hold/release envelope
 *  to all channels specified by an SkChannelArray.
 *
 *  Only supports single float channel types (positions and euler rotations).
 *  Assumes amplitude and sustain are relative to zero.
 */
class MeCtAdshrEnvelope : public SmartBody::SBController {
public:
	// Public Constants
	static std::string CONTROLLER_TYPE;

protected:
	// Envelope
	float _base_level; // level returned before and after enveloper (and use in attack and release interpolation)
	float _amplitude;  // level of initial envelope peak
	float _attack;     // duration to peak (and indt)
	float _decay;      // duration to decay to sustain level
	float _sustain;    // level to hold after decay
	float _hold;       // duration of sustain hold
	float _release;    // duration of decay to base level

	// Data
	SkChannelArray  _channels;
	SrBuffer<int>   _local_ch_to_buffer;

public:
	/** Constructor */
	MeCtAdshrEnvelope();

	const std::string& controller_type() const;

	/**
	 *  Initializes the controller with a set of channels to write to.
	 */
	bool init(SbmPawn* pawn, SkChannelArray& channels );

	float base_level() const { return _base_level; }
	float amplitude() const  { return _amplitude; }
	float attack() const     { return _attack; }
	float decay() const      { return _decay; }
	float sustain() const    { return _sustain; }
	float hold() const       { return _hold; }
	float release() const    { return _release; }

	void base_level( float base_level );
	void amplitude( float amplitude );
	void attack( float attack );
	void decay( float decay );
	void sustain( float sustain );
	void hold( float hold );
	void release( float release );

	void envelope( float amplitude, float attack, float decay, float sustain, float hold, float release );

	/**
	 *  Implements MeController::controller_channels().
	 */
	SkChannelArray& controller_channels();

	/**
	 *  Implements MeController::controller_duration()
	 *  Returns -1, undefined duration.
	 */
	double controller_duration();

	/**
	 *  Implements MeController::controller_evaluate(..).
	 */
	bool controller_evaluate( double time, MeFrameData& frame );

};


#endif // ME_CT_ADSHR_ENVELOPE_HPP
