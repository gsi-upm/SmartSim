#include "PositionControl.h"

#include "vhcl.h"
#include <FL/gl.h>
#include <FL/glu.h>
#include <sr/sr_gl.h>
#include <sr/sr_plane.h>
#include <sr/sr_sphere.h>
#include <sr/sr_sn.h>
#include <sr/sr_sn_group.h>
# include <sr/sr_sa_gl_render.h>
# include <sr/sr_gl_render_funcs.h>


ObjectControl::ObjectControl()
{
	dragging = false;
	active = false;

	base=60;
	r=6;
	len=r*4;
	s_len=10;
	ss_len=3;	
	worldPt = SrVec(0,170,100);
}


ObjectControl::~ObjectControl()
{

}

SrQuat ObjectControl::getWorldRot()
{
	return worldRot;
}

SrVec ObjectControl::getWorldPt()
{
	return worldPt;
}

void ObjectControl::setWorldPt(SrVec& newPt)
{
	worldPt = newPt;	
}

void ObjectControl::setWorldRot( SrQuat& newRot )
{
	worldRot = newRot;
}

void ObjectControl::setColor(const SrVec &color)
{
	glDisable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glColor3fv(color);
	//glEnable(GL_LIGHTING);
}

void ObjectControl::screenParallelPlane(
	SrCamera& cam,const SrVec &center,SrVec &dirx,SrVec &diry)
{
	// 	FXVec3f e_center=viewer->worldToEye(FXVec3f(center[0],center[1],center[2]));
	// 	FXint s[2];
	// 	viewer->eyeToScreen(s[0],s[1],e_center);

	SrVec win = worldToScreen(cam,center);
	SrVec winX = screenToWorld(cam,win+SrVec(1,0,0));
	SrVec winY = screenToWorld(cam,win+SrVec(0,-1,0));

	dirx = winX - center;
	diry = winY - center;
	// 
	// 	FXfloat zz=viewer->worldToEyeZ(FXVec3f(center[0],center[1],center[2]));
	// 	//printf("zz = %f, s[0] = %f, s[1] = %f\n",zz,s[0],s[1]);
	// 	FXVec3f c0=viewer->eyeToWorld(viewer->screenToEye(s[0],s[1],zz));
	// 	FXVec3f cx=viewer->eyeToWorld(viewer->screenToEye(s[0]+1,s[1],zz));
	// 	FXVec3f cy=viewer->eyeToWorld(viewer->screenToEye(s[0],s[1]-1,zz));


	// 	for (int i=0;i<3;i++){
	// 		dirx[i]=cx[i]-c0[i];
	// 		diry[i]=cy[i]-c0[i];
	// 	}
}



SrVec ObjectControl::worldToEye( const SrCamera& cam, const SrVec& pos )
{
	SrMat mat;
	cam.get_view_mat(mat);
	return pos*mat;	
}

SrVec ObjectControl::worldToScreen( const SrCamera& cam, const SrVec& pos )
{
	GLdouble modelView[16], projection[16];
	int viewport[4];
	GLdouble winX,winY,winZ;
	SrMat mat;
	cam.get_view_mat(mat); mat.get_double(modelView);
	cam.get_perspective_mat(mat); mat.get_double(projection);
	glGetIntegerv(GL_VIEWPORT,viewport);

	gluProject(pos.x,pos.y,pos.z,modelView,projection,viewport,&winX,&winY,&winZ);

	return SrVec((float)winX,(float)winY,(float)winZ);	
}

SrVec ObjectControl::screenToWorld( const SrCamera& cam, const SrVec& win )
{
	GLdouble modelView[16], projection[16];
	int viewport[4];
	GLdouble posX,posY,posZ;
	SrMat mat;
	cam.get_view_mat(mat); mat.get_double(modelView);
	cam.get_perspective_mat(mat); mat.get_double(projection);
	glGetIntegerv(GL_VIEWPORT,viewport);

	gluUnProject(win.x,win.y,win.z,modelView,projection,viewport,&posX,&posY,&posZ);
	return SrVec((float)posX,(float)posY,(float)posZ);
}

