#include "RotationControl.h"

#include "vhcl.h"
#include <FL/gl.h>
#include <sr/sr_plane.h>
#include <sr/sr_sphere.h>
#include <sr/sr_sn.h>
#include <sr/sr_sn_group.h>
#include <sr/sr_sa_gl_render.h>
#include <sr/sr_gl_render_funcs.h>

#include "sbm/gwiz_math.h"

/************************************************************************/
/* Rotation Control Widget                                              */
/************************************************************************/

using namespace gwiz;

RotationControl::RotationControl(void) : ObjectControl()
{		
	dragging=false;
	opdir=3;
	resetColor();

	radius[0]=100;		//inside circle
	radius[1]=85;		//outside circle
	seg=300;
	circle.resize(seg);
	for (int i=0;i<seg;i++)
		circle[i]=SrVec2(cos(2.0f*(float)M_PI/seg*i),sin(2.0f*(float)M_PI/seg*i));
}

RotationControl::~RotationControl(void)
{
}


void RotationControl::identify( std::vector<int>& path )
{
	opdir=path[1];	
	//printf("opdir = %d\n",opdir);
	prevPt = getWorldPt();
	resetColor();
}

void RotationControl::hitOPS(SrCamera& cam)
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glDisable(GL_LIGHTING);
	glPushName(0xffffffff);

	SrVec center=getWorldPt();
	SrVec dirx,diry;
	screenParallelPlane(cam,center,dirx,diry);
	float ratio=(dirx).norm();

	glPushMatrix();
	//	glMultMatrixd(::transpose(pm->t_matrix));
	glTranslatef(center[0],center[1],center[2]);

// 	glLoadName(3);
// 	glBegin(GL_LINE_LOOP);
// 	for (int i=0;i<seg;i++)
// 		glVertex3fv(radius[0]*(dirx*circle[i][0]+diry*circle[i][1]));
// 	glEnd();
// 
// 	glLoadName(4);
// 	glBegin(GL_LINE_LOOP);
// 	for (int i=0;i<seg;i++)
// 		glVertex3fv(radius[1]*(dirx*circle[i][0]+diry*circle[i][1]));
// 	glEnd();
	
	double lineWidth = 20.0;
	
	SrVec nm=cam.getEye() - cam.getCenter();//cross(diry,dirx);
	nm.normalize();
	std::vector<SrVec> circle_copy;
	circle_copy.resize(seg);
	SrVec zero(0,0,0);
	//	Mat3f m=(dragging)?unit_quat_to_matrix(debug_quat)*unit_quat_to_matrix(hdl->quat):
	//		unit_quat_to_matrix(hdl->quat);
	SrMat m;
	getWorldRot().get_mat(m);
	//	Mat3f m=unit_quat_to_matrix(debug_quat)*unit_quat_to_matrix(hdl->quat);
	glLoadName(0);
	for (int i=0;i<seg;i++)
		//		circle_copy[i]=pm->r_matrix*(radius[1]*ratio*(Vec3f(0,circle[i][0],circle[i][1])));
		circle_copy[i]=(radius[1]*ratio*(SrVec(0,circle[i][0],circle[i][1])))*m;
	glColor3fv(colors[0]);	
	
	glLineWidth((GLfloat)lineWidth); // use very thick line to ease the rotation selection
	drawVisibleCircle(circle_copy, zero,nm);

	glLoadName(1);
	for (int i=0;i<seg;i++)
		//		circle_copy[i]=pm->r_matrix*(radius[1]*ratio*(Vec3f(circle[i][1],0,circle[i][0])));
		circle_copy[i]=(radius[1]*ratio*(SrVec(circle[i][1],0,circle[i][0])))*m;
	glColor3fv(colors[1]);
	glLineWidth((GLfloat)lineWidth); 
	drawVisibleCircle(circle_copy, zero, nm);

	glLoadName(2);
	for (int i=0;i<seg;i++)
		//		circle_copy[i]=pm->r_matrix*(radius[1]*ratio*(Vec3f(circle[i][0],circle[i][1],0)));
		circle_copy[i]=(radius[1]*ratio*(SrVec(circle[i][0],circle[i][1],0)))*m;
	glColor3fv(colors[2]);
	glLineWidth((GLfloat)lineWidth); 
	drawVisibleCircle(circle_copy, zero, nm);
	
