/*
 *  sr_camera.cpp - part of Motion Engine and SmartBody-lib
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
 
#include <vhcl.h>
# include <math.h>
# include <sr/sr_box.h>
# include <sr/sr_plane.h>
# include <sr/sr_camera.h>
#include <sstream>

#include <sbm/gwiz_math.h>
#include <sb/SBSubject.h>
#include <sb/SBAttribute.h>
#include <sb/SBScene.h>
#include <sb/SBSimulationManager.h>
#ifdef WIN32
	//#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <wingdi.h>
	#include <GL/gl.h>
#elif defined(BUILD_ANDROID)
	//#include <GLES2/gl2.h>
	//#include <GLES2/gl2ext.h>
#elif defined(SB_IPHONE)
    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>
#elif defined(__APPLE__) || defined(__APPLE_CC__)
       #include <OpenGL/gl.h>
//       #include <Carbon/Carbon.h>
       #define APIENTRY
#elif defined(__FLASHPLAYER__)
	#include <GL/gl.h>
#elif defined(__ANDROID__)
	//#include <GLES/gl.h>
	//#include <GLES2/gl2.h>
	#include <wes_gl.h>
#else
	#include <GL/gl.h>
	#include <GL/glx.h>
#endif



const int SrFrustum::OUTSIDE = 1;
const int SrFrustum::INTERSECT = 2;
const int SrFrustum::INSIDE = 3;

//=================================== SrCamera ===================================

SrCamera::SrCamera () : SBPawn()
 {
	setAttributeGroupPriority("Camera", 50);
	createDoubleAttribute("centerX", true, true, "Camera", 200, false, false, false, "");
	createDoubleAttribute("centerY", true, true, "Camera", 210, false, false, false, "");
	createDoubleAttribute("centerZ", true, true, "Camera", 220, false, false, false, "");
	createVec3Attribute("up", 0, 1, 0, true, "Camera", 230, false, false, false, "");
	createDoubleAttribute("fov", true, true, "Camera", 240, false, false, false, "");
	createDoubleAttribute("near", true, true, "Camera", 250, false, false, false, "");
	createDoubleAttribute("far", true, true, "Camera", 260, false, false, false, "");
	createDoubleAttribute("aspectRatio", true, true, "Camera", 270, false, false, false, "");
	createDoubleAttribute("scale", true, true, "Camera", 280, false, false, false, "");
	
   init ();
   setBoolAttribute("visible", false); // don't show the camera in the scene by default
 }

SrCamera::SrCamera ( const SrCamera* c )
         : SBPawn()
 {
   SrCamera();
   copyCamera(c);
 }

SrCamera::SrCamera ( const SrPnt& e, const SrPnt& c, const SrVec& u )
         :  SBPawn(), eye(e), center(c), up(u)
 {

   SrCamera();
   setEye(eye.x, eye.y, eye.z);
   setCenter(c.x, c.y, c.z);
   setUpVector(up);
   setFov(SR_TORAD(60));
   setNearPlane( 1.0f);
   setFarPlane( 10000.0f );
   setAspectRatio(1.0f);
   setScale(1.0f);
 }

SrCamera::~SrCamera ()
{
}

void SrCamera::copyCamera( const SrCamera* c )
{
	SrCamera* cam2 = const_cast<SrCamera*>(c);
   const SrVec tmpEye = cam2->getEye();
   setEye(tmpEye.x, tmpEye.y, tmpEye.z);
   SrVec tmpCenter = cam2->getCenter();
   setCenter(tmpCenter.x, tmpCenter.y, tmpCenter.z);
   setUpVector(cam2->getUpVector());
   setFov(cam2->fovy);
   setNearPlane(cam2->znear);
   setFarPlane(cam2->zfar);
   setAspectRatio(cam2->aspect);
   setScale(cam2->scale);
}

void SrCamera::setScale(float s)
{
	scale = s;

	SmartBody::DoubleAttribute* attr = dynamic_cast<SmartBody::DoubleAttribute*>(getAttribute("scale"));
	attr->setValueFast(scale);
}


void SrCamera::updateOrientation()
{
	SrMat viewMat;
	get_view_mat(viewMat);
	SrMat rotMat = viewMat.get_rotation().inverse();
	SrQuat quat = SrQuat(rotMat);
	gwiz::quat_t q = gwiz::quat_t(quat.w,quat.x,quat.y,quat.z);
	gwiz::euler_t e = gwiz::euler_t(q);	
	setHPR(SrVec((float)e.h(),(float)e.p(),(float)e.r()));
}

float SrCamera::getScale()
{
	return scale;
}

void SrCamera::setEye(float x, float y, float z)
{
	eye.x = x;
	eye.y = y;
	eye.z = z;

	float xold, yold, zold, h, p, r;
	get_world_offset(xold, yold, zold, h, p, r);
	set_world_offset(x, y, z, h, p, r);	
	
	SmartBody::DoubleAttribute* posX = dynamic_cast<SmartBody::DoubleAttribute*>(getAttribute("posX"));
	posX->setValueFast(x);
	SmartBody::DoubleAttribute* posY = dynamic_cast<SmartBody::DoubleAttribute*>(getAttribute("posY"));
	posY->setValueFast(y);
	SmartBody::DoubleAttribute* posZ = dynamic_cast<SmartBody::DoubleAttribute*>(getAttribute("posZ"));
	posZ->setValueFast(z);
	
	updateOrientation();	
}

SrVec SrCamera::getEye()
{
	return eye;
}

void SrCamera::setCenter(float x, float y, float z)
{
	center.x = x;
	center.y = y;
	center.z = z;

	SmartBody::DoubleAttribute* centerX = dynamic_cast<SmartBody::DoubleAttribute*>(getAttribute("centerX"));
	centerX->setValueFast(x);
	SmartBody::DoubleAttribute* centerY = dynamic_cast<SmartBody::DoubleAttribute*>(getAttribute("centerY"));
	centerY->setValueFast(y);
	SmartBody::DoubleAttribute* centerZ = dynamic_cast<SmartBody::DoubleAttribute*>(getAttribute("centerZ"));
	centerZ->setValueFast(z);

	updateOrientation();
}

SrVec SrCamera::getCenter()
{
	return center;
}

void SrCamera::setUpVector(SrVec u)
{
	up = u;
	updateOrientation();
}

SrVec SrCamera::getUpVector()
{
	return up;
}

SrVec SrCamera::getForwardVector()
{
   SrVec forward = (center - eye);
   forward.normalize();
   return forward;
}

SrVec SrCamera::getRightVector()
{
   return cross(getUpVector(), getForwardVector());
}

void SrCamera::setFov(float fov)
{
	fovy = fov;

	SmartBody::DoubleAttribute* attr = dynamic_cast<SmartBody::DoubleAttribute*>(getAttribute("fov"));
	attr->setValueFast(fovy);
}

float SrCamera::getFov()
{
	return fovy;
}

void SrCamera::setNearPlane(float n)
{
	znear = n;

	SmartBody::DoubleAttribute* attr = dynamic_cast<SmartBody::DoubleAttribute*>(getAttribute("near"));
	attr->setValueFast(znear);
}

float SrCamera::getNearPlane()
{
	return znear;
}

void SrCamera::setFarPlane(float f)
{
	zfar = f;

	SmartBody::DoubleAttribute* attr = dynamic_cast<SmartBody::DoubleAttribute*>(getAttribute("far"));
	attr->setValueFast(zfar);

}

float SrCamera::getFarPlane()
{
	return zfar;
}

void SrCamera::setAspectRatio(float a)
{
	aspect = a;

	SmartBody::DoubleAttribute* attr = dynamic_cast<SmartBody::DoubleAttribute*>(getAttribute("aspectRatio"));
	attr->setValueFast(aspect);
}

float SrCamera::getAspectRatio()
{
	return aspect;
}


void SrCamera::init () 
 { 
   setEye(0, 0, 2.0f ); 
   setCenter(SrVec::null.x, SrVec::null.y, SrVec::null.z);
   setUpVector(SrVec::j);
   setFov(SR_TORAD(60));
   setNearPlane(1.f); 
   setFarPlane(10000.0f); 
   setAspectRatio(1.0f);
   setScale(1.0f);
 }

SrMat& SrCamera::get_view_mat ( SrMat &m ) const
 {
   m.look_at ( eye, center, up );
   return m;
 }

SrMat& SrCamera::get_perspective_mat ( SrMat &m ) const
 {
   m.perspective ( fovy, aspect, znear, zfar );
   return m;
 }

// screenpt coords range in [-1,1]
void SrCamera::get_ray ( float winx, float winy, SrVec &p1, SrVec &p2 ) const
 {
   p1.set ( winx, winy, znear ); // p1 is in the near clip plane

   SrMat M(SrMat::NotInitialized), V(SrMat::NotInitialized), P(SrMat::NotInitialized);

   V.look_at ( eye, center, up );
   P.perspective ( fovy, aspect, znear, zfar );

   M.mult ( V, P ); // equiv to M = V * P

   M.invert();

   p1 = p1 * M; 
   p2 = p1-eye; // ray is in object coordinates, but before the scaling

   p2.normalize();
   p2 *= (zfar-znear);
   p2 += p1;

   float inv_scale = 1.0f/scale;
   p1*= inv_scale;
   p2*= inv_scale;

   //SR_TRACE1 ( "Ray: "<< p1 <<" : "<< p2 );
 }

/* - --------            \
   | |      |             \
   h | bbox |--------------.eye
   | |      |   dist      /
   - --------            /    tan(viewang/2)=(h/2)/dist
*/
void SrCamera::view_all ( const SrBox &box, float fovy_radians )
 {
   SrVec size = box.size();
   float h = SR_MAX(size.x,size.y);

   fovy = fovy_radians;
   up = SrVec::j;
   center = box.center();
   eye = center;
  
   float dist = (h/2)/tanf(fovy/2);
   eye.z = box.b.z + dist;

   // do not update z-far
   //float delta = box.max_size() + 0.0001f;
   //zfar = SR_ABS(eye.z)+delta;

   scale = 1.0f;
 }