SrVec ObjectControl::mouseToWorld( SrCamera& cam, float fx, float fy, float tx, float ty )
{
	SrVec p1, p2, x, inc;	
	SrVec center = cam.getCenter();//getWorldPt();	
	SrVec eye = cam.getEye();
	SrVec eyeDir = eye-cam.getCenter();
	eyeDir.normalize();
	SrPlane plane ( center,  eyeDir);
	cam.get_ray ( fx, fy, p1, x );
	p1 = plane.intersect ( p1, x );
	cam.get_ray ( tx, ty, p2, x );
	p2 = plane.intersect ( p2, x );

	return p2-p1;
}



/************************************************************************/
/* Postion Control Widget                                               */
/************************************************************************/
void PositionControl::drawCenter()
{	
	SrVec center= getWorldPt();
	SrVec color(1, 0, 0);
	drawSphere(center, false, color);
}


PositionControl::PositionControl(void) : ObjectControl()
{	
	opdir = 3;	
	colors[3]=SrVec(100.0f/255.0f, 220.0f/255.0f, 1.f);
	colors[0]=SrVec(1,0,0);
	colors[1]=SrVec(0,154.0f/255.0f,82.0f/255.0f);
	colors[2]=SrVec(0,0,1);

	base=60;
	r=8;
	len=r*4;
	s_len=15;
	ss_len=3;	
}

PositionControl::~PositionControl(void)
{
}

void PositionControl::drawSphere(SrVec& pos, float fRadius, SrVec& color)
{
	//glColor3f(1.0, 0.0, 0.0);
	glEnable(GL_LIGHTING);
	glColor3f(color[0],color[1],color[2]);
	SrSnSphere sphere;
	sphere.shape().center = pos;//SrPnt(0, 0, 0);
	sphere.shape().radius = fRadius;
	sphere.render_mode(srRenderModeSmooth);
	SrGlRenderFuncs::render_sphere(&sphere);
	glDisable(GL_LIGHTING);
}

void PositionControl::drawBox( SrBox& box, bool wireFrame /*= false*/, SrVec& color /*= SrVec(0.f,1.f,1.f)*/ )
{
	glDisable(GL_LIGHTING);	
	SrSnBox sbox;					
	sbox.shape().a = box.a;
	sbox.shape().b = box.b;	
	if (wireFrame)
		sbox.render_mode(srRenderModeLines);
	else
		sbox.render_mode(srRenderModeSmooth);
	sbox.color(SrColor(color[0],color[1],color[2]));
	SrGlRenderFuncs::render_box(&sbox);
}

void PositionControl::identify( std::vector<int>& path )
{
	opdir=path[1];	
	//printf("opdir = %d\n",opdir);
	prevPt = getWorldPt();
	resetColor();
}

