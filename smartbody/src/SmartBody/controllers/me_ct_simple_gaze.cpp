/*
 *  me_ct_simple_gaze.cpp - part of SmartBody-lib
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
 *      Marcus Thiebaux, USC
 *      Andrew n marshall, USC
 *      Ed Fast, USC
 */

#include "controllers/me_ct_simple_gaze.h"
#include "sbm/lin_win.h"
#include "sb/sbm_pawn.hpp"

#include "sbm/gwiz_math.h"
using namespace gwiz;

#include <sr/sr_alg.h>
#include <vhcl_log.h>
#include "sbm/sbm_constants.h"

int G_debug_c_SG = 0;
int G_debug_SG = 0;

///////////////////////////////////////////////////////////////////////////

#define ENABLE_FORWARD_RAY  		1
#define ENABLE_FORWARD_RAY_TEST 	0

vector_t find_nearest_point_on_line_SG(
	vector_t P,  // point near line
	vector_t L1, // point 1 on line
	vector_t L2  // point 2 on line
)	{
	
	vector_t N = ( L2 - L1 ).normal();
	
	// tangent plane equation:
	gwiz::float_t d = -( N.dot( P ) );
	
	// intersect line and plane
	gwiz::float_t A = N.dot( L1 ) + d;
	gwiz::float_t B = -( N.sqlen() );
	gwiz::float_t u = A / B;
	
	return( L1 + N * u );
}

#define FORWARD_RAY_EPSILON ( 0.00001 )

quat_t rotation_ray_to_target_orient_SG(
	quat_t Q,     // target orientation
	vector_t Fd,  // forward ray direction
	vector_t Fr = vector_t( 0.0, 0.0, 1.0 ) // null reference ray direction
)	{
	
	if( Fd.length() < FORWARD_RAY_EPSILON ) {
		LOG( "rotation_ray_to_target_orient_SG ERR: forward direction has no length\n" );
		return( quat_t() );
	}
	
	vector_t Td = Q * Fr; // target direction vector
	
	vector_t axis = Fd.cross( Td );
	gwiz::float_t angle = DEG( acos( Fd.normal().dot( Td.normal() ) ) );
	return( quat_t( angle, axis ) );
}

quat_t rotation_ray_to_target_point_SG(
	vector_t X,   			// target
	vector_t R,   			// center of rotation
	vector_t Fo,  			// forward ray origin
	vector_t Fd,  			// forward ray direction
	gwiz::float_t buffer_ratio = 0.1,	// proportion of buffer zone for pathological case
	int heading_only = FALSE
)	{
	
	if( Fd.length() < FORWARD_RAY_EPSILON ) {
		if( heading_only == FALSE ) {
			LOG( "rotation_ray_to_target_point_SG ERR: forward direction has no length\n" );
		}
		return( quat_t() );
	}
	
	vector_t RX = X - R; // vector from rotation to target
	gwiz::float_t d = RX.length(); // distance from rotation to target

	vector_t RF = Fo - R; // vector from rotation to forward origin
	gwiz::float_t f = RF.length(); // distance from rotation to forward origin

	// pathological case:
	// if target is inside of forward origin radius
	if( d < f )	{

#if 0
		if( heading_only )	{
			// give up:
			return( quat_t() );
		}
		
		// treat preferred 2D heading case:
		vector_t X2 = vector_t( X.x(), 0.0, X.z() );
		vector_t R2 = vector_t( R.x(), 0.0, R.y() );
		vector_t Fo2 = vector_t( Fo.x(), 0.0, Fo.z() );
		vector_t Fd2 = vector_t( Fd.x(), 0.0, Fd.z() );

		return( rotation_ray_to_target_point_SG( X2, R2, Fo2, Fd2, buffer_ratio, TRUE ) );
#else
		return( quat_t() );
#endif
	}

	// blend pathological case if near forward origin radius:
	quat_t Qbuffer; // solution for pathological case
	gwiz::float_t buffer_lerp = 0.0; // amount of Qbuffer to apply
	gwiz::float_t buffer_len = f * buffer_ratio; // length of buffer zone
	
	// if target is near forward origin radius: 
	if( d < ( f + buffer_len ) )	{
		buffer_lerp = 1.0 - ( d - f ) / buffer_len;

		if( !heading_only )	{

			// treat preferred 2D heading case:
			vector_t X2 = vector_t( X.x(), 0.0, X.z() );
			vector_t R2 = vector_t( R.x(), 0.0, R.y() );
			vector_t Fo2 = vector_t( Fo.x(), 0.0, Fo.z() );
			vector_t Fd2 = vector_t( Fd.x(), 0.0, Fd.z() );

			Qbuffer = rotation_ray_to_target_point_SG( X2, R2, Fo2, Fd2, buffer_ratio, TRUE );
		}
	}

	// T: forward ray tangent to rotation sphere
	vector_t T = find_nearest_point_on_line_SG( R, Fo, Fo + Fd );
	
	vector_t RT = T - R; // vector from rotation to forward tangent
	gwiz::float_t r = RT.length(); // radius of sphere

	// if radius is tiny
	if( r < FORWARD_RAY_EPSILON )	{

		// if target is close
		if( d < FORWARD_RAY_EPSILON )	{
			return( quat_t() );
		}

		vector_t axis = Fd.cross( RX );
		gwiz::float_t angle = DEG( acos( Fd.normal().dot( RX.normal() ) ) );
		return( quat_t( angle, axis ) );
	}

	// rho: angle from RX to X-tangent-plane
	gwiz::float_t rho = DEG( acos( r / d ) );

	// gamma: angle from RX to RT
	gwiz::float_t gamma = DEG( acos( RX.normal().dot( RT.normal() ) ) );

	// Aaxis: axis perpendicular to RX and RT
	vector_t Aaxis = RT.cross( RX );

	if( gamma > 180.0 )	{
		gamma = 360.0 - gamma;
		Aaxis = -Aaxis; // axis of shortest rotation
	}

	// alpha: rotation about Ax to move T to X-tangent-plane
	gwiz::float_t alpha = gamma - rho;
	quat_t Qalpha( alpha, Aaxis ); // to apply alpha

	vector_t newT = R + Qalpha * RT; // rotated tangent vector
	vector_t newF = Qalpha * Fd; // rotated forward vector

	// beta: rotation to align new forward with new tangent
	gwiz::float_t beta = DEG( acos( newF.normal().dot( ( X - newT ).normal() ) ) );
	if( beta > 0.0 )	{
		
		// Baxis: axis to which beta is implicitly applied
		vector_t Baxis = newF.cross( X - newT );

		// phi: compare axes to determine sign of beta
		//  because implicit axis (Baxis) is not used
		gwiz::float_t phi = Baxis.normal().dot( ( newT - R ).normal() );
		if( phi < 0.0 ) {
			beta = -beta;
		}
	}
	quat_t Qbeta( beta, RT ); // to apply beta to original tangent-point axis
	
	// combine ( alpha * beta ) with buffer-zone solution
	quat_t Qresult = ( Qalpha * Qbeta ).lerp( buffer_lerp, Qbuffer );
	return( Qresult );
}

