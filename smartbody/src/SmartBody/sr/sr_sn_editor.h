/*  sr_sn_editor.h - part of Motion Engine and SmartBody-lib
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

# ifndef SR_SN_EDITOR_H
# define SR_SN_EDITOR_H

/** \file sr_sn_editor.h 
 * base editor node
 */

# include <sr/sr_sn.h>
# include <sr/sr_sn_group.h>

//==================================== SrSnEditor ====================================

/*! \class SrSnEditor sr_sn_editor.h
    \brief manipulates/edit scene nodes

    SrSnEditor is to be used as a base class to be derived.
    It keeps one child which is the node to manipulate, a 
    transformation matrix, and a list of internal nodes to display
    any required manipulation/edition helpers. */
class SrSnEditor : public SrSn
 { private :
    SrMat _mat;
    SrSn* _child;
    SrSnGroup* _helpers;

   protected :
    /*! Changes the current node to be manipulated, normally this is to be
        managed by the derived class specific implementation */
    void child ( SrSn *sn );

    /*! Destructor dereferences the child and the helpers group.
        Only accessible through unref(). */
    virtual ~SrSnEditor ();

   public :
    /*! Constructor requires the name of the derived class. */
    SrSnEditor ( const char* name );

    /*! Get a reference to the manipulator matrix. */
    SrMat& mat () { return _mat; }

    /*! Set a new manipulation matrix. */
    void mat ( const SrMat& m ) { _mat=m; }

    /*! Get pointer to the helpers group. Only derived classes should operate
        on the helpers class, however other classes (like the draw action) need
        at least reading access */
    SrSnGroup* helpers () const { return _helpers; }

    /*! Get a pointer to the current child. Can be null. */
    SrSn* child () const { return _child; }

    /*! If 1 is returned, the event is no more propagated and the scene will be
        redrawn. The SrSnEditor implementation simply returns 0 */
    virtual int handle_event ( const SrEvent &e );
 };

//================================ End of File =================================================

# endif  // SR_SN_EDITOR_H

