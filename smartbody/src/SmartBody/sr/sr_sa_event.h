/*  sr_sa_event.h - part of Motion Engine and SmartBody-lib
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

# ifndef SR_SA_EVENT_H
# define SR_SA_EVENT_H

/** \file sr_sa.h 
 * propagates events
 */
#include <sb/SBTypes.h>
# include <sr/sr_sa.h>
# include <sr/sr_event.h>

/*! \class SrSaEvent sr_sa_event.h
    \brief propagates events in a scene

    sends an event to the scene graph */
class SBAPI SrSaEvent : public SrSa
 { private :
    SrEvent _ev;
    int _result;

   public :
    SrSaEvent ( const SrEvent& e ) { _ev=e; _result=0; }
    SrEvent& get () { return _ev; }
    int result () const { return _result; }

   private : // virtual methods
    virtual bool editor_apply ( SrSnEditor* m );
 };

//================================ End of File =================================================

# endif  // SR_SA_EVENT_H