#if ENABLE_FORWARD_RAY_TEST
void test_forward_ray( void )	{

#if 0
	vector_t X( 0.0, 4.0, 2.0 ); // heading-only case
	vector_t R( 0.0, 0.0, 0.0 );
	vector_t Fo( 1.0, 5.0, 0.0 );
	vector_t Fd( 0.0, 0.0, 1.0 );
#elif 0
	vector_t X( 4.0, 10.0, 0.0 );
	vector_t R( 4.0, 5.0, 0.0 );
	vector_t Fo( 4.0, 1.0, 0.0 );
	vector_t Fd( 0.0, 1.0, 1.0 );
#elif 0
	vector_t X( 4.0, 10.0, 0.0 );
	vector_t R( 4.0, 0.0, 0.0 );
	vector_t Fo( 0.0, 0.0, 0.0 );
//	vector_t Fd( 0.0, 1.0, 0.0 );
	vector_t Fd( 0.0, 1.0, 1.0 );
#elif 1
	vector_t X( 4.0, 10.0, 10.0 );
	vector_t R( 4.0, 0.0, 0.0 );
	vector_t Fo( 0.0, 0.0, 0.0 );
//	vector_t Fd( 0.0, 1.0, 0.0 );
//	vector_t Fd( 0.0, 0.0, 1.0 );
	vector_t Fd( 0.0, 1.0, 1.0 );
#else
	vector_t X( 4.0, -10.0, 30.0 );
	vector_t R( -3.0, 4.0, 5.0 );
	vector_t Fo( -10.0, 3.0, -2.0 );
	vector_t Fd( 5.0, -10.0, 1.0 );
#endif

	LOG( "--- FORWARD RAY TEST:\n" );
	quat_t Q = rotation_ray_to_target_point_SG( X, R, Fo, Fd, 0.0 );

	LOG( "ROTATION:\n" );
	euler_t E = Q;
	E.print();

	LOG( "NEW FORWARD DIR:\n" );
	vector_t newFd = Q * Fd;
	newFd.normal().print();

	LOG( "FORWARD ORIGIN TO TARGET:\n" );
	vector_t Td = X - ( R + Q * ( Fo - R ) );
	Td.normal().print();

	LOG( "DIFF:\n" );
	( Td.normal() - newFd.normal() ).print();
	
	LOG( "DIFF HEADING-ONLY:\n" );
	( vector_t( Td.x(), 0.0, Td.z() ).normal() - vector_t( newFd.x(), 0.0, newFd.z() ).normal() ).print();
	LOG( "---\n" );
}
#endif

