/*
 *  sr_gl.h - part of Motion Engine and SmartBody-lib
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

# ifndef SR_GL_H
# define SR_GL_H

#include <sb/SBTypes.h>

/** \file sr_gl.h 
 * Sr wrapper and extensions for OpenGL
 *
 * Overload of most used OpenGL functions to directly work with SR types,
 * and some extra utilities.
 */

# include <sr/sr.h>

# ifdef SR_TARGET_WINDOWS // defined in sr.h
# include <Windows.h>
# endif


#ifdef WIN32
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wingdi.h>
#include <GL/gl.h>
#include <GL/glu.h>
#elif defined(SB_IPHONE)
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#elif defined(__APPLE__) || defined(__APPLE_CC__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
//       #include <Carbon/Carbon.h>
#define APIENTRY
#elif defined(__FLASHPLAYER__)
#include <GL/gl.h>
#elif __native_client__
#include <GLES2/gl2.h>
#elif defined(__ANDROID__)
//#include <GLES/gl.h>
#include <GLES2/gl2.h>
//#include "wes_gl.h"
#else
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#endif


// #if defined(SB_IPHONE)
// #include <OpenGLES/ES1/gl.h>
// #include <OpenGLES/ES1/glext.h>
// #elif __APPLE__
// #include <OpenGL/gl.h>
// #include <OpenGL/glu.h>
// #elif __native_client__
// #include <GLES2/gl2.h>
// #elif defined(__ANDROID__)
// //#include <GLES/gl.h>
// #include <GLES2/gl2.h>
// //#include "wes_gl.h"
// //#include "wes_glu.h"
// 
// # else
// # include <GL/gl.h>
// # include <GL/glu.h>
// # endif

class SrVec;
class SrMat;
class SrQuat;
class SrColor;
class SrLight;
class SrOutput;
class SrMaterial;

#if defined(__ANDROID__) || defined(SB_IPHONE)
#define GLES_RENDER 1
#define USE_GL_FIXED_PIPELINE 0
#define GLdouble GLfloat
#else
#define GLES_RENDER 0
#define USE_GL_FIXED_PIPELINE 1
#endif

#if 0 //defined(__ANDRID__)
#define myGLEnable glEnable_wes
#define myGLDisable glDisable_wes
#else
#define myGLEnable glEnable
#define myGLDisable glDisable
#endif

//======================================= geometry ================================
SBAPI void glNormal ( const SrVec &v );

SBAPI void glVertex ( const SrVec &v );
SBAPI void glVertex ( const SrVec &v1, const SrVec &v2 );
SBAPI void glVertex ( const SrVec &v1, const SrVec &v2, const SrVec &v3 );
SBAPI void glVertex ( const SrVec &v1, const SrVec &v2, const SrVec &v3, const SrVec &v4 );
SBAPI void glVertex ( float x, float y, float z );
SBAPI void glVertex ( float x, float y, float z, float a, float b, float c );
SBAPI void glVertex ( float x, float y );
SBAPI void glDrawBox ( const SrVec& a, const SrVec& b ); //!< Send quads with normals forming the box

//====================================== appearance ================================
SBAPI void glColor ( const SrColor& c );

SBAPI void glClearColor ( const SrColor& c );
SBAPI void glLight ( int id, const SrLight& l, bool bind_pos = true ); //!< id = x E {0,...,7}, from GL_LIGHTx
SBAPI void glLightPos( int id, const SrLight& l );
SBAPI void glMaterial ( const SrMaterial &m ); //!< Sets material for GL_FRONT_AND_BACK
SBAPI void glMaterialFront ( const SrMaterial &m );
SBAPI void glMaterialBack ( const SrMaterial &m );

//==================================== matrices ==============================

SBAPI void glMultMatrix ( const SrMat &m );
SBAPI void glLoadMatrix ( const SrMat &m );
SBAPI void glTranslate ( const SrVec &v );
SBAPI void glScale ( float s );
SBAPI void glRotate ( const SrQuat &q );
SBAPI void glLookAt ( const SrVec &eye, const SrVec &center, const SrVec &up );
SBAPI void glPerspective ( float fovy, float aspect, float znear, float zfar );
SBAPI void glGetViewMatrix ( SrMat &m );
SBAPI void glGetProjectionMatrix ( SrMat &m );

//==================================== info ==============================

SBAPI void glPrintInfo ( SrOutput &o );

//================================ End of File ==================================

# endif // SR_GL_H
