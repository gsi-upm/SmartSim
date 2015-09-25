#pragma once
#include <vector>
#include <sr/sr_vec.h>
#include <sr/sr_vec2.h>
#include <sr/sr_camera.h>

class ObjectControl
{
protected:	
	float base,r,len,s_len,ss_len;			
	SrVec worldPt;
	SrVec prevPt;
	SrQuat worldRot;
public:
	bool dragging;
	bool active;

	ObjectControl();
	~ObjectControl();

	void setColor(const SrVec &color);
	virtual SrVec getWorldPt(); // get world position ( from the attached object ? )
	virtual void setWorldPt(SrVec& newPt);
	virtual SrQuat getWorldRot();
	virtual void setWorldRot(SrQuat& newRot);
	

	virtual bool drag(SrCamera& cam, float  fx, float fy, float tx, float ty) = 0;
	virtual void draw(SrCamera& cam) = 0; 

	virtual void hitOPS(SrCamera& cam) {};	
	virtual void identify(std::vector<int>& path) {} ;

public:
	// utility functions
	void screenParallelPlane(SrCamera& cam,const SrVec &center,SrVec &dirx,SrVec &diry);
	SrVec worldToScreen(const SrCamera& cam, const SrVec& pos);
	SrVec worldToEye(const SrCamera& cam, const SrVec& pos);
	SrVec screenToWorld(const SrCamera& cam, const SrVec& win);
	SrVec mouseToWorld(SrCamera& cam, float fx, float fy, float tx, float ty);
};


class SelectionControl : public ObjectControl
{
public:
	SelectionControl();
	~SelectionControl();

	virtual bool drag(SrCamera& cam, float fx, float fy, float tx, float ty);
	virtual void draw(SrCamera& cam);	
};


class PositionControl : public ObjectControl
{
protected:	
	int opdir;	
	SrVec colors[4];
public:
	PositionControl();
	~PositionControl(void);

	virtual void draw(SrCamera& cam);
	virtual bool drag(SrCamera& cam, float  fx, float fy, float tx, float ty);
	virtual void hitOPS(SrCamera& cam);	
	virtual void identify(std::vector<int>& path);

	static void drawSphere(SrVec& pos, float fRadius, SrVec& color);
	static void drawBox(SrBox& box, bool wireFrame, SrVec& color);
	void drawShadowSquare(float x,float y,float z,SrVec& dirx,SrVec& diry,float sz,unsigned int mode);	
protected:
	void drawCenter();		
	void resetColor();
};

