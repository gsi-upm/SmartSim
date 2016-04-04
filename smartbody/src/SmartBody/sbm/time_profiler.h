/*
 *  time_profiler.h - part of SmartBody-lib
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
 *      Marcus Thiebaux, USC
 */

#ifndef TIME_INTERVAL_PROFILER_H
#define TIME_INTERVAL_PROFILER_H

#include <vhcl.h>
#include <stdio.h>
#include <iostream>
#include "sbm/sr_hash_map.h"
#include "sbm/time_regulator.h"

#define DEFAULT_BYPASS			true
#define DEFAULT_ENABLED			true
#define DEFAULT_SNIFF			0.9
#define DEFAULT_AVOID			1.5
#define DEFAULT_THRESHOLD		10.0
#define DEFAULT_DECAYING		0.95
#define DEFAULT_ROLLING 		64
#define MAX_SNIFF_DT			0.2

////////////////////////////////////////////////////////////////////////////////////////

class TimeIntervalProfiler { // T.I.P.
	
	private:
		enum time_profiler_enum_set	{
		
#if 0
			LABEL_SIZE =	8192,
			MAX_GROUPS =	256,
			MAX_PROFILES =	256,
			MAX_ROLLING = 	256
#elif 1
			LABEL_SIZE =	512,
			MAX_GROUPS =	32,
			MAX_PROFILES =	32,
			MAX_ROLLING = 	64
#else
			LABEL_SIZE =	512,
			MAX_GROUPS =	1,
			MAX_PROFILES =	1,
			MAX_ROLLING = 	64
#endif
	};

		typedef struct profile_entry_s {

			int 	id;
			int 	index;
			int 	level;
			char	label[ LABEL_SIZE ];
			bool	spike;
			bool	show;
			bool	req_reset;

			double	event_time;
			double	prev_dt;
			double  interval_dt;

			int 	intra_count;
			double	avg_intra_dt;
			double	max_intra_dt;

			double	decay_dt;

			double	accum_roll_dt;
			int 	accum_count;
			double	roll_dt;
			double	roll_dt_arr[ MAX_ROLLING ];
			int 	roll_index;
			
		} profile_entry_t;

		typedef struct group_entry_s {

			int 	id;
			int 	index;
			char	name[ LABEL_SIZE ];
			bool	req_enable;
			bool	req_disable;
			bool	enabled;
			bool 	open;
			bool	open_err;
			
			bool	req_preload;
			bool	preloading;
			bool	spike;
			int		spike_count;
			bool	req_reset;

			double  interval_dt;
			double	decay_dt;
			double	roll_dt;

			srHashMap <profile_entry_t> profile_map;
			profile_entry_t* profile_p_arr[ MAX_PROFILES ];
			bool	full_err;
			
			int 	profile_arr_count;
			int 	active_profile_count;
			int 	profile_event_count;
			profile_entry_t* curr_profile_p;
			
		} group_entry_t;


		srHashMap <group_entry_t>* group_map;
		group_entry_t* group_p_arr[ MAX_GROUPS ];
		bool	full_err;
		
		int 	id_counter;
		int 	group_arr_count;
		int 	active_group_count;
		int 	group_event_count;

		bool	sys_bypass;
		bool	dis_sys_bypass;
		bool	enabled;
		bool	preloading;

		double	update_time;
		double	update_dt;

		bool	req_print;
		bool	req_report;
		bool	req_erase;
		bool	req_clobber;
		bool	req_enable;
		bool	req_disable;
		bool	req_preload;
		bool	pending_request;
		
		double	abs_threshold;
		double	rel_threshold;
		bool	dyn_abs_thr;
		bool	dyn_rel_thr;
		double	dyn_sniff; // decaying: positive, less than 1, per second.
		double	dyn_avoid; // bumping: greater than 1: small hiccup, x2... large hiccup, x1.1

		double	decaying_factor;
		int 	rolling_length;
		int 	suppression;
		int 	selection;

		void null( void );
		void null_group( group_entry_t* group_p, const char* group_name = "" );
		void null_profile( profile_entry_t* profile_p, const char* label = "" );
		
		void reset_group( group_entry_t* group_p );
		void reset_profile( profile_entry_t* profile_p );
		
///////////////////////////////////////////////////

	public:

		TimeIntervalProfiler( void );
		~TimeIntervalProfiler( void );

		void bypass( bool bp );
		void reset( void );
		void print_legend( void );
		void print( void );
		void report( void );
		void erase( void );
		void enable( bool en );
		void preload( void );
		bool enable( const char* group_name, bool en );
		bool preload( const char* group_name );
		void set_suppression( int sup );
		void set_selection( int sel );
		void set_abs_threshold( double delta );
		void set_rel_threshold( double factor );
		void set_dynamic_abs( bool dyn );
		void set_dynamic_rel( bool dyn );
		void set_sniff( double sniff );
		void set_avoid( double avoid );
		void set_decaying( double s );
		void set_rolling( int len );
///////////////////////////////////////////////////

	private:

		void print_data( void );
		void print_profile( profile_entry_t* profile_p );
		void print_group( group_entry_t *group_p );
		void print_profile_report( profile_entry_t *profile_p, int group_id );
		void print_group_report( group_entry_t* group_p );

		void print_profile_alert( double dt, group_entry_t* group_p, profile_entry_t *profile_p );
		void print_group_alert( const char *prefix, double dt, group_entry_t* group_p );
		
		void accum_profile( profile_entry_t *profile_p );
		bool check_profile_spike( profile_entry_t *profile_p );
		bool check_group_spike( group_entry_t* group_p );
		bool check_profile_show( int level );

		void report_recent_groups( group_entry_t* group_p );
		void report_recent_profiles( group_entry_t* group_p );
		void report_current_group( group_entry_t* group_p );

		void sys_update( double time );

		group_entry_t* get_group( const char* group_name );
		profile_entry_t* get_profile( group_entry_t *group_p, const char* label );

		void accum_mark( group_entry_t *group_p, double time );
		void touch_profile( profile_entry_t *profile_p, int level );
		void touch_group( const char* group_name, int level, const char* label );
		double convert_time( double time );
		
	public:

		void update( double time );
		void update( void );
		void mark_time( const char* group_name, int level, const char* label, double time );
		int mark_time( const char* group_name, double time );
		void mark( const char* group_name, int level, const char* label );
		int mark( const char* group_name );
		static double test_clock( int reps = 0 );
};

////////////////////////////////////////////////////////////////////////////////////////
#endif
