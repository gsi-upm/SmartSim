/*  sr_plane.h - part of Motion Engine and SmartBody-lib
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
 *  License along with SmarBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Marcelo Kallmann, USC (currently UC Merced)
 */

# ifndef SR_PLANE_H
# define SR_PLANE_H

#include <sb/SBTypes.h>
# include <sr/sr_vec.h>

class SBAPI SrPlane
 { public :
    SrVec coords;
    float coordsw;
    static const SrPlane XY; //<! The plane of ( SrVec::null, SrVec::i, SrVec::j )
    static const SrPlane XZ; //<! The plane of ( SrVec::null, SrVec::i, SrVec::k )
    static const SrPlane YZ; //<! The plane of ( SrVec::null, SrVec::j, SrVec::k )

   public:

    /*! The default constructor initializes as the plane passing at (0,0,0)
        with normal (0,0,1). */
    SrPlane ();

    /*! Constructor from center and normal. Normal will be normalized. */
    SrPlane ( const SrVec& center, const SrVec& normal );

    /*! Constructor from three points in the plane. */
    SrPlane ( const SrVec& p1, const SrVec& p2, const SrVec& p3 );

    /*! Set as the plane passing at center with given normal. Normal will be normalized. */
    bool set ( const SrVec& center, const SrVec& normal );

    /*! Set as the plane passing trough three points. */
    bool set ( const SrVec& p1, const SrVec& p2, const SrVec& p3 );

    /*! determines if the plane is parallel to the line [p1,p2], according
        to the (optional) given precision ds. This method is fast (as the
        plane representation is kept normalized) and performs only one 
        subraction and one dot product. */
    bool parallel ( const SrVec& p1, const SrVec& p2, float ds=0 ) const;

    /*! Returns p, that is the intersection between SrPlane and the infinity
        line <p1,p2>. (0,0,0) is returned if they are parallel. Use parallel()
        to test this before. If t is non null, t will contain the relative
        interpolation factor such that p=p1(1-t)+p2(t). */
    SrVec intersect ( const SrVec& p1, const SrVec& p2, float *t=0 ) const;
 };

# endif // SR_PLANE_H
