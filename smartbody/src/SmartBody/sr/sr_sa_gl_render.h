/*  sr_sa_gl_render.h - part of Motion Engine and SmartBody-lib
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

# ifndef SR_SA_GL_RENDER_H
# define SR_SA_GL_RENDER_H

/** \file sr_gl_render_action.h 
 * OpenGL render action
 */
#include <sb/SBTypes.h>
# include <sr/sr_sa.h>
# include <sr/sr_sn_shape.h>

struct SrOverrideAction;
struct SrOGLData;

/*! \class SrSaGlRender sr_gl_render_action.h
    \brief OpenGL render action

    OpenGL render action */
class SBAPI SrSaGlRender : public SrSa
 { public :
    typedef void (*render_function)(SrSnShapeBase*);

   private :
    struct RegData { const char* class_name;
                     render_function rfunc;
                   };
    static SrArray<RegData> _rfuncs;
    friend struct SrOverrideAction;
    friend struct SrOGLData;
    
   public :
    /*! Constructor */
    SrSaGlRender ();

    /*! Virtual destructor. */
    virtual ~SrSaGlRender ();

    /*! Registration is kept in a static array, shared by all instances
        of SrSaGlRender.
        All buit-in SR scene shapes are automatically registered when
        the first SrSaGlRender instance is done. However,
        registration must be explicitly called for user-defined shapes. 
        In case a name already registered is registered again, the new
        one replaces the old one. */
    friend void register_render_function ( const char* class_name, render_function rfunc );

    /*! Traverse all scene graph overriding the render mode of each shape node. */
    void override_render_mode ( SrSn* n, srRenderMode m );

    /*! Traverse all scene graph restoring the original render mode of each
        shape node, saved at the last override_render_mode() call. */
    void restore_render_mode ( SrSn* n );

   private :
    virtual void get_top_matrix ( SrMat& mat );
    virtual int matrix_stack_size ();
    virtual void init_matrix ();
    virtual void mult_matrix ( const SrMat& mat );
    virtual void push_matrix ();
    virtual void pop_matrix ();
    virtual bool shape_apply ( SrSnShapeBase* s );
 };

//================================ End of File =================================================

# endif  // SR_SA_GL_RENDER_H
