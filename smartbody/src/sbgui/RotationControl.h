#pragma once
#include "PositionControl.h"

class RotationControl : public ObjectControl
{
protected:	
	int opdir;	
	SrVec colors[5];
	std::vector<SrVec2> circle;
	float radius[2];
	int seg;

	SrQuat quat0;
public:
	RotationControl(void);
	~RotationControl(void);

	virtual void draw(SrCamera& cam);
	virtual bool drag(SrCamera& cam, float  fx, float fy, float tx, float ty);
	virtual void hitOPS(SrCamera& cam);	
	virtual void identify(std::vector<int>& path);

	void drawVisibleCircle(std::vector<SrVec> &cl,SrVec &center,SrVec &n);
protected:
	void resetColor();
};