void SrCamera::apply_translation_from_mouse_motion ( float lwinx, float lwiny, float winx, float winy )
 {
   SrVec p1, p2, x, inc;

   SrPlane plane ( center, eye-center );

   get_ray ( lwinx, lwiny, p1, x );
   p1 = plane.intersect ( p1, x );
   get_ray ( winx, winy, p2, x );
   p2 = plane.intersect ( p2, x );

   inc = p1-p2;

   inc *= scale;

   *this += inc;
 }

void SrCamera::operator*= ( const SrQuat& q )
 {
   eye -= center;
   eye = eye * q;
   eye += center;
   up -= center;
   up = up * q;
   up += center;
 }

void SrCamera::operator+= ( const SrVec& v )
 {
   eye += v;
   center += v;
 }

void SrCamera::operator-= ( const SrVec& v )
 {
   eye -= v;
   center -= v;
 }

//=============================== friends ==========================================

SrCamera operator* ( const SrCamera& c, const SrQuat& q )
 {
   SrCamera cam(c);
   cam *= q;
   return cam;
 }

SrCamera operator+ ( const SrCamera& c, const SrVec& v )
 {
   SrCamera cam(c);
   cam += v;
   return cam;
 }

SrOutput& operator<< ( SrOutput& out, const SrCamera& c )
 {
//   out << "eye:" << c.eye << " center:" << c.center << " up:" << c.up << srnl;

   out << "eye    " << c.eye << srnl <<
          "center " << c.center << srnl <<
          "up     " << c.up << srnl <<
          "fovy   " << c.fovy << srnl <<
          "znear  " << c.znear << srnl <<
          "zfar   " << c.zfar << srnl <<
          "aspect " << c.aspect << srnl <<
          "scale  " << c.scale << srnl;

   return out;
 }

