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

#include "vhcl_log.h"
#include "ParserBVH.h"
#include <iostream>
#include <stack>
#include <sstream>
#include <cstdlib>
#include <boost/algorithm/string/trim.hpp>
#include <map>
#include "sr/sr_mat.h"
#include "sr/sr_quat.h"

using namespace std;

enum {BVHXPOSITION, BVHYPOSITION, BVHZPOSITION, BVHXROTATION, BVHYROTATION, BVHZROTATION};
enum {ROTXYZ, ROTXZY, ROTYXZ, ROTYZX, ROTZXY, ROTZYX};

bool ParserBVH::parse(SkSkeleton& skeleton, SkMotion& motion, std::string name, std::ifstream &file, float scale, int N1, int N2)
{
	// check to make sure we have properly opened the file
	if (!file.good())
	{
		LOG("Could not open file\n");
		return false;
	}
	char line[8192];
	skeleton.setName(name);
	int state = 0;
	char* str = NULL;
	stack<SkJoint*> stack;
	SkJoint* cur = NULL;
	int numFrames = 0;
	int curFrame = -1;
	double frameTime = 0;
	int jointCounter = 0;
	int foundRoot = 0; // 0 = root not found, 1 = root found, 2 = next joint found
	int curChannelIndex = 0;

	int postureSize = 0;
	std::vector<ChannelInfo*> channelInfoMap;
	std::vector<std::pair<int, int> > bvhIndex;
	SkChannelArray& skChannels = skeleton.channels();
	SkChannelArray motionChannels;

	while(!file.eof() && file.good())
	{
		file.getline(line, 8192, '\n');
                // remove any trailing \r
                if (line[strlen(line) - 1] == '\r')
                        line[strlen(line) - 1] = '\0';
		if (strlen(line) == 0) // ignore blank lines
			continue;
		switch (state)
		{
			case 0:	// looking for 'HIERARCHY'
				str = strtok(line, " \t");	
				if (strncmp(str, "HIERARCHY", strlen("HIERARCHY")) == 0)
					state = 1;
				else
				{
					LOG("HIERARCHY not found...\n");
					file.close();
					return false;
				}
				break;
			case 1:	// looking for 'ROOT'
				str = strtok(line, " \t");	
				if (str != NULL && strncmp(str, "ROOT", strlen("ROOT")) == 0)
				{
					str = strtok(NULL, " \t");
					if (str != NULL)
					{
						std::string trimmedname = str;
						boost::trim(trimmedname);
						// if there is a namespace, eliminate it
						int pos = trimmedname.find_first_of(":");
						if (pos != std::string::npos)
							trimmedname = trimmedname.substr(pos + 1, trimmedname.size() - pos + 1);
						std::stringstream strstr;
						strstr << trimmedname;
						SkJoint* root = skeleton.add_joint(SkJoint::TypeQuat);
						root->quat()->activate();
						jointCounter++;
						root->name(strstr.str());
						//skeleton.make_active_channels();
						cur = root;
						state = 2;
					}
					else
					{
						LOG("ROOT name not found...\n");
						file.close();
						return false;
					}
				}
				else
				{
					LOG("ROOT not found...\n");
					file.close();
					return false;
				}
				break;
			case 2: // looking for '{'
				str = strtok(line, " \t");
				if (str != NULL && strncmp(str, "{", 1) == 0)
				{
					stack.push(cur);
					state = 3;
				}
				else
				{
					LOG("{ not found...\n");
					file.close();
					return false;
				}
				break;
			case 3: // looking for 'OFFSET'
				str = strtok(line, " \t");
				if (str != NULL && strncmp(str, "OFFSET", strlen("OFFSET")) == 0)
				{
					if (foundRoot == 0)
						foundRoot = 1;
					else if (foundRoot == 1)
						foundRoot = 2;
					double x = 0; double y = 0; double z = 0;
					str = strtok(NULL, " \t");
					x = atof(str);
					str = strtok(NULL, " \t");
					y = atof(str);
					str = strtok(NULL, " \t");
					z = atof(str);
					cur->offset(SrVec(float(x) * scale, float(y) * scale, float(z) * scale));
					//cout << "Found offset of " << x << " " << y << " " << z << " " << endl;
					state = 4;
				}
				else
				{
					LOG("OFFSET not found...\n");
					file.close();
					return false;
				}
				break;
			case 4: // looking for 'CHANNELS'
				str = strtok(line, " \t");
				if (str != NULL && strncmp(str, "CHANNELS", strlen("CHANNELS")) == 0)
				{
					str = strtok(NULL, " \t");
					int numChannels = atoi(str);

					// make sure that only the root has > 3 channels
					if (numChannels > 3 && foundRoot == 2)
					{
						//LOG("Too many channels (%d) found for non-root node at %s, reducing to %d...", numChannels, cur->getName(), numChannels - 3);
						//cur->setIgnoreChannels(true);
					}
					ChannelInfo* channelInfo = new ChannelInfo();
					channelInfo->numBVHChannels = 0;
					channelInfo->order = 0;
					channelInfo->startingChannelIndex = curChannelIndex;
					//LOG("Found %d channels...\n", numChannels);
					bool hasRotation = false;
					for (int c = 0; c < numChannels; c++)
					{
						str = strtok(NULL, " \t");
						if (strncmp(str, "Xrotation", strlen("Xrotation")) == 0)
						{
							channelInfo->channels[c] = BVHXROTATION;
							channelInfo->numBVHChannels++;
							hasRotation = true;
						}
						else if (strncmp(str, "Yrotation", strlen("Yrotation")) == 0)
						{
							channelInfo->channels[c] = BVHYROTATION;							
							channelInfo->numBVHChannels++;
							hasRotation = true;
						}
						else if (strncmp(str, "Zrotation", strlen("Zrotation")) == 0)
						{
							channelInfo->channels[c] = BVHZROTATION;							
							channelInfo->numBVHChannels++;
							hasRotation = true;
						}
						else if (strncmp(str, "Xposition", strlen("Xposition")) == 0)
						{
							channelInfo->channels[c] = BVHXPOSITION;		
							skChannels.add(cur->jointName(), SkChannel::XPos);
							motionChannels.add(cur->jointName(), SkChannel::XPos);
							cur->pos()->limits( SkVecLimits::X, false );
							channelInfo->numBVHChannels++;
							curChannelIndex++;
						}
						else if (strncmp(str, "Yposition", strlen("Yposition")) == 0)
						{
							channelInfo->channels[c] = BVHYPOSITION;		
							skChannels.add(cur->jointName(), SkChannel::YPos);
							motionChannels.add(cur->jointName(), SkChannel::YPos);
							cur->pos()->limits( SkVecLimits::Y, false );
							channelInfo->numBVHChannels++;
							curChannelIndex++;
						}
						else if (strncmp(str, "Zposition", strlen("Zposition")) == 0)
						{
							channelInfo->channels[c] = BVHZPOSITION;	
							skChannels.add(cur->jointName(), SkChannel::ZPos);
							motionChannels.add(cur->jointName(), SkChannel::ZPos);
							cur->pos()->limits( SkVecLimits::Z, false );
							channelInfo->numBVHChannels++;
							curChannelIndex++;
						}
						else
						{
							LOG("Unknown channel: %s...\n", str);;
							file.close();
							return false;
						}
						
					}

					if (hasRotation)
					{
						// determine the rotation order
						channelInfo->order = determineRotationOrder(channelInfo->channels, channelInfo->numBVHChannels);
						skChannels.add(cur->jointName(), SkChannel::Quat);
						motionChannels.add(cur->jointName(), SkChannel::Quat);
						curChannelIndex += 4;
					}
					channelInfoMap.push_back(channelInfo);

					//if (cur->isIgnoreChannels())
					//{
					//	for (int i = 0; i < 3; i++)
					//		channels[i] = channels[i + 3];
					//	numChannels -= 3;
					//}
					state = 5;
				}
				else
				{
					LOG("CHANNELS not found...\n");;
					file.close();
					return false;
				}
				break;
			case 5: // looking for 'JOINT' or 'End Site' or '}' or 'MOTION'
				str = strtok(line, " \t");
				if (strncmp(str, "JOINT", strlen("JOINT")) == 0)
				{
					str = strtok(NULL, "");
					if (str != NULL)
					{
						std::string trimmedname = str;
						boost::trim(trimmedname);
						// if there is a namespace, eliminate it
						int pos = trimmedname.find_first_of(":");
						if (pos != std::string::npos)
							trimmedname = trimmedname.substr(pos + 1, trimmedname.size() - pos + 1);
						SkJoint* parent = stack.top();
						SkJoint* joint = skeleton.add_joint(SkJoint::TypeQuat, parent->index());
						joint->quat()->activate();
						jointCounter++;
						joint->name(trimmedname);
						skeleton.make_active_channels();
						cur = joint;
						//cout << "Found joint " << str << endl;
						state = 2;
					}
					else
					{
						LOG("ROOT name not found...\n");;
						file.close();
						return false;
					}
				}
				else if (strncmp(str, "End", strlen("End")) == 0)
				{
					str = strtok(NULL, " \t");
					if (strncmp(str, "Site", strlen("Site")) == 0)
					{
						state = 6;
					}
					else
					{
						LOG("End site not found...\n");;
						file.close();
						return false;
					}
				}
				else if (strncmp(str, "}", 1) == 0)
				{
					str = strtok(line, " \t");
					if (str != NULL && strncmp(str, "}", 1) == 0)
					{
						stack.pop();
						state = 5;
					}
					else
					{
						LOG("} not found...\n");;
						file.close();
						return false;
					}
				}
				else if (strncmp(str, "MOTION", strlen("MOTION")) == 0)
				{
					state = 9;
				}
				else
				{
					LOG("JOINT or End Site not found...\n");;
					file.close();
					return false;
				}
				break;
			case 6: // looking for 'OFFSET' within end effector
				str = strtok(line, " \t");
				if (str != NULL && strncmp(str, "{", 1) == 0)
				{
					state = 7;
				}
				else
				{
					LOG("{ not found for end effector...\n");;
					cerr << "{ not found for end effector..." << endl;
					file.close();
					return false;
				}
				break;
			case 7:
				str = strtok(line, " \t");
				if (str != NULL && strncmp(str, "OFFSET", strlen("OFFSET")) == 0)
				{
					double x = 0; double y = 0; double z = 0;
					str = strtok(NULL, " \t");
					x = atof(str);
					str = strtok(NULL, " \t");
					y = atof(str);
					str = strtok(NULL, " \t");
					z = atof(str);
					cur->endEffectorOffset(SrVec(float(x) * scale, float(y)* scale, float(z) * scale));
					//cur->setEndEffector(true);
					//LOG("Found end effector at %s", cur->getName());
					//cout << "Found end effector offset of " << x << " " << y << " " << z << " " << endl;
					state = 8;
				}
				else
				{
					LOG("End effector OFFSET not found...\n");;
					file.close();
					return false;
				}
				break;
			case 8: // looking for '}' to finish the  end effector
				str = strtok(line, " \t");
				if (str != NULL && strncmp(str, "}", 1) == 0)
				{
					state = 5;
				}
				else
				{
					LOG("} not found for end effector...\n");;
					file.close();
					return false;
				}
				break;
			case 9: // found 'MOTION', looking for 'Frames'
				str = strtok(line, ":");
				if (str != NULL && strncmp(str, "Frames", strlen("Frames")) == 0)
				{
					str = strtok(NULL, " \t");
					numFrames = atoi(str);
					//LOG("Found %d frames of animation...\n", numFrames);
					state = 10;
				}
				else
				{
					LOG("Frames: not found...\n");;
					file.close();
					return false;
				}
				break;
			case 10: // found 'Frames', looking for 'Frame time:'
				str = strtok(line, ":");
				if (str != NULL && strncmp(str, "Frame Time", strlen("Frame Time")) == 0)
				{
					str = strtok(NULL, " \t");
					frameTime = atof(str);
					//LOG("Frame time is %f...\n", frameTime);
					//curFrame = 0;
					state = 11;

					// set up the bvhIndex map
					const std::vector<SkJoint*>& allJoints = skeleton.joints();
					for (size_t j = 0; j < allJoints.size(); j++)
					{
						ChannelInfo* channelInfo = channelInfoMap[j];
						for (int n = 0; n < channelInfo->numBVHChannels; n++)
							bvhIndex.push_back(std::pair<int, int>(j, n));
					}
					// set up the size of the posture based on the SmartBody skeleton
					//skChannels.compress();
					postureSize = skChannels.floats();

					/*
					for (int m = 0; m < motionChannels.size(); m++)
					{
						std::string name = motionChannels.name(m);
						std::string n = name;
						int y = 0;
					}
					*/
					motion.init(motionChannels);

					/*for (int m = 0; m < motion.channels().size(); m++)
					{
						std::string name = motion.channels().name(m);
						std::string n = name;
						int y = 0;
					}
					*/
				}
				else
				{
					LOG("Frame Time: not found...\n");;
					file.close();
					return false;
				}
				break;
			case 11: // parsing 
				// TODO:
				curFrame++;
				if( (curFrame <= N2) && (curFrame >= N1))
				{
					if (curFrame < numFrames)
					{
						motion.insert_frame(curFrame, float(frameTime) * float(curFrame));

						float* posture = motion.posture(curFrame);
						
						int index = 0;
						str = strtok(line, " \t");
						SkJoint* oldJoint = NULL;
						double frames[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
						// clean up any line feeds or carriage returns
						while (str != NULL && str[0] != 13)
						{
							double val = atof(str);
							int channelNum;
							
							if ((size_t)index >= bvhIndex.size())
							{
								LOG("Data on frame %d at position %d exceeds channel size of %d.", curFrame, index, bvhIndex.size());
								break;
							}

							std::pair<int, int>& pair = bvhIndex[index];
							SkJoint* j = skeleton.joints()[pair.first];
							channelNum = pair.second;
							if (j != oldJoint)
							{
								// add the values to the old joint
								if (oldJoint != NULL)
								{
									if (0)//oldJoint->isIgnoreChannels())
									{
										// shift the frame values down by three
										for (int i = 0; i < 3; i++)
										{
											frames[i] = frames[i + 3];
										}
										index -= 3;
									}
									// TODO: Add data to SkMotion
									ChannelInfo* channelInfo = channelInfoMap[oldJoint->index()];
									ParserBVH::convertBVHtoSmartBody(oldJoint, motion, channelInfo, frames, posture, curFrame * frameTime, scale);
								}

								for (int x = 0; x < 6; x++)
									frames[x] = 0.0;
								oldJoint = j;
							}

							frames[channelNum] = val;
							if (j != NULL)
							{
								// TODO: set the frame timing
								//j->setFrameTime(frameTime);
							}
							else
							{
								LOG("No joint found for index %d and channel #%d on frame %d...\n", index, channelNum, curFrame);
							}
							index++;
						
							str = strtok(NULL, " \t");
						}

						// flush any values to the old joint
						if (oldJoint != NULL)
						{
							// convert the BVH frame into the appropriate SmartBody frame
							ChannelInfo* channelInfo = channelInfoMap[oldJoint->index()];
							ParserBVH::convertBVHtoSmartBody(oldJoint, motion, channelInfo, frames, posture, curFrame * frameTime, scale);
						}
						state = 11;
					}
					else
					{
						state = 12;
					}
				}

				break;
			case 12: 
				state = 50;
				break;
			
			case 50:
				break;
			
			default:
				LOG("State %d not expected.");
				file.close();
				return false;
		}
	}
	//LOG("Finished parsing motion with %d frames...", numFrames);
	file.close();
	//skeleton.recalculateJointList();
	//skeleton.calculateMatrices(0);	
	// by default, bvh files do not contain any sync points.
	// create some defaults
	{
		double duration = double(motion.duration());
		if (duration > 1)
			motion.synch_points.set_time(0.0, 0.2, duration / 2.0, duration / 2.0, duration / 2.0, duration - 0.2, duration);
		else
			motion.synch_points.set_time(0.0, duration / 3.0, duration / 2.0, duration / 2.0, duration / 2.0, duration * 2.0 / 3.0, duration);
		motion.compress();
	}
	return true;
}

void ParserBVH::convertBVHtoSmartBody(SkJoint* joint, SkMotion& motion, ChannelInfo* channelInfo, double data[6], float* posture, double frameTime, float scale)
{
	SrVec rot(0.0f, 0.0f, 0.0f);

	int skChannelOffset = 0;
	bool hasRotation = false;

	for (int c = 0; c < channelInfo->numBVHChannels; c++)
	{
		if (channelInfo->channels[c] == BVHXPOSITION ||
			channelInfo->channels[c] == BVHYPOSITION ||
			channelInfo->channels[c] == BVHZPOSITION)
		{
			if (motion.posture_size() <= channelInfo->startingChannelIndex + skChannelOffset)
				std::cout << "WARNING!" << std::endl;
			if (!joint->parent())
			{ 
				float finalValue = float(data[c]) * scale;
				if (channelInfo->channels[c] == BVHXPOSITION)
					finalValue -= joint->offset().x;
				else if (channelInfo->channels[c] == BVHYPOSITION)
					finalValue -= joint->offset().y;
				else if (channelInfo->channels[c] == BVHZPOSITION)
					finalValue -= joint->offset().z;
				posture[channelInfo->startingChannelIndex + skChannelOffset] = finalValue * scale;
			}
			else
			{
				posture[channelInfo->startingChannelIndex + skChannelOffset] = 0.0;
			}
			skChannelOffset++;
		}
		else if (channelInfo->channels[c] == BVHXROTATION)
		{
			rot.x = float(data[c]) * float(M_PI) / 180.0f;
			hasRotation = true;
		}
		else if (channelInfo->channels[c] == BVHYROTATION)
		{
			rot.y = float(data[c]) * float(M_PI) / 180.0f;
			hasRotation = true;
		}
		else if (channelInfo->channels[c] == BVHZROTATION)
		{
			rot.z = float(data[c]) * float(M_PI) / 180.0f;
			hasRotation = true;
		}
	}
	if (hasRotation)
	{
		SrMat xrot;
		xrot.rotx(rot.x);
		SrMat yrot;
		yrot.roty(rot.y);
		SrMat zrot;
		zrot.rotz(rot.z);

		SrMat matrix;
		if (channelInfo->order == ROTXYZ)
		{
			matrix = zrot * yrot * xrot;
		}
		else if (channelInfo->order == ROTXZY)
		{
			matrix = yrot * zrot * xrot;
		}
		else if (channelInfo->order == ROTYXZ)
		{
			matrix = zrot * xrot * yrot;
		}
		else if (channelInfo->order == ROTYZX)
		{
			matrix = xrot * zrot * yrot;
		}
		else if (channelInfo->order == ROTZXY)
		{
			matrix = yrot * xrot * zrot;
		}
		else if (channelInfo->order == ROTZYX)
		{
			matrix = xrot * yrot * zrot;
		}

		SrQuat quat(matrix);

		if (motion.posture_size() <= channelInfo->startingChannelIndex + skChannelOffset + 3)
			LOG("WARNING! CHANNEL ASSIGNMENT AS POSITION %d EXCEEDS CHANNEL SIZE %d. EXPECT CRASH ON DEALLOCATION OF SKMOTION.",  (channelInfo->startingChannelIndex + skChannelOffset + 3), motion.posture_size());

		posture[channelInfo->startingChannelIndex + skChannelOffset] = quat.w;
		posture[channelInfo->startingChannelIndex + skChannelOffset + 1] = quat.x;
		posture[channelInfo->startingChannelIndex + skChannelOffset + 2] = quat.y;
		posture[channelInfo->startingChannelIndex + skChannelOffset + 3] = quat.z;
		skChannelOffset += 3;
	}
}

int ParserBVH::determineRotationOrder(int bvhChannels[6], int numBVHChannels)
{
	int index = 0;
	if (numBVHChannels > 3)
		index = 3;

	int order = -1;

	if (bvhChannels[index] == BVHXROTATION)
	{
		if (numBVHChannels > index + 1)
		{
			if (bvhChannels[index + 1] == BVHYROTATION)
			{
				order = ROTXYZ;
			}
			else
			{
				order = ROTXZY;
			}
		}
		else
		{
			order = ROTXYZ;
		}
	}
	else if (bvhChannels[index] == BVHYROTATION)
	{
		if (numBVHChannels > index + 1)
		{
			if (bvhChannels[index + 1] == BVHXROTATION)
			{
				order = ROTYXZ;
			}
			else
			{
				order = ROTYZX;
			}
		}
		else
		{
			order = ROTYXZ;
		}
	}
	else if (bvhChannels[index] == BVHZROTATION)
	{
		if (numBVHChannels > index + 1)
		{
			if (bvhChannels[index + 1] == BVHXROTATION)
			{
				order = ROTZXY;
			}
			else
			{
				order = ROTZYX;
			}
		}
		else
		{
			order = ROTZXY;
		}
	}

	if (order == -1)
	{
		order = ROTXYZ;	
	}

	return order;
}
