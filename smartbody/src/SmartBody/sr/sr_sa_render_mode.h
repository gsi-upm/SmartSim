/*  sr_sa_render_mode.h - part of Motion Engine and SmartBody-lib
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

# ifndef SR_SA_RENDER_MODE_H
# define SR_SA_RENDER_MODE_H

/** \file sr_sa_render_mode.h 
 * changes the render mode
 */
#include <sb/SBTypes.h>
# include <sr/sr_sa.h>
# include <sr/sr_sn_shape.h>

/*! \class SrSaRenderMode sr_sa_render_mode.h
    \brief changes the render mode

    changes the render mode of all nodes visited by the action */
class SBAPI SrSaRenderMode : public SrSa
 { private :
    bool _override;
    srRenderMode _render_mode;

   public :

    /*! Constructor that initializes the action to override the render mode to m */
    SrSaRenderMode ( srRenderMode m ) { set_mode(m); }

    /*! Constructor that initializes the action to restore the original render mode */
    SrSaRenderMode () { _render_mode=srRenderModeSmooth; _override=false; }

    /*! Set the mode m to be overriden */
    void set_mode ( srRenderMode m ) { _render_mode=m; _override=true; }

    /*! Set the action to restore the original render mode */
    void restore_mode () { _override=false; }

   private : // virtual methods
    virtual void mult_matrix ( const SrMat& mat ) {}
    virtual void push_matrix () {}
    virtual void pop_matrix () {}
    virtual bool shape_apply ( SrSnShapeBase* s );
    virtual bool matrix_apply ( SrSnMatrix* m ) { return true; }
 };

//================================ End of File =================================================

# endif  // SR_SA_RENDER_MODE_H