void PositionControl::hitOPS(SrCamera& cam)
{
	glDisable(GL_LIGHTING);
	glPushName(0xffffffff);

	float hitScale = 1.0;	
	float lineScale = 1.0;
	float centerScale = 1.0;
	glLineWidth(lineScale);
	glLoadName(3);
	//draw center square
	SrVec center= getWorldPt();

	SrVec dirx,diry;
	screenParallelPlane(cam,center,dirx,diry);
	float ratio= dirx.norm();

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glBegin(GL_QUADS);
	glVertex3fv(center-s_len*hitScale*dirx-s_len*hitScale*diry);
	glVertex3fv(center+s_len*hitScale*dirx-s_len*hitScale*diry);
	glVertex3fv(center+s_len*hitScale*dirx+s_len*hitScale*diry);
	glVertex3fv(center-s_len*hitScale*dirx+s_len*hitScale*diry);
	glEnd();

	//drawShadowSquare(center[0],center[1],center[2],dirx,diry,s_len,GL_QUADS);
	SrVec color(1.0, 0, 0);
	drawSphere(center,ratio*s_len*hitScale, color);

	glPushMatrix();
	glTranslatef(center[0],center[1],center[2]);

	glLoadName(0);
	glPushMatrix();
	glBegin(GL_LINES);
	glColor3fv(colors[0]);
	glVertex3f(0,0,0);
	glVertex3f(base*ratio,0,0);
	glEnd();

	glTranslatef(base*ratio,0,0);
	glRotatef(90,0,1,0);
	GLUquadricObj *arx=gluNewQuadric();
	gluCylinder(arx,r*ratio*hitScale,0,len*ratio,10,10);
	gluDeleteQuadric(arx);	
	glPopMatrix();

	glLoadName(1);
	glPushMatrix();
	glBegin(GL_LINES);
	glColor3fv(colors[0]);
	glVertex3f(0,0,0);
	glVertex3f(0,base*ratio,0);
	glEnd();

	glTranslatef(0,base*ratio,0);
	glRotatef(-90,1,0,0);
	GLUquadricObj *ary=gluNewQuadric();
	gluCylinder(ary,r*ratio*hitScale,0,len*ratio,10,10);
	gluDeleteQuadric(ary);
	glPopMatrix();

	glLoadName(2);
	glPushMatrix();
	glBegin(GL_LINES);
	glColor3fv(colors[0]);
	glVertex3f(0,0,0);
	glVertex3f(0,0,base*ratio);
	glEnd();
	glTranslatef(0,0,base*ratio);
	GLUquadricObj *arz=gluNewQuadric();
	gluCylinder(arz,r*ratio*hitScale,0,len*ratio,10,10);
	gluDeleteQuadric(arz);
	glPopMatrix();
	glPopMatrix();
	glPopName();
	glLineWidth(1.0);
}

