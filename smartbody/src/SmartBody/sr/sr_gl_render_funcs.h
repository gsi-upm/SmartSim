/*
 *  sr_gl_funcs.h - part of Motion Engine and SmartBody-lib
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

# ifndef SR_GL_RENDER_FUNCS_H
# define SR_GL_RENDER_FUNCS_H

#include <sb/SBTypes.h>

/** \file sr_gl_render_funcs.h 
 * OpenGL render functions of SR shapes
 */

class SrSnShapeBase;
class DeformableMeshInstance;

/*! All render functions used to render SR shapes are static methods
    of class SrGlRenderFuncs. They are automatically registered to
    the ogl render action. See SrGlRenderAction class description. */
class SBAPI SrGlRenderFuncs
 { public:

	static void renderBlendFace( DeformableMeshInstance* shape);
	
	static void renderDeformableMesh( DeformableMeshInstance* shape, bool showSkinWeight = false );
    static void render_model ( SrSnShapeBase* shape );

    static void render_lines ( SrSnShapeBase* shape );

    static void render_points ( SrSnShapeBase* shape );

    /*! The default render mode is smooth, and flat has the same
        effect of smooth shading. */
    static void render_box ( SrSnShapeBase* shape );

    /*! The resolution value stored in SrSnShapeBase
        indicates how many triangles is used to
        draw the sphere. A value lower or equal to 0.2 defines
        the first level (8 triangles), then levels are increased
        each time 0.2 is added to the resolution. Resolution 1.0
        represents a well discretized sphere. */
    static void render_sphere ( SrSnShapeBase* shape );

    /*! The resolution value stored in SrSnShapeBase
        represents 1/10th of the number of edges discretizing the base
        circle of the cylinder. For ex., resolution 1.0 results in
        10 edges, and gives 40 triangles to render the cylinder. */
    static void render_cylinder ( SrSnShapeBase* shape );

    static void render_polygon ( SrSnShapeBase* shape );

    static void render_polygons ( SrSnShapeBase* shape );
 };

//================================ End of File =================================================

# endif  // SR_GL_RENDER_FUNCS_H

