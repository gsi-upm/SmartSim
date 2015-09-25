#include "SteerPath.h"
#include <sr/sr_line.h>
#include <float.h>
#include <math.h>
#include <vhcl.h>

SteerPath::SteerPath(void)
{
	currentGoal = 1;
}

SteerPath::~SteerPath(void)
{
}

void SteerPath::initPath( const std::vector<SrPnt>& pts, float radius )
{
	pathPts = pts;
	pathRadius = radius;
	for (unsigned int i=1; i<pathPts.size(); i++)
	{
		SrVec lineDir = pathPts[i] - pathPts[i-1];
		float length = lineDir.len();
		pathSegLength.push_back(length);
		lineDir.normalize();
		pathSegDir.push_back(lineDir);
	}
	currentGoal = 1;
}

void SteerPath::clearPath()
{
	pathPts.clear();
	pathSegLength.clear();
	pathSegDir.clear();
	pathRadius = 0.f;
	currentGoal = 1;
}

bool SteerPath::atLastGoal()
{
	return (currentGoal >= pathPts.size()-1);
}

void SteerPath::advanceToNextGoal( float length )
{
	unsigned int iPath = currentGoal;
	if (iPath > pathPts.size()-1)
	{
		iPath = pathPts.size()-1;
	}	
	float pathTotalLength = 0.f;
	for (unsigned int i=0;i<iPath;i++)
	{
		pathTotalLength += pathSegLength[i];
	}
	if (pathTotalLength < length)
	{
		currentGoal++;
		//LOG("next goal = %d",currentGoal);
	}
	
}

SrVec SteerPath::closestPointOnNextGoal( const SrVec& pt, SrVec& tangent, float& dist )
{
	SrVec closePt;	
	unsigned int iPath = currentGoal;
	if (iPath > pathPts.size()-1)
	{
		iPath = pathPts.size()-1;
	}	

	SrLine line(pathPts[iPath-1],pathPts[iPath]);
	float k;
	SrVec linePt = line.closestpt(pt,&k);
	float lineDist = (linePt - pt).len();	
	closePt = linePt;
	tangent = pathSegDir[iPath-1];	
	return closePt;
}


SrVec SteerPath::pathGoalPoint()
{
	unsigned int iPath = currentGoal;
	if (iPath > pathPts.size()-1)
	{
		iPath = pathPts.size()-1;
	}	
	return pathPts[iPath];	
}

SrVec SteerPath::closestPointOnPath( const SrVec& pt, SrVec& tangent, float& dist )
{
	SrVec closePt;
	float minDist = FLT_MAX;
	for (unsigned int i=1; i<pathPts.size(); i++)
	{
		SrLine line(pathPts[i-1],pathPts[i]);
		float k;
		SrVec linePt = line.closestpt(pt,&k);
		float lineDist = (linePt - pt).len();

		if (lineDist < minDist) // line segment distance
		{
			minDist = lineDist;
			closePt = linePt;
			tangent = pathSegDir[i-1];
			tangent.normalize();	
		}		
	}	
	return closePt;
}

SrVec SteerPath::pathTangent( float length )
{
	float remain = length;
	if (remain > pathLength())
		remain = pathLength() - 0.01f;	
	SrVec outDir;
	for (size_t i=0;i<pathSegLength.size();i++)
	{
		float pathSegLen = pathSegLength[i];
		if (remain <= pathSegLength[i])
		{
			outDir = pathSegDir[i];
			break;
		}
		else
		{
			remain -= pathSegLength[i];
		}
	}
	return outDir;
}

SrVec SteerPath::pathPoint( float length )
{
	float remain = length;
	if (remain > pathLength())
		remain = pathLength() - 0.01f;
	SrVec outPt;
	for (size_t i=0; i < pathSegLength.size(); i++)
	{
		float pathSegLen = pathSegLength[i];
		if (remain <= pathSegLength[i])
		{
			outPt = pathPts[i] + pathSegDir[i]*remain;
			break;
		}
		else
		{
			remain -= pathSegLength[i];
		}
	}
	return outPt;
}



float SteerPath::pathDistance( const SrVec& pt )
{
	float minDist = FLT_MAX;
	float totalSegDist = 0.f;
	float outDist = 0.f;
	for (size_t i=0; i < pathSegLength.size(); i++)
	{
		SrLine line(pathPts[i],pathPts[i+1]);
		SrVec linePt = line.closestpt(pt);
		float lineDist = (linePt - pt).len();
		float projectDist = (linePt - pathPts[i]).len();
		if (lineDist < minDist)		
		{
			minDist = lineDist;
			outDist = totalSegDist + projectDist;			
		}	
		totalSegDist += pathSegLength[i];
	}
	return outDist;
}


float SteerPath::pathLength()
{
	float totalLength = 0.f;
	for (size_t i=0;i<pathSegLength.size();i++)
	{
		totalLength += pathSegLength[i];
	}
	return totalLength;
}

float SteerPath::pathCurvature( float start, float end )
{
	SrVec dir1 = pathTangent(start);
	SrVec dir2 = pathTangent(end);
	float dotValue = dot(dir1,dir2);
	if (dotValue > 1) dotValue = 1; if (dotValue < -1) dotValue = -1;
	return fabs(acos(dotValue));	
}