void PositionControl::draw(SrCamera& cam)
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	//if (active)
	{
		SrVec center = getWorldPt();
		SrVec dirx,diry;
		screenParallelPlane(cam,center,dirx,diry);
		float ratio=(dirx).norm();
		//draw center square
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_BLEND);
		glColor3fv(colors[3]);
		drawShadowSquare(center[0],center[1],center[2],dirx,diry,s_len,GL_LINE_LOOP);

		if (!dragging){
			glPushMatrix();
			glTranslatef(center[0],center[1],center[2]);

			//draw axis
			glDisable(GL_LIGHTING);			
			glBegin(GL_LINES);
			glColor3fv(colors[0]);
			glVertex3f(0,0,0);
			glVertex3f(base*ratio,0,0);
			glColor3fv(colors[1]);
			glVertex3f(0,0,0);
			glVertex3f(0,base*ratio,0);
			glColor3fv(colors[2]);
			glVertex3f(0,0,0);
			glVertex3f(0,0,base*ratio);
			glEnd();


			//draw arrow
			//glEnable(GL_LIGHTING);
			setColor(colors[0]);
			//glColor3fv(colors[0]);
			glPushMatrix();
			glTranslatef(base*ratio,0,0);
			glRotatef(90,0,1,0);
			GLUquadricObj *arx=gluNewQuadric();
			gluCylinder(arx,r*ratio,0,len*ratio,10,10);
			gluDeleteQuadric(arx);
			glPopMatrix();

			setColor(colors[1]);
			//glColor3fv(colors[1]);
			glPushMatrix();
			glTranslatef(0,base*ratio,0);
			glRotatef(-90,1,0,0);
			GLUquadricObj *ary=gluNewQuadric();
			gluCylinder(ary,r*ratio,0,len*ratio,10,10);
			gluDeleteQuadric(ary);
			glPopMatrix();

			setColor(colors[2]);
			//glColor3fv(colors[2]);
			glPushMatrix();
			glTranslatef(0,0,base*ratio);
			GLUquadricObj *arz=gluNewQuadric();
			gluCylinder(arz,r*ratio,0,len*ratio,10,10);
			gluDeleteQuadric(arz);
			glPopMatrix();

			glPopMatrix();
		
		}else{
			//draw the tail shadow square
			glDisable(GL_LIGHTING);
			glColor3f(.3f,.3f,.3f);
			//drawShadowSquare(p_center0[0],p_center0[1],p_center0[2],dirx,diry,ss_len,GL_QUADS);

			//draw the moving axis
			glPushMatrix();
			glTranslatef(center[0],center[1],center[2]);
			switch(opdir)
			{
			case 0:
				{
					//line
					glDisable(GL_LIGHTING);
					glBegin(GL_LINES);
					glColor3fv(colors[0]);
					glVertex3f(0,0,0);
					glVertex3f(base*ratio,0,0);
					glEnd();
					//head
					//glEnable(GL_LIGHTING);
					setColor(colors[0]);
					glPushMatrix();
					glTranslated(base*ratio,0,0);
					glRotatef(90,0,1,0);
					GLUquadricObj *arx=gluNewQuadric();
					gluCylinder(arx,r*ratio,0,len*ratio,10,10);
					gluDeleteQuadric(arx);
					glPopMatrix();
					break;
				}
			case 1:
				{
					//lines
					glDisable(GL_LIGHTING);
					glBegin(GL_LINES);
					glColor3fv(colors[1]);
					glVertex3f(0,0,0);
					glVertex3f(0,base*ratio,0);
					glEnd();
					//head
					//glEnable(GL_LIGHTING);
					setColor(colors[1]);
					glPushMatrix();
					glTranslatef(0,base*ratio,0);
					glRotated(-90,1,0,0);
					GLUquadricObj *ary=gluNewQuadric();
					gluCylinder(ary,r*ratio,0,len*ratio,10,10);
					gluDeleteQuadric(ary);
					glPopMatrix();
					break;
				}
			case 2:
				{
					//line
					glDisable(GL_LIGHTING);
					glBegin(GL_LINES);
					glColor3fv(colors[2]);
					glVertex3f(0,0,0);
					glVertex3f(0,0,base*ratio);
					glEnd();
					//head
					//glEnable(GL_LIGHTING);
					setColor(colors[2]);
					glPushMatrix();
					glTranslated(0,0,base*ratio);
					GLUquadricObj *arz=gluNewQuadric();
					gluCylinder(arz,r*ratio,0,len*ratio,10,10);
					gluDeleteQuadric(arz);
					glPopMatrix();
					break;
				}
			case 3:
				{
					//line
					glDisable(GL_LIGHTING);
					glBegin(GL_LINES);
					glColor3fv(colors[0]);
					glVertex3f(0,0,0);
					glVertex3f(base*ratio,0,0);
					glColor3fv(colors[1]);
					glVertex3f(0,0,0);
					glVertex3f(0,base*ratio,0);
					glColor3fv(colors[2]);
					glVertex3f(0,0,0);
					glVertex3f(0,0,base*ratio);
					glEnd();
					break;
				}
			}
			glPopMatrix();
			//draw shadow
			/*
			glPushMatrix();
			glTranslatef(p_center0[0],p_center0[1],p_center0[2]);
			glDisable(GL_LIGHTING);
			glColor3f(0.3,0.3,0.3);
			switch(opdir)
			{
			case 3:
				glBegin(GL_LINES);
				glVertex3f(0,0,0);
				glVertex3f(base*ratio0,0,0);
				glVertex3f(0,0,0);
				glVertex3f(0,base*ratio0,0);
				glVertex3f(0,0,0);
				glVertex3f(0,0,base*ratio0);
				glEnd();
				drawShadowSquare(base*ratio0,0,0,dirx,diry,ss_len,GL_QUADS);
				drawShadowSquare(0,base*ratio0,0,dirx,diry,ss_len,GL_QUADS);
				drawShadowSquare(0,0,base*ratio0,dirx,diry,ss_len,GL_QUADS);

				break;
			case 0:
				glBegin(GL_LINES);
				glVertex3f(0,0,0);
				glVertex3f(base*ratio0,0,0);
				glEnd();
				drawShadowSquare(base*ratio0,0,0,dirx,diry,ss_len,GL_QUADS);
				break;
			case 1:
				glBegin(GL_LINES);
				glVertex3f(0,0,0);
				glVertex3f(0,base*ratio0,0);
				glEnd();
				drawShadowSquare(0,base*ratio0,0,dirx,diry,ss_len,GL_QUADS);
				break;
			case 2:
				glBegin(GL_LINES);
				glVertex3f(0,0,0);
				glVertex3f(0,0,base*ratio0);
				glEnd();
				drawShadowSquare(0,0,base*ratio0,dirx,diry,ss_len,GL_QUADS);
				break;
			}
			glPopMatrix();
			*/
			
		}
		
	}
}