///////////////////////////////////////////////////////////////////////////

MeCtSimpleGazeJoint::MeCtSimpleGazeJoint( void )	{
	
	limit = 180.0;
	weight = 1.0;
	speed = 1000000.0;
	duration = 1.0;
	smooth = 0.0;
	
	forward_pos = vector_t();
	forward_ref = vector_t( 0.0, 0.0, 1.0 );
	forward_rot = quat_t();
//	forward_rot = euler_t( 45.0, 0.0, 0.0 );
//	forward_rot = euler_t( 0.0, 45.0, 0.0 );
//	forward_rot = euler_t( 0.0, 0.0, 45.0 );
	
	joint_p = NULL;
}

void MeCtSimpleGazeJoint::init( SkJoint* j_p )	{
	
	joint_p = j_p;
}

void MeCtSimpleGazeJoint::begin( void )	{

	capture_joint_state();
	prev_local_rot = local_rot;
}

#define ENABLE_CONSTRAIN	1

quat_t MeCtSimpleGazeJoint::evaluate( float dt, vector_t target_pos, quat_t off_rot, float scale_factor )	{
	
	capture_joint_state();
	quat_t Q_task = ( rotation_to_target( target_pos ) * off_rot ) * scale_factor;
#if ENABLE_CONSTRAIN
	quat_t Q = constrain( dt, Q_task );
#else
	quat_t Q = Q_task;
#endif
	prev_local_rot = Q;
	return( Q );
}

quat_t MeCtSimpleGazeJoint::evaluate( float dt, quat_t target_rot, quat_t off_rot, float scale_factor )	{
	
	capture_joint_state();
	quat_t Q_task = ( rotation_to_target( target_rot ) * off_rot ) * scale_factor;
#if ENABLE_CONSTRAIN
	quat_t Q = constrain( dt, Q_task );
#else
	quat_t Q = Q_task;
#endif
	prev_local_rot = Q;
	return( Q );
}

void MeCtSimpleGazeJoint::capture_joint_state( void ) {
	SrMat sr_M;
	gwiz::matrix_t M;
	int i, j;

	if( joint_p )	{

		sr_M = joint_p->lmat();
		for( i=0; i<4; i++ )	{
			for( j=0; j<4; j++ )	{
				M.set( i, j, sr_M.get( i, j ) );
			}
		}
		local_pos = M.translation( gwiz::COMP_M_TR );
		local_rot = M.quat( gwiz::COMP_M_TR );

		sr_M = joint_p->gmat();
		for( i=0; i<4; i++ )	{
			for( j=0; j<4; j++ )	{
				M.set( i, j, sr_M.get( i, j ) );
			}
		}
		world_pos = M.translation( gwiz::COMP_M_TR );
		world_rot = M.quat( gwiz::COMP_M_TR );

		SkJoint* parent_p = joint_p->parent();
		if( parent_p )	{

			sr_M = parent_p->gmat();
			for( i=0; i<4; i++ )	{
				for( j=0; j<4; j++ )	{
					M.set( i, j, sr_M.get( i, j ) );
				}
			}
			parent_pos = M.translation( gwiz::COMP_M_TR );
			parent_rot = M.quat( gwiz::COMP_M_TR );
		}
		else	{
			const char *name = joint_p->jointName().c_str();
			LOG( "MeCtSimpleGazeJoint::capture_joint_state ERR: parent of joint '%s' not found\n", name );
		}
	}
}

#define DFL_FORWARD_RAY_BUFFER_ZONE_RATIO 0.2

quat_t MeCtSimpleGazeJoint::rotation_to_target( vector_t target_pos )	{
	
	// parent coord with forward-ray
	/*
	quat_t rotation_ray_to_target_point_SG(
		vector_t X,   // target
		vector_t R,   // center of rotation
		vector_t Fo,  // forward ray origin
		vector_t Fd,  // forward ray direction
		float_t buffer_ratio = 0.1,	// proportion of buffer zone for pathological case
	)
	*/
	
	euler_t E_forward = forward_rot;
	quat_t forward_ph = euler_t( E_forward.p(), E_forward.h(), 0.0 );
	quat_t forward_roll = euler_t( 0.0, 0.0, E_forward.r() );
	
	vector_t l_target_pos = ( -parent_rot ) * ( target_pos - parent_pos );
	vector_t forward_dir = ( -forward_ph ) * forward_ref.normal();

	quat_t Q = rotation_ray_to_target_point_SG(
		l_target_pos,
		local_pos,
		forward_pos,
		forward_dir,
		DFL_FORWARD_RAY_BUFFER_ZONE_RATIO
	);
	return( Q * forward_roll );
}

