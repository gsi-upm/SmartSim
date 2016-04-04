/*
 *  sr_spline.h - part of Motion Engine and SmartBody-lib
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

/** \file sr_spline.h
 * piecewise spline */

# ifndef SR_SPLINE_H
# define SR_SPLINE_H

#include <sb/SBTypes.h>
# include <math.h>
# include <sr/sr_array.h> 

/*! \class SrSpline sr_spline.h
    \brief A N-dimensional piecewise cubic spline 
    
    Implements a N-dimensional piecewise cubic spline 
    
    Given control points x0 x1 x2 x3, "their cubic spline"
    S(t) = at3 + bt2  + ct + x0 has coefficients
    c=3(x1-x0), b=3(x2-x1)-c, a=x3-x0-c-b.
        
    This class is not yet implemented!
     */
class SBAPI SrSpline
 { private :
    int _dim;               // the dimension of each point
    int _pieces;            // the number of cubic pieces
    SrArray<float> _spline; // each (3*_dim) position starts one cubic spline
       
   private :

   public :

    /*! The constructor initializes a piecewise spline with
        given dimension and number of knots. */
    SrSpline ( int d=0, int k=0 ) { init(d,k); }

    /*! Copy constructor. */
    SrSpline ( const SrSpline& c );

    /*! Initializes spline in given dimension and number of knots.
        The start and end points are considered to be knots, thus a meaningfull
        number of knots will be >= 2. */
    void init ( int d, int k );

    /*! Returns the number of knots (which include endpoints) */
    int knots () const { return _pieces+1; }
    
    /*! Returns a pointer to the n-dimensional coordinates of given knot
        Parameter k must obey 0<=k<knots() */
    float* knot ( int k ) { return &_spline[k*_dim*3]; }
    
    /*! Returns a pointer to the n-dimensional coordinates of the control
        point c relative to the spline starting at knot k.
        Parameters must obey 0<=k<knots()-1, and 1<=c<=2 */
    float* control ( int k, int c ) { return &_spline[(k*_dim*3)+(_dim*c)]; }
    
    /*! Output */
    //friend SrOutput& operator<< ( SrOutput& out, const SrSpline& c );

    /*! Input */
    //friend SrInput& operator>> ( SrInput& in, SrSpline& c );
 };

//============================== end of file ===============================

# endif // SR_SPLINE_H


