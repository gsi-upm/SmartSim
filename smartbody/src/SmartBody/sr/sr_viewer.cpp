/*
 *  sr_viewer.cpp - part of SBM: SmartBody Module
 *  Copyright (C) 2008  University of Southern California
 *
 *  SBM is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SBM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SBM.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Marcelo Kallmann, USC (currently at UC Merced)
 */

#include "sr_viewer.h"

SrViewer::SrViewer()
{
}


SrViewer::SrViewer( int x, int y, int w, int h, const char *label )
{
}

SrViewer::~SrViewer()
{
}

SrSn* SrViewer::root ()
{
	return NULL;
}

void SrViewer::root ( SrSn *r )
{
}

void SrViewer::view_all()
{
}

void  SrViewer::render()
{
}

void  SrViewer::label_viewer(const char* str)
{
}

SrCamera* SrViewer::get_camera()
{
	return NULL;
}

void SrViewer::set_camera ( const SrCamera* cam )
{
}

void SrViewer::show_viewer()
{
}

void SrViewer::hide_viewer()
{
}


SrViewerFactory::SrViewerFactory()
{
}

SrViewer* SrViewerFactory::create(int x, int y, int w, int h)
{
	return new SrViewer(x, y, w, h);
}

void SrViewerFactory::remove(SrViewer* viewer)
{
}

void SrViewerFactory::reset(SrViewer* viewer)
{
}

void SrViewerFactory::setDefaultSize(int x, int y, int w, int h)
{
	if (x != -1)
		_x = x;
	if (y != -1)
		_y = y;
	if (w != -1)
		_w = w;
	if (h != -1)
		_h = h;
}

int SrViewerFactory::getX()
{
	return _x;
}

int SrViewerFactory::getY()
{
	return _y;
}

int SrViewerFactory::getW()
{
	return _w;
}

int SrViewerFactory::getH()
{
	return _h;
}


//================================ End of File =================================================