quat_t MeCtSimpleGazeJoint::rotation_to_target( quat_t target_rot )	{
	
#if 1
	
/*
	quat_t rotation_ray_to_target_orient_SG(
		quat_t Q,     // target orientation
		vector_t Fd,  // forward ray direction
		vector_t Fr = vector_t( 0.0, 0.0, 1.0 ) // null reference ray direction
	)
*/

	return( ( -parent_rot ) * target_rot * forward_rot );
	
#else
	return( ( -parent_rot ) * target_rot );
#endif
}

quat_t MeCtSimpleGazeJoint::constrain_ellipse( quat_t task_rot )	{
	
	float lim_x = limit * 0.4f;
	float lim_y = limit;
	float lim_z = limit * 0.3f;
	
	vector_t st = task_rot.swingtwist();
	float sw_x = (float)st.x();
	float sw_y = (float)st.y();

	// Marcelo's exact ellipse solution:
	if( sr_in_ellipse( sw_x, sw_y, lim_x, lim_y ) > 0.0 )	{
		sr_get_closest_on_ellipse( lim_x, lim_y, sw_x, sw_y );
	}

	float tw = (float)st.z();
	if( tw > lim_z ) tw = lim_z;
	if( tw < -lim_z ) tw = -lim_z;
	
	return( quat_t( sw_x, sw_y, tw ) );
}

quat_t MeCtSimpleGazeJoint::constrain_quat( quat_t task_rot )	{

	if( task_rot.degrees() > limit )	{
		task_rot = quat_t( limit, task_rot.axis() );
	}
	return( task_rot );
}

quat_t MeCtSimpleGazeJoint::constrain_box( quat_t task_rot )	{

	float lim_x = limit * 0.4f;
	float lim_y = limit;
	float lim_z = limit * 0.3f;

	vector_t st = task_rot.swingtwist();
	float sw_x = (float)st.x();
	float sw_y = (float)st.y();
	float tw = (float)st.z();

	if( sw_x > lim_x ) sw_x = lim_x;
	if( sw_x < -lim_x ) sw_x = -lim_x;
	if( sw_y > lim_y ) sw_y = lim_y;
	if( sw_y < -lim_y ) sw_y = -lim_y;
	if( tw > lim_z ) tw = lim_z;
	if( tw < -lim_z ) tw = -lim_z;

	return( quat_t( sw_x, sw_y, tw ) );
}

quat_t MeCtSimpleGazeJoint::constrain_quat_speed( float dt, quat_t task_rot )    {

/*
	gwiz_math: PROBLEMS
		safe arc-trig
		quat-mult * normalize()
*/

	quat_t Q_dif = task_rot * ( -prev_local_rot );
	
	float dps = (float)( Q_dif.degrees() / dt );
	if( dps < speed )	{
		return( task_rot );
	}

	quat_t Q_a = quat_t( speed * dt, Q_dif.axis() );
	quat_t Q_ret = Q_a * prev_local_rot;

	return( Q_ret );
}

#define SMOOTH_RATE_REF (30.0f)

quat_t MeCtSimpleGazeJoint::constrain_smooth( float dt, quat_t task_rot )	{

	if( smooth > 0.0 )	{
		float s = (float)(0.01 + ( 1.0 - powf( smooth, dt * SMOOTH_RATE_REF ) ) * 0.99);
		return(
			prev_local_rot.lerp( 
				s, 
				task_rot
			)
		);
	}
	return( task_rot );
}
	
quat_t MeCtSimpleGazeJoint::constrain( float dt, quat_t task_rot )	{
	
	quat_t Q_w = task_rot * weight;
	
	quat_t Q_c = 
#if 0
		constrain_quat( Q_w );
#elif 0
		constrain_box( Q_w );
#else
		constrain_ellipse( Q_w );
#endif

#if 0
	quat_t Q_sm = 
		constrain_smooth(
			dt,
			Q_c
		);
#else
	quat_t Q_sp = 
		constrain_quat_speed(
			dt,
			Q_c
		);
	
	quat_t Q_sm = 
		constrain_smooth(
			dt,
			Q_sp
		);
#endif

	return( Q_sm );
}

///////////////////////////////////////////////////////////////////////////

/*
	GAZING HIERARCHY:    { "spine1", "spine2", "spine3", "spine4", "spine5", "skullbase", "face_top_parent", "eyeball_left", "eyeball_right" }
	FULL GAZING JOINTS:  { "spine1", "spine2", "spine3", "spine4", "spine5", "skullbase", "eyeball_left", "eyeball_right" }

	BACK: { "spine1", "spine2", "spine3" }

	NECK: { "spine4", "spine5", "skullbase" }

	EYES: { "eyeball_left", "eyeball_right" }
*/

