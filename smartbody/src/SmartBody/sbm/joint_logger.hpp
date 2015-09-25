/*
 *  joint_logger.hpp - part of SmartBody-lib
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

#ifndef JOINT_LOGGER_HPP
#define JOINT_LOGGER_HPP


#include <sstream>

#include <controllers/me_evaluation_logger.hpp>

// Predeclare class and namespace because of circular reference:
namespace joint_logger {
	class EvaluationLogger;
}
#include "sbm/mcontrol_util.h"


namespace joint_logger {
	/**
	 *  MeEvaluationLogger implementation for SBM.
	 *
	 *  Prints context entering and exiting (with channel data on enter),
	 *  and controller results on post_evaluate.
	 */
	class EvaluationLogger :
		public MeEvaluationLogger
	{
	protected:
		/////////////////////////////////////////////////////////////////////
		//  Private Data
		double      _expiration_time;
		bool        _enabled;
		int         _indent_len;
		std::string _indent;

	public:
		/** Constructor */
		EvaluationLogger();


		/**
		 *  Sets the expiration time for a logging session.
		 */
		void set_expiration_time( double time )
		{	_expiration_time = time; }

		/**
		 *  Prints context name and type,
		 *  with initial data for the context's selected channels.
		 */
		void context_pre_evaluate(
			double time,
			MeControllerContext& context,
			MeFrameData& frame
		);

		/**
		 *  Prints the context name and type, denoting completion.
		 */
		void context_post_evaluate(
			double time,
			MeControllerContext& context,
			MeFrameData& frame
		);

		/**
		 *  Does nothing in this MeEvaluationLogger implementation.
		 */
		void controller_pre_evaluate(
			double time,
			MeControllerContext& context,
			MeController& ct,
			MeFrameData& frame
			);

		/**
		 *  Prints the controller results for the context's selected channels.
		 */
		void controller_post_evaluate(
			double time,
			MeControllerContext& context,
			MeController& ct,
			MeFrameData& frame
		);

	protected:
		/**
		 *  Formats channels at current indent.
		 */
		void format_channel_data( MeControllerContext& context,
		                          MeFrameData& frame,
							      std::ostringstream& out );
	};



	/**
	 *  Handles the following sbm commands:
	 *     set joint_logger joints [+|-] <joint name>*
	 *     set joint_logger duration <seconds>
	 *
	 *  Setting joints will define the list of joints to be logged.
	 *  The list of joint names can be prefixed with a + or - to
	 *  mark the list as additive or substractive.
	 *
	 *  Setting the duration will define how many seconds the joints
	 *  should be printed.  The duration will default to one second
	 *  to avoid print flooding the command window.
	 */
	int set_func( srArgBuffer& args, mcuCBHandle *mcu_p );

	/**
	 *  Handles the following sbm commands:
	 *     print joint_logger joints
	 *     print joint_logger duration
	 *
	 *  Printing the joints will print out the current list of joint
	 *  names marked for printing.
	 *
	 *  Printing the duration simple prints the current duration set.
	 */
	int print_func( srArgBuffer& args, mcuCBHandle *mcu_p );

	/**
	 *  Handles the following sbm commands:
	 *     joint_logger [start]
	 *  To start a log for duration seconds.
	 *     joint_logger stop
	 *  To stop current logging
	 */
	int start_stop_func( srArgBuffer& args, mcuCBHandle *mcu_p );
} //end namespace



#endif // JOINT_LOGGER_HPP
