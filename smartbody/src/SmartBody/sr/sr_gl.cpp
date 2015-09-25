/*
 *  sr_gl.cpp - part of Motion Engine and SmartBody-lib
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

# include <sr/sr_mat.h>
# include <sr/sr_vec.h>
# include <sr/sr_color.h>
# include <sr/sr_quat.h>
# include <sr/sr_output.h>
# include <sr/sr_material.h>
# include <sr/sr_light.h>
# include <sr/sr_gl.h>

# include <math.h>

# ifdef SR_TARGET_WINDOWS
# include <Windows.h>
# endif


//======================================= geometry ================================

#if !defined(GLES_RENDER)
void glNormal ( const SrVec &v )
{
   glNormal3fv ( (const float*) v );
 }

void glVertex ( const SrVec &v )
 {
   glVertex3fv ( (const float*) v );
 }

void glVertex ( const SrVec &v1, const SrVec &v2 )
 {
   glVertex3fv ( (const float*) v1 );
   glVertex3fv ( (const float*) v2 );
 }

void glVertex ( const SrVec &v1, const SrVec &v2, const SrVec &v3 )
 {
   glVertex3fv ( (const float*) v1 );
   glVertex3fv ( (const float*) v2 );
   glVertex3fv ( (const float*) v3 );
 }

void glVertex ( const SrVec &v1, const SrVec &v2, const SrVec &v3, const SrVec &v4 )
 {
   glVertex3fv ( (const float*) v1 );
   glVertex3fv ( (const float*) v2 );
   glVertex3fv ( (const float*) v3 );
   glVertex3fv ( (const float*) v4 );
 }

void glVertex ( float x, float y, float z )
 {
   glVertex3f ( x, y, z );
 }

void glVertex ( float x, float y, float z, float a, float b, float c )
 {
   glVertex3f ( x, y, z );
   glVertex3f ( a, b, c );
 }

void glVertex ( float x, float y )
 {
   glVertex2f ( x, y );
 }

void glDrawBox ( const SrVec& a, const SrVec& b )
 {
   glBegin ( GL_QUADS );

   glNormal3f ( 0, 0, 1 );
   glVertex3f ( a.x, b.y, b.z );
   glVertex3f ( a.x, a.y, b.z );
   glVertex3f ( b.x, a.y, b.z );
   glVertex3f ( b.x, b.y, b.z );

   glNormal3f ( 0, 0, -1 );
   glVertex3f ( b.x, b.y, a.z );
   glVertex3f ( b.x, a.y, a.z );
   glVertex3f ( a.x, a.y, a.z );
   glVertex3f ( a.x, b.y, a.z );

   glNormal3f ( 1, 0, 0 );
   glVertex3f ( b.x, b.y, b.z );
   glVertex3f ( b.x, a.y, b.z );
   glVertex3f ( b.x, a.y, a.z );
   glVertex3f ( b.x, b.y, a.z );

   glNormal3f ( -1, 0, 0 );
   glVertex3f ( a.x, a.y, a.z );
   glVertex3f ( a.x, a.y, b.z );
   glVertex3f ( a.x, b.y, b.z );
   glVertex3f ( a.x, b.y, a.z );

   glNormal3f ( 0, 1, 0 );
   glVertex3f ( b.x, b.y, b.z );
   glVertex3f ( b.x, b.y, a.z );
   glVertex3f ( a.x, b.y, a.z );
   glVertex3f ( a.x, b.y, b.z );

   glNormal3f ( 0, -1, 0 );
   glVertex3f ( b.x, a.y, b.z );
   glVertex3f ( a.x, a.y, b.z );
   glVertex3f ( a.x, a.y, a.z );
   glVertex3f ( b.x, a.y, a.z );

   glEnd ();
 }
//====================================== appearance ================================

void glColor ( const SrColor& c )
 {
   glColor4ubv ( (const GLubyte*)&c );
 }

#endif

void glClearColor ( const SrColor& c )
 {
   glClearColor ( float(c.r)/255.0f,
                  float(c.g)/255.0f,
                  float(c.b)/255.0f,
                  float(c.a)/255.0f );
 }

void glLight ( int id, const SrLight& l, bool bind_pos )
 {
   float f[4];

   GLenum n = id==0? GL_LIGHT0 :
              id==1? GL_LIGHT1 :
              id==2? GL_LIGHT2 :
              id==3? GL_LIGHT3 :
              id==4? GL_LIGHT4 :
              id==5? GL_LIGHT5 :
              id==6? GL_LIGHT6 : GL_LIGHT7;

   glLightf  ( n, GL_SPOT_EXPONENT, l.spot_exponent );
   glLightfv ( n, GL_SPOT_DIRECTION, (float*)l.spot_direction );
   glLightf  ( n, GL_SPOT_CUTOFF, l.spot_cutoff );
   glLightf  ( n, GL_CONSTANT_ATTENUATION, l.constant_attenuation );
   glLightf  ( n, GL_LINEAR_ATTENUATION, l.linear_attenuation );
   glLightf  ( n, GL_QUADRATIC_ATTENUATION, l.quadratic_attenuation );

   l.ambient.get(f);  glLightfv ( n, GL_AMBIENT, f );
   l.diffuse.get(f);  glLightfv ( n, GL_DIFFUSE, f );
   l.specular.get(f); glLightfv ( n, GL_SPECULAR, f );

   if( bind_pos )	{
	l.position.get ( f );
	f[3] = l.directional? 0.0f:1.0f;
	glLightfv ( n, GL_POSITION, f );
   }
   myGLEnable( n );
 }

void glLightPos( int id, const SrLight& l )	{
   float f[4];
   GLenum n = id==0? GL_LIGHT0 :
              id==1? GL_LIGHT1 :
              id==2? GL_LIGHT2 :
              id==3? GL_LIGHT3 :
              id==4? GL_LIGHT4 :
              id==5? GL_LIGHT5 :
              id==6? GL_LIGHT6 : GL_LIGHT7;

	l.position.get ( f );
	f[3] = l.directional? 0.0f:1.0f;
	glLightfv ( n, GL_POSITION, f );
}

void glMaterial ( const SrMaterial &m )
 {
   float f[4];
   m.diffuse.get(f);  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,f);

//sr_out<<"glMaterial: "<<m.diffuse.r<<srspc<<m.diffuse.g<<srspc<<m.diffuse.b<<srnl;

   m.ambient.get(f);  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,f);
   m.specular.get(f); glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,f);
   m.emission.get(f); glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,f);
#if GLES_RENDER
   glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, m.shininess);
#else
   glMateriali ( GL_FRONT_AND_BACK, GL_SHININESS, int(m.shininess) );
#endif
 }

//==================================== matrices ==============================

void glMultMatrix ( const SrMat &m )
 {
	 SrMat tm = m;
   glMultMatrixf ( (float*)tm );
 }

void glLoadMatrix ( const SrMat &m )
 {
	 SrMat tm = m;
   glLoadMatrixf ( (float*)tm );
 }

void glTranslate ( const SrVec &v )
 {
   glTranslatef ( v.x, v.y, v.z );
 }

void glScale ( float s )
 {
   glScalef ( (float)s, (float)s, (float)s );
 }

void glRotate ( const SrQuat &q )
 {
   SrMat m(SrMat::NotInitialized);
   q.get_mat(m);
   glMultMatrixf ( (float*)m );
 }

void glLookAt ( const SrVec &eye, const SrVec &center, const SrVec &up )
 {
   SrMat m(SrMat::NotInitialized);
   m.look_at ( eye, center, up );
   glMultMatrixf ( (float*)m );
 }

void glPerspective ( float fovy, float aspect, float znear, float zfar )
 {
   GLdouble ymax = (GLdouble)znear * tan ( fovy/2 );

#if GLES_RENDER
   /*
   glFrustumf( (GLdouble) ((-ymax)*(GLdouble)aspect), // xmin
               (GLdouble) (( ymax)*(GLdouble)aspect), // xmax
               (GLdouble) (-ymax),                    // ymin
               (GLdouble) ymax, 
               (GLdouble) znear, 
               (GLdouble) zfar   
             );
			 */
