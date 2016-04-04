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

#include "ParserASFAMC.h"
#include "sr/sr_euler.h"
#include <iostream>
#include <stack>
#include <vector>
#include <sstream>
#include <cctype>
#include <string>
#include <string.h>
#include <sb/SBJoint.h>

using namespace std;
#ifdef WIN32
	#define strcasecmp _stricmp
#endif

bool ParserASFAMC::parseAsf(SmartBody::SBSkeleton& skeleton, std::ifstream& metaFile, float scale)
{
	// parse .asf file first
	// check to make sure we have properly opened the file
	if (!metaFile.good())
	{
		LOG("ParserASFAMC::parse ERR: Could not open meta file.\n");
		return false;
	}

	char line[4096];
	int state = 0;
	char* str;
	SmartBody::SBJoint* cur = NULL;
	int curFrame = -1;
	double frameTime = 0.016777f;	// assume fps = 60
	bool useDegrees = true;
	vector<JointInfo*> jointInfoList;
	JointInfo* curJointInfo = NULL;

	while(!metaFile.eof() && metaFile.good())
	{
		metaFile.getline(line, 4096, '\n');
                if (line[strlen(line) - 1] == '\r')
                        line[strlen(line) - 1] = '\0';

		if (strlen(line) == 0) // ignore blank lines
			continue;
		if (strncmp(line, "#", 1) == 0) // ignore comment lines
			continue;

		switch (state)
		{
			case 0:	// looking for ':version'
				str = strtok(line, " ");
				if (strcmp(str, ":version") == 0)
				{
					state = 1;
				}
				else
				{
					LOG(":version not found...\n");
					metaFile.close();
					return false;
				}
				break;
			case 1:	// looking for ':name'
				str = strtok(line, " ");
				if (strcmp(str, ":name") == 0)
				{
					str = strtok(NULL, " ");
					if (str != NULL)
						skeleton.setName(str);
					state = 2;
				}
				else
				{
					LOG(":name not found...\n");
					metaFile.close();
					return false;
				}
				break;
			case 2: // looking for ':units'
				str = strtok(line, " ");
				if (strcmp(str, ":units") == 0)
				{
					state = 3;
				}
				else
				{
					LOG(":units not found...\n");
					metaFile.close();
					return false;
				}
				break;
			case 3: // units parameters or // looking for ':documentation'
				str = strtok(line, " ");
				if (strcmp(str, ":documentation") == 0)
				{
					state = 4;
				}
				else
				{
					char *value = strtok(NULL, " ");
					if (value != NULL)
					{
						LOG("Found parameter %s = %s.", str, value);
					}
					else
					{
						LOG("Problem parsing :units values...\n");
						metaFile.close();
						return false;
					}
				}
				break;
			case 4: // documentation values
				str = strtok(line, " ");
				if (strcmp(str, ":root") == 0)
				{
					cur = dynamic_cast<SmartBody::SBJoint*>(skeleton.add_joint(SkJoint::TypeQuat));
					skeleton.root(cur);
					cur->name("root");
					cur->quat()->activate();
					JointInfo* newJointInfo = new JointInfo();
					curJointInfo = newJointInfo;
					curJointInfo->name = "root";
					jointInfoList.push_back(newJointInfo);
					state = 5;
				}
				else // assume documentaion data
				{
					// ignore it
				}
				break;
			case 5: // root parameters or // looking for ':bonedata'
				str = strtok(line, " ");
				if (strcmp(str, ":bonedata") == 0)
				{
					state = 6;
				}
				else
				{
					if (strcmp(str, "order") == 0)
					{
						while ((str = strtok(NULL, " ")) != NULL)
						{
							curJointInfo->dof.push_back(str);
							if (strcasecmp(str, "tx") == 0)
							{
								cur->pos()->limits(SkVecLimits::X, false);
								skeleton.channels().add(cur->jointName(), SkChannel::XPos);
							}
							else if (strcasecmp(str, "ty") == 0)
							{
								cur->pos()->limits(SkVecLimits::Y, false);
								skeleton.channels().add(cur->jointName(), SkChannel::YPos);
							}
							else if (strcasecmp(str, "tz") == 0)
							{
								cur->pos()->limits(SkVecLimits::Z, false);
								skeleton.channels().add(cur->jointName(), SkChannel::ZPos);
							}
							else if (strcasecmp(str, "rx") == 0)
							{
								int quatId = skeleton.channels().search(cur->jointName(), SkChannel::Quat);
								if (quatId < 0)
									skeleton.channels().add(cur->jointName(), SkChannel::Quat);
							}
							else if (strcasecmp(str, "ry") == 0)
							{
								int quatId = skeleton.channels().search(cur->jointName(), SkChannel::Quat);
								if (quatId < 0)
									skeleton.channels().add(cur->jointName(), SkChannel::Quat);
							}
							else if (strcasecmp(str, "rz") == 0)
							{
								int quatId = skeleton.channels().search(cur->jointName(), SkChannel::Quat);
								if (quatId < 0)
									skeleton.channels().add(cur->jointName(), SkChannel::Quat);
							}
							else
							{
								LOG("Unrecognized channel '%s', continuing...", str);
							}
						}
						for (size_t d = 0; d < curJointInfo->dof.size(); d++)
							cur->setAsfChannels(curJointInfo->dof);
						
					}
					else if (strcmp(str, "axis") == 0)
					{
						// assume axis is XYZ for orientation parameters that follow
						// ignore it
					}
					else if (strcmp(str, "orientation") == 0)
					{
						// ignore it for now
					}
					else if (strcmp(str, "position") == 0)
					{
						SrVec pos;
						int posCount = 0;
						while ((str = strtok(NULL, " ")) != NULL)
						{
							if (posCount == 0)	pos.x = (float)atof(str);
							if (posCount == 1)	pos.y = (float)atof(str);
							if (posCount == 2)	pos.z = (float)atof(str);
							posCount++;
						}
						cur->offset(pos);
					}
					else
					{
						LOG("Unknown token '%s', expected ':bonedata', 'order', 'axis', 'position', 'orientation'...\n", str);
						metaFile.close();
						return false;
					}
				}
				break;
			case 6: // in bonedata or // looking for ':hierarchy'
				str = strtok(line, " ");
				if (strcmp(str, ":hierarchy") == 0)
				{
					state = 15;
				}
				else if (strcmp(str, "begin") == 0)
				{
					state = 7;
				}
				else
				{
					LOG("Unknown token '%s', expected ':hierarchy' or 'begin'...\n", str);
					metaFile.close();
					return false;
				}
				break;
			case 7: // found begin, looking for an id
				str = strtok(line, " ");
				if (strcmp(str, "id") == 0)
				{
					state = 8;
				}
				else
				{
					LOG("Unknown token '%s', expected 'id'...\n", str);
					metaFile.close();
					return false;
				}
				break;
			case 8: // found begin, looking for name
				str = strtok(line, " ");
				if (strcmp(str, "name") == 0)
				{
					str = strtok(NULL, " ");
					if (str != NULL)
					{
						JointInfo* newJointInfo = new JointInfo();
						curJointInfo = newJointInfo;
						newJointInfo->name = str;
						jointInfoList.push_back(newJointInfo);
						state = 9;
					}
					else // assign a name based on the joint number
					{
						LOG("Unknown name for joint....\n", str);
						metaFile.close();
						return false;
					}
				}
				else
				{
					LOG("Unknown token '%s', expected 'name'...\n", str);
					metaFile.close();
					return false;
				}
				break;
			case 9: // found begin, looking for direction
				str = strtok(line, " ");
				if (strcmp(str, "direction") == 0)
				{
					SrVec dir;
					int posCount = 0;
					while ((str = strtok(NULL, " ")) != NULL)
					{
						if (posCount == 0)	dir.x = (float)atof(str);
						if (posCount == 1)	dir.y = (float)atof(str);
						if (posCount == 2)	dir.z = (float)atof(str);
						posCount++;
					}
					curJointInfo->direction = dir;
//					LOG("Current direction of %s is %f %f %f...\n", cur->getName(), curDirection[0], curDirection[1], curDirection[2]);
					state = 10;
				}
				else
				{
					LOG("Unknown token '%s', expected 'direction'...\n", str);
					metaFile.close();
					return false;
				}
				break;
			case 10: // found begin, looking for length
				str = strtok(line, " ");
				if (strcmp(str, "length") == 0)
				{
					str = strtok(NULL, " ");
					double length = atof(str);
					curJointInfo->length = (float)length;
					state = 11;
				}
				else
				{
					LOG("Unknown token '%s', expected 'length'...\n", str);
					metaFile.close();
					return false;
				}
				break;
			case 11: // found begin , looking for axis;
				str = strtok(line, " ");
				if (strcmp(str, "axis") == 0)
				{
					SrVec axis;
					int axisCount = 0;
					while ((str = strtok(NULL, " ")) != NULL && axisCount < 3)
					{
						if (axisCount == 0) axis.x = (float)atof(str);
						if (axisCount == 1) axis.y = (float)atof(str);
						if (axisCount == 2) axis.z = (float)atof(str);
						axisCount++;
					}
					if (useDegrees)
						axis *= (float)M_PI / 180.0f;
					curJointInfo->axis = axis;
					state = 12;
				}
				else
				{
					LOG("Unknown token '%s', expected 'axis'...\n", str);
					metaFile.close();
					return false;
				}
				break;
			case 12: // found begin, looking for dof
				str = strtok(line, " ");
				if (strcmp(str, "dof") == 0)
				{
					while ((str = strtok(NULL, " ")) != NULL)
					{
						curJointInfo->dof.push_back(str);
					}
					state = 13;
				}
				else if (strcmp(str, "limits") == 0) // no dof found, passed to limits
				{
					state = 14;
				}
				else if (strcmp(str, "end") == 0) // no dof found, passed to end
				{
					state = 6;
				}
				else
				{
					LOG("Unknown token '%s', expected 'dof'...\n", str);
					metaFile.close();
					return false;
				}
				break;
			case 13: // found begin , looking for limits;
				str = strtok(line, " ");
				if (strcmp(str, "limits") == 0)
				{
					// ignore limits
					state = 14;
				}
				else
				{
					LOG("Unknown token '%s', expected 'limits'...\n", str);
					metaFile.close();
					return false;
				}
				break;
			case 14: // found begin , looking for limit data or // looking for end;
				str = strtok(line, " ");
				if (strcmp(str, "end") == 0)
				{
					// return to previous state, look for :hierarchy or another begin
					state = 6;
				}
				else if (strncmp(str, "(", 1) == 0)
				{
					// found more limits, ignore
				}
				else
				{
					LOG("Unknown token '%s', expected ':hierarchy' or limit data...\n", str);
					metaFile.close();
					return false;
				}
				break;
			case 15: // found :hierarchy, looking for begin
				str = strtok(line, " ");
				if (strcmp(str, "begin") == 0)
				{
					state = 16;
				}
				else
				{
					LOG("Unknown token '%s', expected 'begin'...\n", str);
					metaFile.close();
					return false;
				}
				break;
			case 16: // parent-child joint mapping
				str = strtok(line, " ");
				if (strcmp(str, "end") == 0)
				{
					state = 17;
				}
				else
				{
					SkJoint* parent = skeleton.search_joint(str);
					if (parent == NULL)
					{
						LOG("Unknown parent '%s' when determining joint hierarchy...\n", str);
						metaFile.close();
						return false;
					}
					int parentId = parent->index();
					JointInfo* parentInfo = getJointInfo(str, jointInfoList);
					if (parentInfo == NULL)
					{
						LOG("Unknown parent '%s' when determining joint hierarchy...\n", str);
						metaFile.close();
						return false;
					}

					JointInfo* childInfo = NULL;
					while ((str = strtok(NULL, " ")) != NULL)
					{
						childInfo = getJointInfo(str, jointInfoList);
						if (childInfo == NULL)
						{
							LOG("Unknown child '%s' when determining joint hierarchy...\n", str);
							metaFile.close();
							return false;
						}
						SmartBody::SBJoint* child = dynamic_cast<SmartBody::SBJoint*>(skeleton.add_joint(SkJoint::TypeQuat, parentId));
						child->name(childInfo->name);
						child->quat()->activate();
						child->pos()->limits(SkVecLimits::X, false);
						child->pos()->limits(SkVecLimits::Y, false);
						child->pos()->limits(SkVecLimits::Z, false);
						SrVec offset = parentInfo->direction * parentInfo->length;
						child->offset(offset);
						child->setAsfAxis(childInfo->axis);
						child->setAsfChannels(childInfo->dof);

						bool hasRotation = false;
						for (unsigned int i = 0; i < childInfo->dof.size(); i++)
						{
							const char* dofChar = childInfo->dof[i].c_str();
							if (strcasecmp(dofChar, "rx") == 0 || strcasecmp(dofChar, "ry") == 0 || strcasecmp(dofChar, "rz") == 0)
							{
								hasRotation = true;
								break;
							}
						}
						if (hasRotation)
							skeleton.channels().add(child->jointName(), SkChannel::Quat);
					}
				}
				break;
			case 17:
				metaFile.close();
				break;
			default:
				cerr << "State " << state << " not expected..." << endl;
				metaFile.close();
				return false;
		}

	}
	skeleton.compress();
	skeleton.make_active_channels();
	skeleton.updateGlobalMatricesZero();

	return true;

}