#if 0
	SrQuat rot = getWorldRot();
	SrMat mat;
	GLdouble m[16];
	rot.get_mat(mat); mat.get_double(m);
	glMultMatrixd(m);
	glLoadName(0);
	glBegin(GL_LINE_LOOP);
	for (int i=0;i<seg;i++)
		glVertex3fv(radius[1]*ratio*(SrVec(0,circle[i][0],circle[i][1])));
	glEnd();

	glLoadName(1);
	glBegin(GL_LINE_LOOP);
	for (int i=0;i<seg;i++)
		glVertex3fv(radius[1]*ratio*(SrVec(circle[i][1],0,circle[i][0])));
	glEnd();

	glLoadName(2);
	glBegin(GL_LINE_LOOP);
	for (int i=0;i<seg;i++)
		glVertex3fv(radius[1]*ratio*(SrVec(circle[i][0],circle[i][1],0)));
	glEnd();
#endif

	glPopMatrix();
	glPopName();

	glLineWidth(1.0);
	
}

void RotationControl::draw(SrCamera& cam)
{
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	//double lineWidth[5] = { 1.0, 1.0, 1.0, 1.0, 1.0} ;
	double lineWidth[5] = { 2.0, 2.0, 2.0, 2.0, 2.0} ;

	lineWidth[opdir] = 4.0;	
	SrVec zero(0,0,0);
	
	//if (active)
	{
		SrVec center=getWorldPt();
		SrVec dirx,diry;
		screenParallelPlane(cam,center,dirx,diry);
		float ratio=(dirx).norm();
		SrVec nm=cam.getEye() - cam.getCenter();//cross(diry,dirx);
		nm.normalize();

		glDisable(GL_LIGHTING);
		glPushMatrix();
		//	glMultMatrixd(::transpose(pm->t_matrix));
		glTranslatef(center[0],center[1],center[2]);

		std::vector<SrVec> circle_copy;
		circle_copy.resize(seg);

		//	Mat3f m=(dragging)?unit_quat_to_matrix(debug_quat)*unit_quat_to_matrix(hdl->quat):
		//		unit_quat_to_matrix(hdl->quat);
		SrMat m;
		getWorldRot().get_mat(m);
		//	Mat3f m=unit_quat_to_matrix(debug_quat)*unit_quat_to_matrix(hdl->quat);
		for (int i=0;i<seg;i++)
			//		circle_copy[i]=pm->r_matrix*(radius[1]*ratio*(Vec3f(0,circle[i][0],circle[i][1])));
			circle_copy[i]=(radius[1]*ratio*(SrVec(0,circle[i][0],circle[i][1])))*m;
		glColor3fv(colors[0]);	
		glLineWidth((GLfloat)lineWidth[0]);
		drawVisibleCircle(circle_copy, zero,nm);

		for (int i=0;i<seg;i++)
			//		circle_copy[i]=pm->r_matrix*(radius[1]*ratio*(Vec3f(circle[i][1],0,circle[i][0])));
			circle_copy[i]=(radius[1]*ratio*(SrVec(circle[i][1],0,circle[i][0])))*m;
		glColor3fv(colors[1]);
		glLineWidth((GLfloat)lineWidth[1]);
		drawVisibleCircle(circle_copy, zero, nm);

		for (int i=0;i<seg;i++)
			//		circle_copy[i]=pm->r_matrix*(radius[1]*ratio*(Vec3f(circle[i][0],circle[i][1],0)));
			circle_copy[i]=(radius[1]*ratio*(SrVec(circle[i][0],circle[i][1],0)))*m;
		glColor3fv(colors[2]);
		glLineWidth((GLfloat)lineWidth[2]);
		drawVisibleCircle(circle_copy, zero, nm);

		glLineWidth(1.0); // reset to default			
		glColor3fv(colors[3]);
		glBegin(GL_LINE_LOOP);
		for (int i=0;i<seg;i++)
			glVertex3fv(radius[0]*(dirx*circle[i][0]+diry*circle[i][1]));
		glEnd();

		glColor3fv(colors[4]);
		glBegin(GL_LINE_LOOP);
		for (int i=0;i<seg;i++)
			glVertex3fv(radius[1]*(dirx*circle[i][0]+diry*circle[i][1]));
		glEnd();

		glPopMatrix();
		glEnable(GL_LIGHTING);
	}
	glPopAttrib();
	
}

