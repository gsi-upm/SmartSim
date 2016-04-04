/*
 *  me_ct_periodic_replay.hpp - part of SmartBody-lib's Motion Engine
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

#ifndef ME_CT_PERIODIC_REPLAY_HPP
#define ME_CT_PERIODIC_REPLAY_HPP

#include <controllers/me_ct_unary.hpp>


/**
 *  Unary container controller that modifies the time sent to the child evaluator,
 *  repeating a period defined by (child_time_offset, child_time_offset+period]
 *  over period seconds.
 */
class MeCtPeriodicReplay : public MeCtUnary {
public:
	///////////////////////////////////////////////////////////////////////
	//  Public Constants
	static std::string CONTROLLER_TYPE;

protected:
	double period_offset;
	double period;
	double child_time_offset;
	

public:
	MeCtPeriodicReplay( MeController* child = NULL );
	// virtual ~MeCtTimeShiftWarp(); // Default destructor call ~MeCtUnary to delete child

	void init( double period );
	void init( double period, double period_offset, double child_time_offset );

    const std::string& controller_type() const;

    double controller_duration();
    //SkChannelArray& controller_channels(); // implemented in MeCtUnary

protected:
	bool controller_evaluate( double t, MeFrameData & frame );
};


#endif // ME_CT_PERIODIC_REPLAY_HPP