/*
	height: segment length
		spine1: 7.8 		7.8
		spine2: 17.7		9.9
		spine3: 28.9		11.2
		spine4: 56.0		27.1
		spine5: 59.7		3.7
		skullbase: 64.7 	5.0
		eyeball: 70.6		5.9
*/

#define GAZE_JOINT_SPINE1	0
#define GAZE_JOINT_SPINE2	1
#define GAZE_JOINT_SPINE3	2

#define GAZE_JOINT_SPINE4	3
#define GAZE_JOINT_SPINE5	4
#define GAZE_JOINT_SKULL	5

#define GAZE_JOINT_EYE_L	6
#define GAZE_JOINT_EYE_R	7
#define NUM_GAZE_JOINTS 	8

std::string MeCtSimpleGaze::type_name = "SimpleGaze";

MeCtSimpleGaze::MeCtSimpleGaze( void )	{
	
	start = 0;
	prev_time = 0.0;
	_duration = -1.0f;

	skeleton_ref_p = NULL;
	ref_joint_str = NULL;
	ref_joint_p = NULL;
	
	target_mode = TARGET_POINT;	
	flexible_joint = GAZE_JOINT_SPINE4;
	priority_joint = GAZE_JOINT_EYE_L;
	
	joint_count = 0;
	joint_arr = NULL;
}

MeCtSimpleGaze::~MeCtSimpleGaze( void )	{
	
	if( ref_joint_str ) {
		free( ref_joint_str );
		ref_joint_str = NULL;
	}
}

void MeCtSimpleGaze::init( SbmPawn* pawn )	{
	char joint_labels[ NUM_GAZE_JOINTS ][ MAX_JOINT_LABEL_LEN ] = {
		"spine1",
		"spine2",
		"spine3",
		"spine4",
		"spine5",
		"skullbase",
		"eyeball_left",
		"eyeball_right"
	};
	int i;
	
	joint_count = NUM_GAZE_JOINTS;
	joint_arr = new MeCtSimpleGazeJoint[ joint_count ];

	for( i = 0; i < joint_count; i++ )	{
		_channels.add( joint_labels[ i ] , SkChannel::Quat );
	}

	MeController::init(pawn);
}

void MeCtSimpleGaze::set_target_joint( float x, float y, float z, SkJoint* joint_p )	{

	target_mode = TARGET_POINT;	
	point_target_pos = vector_t( x, y, z );
	if( ref_joint_str ) {
		free( ref_joint_str );
		ref_joint_str = NULL;
	}
	ref_joint_p = joint_p;
}

void MeCtSimpleGaze::set_target( float x, float y, float z, char *ref_joint_name )	{

	target_mode = TARGET_POINT;	
	point_target_pos = vector_t( x, y, z );
	if( ref_joint_str ) {
		free( ref_joint_str );
	}
	ref_joint_str = _strdup( ref_joint_name );
	ref_joint_p = NULL;
}

void MeCtSimpleGaze::set_orient_joint( float p, float h, float r, SkJoint* joint_p )	{

	target_mode = TARGET_ORIENT;
	orient_target_rot = euler_t( p, h, r );
	if( ref_joint_str ) {
		free( ref_joint_str );
		ref_joint_str = NULL;
	}
	ref_joint_p = joint_p;
}

void MeCtSimpleGaze::set_orient( float p, float h, float r, char *ref_joint_name )	{

	target_mode = TARGET_ORIENT;
	orient_target_rot = euler_t( p, h, r );
	if( ref_joint_str ) {
		free( ref_joint_str );
	}
	ref_joint_str = _strdup( ref_joint_name );
	ref_joint_p = NULL;
}

void MeCtSimpleGaze::set_offset_swing( float off_p, float off_h, float off_r )	{
	
	offset_rot = quat_t( off_p, off_h, off_r );
}

void MeCtSimpleGaze::set_offset_polar( float off_d, float off_a, float off_r )	{
	
	vector_t swing_axis = euler_t( 0.0, 0.0, off_d ) * vector_t( -1.0, 0.0, 0.0 );
	offset_rot = quat_t( off_a, swing_axis ) * quat_t( off_r, vector_t( 0.0, 0.0, 1.0 ) );	
}