SrInput& operator>> ( SrInput& inp, SrCamera& c )
 {
   while ( 1 )
    { if ( inp.get_token()==SrInput::EndOfFile ) break;
      if ( inp.last_token()=="eye" ) inp>>c.eye;
      else if ( inp.last_token()=="center" ) inp>>c.center;
      else if ( inp.last_token()=="up" ) inp>>c.up;
      else if ( inp.last_token()=="fovy" ) inp>>c.fovy;
      else if ( inp.last_token()=="znear" ) inp>>c.znear;
      else if ( inp.last_token()=="zfar" ) inp>>c.zfar;
      else if ( inp.last_token()=="aspect" ) inp>>c.aspect;
      else if ( inp.last_token()=="scale" ) inp>>c.scale;
      else { inp.unget_token(); break; }
    }
   return inp;
 }

void SrCamera::print()
{
	std::stringstream strstr;
	strstr << "   Camera Info " << std::endl;
	strstr << "-> eye position:	" << "(" << eye.x << ", " << eye.y << ", " << eye.z << ")" << std::endl;
	strstr << "-> center position:	" << "(" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
	strstr << "-> up vector:		" << "(" << up.x << ", " << up.y << ", " << up.z << ")" << std::endl;
	strstr << "-> fovy:		" << fovy << std::endl;
	strstr << "-> near plane:		" << znear << std::endl;
	strstr << "-> far plane:		" << zfar << std::endl;
	strstr << "-> aspect:		" << aspect << std::endl;
	strstr << "-> scale:		" << scale << std::endl;
	LOG(strstr.str().c_str());
}

