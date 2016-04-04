/*  sr_sa_bbox.h - part of Motion Engine and SmartBody-lib
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

# ifndef SR_SA_BBOX_H
# define SR_SA_BBOX_H

/** \file sr_sa_bbox.h 
 * retrives the bbox
 */
#include <sb/SBTypes.h>
# include <sr/sr_sa.h>

/*! \class SrSaBBox sr_sa_bbox.h
    \brief bbox action

    Retrieves the bounding box of a scene */
class SBAPI SrSaBBox : public SrSa
 { private :
    SrBox _box;

   public :
    void init () { _box.set_empty(); }
    void apply ( SrSn* n ) { init(); SrSa::apply(n); }
    const SrBox& get () const { return _box; }

   private : // virtual methods
    virtual bool shape_apply ( SrSnShapeBase* s );
 };

//================================ End of File =================================================

# endif  // SR_SA_BBOX_H

