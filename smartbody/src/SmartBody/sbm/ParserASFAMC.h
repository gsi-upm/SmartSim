/**************************************************
Copyright 2005 by Ari Shapiro and Petros Faloutsos

DANCE
Dynamic ANimation and Control Environment

 ***************************************************************
 ******General License Agreement and Lack of Warranty ***********
 ****************************************************************

This software is distributed for noncommercial use in the hope that it will 
be useful but WITHOUT ANY WARRANTY. The author(s) do not accept responsibility
to anyone for the consequences	of using it or for whether it serves any 
particular purpose or works at all. No warranty is made about the software 
or its performance. Commercial use is prohibited. 

Any plugin code written for DANCE belongs to the developer of that plugin,
who is free to license that code in any manner desired.

Content and code development by third parties (such as FLTK, Python, 
ImageMagick, ODE) may be governed by different licenses.
You may modify and distribute this software as long as you give credit 
to the original authors by including the following text in every file 
that is distributed: */

/*********************************************************
	Copyright 2005 by Ari Shapiro and Petros Faloutsos

	DANCE
	Dynamic ANimation and Control Environment
	-----------------------------------------
	AUTHOR:
		Ari Shapiro (ashapiro@cs.ucla.edu)
	ORIGINAL AUTHORS: 
		Victor Ng (victorng@dgp.toronto.edu)
		Petros Faloutsos (pfal@cs.ucla.edu)
	CONTRIBUTORS:
		Yong Cao (abingcao@cs.ucla.edu)
		Paco Abad (fjabad@dsic.upv.es)
**********************************************************/

#ifndef ASFAMCPARSER_H
#define ASFAMCPARSER_H


#include <fstream>
#include <sb/SBSkeleton.h>
#include <sb/SBMotion.h>

struct JointInfo
{
	std::string name;
	SrVec direction;
	float length;
	SrVec axis;
	std::vector<std::string> dof;
};

class ParserASFAMC
{
	public:
		static bool parseAsf(SmartBody::SBSkeleton& skeleton, std::ifstream& metaFile, float scale);
		static bool parseAmc(SmartBody::SBMotion& motion, SmartBody::SBSkeleton* skeleton, std::ifstream& dataFile, float scale);
	
	private:
		static int getMotionChannelId(SkChannelArray& channels, std::string channelType, std::string jointName);
		static JointInfo* getJointInfo(std::string jointName, std::vector<JointInfo*>& jointInfoList);
};

#endif