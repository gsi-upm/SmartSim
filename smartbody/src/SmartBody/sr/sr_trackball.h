/*
 *  sr_trackball.h - part of Motion Engine and SmartBody-lib
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
 *      Marcelo Kallmann, USC (currently UC Merced)
 */

# ifndef SR_TRACKBALL_H
# define SR_TRACKBALL_H

/** \file sr_trackball.h 
 * trackball manipulation
 */
#include <sb/SBTypes.h>
# include <sr/sr_vec.h>
# include <sr/sr_quat.h>

class SrMat;

/*! \class SrTrackball sr_trackball.h
    \brief trackball manipulation

    SrTrackball maintains a rotation with methods implementing a
    trackball-like manipulations, and etc. */
class SBAPI SrTrackball
 { public :
    SrQuat rotation;    //!< current rotation
    SrQuat last_spin;

   public :
    
    /*! Initialize the trackball with the default parameters, see init(). */
    SrTrackball ();

    /*! Copy constructor. */
    SrTrackball ( const SrTrackball& t );

    /*! Set the parameters to their default values, which is a null rotation. */
    void init ();

    /*! Set m to be the equivalent transformation matrix. A reference to m
        is returned. */
    SrMat& get_mat ( SrMat& m ) const;

    /*! Gets the rotation induced by a mouse displacement,
        according to the trackball metaphor. Window coordinates must be
        normalized in [-1,1]x[-1,1]. */
    static void get_spin_from_mouse_motion ( float lwinx, float lwiny, float winx, float winy, SrQuat& spin );

    /*! Accumulates the rotation induced by a mouse displacement,
        according to the trackball metaphor. Window coordinates must be
        normalized in [-1,1]x[-1,1]. */
    void increment_from_mouse_motion ( float lwinx, float lwiny, float winx, float winy );

    /*! Accumulates the rotation with the given quaternion (left-multiplied) */
    void increment_rotation ( const SrQuat& spin );

    /*! Outputs trackball data for inspection. */
    friend SrOutput& operator<< ( SrOutput& out, const SrTrackball& tb );
 };

//================================ End of File =================================================

# endif // SR_TRACKBALL_H

