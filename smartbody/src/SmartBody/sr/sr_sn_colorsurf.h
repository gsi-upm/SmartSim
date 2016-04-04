/*
 *  sr_sn_colorsurf.h - part of Motion Engine and SmartBody-lib
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
 *  License along with SmarBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Marcelo Kallmann, USC (currently UC Merced)
 *      David Huang
 */

# ifndef SN_COLOR_SURF_H
# define SN_COLOR_SURF_H

/** \file sr_sn_colorsurf.h 
 * color interpolation in a mesh
 */
#include <sb/SBTypes.h>
#include <vector>
# include <sr/sr_model.h>
# include <sr/sr_sn_shape.h>


/*! \class SrSnColorSurf sr_sn_colorsurf.h
    \brief model color interpolation in a mesh

    Performs color interpolation among mesh vertices. */
class SrSnColorSurf : public SrSnShapeBase
 { protected :
    SrModel* _model;
	

   public :
    static const char* class_name; //<! Contains string SrSnColorSurf
	std::vector< std::vector<int> > vtxAdjList;
	SrVec surfaceScale;

   protected :

    /* Constructor for derived classes */
    SrSnColorSurf ( const char* classname );

   public :

    /* Constructor may receive a SrModel to reference. If the
       given pointer is null (the default) a new one is used. */
   SBAPI SrSnColorSurf ( SrModel* m=0 );

    /* Destructor. */
   SBAPI ~SrSnColorSurf ();

    /*! Set the shared SrModel object to display and
        mark this shape node as changed. A null pointer
        can be given, in which case a new SrModel is used. */
    SBAPI void model( SrModel* m );

    /*! Access to the (always valid) shared SrModel object. */
    SBAPI SrModel* model () const { return _model; }

    /*! Const access to the (always valid) shared SrModel. */
    SBAPI const SrModel* cmodel () const { return _model; }

   public :

    /*! Returns the bounding box of all vertices used.
        The returned box can be empty. */
    SBAPI void get_bounding_box ( SrBox &b ) const;

    /*! This method is called right before gl_render_node() and 
        loads any needed texture not yet loaded.*/
    SBAPI virtual void gl_prior_render_node () const;

    /*! Makes OpenGL calls to draw the lines.
        If no colors are specified, SnShape::color() is used.
        The line width can be set with SnShape::resolution(). */
    SBAPI virtual void gl_render_node(bool alphaBlend=true) const;
 };

//================================ End of File =================================================

# endif  // SN_COLOR_SURF_H

