#pragma once
#include <vector>
#include <sr/sr_vec.h>

class SteerPathBase
{
public:
	virtual SrVec closestPointOnPath(const SrVec& pt, SrVec& tangent, float& dist) = 0;
	virtual SrVec pathPoint(float length) = 0;
	virtual SrVec pathTangent(float length) = 0;	
	virtual float pathDistance(const SrVec& pt) = 0;
	virtual float pathLength() = 0;	

};

class SteerPath : public SteerPathBase// polyline path
{
protected:	
	std::vector<SrVec> pathPts;
	std::vector<SrVec> pathSegDir;
	std::vector<float> pathSegLength;
	float              pathRadius;
	unsigned int       currentGoal;
public:
	SteerPath(void);
	~SteerPath(void);
	
	void initPath(const std::vector<SrPnt>& pts, float radius);	
	void clearPath();
		
	virtual SrVec closestPointOnPath(const SrVec& pt, SrVec& tangent, float& dist);	
	virtual SrVec pathPoint(float length);
	virtual SrVec pathTangent(float length);
	virtual float pathDistance(const SrVec& pt);
	virtual float pathLength();
	virtual float pathCurvature(float start, float end);
	SrVec closestPointOnNextGoal(const SrVec& pt, SrVec& tangent, float& dist);		
	SrVec pathGoalPoint();
	bool atLastGoal();
	void advanceToNextGoal(float length);
};
