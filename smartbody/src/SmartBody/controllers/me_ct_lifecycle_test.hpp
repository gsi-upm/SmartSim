/*
 *  me_ct_lifecycle_test.hpp - part of SmartBody-lib's Motion Engine
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
 *      Andrew n marshall, USC
 */

#ifndef ME_CT_LIFECYCLE_TEST_HPP
#define ME_CT_LIFECYCLE_TEST_HPP


#include "controllers/me_ct_unary.hpp"



class MeCtLifecycleTest: public MeCtUnary {
public:
	//////////////////////////////////////////////////////////
	//  Public Constants
	static std::string CONTROLLER_TYPE;

protected:
	//////////////////////////////////////////////////////////
	//  Private Data
	std::ostream* out;
	unsigned int  evaluate_count;  // flag to disable spew

public:
	//////////////////////////////////////////////////////////
	//  Public Methods
	
	MeCtLifecycleTest();

	// Does not take ownership of the ostream
	void init( MeController* child, std::ostream* = &std::cout );
	size_t count_children();

protected:
	//////////////////////////////////////////////////////////
	//  Private Methods
	void controller_init();
	void context_updated();
	void controller_start();
	void controller_stop();
	void controller_map_updated();
	bool controller_evaluate( double t, MeFrameData& frame );
	SkChannelArray& controller_channels();
	double controller_duration();
	const std::string& controller_type() const;
	bool remove_child( MeController* child );
	void print_state( int tab_count );
	void print_children( int tab_count );

	void print_method_entry( std::string method, bool reset_eval_count = true ) const;
};

#endif // ME_CT_LIFECYCLE_TEST_HPP