#if defined(SB_IPHONE)
   glPerspective(fovy, aspect, znear, zfar);
#else
   gluPerspective(fovy, aspect, znear, zfar);
#endif
#else
   glFrustum ( (GLdouble) ((-ymax)*(GLdouble)aspect), // xmin
               (GLdouble) (( ymax)*(GLdouble)aspect), // xmax
               (GLdouble) (-ymax),                    // ymin
               (GLdouble) ymax, 
               (GLdouble) znear, 
               (GLdouble) zfar   
             );
#endif
 }

void glGetViewMatrix ( SrMat &m )
 {
#if GLES_RENDER
	 // no implementation
#else
   glGetFloatv ( GL_MODELVIEW_MATRIX, (float*)m );
#endif
 }

void glGetProjectionMatrix ( SrMat &m )
 {
#if GLES_RENDER
	 // no implementation
#else
   glGetFloatv ( GL_PROJECTION_MATRIX, (float*)m );
#endif
 }

//=================================== info ====================================

static const char* sr_error_string ()
 {
   switch ( glGetError() )
    { case GL_NO_ERROR : return "no error";
      case GL_INVALID_ENUM : return "invalid enum";
      case GL_INVALID_VALUE : return "invalid value";
      case GL_INVALID_OPERATION : return "invalid operation";
#if !GLES_RENDER
      case GL_STACK_OVERFLOW : return "stack overflow";
      case GL_STACK_UNDERFLOW : return "stack underflow";
#endif
      case GL_OUT_OF_MEMORY : return "out of memory";
      default : return "error not recognized";
    }
 }