bool ParserASFAMC::parseAmc(SmartBody::SBMotion& motion, SmartBody::SBSkeleton* skeleton, std::ifstream& dataFile, float scale)
{
	// parse .amc files
	if (!dataFile.good())
		return true;

	std::map<std::string, JointInfo*> jointInfoMap;

	// load the joint info for each joint
	int numJoints = skeleton->getNumJoints();
	for (int j = 0; j < numJoints; j++)
	{
		SmartBody::SBJoint* joint = skeleton->getJoint(j);
		JointInfo* jointInfo = new JointInfo();
		jointInfo->name = joint->getName();
		std::vector<std::string>& asfChannels = joint->getAsfChannels();
		for (size_t c = 0; c < asfChannels.size(); c++)
		{
			jointInfo->dof.push_back(asfChannels[c]);
		}
		jointInfo->axis = joint->getAsfAxis();
		jointInfoMap[jointInfo->name] = jointInfo;
	}
	
	char line[4096];

	int state = 17;
	char* str = NULL;
	bool useDegrees = true;
	double frameTime = 0.016777f;
	char frameStr[128];

	motion.init(skeleton->channels());
	SkChannelArray& motionChannels = motion.channels();
	int curFrame = 0;
	int parsedFrameNum;
	while(!dataFile.eof() && dataFile.good())
	{
		dataFile.getline(line, 4096, '\n');
                if (line[strlen(line) - 1] == '\r')
                        line[strlen(line) - 1] = '\0';

		if (strlen(line) == 0) // ignore blank lines
			continue;
		if (strncmp(line, "#", 1) == 0) // ignore comment lines
			continue;

		switch (state)
		{
			case 17: // start of motion data file parse
				str = strtok(line, " ");
				if (strcasecmp(str, ":FULLY-SPECIFIED") == 0)
				{
					// ignore
				}
				else if (strcasecmp(str, ":DEGREES") == 0)
				{
					useDegrees = true;
				}
				else if (strcasecmp(str, ":RADIANS") == 0)
				{
					useDegrees = false;
				}
				else if (atoi(str) != 0)
				{
					parsedFrameNum = atoi(str);
					curFrame = parsedFrameNum - 1;
					motion.insert_frame(curFrame, float(frameTime * curFrame));
					for (int i = 0; i < motion.posture_size(); i++)
						motion.posture(curFrame)[i] = 0.0f;
					//std::cout << "FRAME NUMBER " << curFrame << std::endl;
					state = 18;
				}
				else
				{
					LOG("Unknown token '%s', continuing...", str);
				}
				break;
			case 18:
				// get the joint name
				str = strtok(line, " ");
				// is this a frame nunmber and not a joint?
				parsedFrameNum = atoi(str);
				sprintf(frameStr, "%d", parsedFrameNum);
				if (strcmp(frameStr, str) == 0)
				{
					// this is a frame number
					curFrame = parsedFrameNum - 1;
					motion.insert_frame(curFrame, float(frameTime * curFrame));
					for (int i = 0; i < motion.posture_size(); i++)
						motion.posture(curFrame)[i] = 0.0f;
					break;
				}
				// find the joint
				JointInfo* jointInfo = NULL;//getJointInfo(str, jointInfoList);
				std::map<std::string, JointInfo*>::iterator iter = jointInfoMap.find(str);
				if (iter != jointInfoMap.end())
				{
					jointInfo = (*iter).second;
				}
				
				if (!jointInfo)
				{
					LOG("Unknown joint '%s', continue...", str);
					break;
				}
				// get the data
				float frames[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
				int curChannel = 0;
				while ((str = strtok(NULL, " ")) != NULL)
				{
					frames[curChannel] = (float)atof(str);
					curChannel++;
				}
				if (curChannel != jointInfo->dof.size())
				{
					LOG("Unmatched number of channels.");
				}
				// assign the frame
				for (unsigned int i = 0; i < jointInfo->dof.size(); i++)
				{
					int id = getMotionChannelId(motionChannels, jointInfo->dof[i], jointInfo->name);
					if (id >= 0)
						motion.posture(curFrame)[id] = frames[i];
					else
					{
						LOG("getMotionChannelId returning -1.");
					}
				}
				break;
		}
	}
	dataFile.close();
	if (state != 18)
	{
		LOG("Motion incompletely parsed, finished at state %d...", state);
		dataFile.close();
	}
	else
	{
		LOG("Finished parsing motion with %d frames...", curFrame);
	}

	// Get rot order 
	// Here assume all the joints are the same order with root joint, maybe there's case where each joint has independent rotation order
	int order = -1;
	JointInfo* rootInfo = NULL;
	if (jointInfoMap.size() > 0)
	{
		std::map<std::string, JointInfo*>::iterator iter = jointInfoMap.find("root");
		if (iter != jointInfoMap.end())
		{
			rootInfo = (*iter).second;
		}
	}

	if (rootInfo)
	{
		std::vector<std::string> orderVec;
		for (unsigned int i = 0; i < rootInfo->dof.size(); i++)
		{
			if (rootInfo->dof[i] == "RX" || rootInfo->dof[i] == "rx")
				orderVec.push_back("x");
			if (rootInfo->dof[i] == "RY" || rootInfo->dof[i] == "ry")
				orderVec.push_back("y");
			if (rootInfo->dof[i] == "RZ" || rootInfo->dof[i] == "rz")
				orderVec.push_back("z");
		}
		if (orderVec[0] == "x" && orderVec[1] == "y" && orderVec[2] == "z")
			order = 123;
		if (orderVec[0] == "x" && orderVec[1] == "z" && orderVec[2] == "y")
			order = 132;
		if (orderVec[0] == "y" && orderVec[1] == "x" && orderVec[2] == "z")
			order = 213;
		if (orderVec[0] == "y" && orderVec[1] == "z" && orderVec[2] == "x")
			order = 231;
		if (orderVec[0] == "z" && orderVec[1] == "x" && orderVec[2] == "y")
			order = 312;
		if (orderVec[0] == "z" && orderVec[1] == "y" && orderVec[2] == "x")
			order = 321;
	}

	// change rotation to quaternion
	std::vector<int> quatIndices;
	for (int i = 0; i < motionChannels.size(); i++)
	{
		SkChannel& chan = motionChannels[i];
		if (chan.type == SkChannel::Quat)
		{
			quatIndices.push_back(i);
		}
	}
	for (int frameCt = 0; frameCt < motion.frames(); frameCt++)
		for (size_t i = 0; i < quatIndices.size(); i++)
		{
			// Matrix M
			int quatId = motionChannels.float_position(quatIndices[i]);
			float rotx = motion.posture(frameCt)[quatId + 0] / scale;
			float roty = motion.posture(frameCt)[quatId + 1] / scale;
			float rotz = motion.posture(frameCt)[quatId + 2] / scale;
			if (useDegrees)
			{
				rotx *= float(M_PI) / 180.0f;
				roty *= float(M_PI) / 180.0f;
				rotz *= float(M_PI) / 180.0f;
			}
			SrMat M;
			sr_euler_mat(order, M, rotx, roty, rotz);

			// Matrix C
			const std::string& jName = motion.channels().name(quatIndices[i]);
			JointInfo* jointInfo = NULL;

			std::map<std::string, JointInfo*>::iterator iter = jointInfoMap.find(jName);
			if (iter != jointInfoMap.end())
				jointInfo = (*iter).second;

			SrMat C;
			sr_euler_mat(order, C, jointInfo->axis.x, jointInfo->axis.y, jointInfo->axis.z);
		
			SrMat mat = C.inverse() * M * C;
			SrQuat quat = SrQuat(mat);
			motion.posture(frameCt)[quatId + 0] = quat.w;
			motion.posture(frameCt)[quatId + 1] = quat.x;
			motion.posture(frameCt)[quatId + 2] = quat.y;
			motion.posture(frameCt)[quatId + 3] = quat.z;
		}

	double duration = double(motion.duration());
	motion.synch_points.set_time(0.0, duration / 4.0, duration / 3.0, duration / 2.0, duration * 2.0 / 3.0, duration * 3.0 / 4.0, duration);
	motion.compress();	

	// clean up
	for (std::map<std::string, JointInfo*>::iterator iter = jointInfoMap.begin();
		 iter != jointInfoMap.end();
		 iter++)
	{
		delete (*iter).second;
	}
	return true;
}

int ParserASFAMC::getMotionChannelId(SkChannelArray& mChannels, std::string sourceName, std::string jName)
{
	int id = -1;
	int dataId = -1;
	SkChannel::Type chanType;
	if (strcasecmp(sourceName.c_str(), "tx") == 0)
	{
		chanType = SkChannel::XPos;
		id = mChannels.search(jName.c_str(), chanType);
		dataId = mChannels.float_position(id);
	}
	else if (strcasecmp(sourceName.c_str(), "ty") == 0)
	{
		chanType = SkChannel::YPos;
		id = mChannels.search(jName.c_str(), chanType);
		dataId = mChannels.float_position(id);
	}
	else if (strcasecmp(sourceName.c_str(), "tz") == 0)
	{
		chanType = SkChannel::ZPos;
		id = mChannels.search(jName.c_str(), chanType);
		dataId = mChannels.float_position(id);
	}
	else if (strcasecmp(sourceName.c_str(), "rx") == 0)
	{
		chanType = SkChannel::Quat;
		id = mChannels.search(jName.c_str(), chanType);
		dataId = mChannels.float_position(id) + 0;
	}
	else if (strcasecmp(sourceName.c_str(), "ry") == 0)
	{
		chanType = SkChannel::Quat;
		id = mChannels.search(jName.c_str(), chanType);
		dataId = mChannels.float_position(id) + 1;
	}
	else if (strcasecmp(sourceName.c_str(), "rz") == 0)
	{
		chanType = SkChannel::Quat;
		id = mChannels.search(jName.c_str(), chanType);
		dataId = mChannels.float_position(id) + 2;
	}
	return dataId;
}

JointInfo* ParserASFAMC::getJointInfo(std::string jointName, std::vector<JointInfo*>& jointInfoList)
{
	JointInfo* jointInfo = NULL;
	for (unsigned int cnum = 0; cnum < jointInfoList.size(); cnum++)
	{
		std::string jName =  jointInfoList[cnum]->name;
		if (jointName == jName)
		{
			jointInfo = jointInfoList[cnum];
			break;
		}
	}	
	return jointInfo;
}
