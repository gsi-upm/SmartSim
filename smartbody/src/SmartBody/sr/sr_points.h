/*  sr_points.h - part of Motion Engine and SmartBody-lib
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
 *      Andrew n marshall, USC
 */

# ifndef SR_POINTS_H
# define SR_POINTS_H

/** \file sr_points.h 
 * manages a set of points
 */
#include <sb/SBTypes.h>
# include <sr/sr_box.h>
# include <sr/sr_vec2.h>
# include <sr/sr_color.h>
# include <sr/sr_array.h>

/*! \class SrPoints sr_points.h
    \brief a set of points

    Keeps the description of points.
    Points are stored in array P.
    An optional array A with atributes (color and size) can be used.
    Even if P is a public member, it is the user responsability to
    mantain the size of P equal to the size of A, if A!=0. */   
class SBAPI SrPoints
 { public :
    SrArray<SrPnt> P;  //<! Array of used points
    struct Atrib { SrColor c; float s; };
    SrArray<Atrib>* A; //<! Optional to use attributes
    static const char* class_name;

   public :

    /* Default constructor. */
    SrPoints ();

    /* Destructor . */
    virtual ~SrPoints ();

    /*! Set the size of array P to zero. */
    void init ();

    /*! Allocates A if needed, and set the size of array P and A to zero. */
    void init_with_attributes ();

    /*! Returns true if P array is empty; false otherwise. */
    bool empty () const { return P.size()==0? true:false; }

    /*! Compress array P and A. */
    void compress ();

    /*! Push in P a new point */
    void push ( const SrPnt& p );
    void push ( const SrPnt2& p );
    void push ( float x, float y, float z=0 );

    /*! Push in P a new point and push attributes in A. Only valid
        if init_with_attributes() was called before */
    void push ( const SrPnt& p, SrColor c, float size=1.0f );
    void push ( const SrPnt2& p, SrColor c, float size=1.0f );
    void push ( float x, float y, SrColor c, float size=1.0f );
    void push ( float x, float y, float z, SrColor c, float size=1.0f );

    /*! Returns the bounding box of all vertices used. The returned box can be empty. */
    void get_bounding_box ( SrBox &b ) const;
 };


//================================ End of File =================================================

# endif  // SR_POINTS_H