void MeCtSimpleGaze::set_speed( float back_dps, float neck_dps, float eyes_dps )	{
	
	float dps = back_dps * 0.3333f;
	joint_arr[ GAZE_JOINT_SPINE1 ].speed = dps;
	joint_arr[ GAZE_JOINT_SPINE2 ].speed = dps;
	joint_arr[ GAZE_JOINT_SPINE3 ].speed = dps;

	dps = neck_dps * 0.3333f;
	joint_arr[ GAZE_JOINT_SPINE4 ].speed = dps;
	joint_arr[ GAZE_JOINT_SPINE5 ].speed = dps;
	joint_arr[ GAZE_JOINT_SKULL ].speed = dps;

	joint_arr[ GAZE_JOINT_EYE_L ].speed = eyes_dps;
	joint_arr[ GAZE_JOINT_EYE_R ].speed = eyes_dps;
}

void MeCtSimpleGaze::set_smooth( float back_sm, float neck_sm, float eyes_sm )	{
	
	if( back_sm < 0.0 ) back_sm = 0.0;
	if( back_sm > 1.0 ) back_sm = 1.0;
	if( neck_sm < 0.0 ) neck_sm = 0.0;
	if( neck_sm > 1.0 ) neck_sm = 1.0;
	if( eyes_sm < 0.0 ) eyes_sm = 0.0;
	if( eyes_sm > 1.0 ) eyes_sm = 1.0;
	
	joint_arr[ GAZE_JOINT_SPINE1 ].smooth = back_sm;
	joint_arr[ GAZE_JOINT_SPINE2 ].smooth = back_sm;
	joint_arr[ GAZE_JOINT_SPINE3 ].smooth = back_sm;

	joint_arr[ GAZE_JOINT_SPINE4 ].smooth = neck_sm;
	joint_arr[ GAZE_JOINT_SPINE5 ].smooth = neck_sm;
	joint_arr[ GAZE_JOINT_SKULL ].smooth = neck_sm;

	joint_arr[ GAZE_JOINT_EYE_L ].smooth = eyes_sm;
	joint_arr[ GAZE_JOINT_EYE_R ].smooth = eyes_sm;
}

void MeCtSimpleGaze::inspect_skeleton( SkJoint* joint_p, int depth )	{
	int i, j, n;
	
	if( joint_p )	{
		const char *name = joint_p->jointName().c_str();
		for( j=0; j<depth; j++ ) { LOG( " " ); }
		LOG( "%s\n", name );
		n = joint_p->num_children();
		for( i=0; i<n; i++ )	{
			inspect_skeleton( joint_p->child( i ), depth + 1 );
		}
	}
}

void MeCtSimpleGaze::inspect_skeleton_local_transform( SkJoint* joint_p, int depth )	{
	
	if( joint_p )	{
		const char *name = joint_p->jointName().c_str();
		gwiz::matrix_t M;
		int i, j;

		SrMat sr_M = joint_p->lmat();
		for( i=0; i<4; i++ )	{
			for( j=0; j<4; j++ )	{
				M.set( i, j, sr_M.get( i, j ) );
			}
		}
		vector_t pos = M.translation( gwiz::COMP_M_TR );
		euler_t rot = M.euler( gwiz::COMP_M_TR );

		for( j=0; j<depth; j++ ) { LOG( " " ); }
		LOG( "%s : pos{ %.3f %.3f %.3f } : phr{ %.2f %.2f %.2f }\n", 
			name,
			pos.x(), pos.y(), pos.z(),
			rot.p(), rot.h(), rot.r()
		);

		int n = joint_p->num_children();
		for( i=0; i<n; i++ )	{
			inspect_skeleton_local_transform( joint_p->child( i ), depth + 1 );
		}
	}
}

void MeCtSimpleGaze::inspect_skeleton_world_transform( SkJoint* joint_p, int depth )	{
	
	if( joint_p )	{
		const char *name = joint_p->jointName().c_str();
		gwiz::matrix_t M;
		int i, j;

		joint_p->update_gmat_up();
		SrMat sr_M = joint_p->gmat();
		for( i=0; i<4; i++ )	{
			for( j=0; j<4; j++ )	{
				M.set( i, j, sr_M.get( i, j ) );
			}
		}
		vector_t pos = M.translation( gwiz::COMP_M_TR );
		euler_t rot = M.euler( gwiz::COMP_M_TR );

		for( j=0; j<depth; j++ ) { LOG( " " ); }
		LOG( "%s : pos{ %.3f %.3f %.3f } : phr{ %.2f %.2f %.2f }\n", 
			name,
			pos.x(), pos.y(), pos.z(),
			rot.p(), rot.h(), rot.r()
		);
		
		int n = joint_p->num_children();
		for( i=0; i<n; i++ )	{
			inspect_skeleton_world_transform( joint_p->child( i ), depth + 1 );
		}
	}
}