bool PositionControl::drag(SrCamera& cam,  float fx, float fy, float tx, float ty )
{
    SrVec center = getWorldPt();
    SrVec inc = SrVec(0,0,0);// = mouseToWorld(cam,fx,fy,tx,ty);
	float ratio; 

	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT,viewport);

	if (opdir == 3)
	{
		inc = mouseToWorld(cam,fx,fy,tx,ty);
		ratio = -worldToEye(cam,center).z/(cam.getCenter()-cam.getEye()).norm();
		//LOG("opdir = %d, ratio = %f",opdir, ratio);
		inc *= ratio;
	}
	else
	{
		SrVec center=getWorldPt();
		SrVec dirx,diry;
		screenParallelPlane(cam,center,dirx,diry);
		float ratio=dirx.norm();

		//LOG("opdir = %d, ratio = %f",opdir, ratio);

		SrVec t(center[0],center[1],center[2]);
		t[opdir]+=ratio*base;

		//SrVec c0=viewer->worldToEye(t);
		//SrVec s0=eyeToScreen(viewer,c0);
		SrVec s0 = worldToScreen(cam,t);

		SrVec t1=t;
		t1[opdir]+=1;
		SrVec s1=worldToScreen(cam,t1);

		SrVec t_1=t;
		t_1[opdir]-=1;
		SrVec s_1=worldToScreen(cam,t_1);

		float p=(s1-s0).norm();
		float n=-(s_1-s0).norm();
		// convert normalize coordinate to screen coordinate
		SrVec mouseMove = SrVec((tx-fx)*viewport[2],(ty-fy)*viewport[3],0.0)*0.5f;
		float w= dot(mouseMove,(s1-s0))/p;

		//alpha*h/(h+a)
		/*		float alpha=2.*p*n/(n+p);
		float a=(n-p)/(n+p);
		d3[opdir]=a*w/(alpha-w);
		*/
		//beta*h/(1+b*h)
		float beta=2.0f*p*n/(n-p);
		float b=(n+p)/(n-p);
		inc[opdir]=w/(beta-b*w);	
		//printf("opdir = %d  ", opdir);
		//sr_out << "inc = " << inc << "  ";
	}
	//sr_out << "inc = " << inc << "  ";
	//sr_out << "new center = " << center << srnl;
	center += inc;
	setWorldPt(center);
	return true;
}

void PositionControl::resetColor()
{
	colors[3]=SrVec(100.0f/255.0f, 220.0f/255.0f, 1.0f);
	colors[0]=SrVec(1,0,0);
	colors[1]=SrVec(0,154.0f/255.0f,82.0f/255.0f);
	colors[2]=SrVec(0,0,1);

	if (0<=opdir && opdir<=3)
		colors[opdir]=SrVec(1,1,0);
}

void PositionControl::drawShadowSquare( float x,float y,float z,SrVec& dirx,SrVec& diry,float sz,unsigned int mode )
{
	glPushMatrix();
	glTranslatef(x,y,z);
	glBegin(mode);
	glVertex3fv(-sz*dirx-sz*diry);
	glVertex3fv( sz*dirx-sz*diry);
	glVertex3fv( sz*dirx+sz*diry);
	glVertex3fv(-sz*dirx+sz*diry);
	glEnd();
	glPopMatrix();
}


SelectionControl::SelectionControl()
{

}

SelectionControl::~SelectionControl()
{

}

bool SelectionControl::drag( SrCamera& cam, float fx, float fy, float tx, float ty )
{
	return true;
}

void SelectionControl::draw( SrCamera& cam )
{

}