/*
const GLubyte *glGetString(GLenum name)
name Specifies a symbolic constant, one of GL_VENDOR, GL_RENDERER, GL_VERSION, or GL_EXTENSIONS. 
*/

void glPrintInfo ( SrOutput &o )
 { 
   GLint i, v[2];

   o << "Last Error : " << sr_error_string() << srnl;

#if !GLES_RENDER
   glGetIntegerv(GL_STEREO,&i);
   o<<"GL_STEREO : " << (int)i << '\n';
   glGetIntegerv(GL_MAX_LIST_NESTING,&i);  

   o<<"GL_MAX_LIST_NESTING : " << (int)i << '\n';

   glGetIntegerv(GL_MAX_ATTRIB_STACK_DEPTH,&i); 
   o<<"GL_MAX_ATTRIB_STACK_DEPTH : " << (int)i << '\n';

   glGetIntegerv(GL_MAX_NAME_STACK_DEPTH,&i);  
   o<<"GL_MAX_NAME_STACK_DEPTH : " << (int)i << '\n';

   glGetIntegerv(GL_MAX_EVAL_ORDER,&i);  
   o<<"GL_MAX_EVAL_ORDER : " << (int)i << '\n';

   glGetIntegerv(GL_MAX_PIXEL_MAP_TABLE,&i);  
   o<<"GL_MAX_PIXEL_MAP_TABLE : " << (int)i << '\n';

   glGetIntegerv(GL_ACCUM_RED_BITS ,&i); 
   o<<"GL_ACCUM_REG_BITS : " << (int)i << '\n';

   glGetIntegerv(GL_ACCUM_GREEN_BITS ,&i); 
   o<<"GL_ACCUM_GREEN_BITS : " << (int)i << '\n';

   glGetIntegerv(GL_ACCUM_BLUE_BITS ,&i); 
   o<<"GL_ACCUM_BLUE_BITS : " << (int)i << '\n';

   glGetIntegerv(GL_ACCUM_ALPHA_BITS ,&i); 
   o<<"GL_ACCUM_ALPHA_BITS : " << (int)i << '\n';


   glGetIntegerv(GL_MAX_LIGHTS,&i);  
   o<<"GL_MAX_LIGHTS : " << (int)i << '\n';   

   glGetIntegerv(GL_MAX_VIEWPORT_DIMS,v);  
   o<<"GL_MAX_VIEWPORT_DIMS : (" << v[0] <<","<<v[1] << ")\n";

   glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH,&i); 
   o<<"GL_MAX_MODELVIEW_STACK_DEPTH (>=32) : " << (int)i << '\n';

   glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH,&i);  
   o<<"GL_MAX_PROJECTION_STACK_DEPTH : " << (int)i << '\n';   

   glGetIntegerv(GL_MAX_TEXTURE_STACK_DEPTH,&i);  
   o<<"GL_MAX_TEXTURE_STACK_DEPTH : " << (int)i << '\n';

   glGetIntegerv(GL_MAX_TEXTURE_SIZE,&i);  
   o<<"GL_MAX_TEXTURE_SIZE : " << (int)i << '\n';

   glGetIntegerv(GL_MAX_CLIP_PLANES,&i);  
   o<<"GL_MAX_CLIP_PLANES : " << (int)i << '\n';   

   o << "GL_VENDOR : "     << (char*) glGetString (GL_VENDOR) << '\n';
   o << "GL_RENDERER : "   << (char*) glGetString (GL_RENDERER) << '\n';
   o << "GL_VERSION : "    << (char*) glGetString (GL_VERSION) << '\n';

   SrInput inp ( (char*) glGetString (GL_EXTENSIONS) );
   o << "GL_EXTENSIONS : \n";
   
   int n=0;
   while ( true )
    { inp.get_token();
      if ( inp.finished() ) break;
      o << srspc << srspc << (++n) << ": " << inp.last_token() << srnl;
    }
 #endif

 }


/*
OpenGL pipeline :
    Co -> Mv -> Ce -> Mp -> Cc -> Md -> Cd -> Mw -> Cw

    o * ViewMat * ProjMat * Division * Viewport -> screen

    Co : Object coordinates                 Vo = ( x, y, z, w )
    Mv : Model view matrix
    Ce : Eye coordinates                    Ve = Vo * Mv
         Application-specific clipping can 
         be done with glClipPlane
    Mp : Projection matrix 
         see: glFrustum, glOrtho, gluLookAt
    Cc : Clip coordinates                   Vc = Ve * Mp
         Vc is clipped by the view volume
    Md : Perspective division
    Cd : Normalized device coordinates      Vd = Vc / w
         w becomes 1.0 and is discarded
         z is in the near clipping plane
    Mw : Viewport transformation
         glDepthRange will map z to (0,1)
         glViewport sets window x, y, w, h
    Cw : Window Coordinates                 Vw = ( (Vd.x+1)(w/2)+x, (Vd.y+1)(h/2)+y )
         z is discarded

*/

//======================================= EOF ========================================