void SrCamera::reset()
{
	init();
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	float scale = 1.f/SmartBody::SBScene::getScene()->getScale();
	setEye(0, 1.66f*scale, 1.85f*scale);
	setCenter(0, 0.92f*scale, 0);
	float znear = 0.01f*scale;
	float zfar = 100.0f*scale;
	setNearPlane(znear);
	setFarPlane(zfar);
}

void SrCamera::setPosition(SrVec position)
{
	this->setEye(position.x, position.y, position.z);
}

void SrCamera::notify(SmartBody::SBSubject* subject)
{
	SmartBody::SBPawn::notify(subject);

	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		if (attribute->getName() == "posX")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			SrVec vec = getEye();
			vec.x = (float) val;
			setEye(vec.x, vec.y, vec.z);
		}
		else if (attribute->getName() == "posY")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			SrVec vec = getEye();
			vec.y = (float) val;
			setEye(vec.x, vec.y, vec.z);
		}
		else if (attribute->getName() == "posZ")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			SrVec vec = getEye();
			vec.z = (float) val;
			setEye(vec.x, vec.y, vec.z);
		}
		else if (attribute->getName() == "centerX")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			SrVec vec = getCenter();
			vec.x = (float) val;
			setCenter(vec.x, vec.y, vec.z);
		}
		else if (attribute->getName() == "centerY")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			SrVec vec = getCenter();
			vec.y = (float) val;
			setCenter(vec.x, vec.y, vec.z);
		}
		else if (attribute->getName() == "centerZ")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			SrVec vec = getCenter();
			vec.z = (float) val;
			setCenter(vec.x, vec.y, vec.z);
		}
		if (attribute->getName() == "rotX")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			SrVec hpr = this->getHPR();
		
		}
		else if (attribute->getName() == "rotY")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			SrVec hpr = this->getHPR();
		}
		else if (attribute->getName() == "rotZ")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			SrVec hpr = this->getHPR();
		}
		else if (attribute->getName() == "fov")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			setFov((float) val);
		}
		else if (attribute->getName() == "near")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			setNearPlane((float) val);
		}
		else if (attribute->getName() == "far")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			setFarPlane((float) val);
		}
		else if (attribute->getName() == "aspectRatio")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			setAspectRatio((float) val);
		}
		else if (attribute->getName() == "scale")
		{
			double val = this->getDoubleAttribute(attribute->getName());
			setScale((float) val);
		}
	}
}

SBAPI  void SrCamera::afterUpdate( double time )
{	
	if (smoothTargetCam)
	{
		SrCamera* cam = SmartBody::SBScene::getScene()->getCamera(targetCam);
		if (!cam) 
		{
			//LOG("Target camera '%s' does not exists.",camName.c_str());
			return;
		}
		if (time >= camStartTime && time <= camEndTime && camStartTime != camEndTime)
		{
			float weight = (float)(time-camStartTime)/(float)(camEndTime - camStartTime);
			SrVec newEye = initialEye*(1.f-weight) + cam->getEye()*(weight);
			SrVec newCenter = initialCenter*(1.f-weight) + cam->getCenter()*(weight);
			SrVec newUp = initialUp*(1.f-weight) + cam->getUpVector()*(weight);
			float newFov = initialFovy*(1.f-weight) + cam->getFov()*(weight);

			setEye(newEye.x,newEye.y,newEye.z);
			setCenter(newCenter.x,newCenter.y,newCenter.z);
			setUpVector(newUp);
			setFov(newFov);
		}
		else
		{
			smoothTargetCam = false;
		}
	}
	SBPawn::afterUpdate(time);
}