bool RotationControl::drag(SrCamera& cam,  float fx, float fy, float tx, float ty )
{
	dragging=true;

	SrVec center=getWorldPt();
	SrVec dirx,diry;
	screenParallelPlane(cam,center,dirx,diry);
	float ratio=(dirx).norm();
	SrVec nm=cam.getEye() - cam.getCenter();//cross(diry,dirx);
	nm.normalize();

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	
	fx = ((fx+1.0f)*viewport[2]*0.5f);
	fy = ((fy+1.0f)*viewport[3]*0.5f);

	tx = ((tx+1.0f)*viewport[2]*0.5f);
	ty = ((ty+1.0f)*viewport[3]*0.5f);

	if (opdir<3){
		float min_f_d=1e10,min_t_d=1e10;
		int min_f_id,min_t_id;
		//int sx,sy;
		SrVec cr;
		for (int i=0;i<seg;i++){
			if (opdir==0)
				cr=SrVec(0,circle[i][0],circle[i][1]);
			else if (opdir==1)
				cr=SrVec(circle[i][1],0,circle[i][0]);
			else
				cr=SrVec(circle[i][0],circle[i][1],0);
			//			Vec3 x=pm->t_matrix*pm->r_matrix*(radius[1]*ratio*cr);
			SrVec x= center + (radius[1]*ratio*cr)*getWorldRot();//unit_quat_to_matrix(hdl->quat)*(radius[1]*ratio*cr);
			if (dot((x-center),nm)>0){
				SrVec sx= worldToScreen(cam,x);//viewer->worldToEye(FXVec3f(x[0],x[1],x[2]));
				//viewer->eyeToScreen(sx,sy,e);
				if (min_f_d>SrVec(sx.x-fx,sx.y-fy,0).norm2()){
					min_f_id=i;
					min_f_d=SrVec(sx.x-fx,sx.y-fy,0).norm2();
				}
				if (min_t_d>SrVec(sx.x-tx,sx.y-ty,0).norm2()){
					min_t_id=i;
					min_t_d=SrVec(sx.x-tx,sx.y-ty,0).norm2();
				}
			}
		}
		SrVec axis=SrVec(0,0,0);
		axis[opdir]=1;
		SrQuat quatOffset;
		float angle = 2.0f*(float)M_PI/seg*(min_t_id-min_f_id);
		quatOffset.set(axis,angle);
		SrQuat newRot = getWorldRot()*quatOffset;
		setWorldRot(newRot);
		//axis=unit_quat_to_matrix(hdl->quat)*axis;
		//		mm->Manipulate(hdl,axis,2.*M_PI/seg*(min_t_id-min_f_id));
		//mm->RegisterManipulation(hdl,axis,2.*M_PI/seg*(min_t_id-min_f_id));
	}

	return true;
}

void RotationControl::resetColor()
{
	colors[0]=SrVec(1,0,0);
	colors[1]=SrVec(0,154.0f/255.0f,82.0f/255.0f);
	colors[2]=SrVec(0,0,1);
	colors[3]=SrVec(100.0f/255.0f, 220.0f/255.0f, 1.);
	colors[4]=SrVec(0.5,0.5,0.5);

	if (0<=opdir && opdir<=3)
		colors[opdir]=SrVec(1,1,0);	
}

void RotationControl::drawVisibleCircle( std::vector<SrVec> &cl,SrVec &center,SrVec &n )
{
	static bool first=true;
	int i=0;
	int ct=0;

// 	glBegin(GL_LINE_LOOP);
// 	for (int i=0;i<seg;i++)
// 		glVertex3fv(cl[i]);
// 	glEnd();

	

	if (dot((cl[i]-center),n)>=0){
		while(dot((cl[i]-center),n)>=0 && (size_t)ct<circle.size()){
			i=(i==0)?cl.size()-1:i-1;
			ct++;
		}
		i=((size_t)i<cl.size()-1)?i+1:0;
	}
	else{
		while(dot(cl[i]-center,n)<0)
			i=((size_t)i<cl.size()-1)?i+1:0;
	}
	

// 	printf("start index = %d\n",i);
// 	if (i==0)
// 	{
// 		printf("ct=%d, dot product = %f\n",ct,dot((cl[i]-center),n));
// 	}

	ct=0;
	glBegin(GL_LINE_STRIP);
	while(dot((cl[i]-center),n)>=0 && (size_t)ct<=circle.size()){
		glVertex3fv(cl[i]);
		i=((size_t)i<cl.size()-1)?i+1:0;
		ct++;
	}	
	glEnd();	
}