void MeCtSimpleGaze::update_skeleton_gmat( void )	{

	if( skeleton_ref_p )	{
		SkJoint* skull_joint_p = skeleton_ref_p->search_joint( "skullbase" );
		skull_joint_p->update_gmat_up();

		SkJoint* eye_joint_p = skeleton_ref_p->search_joint( "eyeball_left" );
		eye_joint_p->update_gmat_up( skull_joint_p );

		eye_joint_p = skeleton_ref_p->search_joint( "eyeball_right" );
		eye_joint_p->update_gmat_up( skull_joint_p );
	}
	else	{
		fprintf( stderr, "MeCtSimpleGaze::update_skeleton_gmat ERR: skeleton NOT FOUND\n" );
	}
}

SkJoint* MeCtSimpleGaze::reference_joint( void )	{

	if( ref_joint_str )	{
		if( ref_joint_p == NULL )	{
			if( skeleton_ref_p )	{
				ref_joint_p = skeleton_ref_p->search_joint( ref_joint_str );
				if( ref_joint_p == NULL )	{
					fprintf( stderr, "MeCtSimpleGaze::reference_joint ERR: joint '%s' NOT FOUND in skeleton\n", ref_joint_str );
					free( ref_joint_str );
					ref_joint_str = NULL;
				}
			}
			else	{
				fprintf( stderr, "MeCtSimpleGaze::reference_joint ERR: skeleton NOT FOUND\n" );
			}
		}
	}
	return( ref_joint_p );
}

vector_t MeCtSimpleGaze::world_target_point( void )	{
	
	SkJoint* joint_p = reference_joint();
	if( joint_p )	{
		SrMat sr_M;
		gwiz::matrix_t M;
		int i, j;
		
		joint_p->update_gmat_up();
		sr_M = joint_p->gmat();
		for( i=0; i<4; i++ )	{
			for( j=0; j<4; j++ )	{
				M.set( i, j, sr_M.get( i, j ) );
			}
		}
		vector_t pos = M.translation( gwiz::COMP_M_TR );
		quat_t rot = M.quat( gwiz::COMP_M_TR );
		return( pos + rot * point_target_pos );
	}
	return( point_target_pos );
}

quat_t MeCtSimpleGaze::world_target_orient( void )	{
	
	SkJoint* joint_p = reference_joint();
	if( joint_p )	{
		SrMat sr_M;
		gwiz::matrix_t M;
		int i, j;
		
		joint_p->update_gmat_up();
		sr_M = joint_p->gmat();
		for( i=0; i<4; i++ )	{
			for( j=0; j<4; j++ )	{
				M.set( i, j, sr_M.get( i, j ) );
			}
		}
		quat_t rot = M.quat( gwiz::COMP_M_TR );
		return( rot * orient_target_rot );
	}
	return( orient_target_rot );
}

void MeCtSimpleGaze::controller_start( void )	{
	int i;
	
	start = 1;

	//{	// The following are for debugger runtime inspection
	//	SkChannelArray& channels = _context->channels();
	//	int size = channels().size();
	//	const char* ch_name = channels.name( 0).get_string();
	//	SkChannel::Type ch_type = channels.type(0);
	//	SkJoint* ch_joint = channels.joint( 0 );
	//}
	
	if( _context->channels().size() > 0 )	{
		skeleton_ref_p = _context->channels().skeleton();
	}
	
	update_skeleton_gmat();

// TODO: measure task angle for skull time control: 
//  spine1.parent_rot -> target
//  skull_rot -> target

	// set forward position to eyeball center in local coords
	joint_arr[ GAZE_JOINT_SPINE1 ].forward_pos = vector_t( 0.0, 63.0, 8.3 );
	joint_arr[ GAZE_JOINT_SPINE2 ].forward_pos = vector_t( 0.0, 53.1, 8.3 );
	joint_arr[ GAZE_JOINT_SPINE3 ].forward_pos = vector_t( 0.0, 41.9, 8.3 );
	joint_arr[ GAZE_JOINT_SPINE4 ].forward_pos = vector_t( 0.0, 14.8, 8.3 );
	joint_arr[ GAZE_JOINT_SPINE5 ].forward_pos = vector_t( 0.0, 11.1, 8.3 );
	joint_arr[ GAZE_JOINT_SKULL ].forward_pos =  vector_t( 0.0, 5.9, 8.3 );

	for( i=0; i<joint_count; i++ )	{

		float local_contrib;
		if( i < priority_joint )	{
			local_contrib = 1.0f / ( (float)( priority_joint + 1 - i ) );
		}
		else	{
			local_contrib = 1.0;
		}

		float local_flex = 1.0;
#if 1
		if( i < flexible_joint )	{
			local_flex = (float)( i + flexible_joint ) / ( (float)( flexible_joint * 2 ) );
		}
		else	{
			local_flex = 1.0;
		}
#endif
		
		joint_arr[ i ].weight = local_contrib * local_flex;
		
		float lim = 40.0;
		if( i < GAZE_JOINT_SPINE4 ) {
			lim = local_flex * 20.0f;
		}
		else
		if( i < GAZE_JOINT_SKULL )	{
			lim = local_flex * 30.0f;
		}
		
		joint_arr[ i ].limit = lim;

		int context_index = _toContextCh[ i ];
		if( context_index < 0 ) {
			fprintf( stderr, "MeCtSimpleGaze::controller_start ERR: joint idx:%d NOT FOUND in skeleton\n", i );
		} 
		else {
			SkJoint* joint_p = _context->channels().joint( context_index );
			if( !joint_p )	{
				fprintf( stderr, "MeCtSimpleGaze::controller_start ERR: joint context-idx:%d NOT FOUND in skeleton\n", context_index );
			} 
			else {
				joint_arr[ i ].init( joint_p );
				joint_arr[ i ].begin();
			}
		}

#if 0
		LOG( "start: [%d]", i );
		LOG( " c:%.4f x f:%.4f == w: %.4f", 
			local_contrib, local_flex, 
			joint_arr[ i ].weight 
		);
		LOG( " lim: %.4f\n", 
			joint_arr[ i ].limit 
		);
#endif
	}
}

