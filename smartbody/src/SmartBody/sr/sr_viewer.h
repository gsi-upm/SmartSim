/*
 *  sr_viewer.h - part of Motion Engine and SmartBody-lib
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

/** \file sr_viewer.h
 * A fltk-opengl viewer
 */

# ifndef SR_VIEWER_H
# define SR_VIEWER_H

#include <sb/SBTypes.h>
#include <cstdio>

class SrQuat;
class SrEvent;
class SrCamera;
class SrSn;
class SrViewerData;

/*! \class SrViewer sr_viewer.h
    \brief A  viewer

    SrViewer implements a viewer to render objects derived from SrSn.
    The viewer has currently a planar and a tridimensional examiner mode.
    In ModePlanar, only transformation on the XY plane are accepted.
    In all modes, mouse interaction is done together with Ctrl and Shift modifiers.
    A popup menu appears with a right button click or ctrl+shift+m. */
 class SBAPI SrViewer
 {
   public : //----> public methods 

	SrViewer();

    /*! Constructor needs the size and location of the window. */
    SrViewer ( int x, int y, int w, int h, const char *label=0 );

    /*! Destructs all internal data, and calls unref() for the root node. */
    virtual ~SrViewer ();

    /*! Retreave the scene root pointer, without calling unref() for it. Note that
        if the user does not give any root node to SrViewer, an empty (but valid)
        SrSnGroup is returned. */
    virtual SrSn *root ();

    /*! Changes the scene root pointer. When the new node r is given, r->ref() is 
        called, and the old root node has its unref() method called. If r is null,
        an empty SrSnGroup is created and used as root */
    virtual void root ( SrSn *r );


    /*! Sets the camera to see the whole bounding box of the scene. The camera
        center is put in the center of the bounding box, and the eye is put in
        the line passing throught the center and parallel to the z axis, in a
        sufficient distance from the center to visualize the entire bounding,
        leaving the camera with a 60 degreed fovy. The up vector is set to (0,1,0). */
    virtual void view_all ();

    /*! Will make SrViewer to render the scene in the next fltk loop. If the
        current scene is spinning, then the scene is already being rendered by the
        spin animation timeout with a defined frequency, an then a call to render()
        will have no effect. */
    virtual void render ();

	// feng : since we need access to OpenGL context before any shader call, 
	// this hack ensure that we have a valid gl context ( from fltkViewer or other render GUI ) before any GPGPU calls
	virtual void makeGLContext() {}

	virtual void label_viewer(const char* str);

    virtual SrCamera* get_camera();
    virtual void set_camera ( const SrCamera* cam );

	virtual void show_viewer();
	virtual void hide_viewer();

 };

class SrViewerFactory
{
	public:
		SBAPI SrViewerFactory();

		SBAPI void setDefaultSize(int x, int y, int w, int h);
		SBAPI int getX();
		SBAPI int getY();
		SBAPI int getW();
		SBAPI int getH();
		
		SBAPI virtual SrViewer* create(int x, int y, int w, int h);
		SBAPI virtual void remove(SrViewer* viewer);
		SBAPI virtual void reset(SrViewer* viewer);

protected:
		int _x;
		int _y;
		int _w;
		int _h;

};

//================================ End of File =================================================

# endif // SR_VIEWER_H