SBAPI void SrCamera::setCameraParameterSmooth( std::string camName, float smoothTime )
{
	SrCamera* cam = SmartBody::SBScene::getScene()->getCamera(camName);
	if (!cam) 
	{
		LOG("Target camera '%s' does not exists.",camName.c_str());
		return;
	}

	camStartTime = (float) SmartBody::SBScene::getScene()->getSimulationManager()->getTime();
	camEndTime = camStartTime + smoothTime;

	initialEye = eye;
	initialCenter = center;
	initialUp = up;
	initialFovy = fovy;
	targetCam = camName;
	smoothTargetCam = true;
}



//================================ SrFrustrum class =========================================

#define ANG2RAD 3.14159265358979323846/180.0

SrFrustum::SrFrustum() {}

SrFrustum::~SrFrustum() {}

bool SrFrustum::pointInFrustum(SrVec &point) {
   int p;
 
   for( p = 0; p < 6; p++ )
      if( frustum[p][0] * point.x + frustum[p][1] * point.y + frustum[p][2] * point.z + frustum[p][3] <= 0 )
         return false;
   return true;
}

void SrFrustum::extractFrustum()
{
#if !defined(__ANDROID__)
   float   proj[16];
   float   modl[16];
   float   clip[16];
   float   t;
 

   /* Get the current PROJECTION matrix from OpenGL */
   glGetFloatv( GL_PROJECTION_MATRIX, proj );
 
   /* Get the current MODELVIEW matrix from OpenGL */
   glGetFloatv( GL_MODELVIEW_MATRIX, modl );
 
   /* Combine the two matrices (multiply projection by modelview) */
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];
 
   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];
 
   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];
 
   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];
 
   /* Extract the numbers for the RIGHT plane */
   frustum[0][0] = clip[ 3] - clip[ 0];
   frustum[0][1] = clip[ 7] - clip[ 4];
   frustum[0][2] = clip[11] - clip[ 8];
   frustum[0][3] = clip[15] - clip[12];
 
   /* Normalize the result */
   t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
   frustum[0][0] /= t;
   frustum[0][1] /= t;
   frustum[0][2] /= t;
   frustum[0][3] /= t;
 
   /* Extract the numbers for the LEFT plane */
   frustum[1][0] = clip[ 3] + clip[ 0];
   frustum[1][1] = clip[ 7] + clip[ 4];
   frustum[1][2] = clip[11] + clip[ 8];
   frustum[1][3] = clip[15] + clip[12];
 
   /* Normalize the result */
   t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
   frustum[1][0] /= t;
   frustum[1][1] /= t;
   frustum[1][2] /= t;
   frustum[1][3] /= t;
 
   /* Extract the BOTTOM plane */
   frustum[2][0] = clip[ 3] + clip[ 1];
   frustum[2][1] = clip[ 7] + clip[ 5];
   frustum[2][2] = clip[11] + clip[ 9];
   frustum[2][3] = clip[15] + clip[13];
 
   /* Normalize the result */
   t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
   frustum[2][0] /= t;
   frustum[2][1] /= t;
   frustum[2][2] /= t;
   frustum[2][3] /= t;
 
   /* Extract the TOP plane */
   frustum[3][0] = clip[ 3] - clip[ 1];
   frustum[3][1] = clip[ 7] - clip[ 5];
   frustum[3][2] = clip[11] - clip[ 9];
   frustum[3][3] = clip[15] - clip[13];
 
   /* Normalize the result */
   t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
   frustum[3][0] /= t;
   frustum[3][1] /= t;
   frustum[3][2] /= t;
   frustum[3][3] /= t;
 
   /* Extract the FAR plane */
   frustum[4][0] = clip[ 3] - clip[ 2];
   frustum[4][1] = clip[ 7] - clip[ 6];
   frustum[4][2] = clip[11] - clip[10];
   frustum[4][3] = clip[15] - clip[14];
 
   /* Normalize the result */
   t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
   frustum[4][0] /= t;
   frustum[4][1] /= t;
   frustum[4][2] /= t;
   frustum[4][3] /= t;
 
   /* Extract the NEAR plane */
   frustum[5][0] = clip[ 3] + clip[ 2];
   frustum[5][1] = clip[ 7] + clip[ 6];
   frustum[5][2] = clip[11] + clip[10];
   frustum[5][3] = clip[15] + clip[14];
 
   /* Normalize the result */
   t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
   frustum[5][0] /= t;
   frustum[5][1] /= t;
   frustum[5][2] /= t;
   frustum[5][3] /= t;
#endif
}

//================================ End of File =========================================