bool MeCtSimpleGaze::controller_evaluate( double t, MeFrameData& frame )	{

#if 1
	static int once = 1;
	if( once )	{
		once = 0;
		
#if ENABLE_FORWARD_RAY_TEST
		test_forward_ray();
#endif
#if 0
		LOG( "-- skeleton:\n" );
		if( skeleton_ref_p )	{
			SkJoint* joint_p = skeleton_ref_p->search_joint( SbmPawn::WORLD_OFFSET_JOINT_NAME );
//			inspect_skeleton( joint_p );
//			inspect_skeleton_local_transform( joint_p );
			inspect_skeleton_world_transform( joint_p );
		}
		LOG( "--\n" );
#endif
	}
#endif
	
	if( _duration > 0.0 )	{
		if( t > (double)_duration )	{
			return( FALSE );
		}
	}
	
	float dt;
	if( start ) {
		start = 0;
		dt = 0.001f;
	}
	else	{
		dt = (float)(t - prev_time);
	}
	prev_time = t;

	update_skeleton_gmat();
	
	vector_t w_point;
	quat_t w_orient;
	if( target_mode == TARGET_POINT )	{
		w_point = world_target_point();
	}
	else	{
		w_orient = world_target_orient();
	}
	
	SrBuffer<float>& buff = frame.buffer();
	int channels_size = _channels.size();

	for( int i=0; i<channels_size; ++i ) {
		int index = frame.toBufferIndex( _toContextCh[ i ] );
			
if( G_debug_c_SG )	{
if( i == GAZE_JOINT_SKULL )	{
	G_debug_SG = 1;
}
}

		quat_t Q_out;
		if( target_mode == TARGET_POINT )	{
			Q_out = joint_arr[ i ].evaluate( dt, w_point, offset_rot, 1.0 );
		}
		else	{
			Q_out = joint_arr[ i ].evaluate( dt, w_orient, offset_rot, 1.0 );
		}

#define SPINE_LOCK_HEAD_TEST 0

#if SPINE_LOCK_HEAD_TEST
		if( i == GAZE_JOINT_SPINE1 )	{
//			euler_t e( 0.0, 0.0, 0.0 );
//			euler_t e( -90.0, 0.0, 0.0 );
//			euler_t e( 90.0, 0.0, 0.0 );
//			euler_t e( 0.0, 90.0, 0.0 );
//			euler_t e( 0.0, 180.0, 0.0 );
//			euler_t e( 0.0, 0.0, 90.0 );
//			Q_out = e;
		}
#endif

#if SPINE_LOCK_HEAD_TEST
		if( ( i == GAZE_JOINT_SPINE1 )||( i == GAZE_JOINT_SKULL ) )	{
			buff[ index + 0 ] = (float) Q_out.w();
			buff[ index + 1 ] = (float) Q_out.x();
			buff[ index + 2 ] = (float) Q_out.y();
			buff[ index + 3 ] = (float) Q_out.z();
		}
#else
		buff[ index + 0 ] = (float) Q_out.w();
		buff[ index + 1 ] = (float) Q_out.x();
		buff[ index + 2 ] = (float) Q_out.y();
		buff[ index + 3 ] = (float) Q_out.z();
#endif

G_debug_SG = 0;

		// Mark channel changed
		frame.channelUpdated( i );
	}

if( G_debug_c_SG )	{
	G_debug_c_SG--;
	LOG( "DEBUG %d\n", G_debug_c_SG );
}

	return( TRUE );
}

///////////////////////////////////////////////////////////////////////////
