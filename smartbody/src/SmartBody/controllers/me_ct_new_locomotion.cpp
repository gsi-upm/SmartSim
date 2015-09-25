/*
 *  me_ct_new_locomotion.cpp - part of Motion Engine and SmartBody-lib
 *  Copyright (C) 2011  University of Southern California
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
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Yuyu Xu, Alain Juarez Perez ICT USC
 */
#include <assert.h>
#include <boost/foreach.hpp>
#include "controllers/me_ct_new_locomotion.h"
#include <controllers/me_ct_param_animation_utilities.h>
#include <controllers/me_ct_scheduler2.h>
#include <sb/SBScene.h>
#include <sbm/gwiz_math.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBRetarget.h>


std::string MeCtNewLocomotion::_type_name = "NewLocomotion";

MeCtNewLocomotion::MeCtNewLocomotion() :  SmartBody::SBController()
{
	scootSpd = 0.0f;	//	unit: centermeter/sec
	movingSpd = 0.0f;	//	unit: centermeter/sec
	turningSpd = 0.0f;	//	unit: deg/sec
	motionSpd = 0.0f;
	_valid = false;
	_analysis = false;
	_lastTime = -1.0f;
	_prevTime = -1.0f;
	startTime = -1.0;
	dataCycle = smoothCycle = NULL;
	LeftFading.prev_time = -1.0f;
	RightFading.prev_time = -1.0f;
	_duration = -1.0f;
	useIKRt = false;
	useIKLf = false;
	sk = NULL;
	walkScale = 1.0f;
	walkSpeedGain = 1.0f;
	motionSpeed = 0.0f;

	setDefaultAttributeGroupPriority("EnhancedLocomotion", 600);
	addDefaultAttributeString("walkCycle", "", "EnhancedLocomotion");
	addDefaultAttributeString("walkSkeleton", "", "EnhancedLocomotion");
	addDefaultAttributeDouble("walkScale", 1.0, "EnhancedLocomotion");
	addDefaultAttributeDouble("walkSpeedGain", 1.0, "EnhancedLocomotion");
	addDefaultAttributeString("LEndEffectorJoint", "l_forefoot", "EnhancedLocomotion");
	addDefaultAttributeString("REndEffectorJoint", "r_forefoot", "EnhancedLocomotion");
	addDefaultAttributeString("CenterHipJoint", "base", "EnhancedLocomotion");
	addDefaultAttributeDouble("FadeIn", 0.2, "EnhancedLocomotion");
	addDefaultAttributeDouble("FadeOut", 0.4, "EnhancedLocomotion");
	addDefaultAttributeDouble("TurningRate", 15.0, "EnhancedLocomotion");
	addDefaultAttributeString("footPlantRight", "", "EnhancedLocomotion");
	addDefaultAttributeString("footPlantLeft", "", "EnhancedLocomotion");
	addDefaultAttributeBool("startsWithRight", true, "EnhancedLocomotion");
}

MeCtNewLocomotion::~MeCtNewLocomotion()
{
}

void MeCtNewLocomotion::init(SbmCharacter* sbChar)
{
	character = sbChar;

	attributes_names.push_back("walkCycle");
	attributes_names.push_back("walkSkeleton");
	attributes_names.push_back("walkScale");
	attributes_names.push_back("walkSpeedGain");
	attributes_names.push_back("CenterHipJoint");
	attributes_names.push_back("LEndEffectorJoint");
	attributes_names.push_back("REndEffectorJoint");
	attributes_names.push_back("FadeIn");
	attributes_names.push_back("FadeOut");
	attributes_names.push_back("TurningRate");
	attributes_names.push_back("footPlantRight");
	attributes_names.push_back("footPlantLeft");
	attributes_names.push_back("startsWithRight");
	for(unsigned int i = 0; i< attributes_names.size(); i++)
	{
		SmartBody::SBAttribute* a = character->getAttribute(attributes_names[i]);
		if (a)
			a->registerObserver(this);
	}
	tempBuffer.size(1000);
	setup();
}

void MeCtNewLocomotion::setup()
{
	if (character->getStringAttribute("walkCycle").empty() || character->getStringAttribute("walkSkeleton").empty() || 
		character->getStringAttribute("footPlantRight").empty() || character->getStringAttribute("footPlantLeft").empty() )
		return;

	bool SameMotion=false;
	if(dataCycle)//If there is a motion, check if is the same one
	{	
		SameMotion = dataCycle->getName() == character->getStringAttribute("walkCycle");
	}
	if(!SameMotion)
	{
		dataCycle  = SmartBody::SBScene::getScene()->getMotion(character->getStringAttribute("walkCycle"));
		if (!dataCycle)
		return;
		dataCycle->setName(character->getStringAttribute("walkCycle"));
	}
	if (!dataCycle)
		return;
	hipjoint = character->getStringAttribute("CenterHipJoint");
	lend = character->getStringAttribute("LEndEffectorJoint");
	rend = character->getStringAttribute("REndEffectorJoint");
	fadein  = (float)character->getDoubleAttribute("FadeIn");
	fadeout = (float)character->getDoubleAttribute("FadeOut");

	std::string skeletonName = character->getStringAttribute("walkSkeleton");
	dataCycle->setMotionSkeletonName(skeletonName);

	sk = new SmartBody::SBSkeleton(character->getSkeleton());

	if (!sk)
		return;

	bool isNewWalkScale = false;
	float scale = (float) character->getDoubleAttribute("walkScale");
	if (fabs(walkScale - scale) > .0001)
		isNewWalkScale = true;
	walkScale = scale;

	bool isNewWalkSpeedGain = false;
	float speedGain = (float) character->getDoubleAttribute("walkSpeedGain");
	if (fabs(walkSpeedGain - speedGain) > .0001)
		isNewWalkSpeedGain = true;
	walkSpeedGain = speedGain;


	if(!SameMotion)
	{
		if(!character->getBoolAttribute("startsWithRight"))
		{
			dataCycle = dataCycle->mirror("", skeletonName);
			dataCycle->setName(character->getStringAttribute("walkCycle"));
			dataCycle->setMotionSkeletonName(skeletonName);
		}
		smoothCycle = dataCycle->smoothCycle("", 0.5f);
		smoothCycle->setMotionSkeletonName(skeletonName);
		smoothCycle->connect(sk);
		motionSpeed = smoothCycle->getJointSpeed(sk->getJointByName(hipjoint), (float)smoothCycle->getTimeStart() , (float)smoothCycle->getTimeStop());
		smoothCycle->disconnect();
	}
	if(!smoothCycle)
		return;
	if( isNewWalkScale || isNewWalkSpeedGain || motionSpd == 0.0f)
	{
		motionSpd = walkScale * motionSpeed * walkSpeedGain ;
	}
	if(!SameMotion)
	{
		_lastTime = -1.0;
		SmartBody::SBJoint* rootJoint = character->getSkeleton()->getJointByName(hipjoint);
		ik_scenario.ikTreeNodes.clear();
		std::vector<std::string> stopJoints;
		stopJoints.push_back(lend);
		stopJoints.push_back(rend);
		stopJoints.push_back("spine1");
		ik_scenario.buildIKTreeFromJointRoot(rootJoint,stopJoints);	

		double ikReachRegion = character->getHeight()*0.02f;		
		ikDamp = ikReachRegion*ikReachRegion*14.0;
		MeController::init(character);
		ik.dampJ = ikDamp;
		ik.refDampRatio = 0.01;

		std::string rightPlants = character->getStringAttribute("footPlantRight");
		std::vector<std::string> rPlantVector;
		vhcl::Tokenize(rightPlants, rPlantVector, ",");
		rplant.clear();
		rplant.resize(rPlantVector.size());
		for (size_t p = 0; p < rPlantVector.size(); p++)
		{
			rplant[p] = atoi(rPlantVector[p].c_str());
		}

		std::string leftPlants = character->getStringAttribute("footPlantLeft");
		std::vector<std::string> lPlantVector;
		vhcl::Tokenize(leftPlants, lPlantVector, ",");
		lplant.clear();
		lplant.resize(lPlantVector.size());
		for (size_t p = 0; p < lPlantVector.size(); p++)
		{
			lplant[p] = atoi(lPlantVector[p].c_str());
		}
	}
	
}

bool MeCtNewLocomotion::controller_evaluate(double t, MeFrameData& frame)
{
	if (!smoothCycle)
		return true;
	BufferRef=&(frame.buffer());
	if (character && _valid)
	{
		play((float)t);
	}
	return true;
}

float MeCtNewLocomotion::legDistance(bool Leftleg)
{
	SmartBody::SBSkeleton *sk2 = character->getSkeleton();
	sk2->update_global_matrices();
	std::string jointName;
	jointName = (Leftleg)? "l_hip" : "r_hip";
	SrVec goal;
	if(posConsLf.count(lend) && posConsRt.count(rend))
		goal = (Leftleg)? posConsLf[lend]->getPosConstraint() : posConsRt[rend]->getPosConstraint();
	SmartBody::SBJoint* joint = sk2->getJointByName(jointName);	
	SrVec jointPosition = joint->gmat().get_translation();
	return (jointPosition - goal).len()-0.9f;
}

void MeCtNewLocomotion::loopMotion(float def, float speed)
{
	_analysis = true;
	static bool printOnce=true;
	char pawnname[20];
	sprintf(pawnname, "pawn%.1f%.2f", def,speed);
	startTime = 0.0f;
	float diff = 1.0f/30.0f;
	setDesiredHeading(0.0f);
	float temp = motionSpd;
	motionSpd = speed;
	def = def/(float)smoothCycle->frames();
	for(float t = 0; t < smoothCycle->duration() + 0.01; t += diff)
	{
		setDesiredHeading(getDesiredHeading()+def);
		play(t, true);
	}
	printOnce=false;
	float x, y, z, yaw, pitch, roll;		
 	character->get_world_offset(x, y, z, yaw, pitch, roll); 
	SrVec pos(x,y,z);
	addPawn(pos,pawnname);
	reset(true);
	motionSpd = temp;
	_analysis = false;
}

void MeCtNewLocomotion::play(float t, bool useTemp)
{
	if(useTemp)
		BufferRef = &tempBuffer;
	float dt = 1.0f / 60.0f;
	if (_lastTime > 0.0)
		dt = float(t - _lastTime);
	dt *= walkSpeedGain;
	_lastTime = t;
	if(_prevTime > 0.0f)
		t = _prevTime + dt;
	else
		_prevTime = 0.0f;
	_prevTime = t;
	if(startTime < 0.0)
		startTime = t;
	motionTime = fmod(t-startTime, smoothCycle->duration());
	float x, y, z, yaw, pitch, roll;		
 	character->get_world_offset(x, y, z, yaw, pitch, roll); 
 	yaw = desiredHeading;
 	float movingDist = motionSpd * dt;
 	x += movingDist * sin(yaw * (float)M_PI / 180.0f);
 	z += movingDist * cos(yaw * (float)M_PI / 180.0f);

	gwiz::quat_t q = gwiz::euler_t(pitch,yaw,roll);
	SrQuat woQuat = SrQuat(q.wf() ,q.xf(),q.yf(),q.zf());
	SrVec woPos = SrVec(x,y,z); 		
	//*/
	std::vector<SrQuat> tempQuatList(ik_scenario.ikTreeNodes.size()); 
	if (fabs(LeftFading.prev_time + RightFading.prev_time + 2.0f) <0.001f ) // first start
	{					
		updateChannelBuffer(*BufferRef, motionTime);//Read from skeleton->Write on Buffer
		updateWorldOffset(*BufferRef, woQuat, woPos);
		updateChannelBuffer(*BufferRef,tempQuatList, true);//Read from Buffer->Write on tempQuatList
		
		ik_scenario.setTreeNodeQuat(tempQuatList,QUAT_INIT);
		ik_scenario.setTreeNodeQuat(tempQuatList,QUAT_PREVREF);
		ik_scenario.setTreeNodeQuat(tempQuatList,QUAT_CUR);
	}

	LeftFading.updateDt(t);
	RightFading.updateDt(t);

	updateChannelBuffer(*BufferRef, motionTime);//Read from skeleton->Write on Buffer
	updateWorldOffset(*BufferRef, woQuat, woPos);
	updateChannelBuffer(*BufferRef,tempQuatList, true);//Read from Buffer->Write on tempQuatList

	updateConstraints(motionTime/(float)smoothCycle->getFrameRate());
		
	ik_scenario.setTreeNodeQuat(tempQuatList,QUAT_REF);	
	character->getSkeleton()->update_global_matrices();
	ik_scenario.ikGlobalMat = character->getSkeleton()->getJointByName(hipjoint)->gmat();
	
	for (int i=0;i<3;i++)
		ik_scenario.ikTreeRootPos[i] = character->getSkeleton()->getJointByName(hipjoint)->pos()->value(i);
	//*/Character pose
	ik.setDt(LeftFading.dt);
	if (LeftFading.fadeMode == Fading::FADING_MODE_IN)
		useIKLf = true;
	if (RightFading.fadeMode == Fading::FADING_MODE_IN)
		useIKRt = true;
	if (LeftFading.updateFading(LeftFading.dt))
		useIKLf = false;
	if( RightFading.updateFading(RightFading.dt))
		useIKRt = false;

	if(useIKLf && _analysis && LeftFading.fadeMode == Fading::FADING_MODE_OFF)
	{
		errorSum += max(legDistance(1), 0.0f);
	}
	if(useIKRt && _analysis && RightFading.fadeMode == Fading::FADING_MODE_OFF)
	{
		errorSum += max(legDistance(0), 0.0f);
	}
	//*/Left
	ik_scenario.ikPosEffectors = &posConsLf;
	ik_scenario.ikRotEffectors = &rotConsLf;
	ik.update(&ik_scenario);
	ik_scenario.copyTreeNodeQuat(QUAT_CUR,QUAT_INIT);			
	for (unsigned int i = 0; i < ik_scenario.ikTreeNodes.size(); i++)
	{
		MeCtIKTreeNode* node = ik_scenario.ikTreeNodes[i];
		SrQuat qEval = node->getQuat(QUAT_CUR);
		SrQuat qInit = node->getQuat(QUAT_REF);
		qEval.normalize();
		qInit.normalize();
		char a=node->getNodeName().c_str()[0];
		if(a!='l')
			continue;
		float tempdist = d(qInit,qEval);
		//if(tempdist>0.7f)
			//LOG("%s  :  %f", node->getNodeName().c_str(), tempdist);
		tempQuatList[i] = slerp(qInit,qEval,LeftFading.blendWeight);
	}
	//*/
	//*/Right
	ik_scenario.ikPosEffectors = &posConsRt;
	ik_scenario.ikRotEffectors = &rotConsRt;
	ik.update(&ik_scenario);
	ik_scenario.copyTreeNodeQuat(QUAT_CUR,QUAT_INIT);	
	for (unsigned int i = 0; i < ik_scenario.ikTreeNodes.size(); i++)
	{
		MeCtIKTreeNode* node = ik_scenario.ikTreeNodes[i];
		SrQuat qEval = node->getQuat(QUAT_CUR);
		SrQuat qInit = node->getQuat(QUAT_REF);
		qEval.normalize();
		qInit.normalize();
		char a=node->getNodeName().c_str()[0];
		if(a!='r')
			continue;
		float tempdist = d(qInit,qEval);
		//if(tempdist>0.7f)
		//{
			//LOG("%s  :  %f", node->getNodeName().c_str(), tempdist);
		//}
		tempQuatList[i] = slerp(qInit,qEval,RightFading.blendWeight);
	}
	//*/
	updateChannelBuffer(*BufferRef,tempQuatList);//Read from tempQuatList->Write on Buffer
	updateWorldOffset(*BufferRef, woQuat, woPos);	
	character->set_world_offset(x, y, z, yaw, pitch, roll); 
	RightFading.prev_time=LeftFading.prev_time = t;
	if(_analysis)
	{
		writeToSkeleton(*BufferRef);
		check_collision();
	}
}

void MeCtNewLocomotion::updateChannelBuffer(SrBuffer<float>& buffer, std::vector<SrQuat>& quatList, bool bRead)
{
	int count = 0;	
	const IKTreeNodeList& nodeList = ik_scenario.ikTreeNodes;
	BOOST_FOREACH(SrQuat& quat, quatList)
	{
		int chanId;
		int index;
		SkJoint* joint = nodeList[count++]->joint;
		chanId = _context->channels().search(joint->getMappedJointName(), SkChannel::Quat);
		if (chanId < 0)
			continue;

		index = _context->toBufferIndex(chanId);
		if (index < 0 )
		{
			if (bRead)
			{
				quat = SrQuat();
			}
		}
		else
		{
			if (bRead)
			{
				quat.w = buffer[index] ;
				quat.x = buffer[index + 1] ;
				quat.y = buffer[index + 2] ;
				quat.z = buffer[index + 3] ;			
			}
			else
			{
				buffer[index] = quat.w;
				buffer[index + 1] = quat.x;
				buffer[index + 2] = quat.y;
				buffer[index + 3] = quat.z;
			}
		}			
	}
}

void MeCtNewLocomotion::updateChannelBuffer(SrBuffer<float>& buffer, float t)
{
	SmartBody::SBRetargetManager* retargetManager = SmartBody::SBScene::getScene()->getRetargetManager();
	SmartBody::SBRetarget* retarget =retargetManager->getRetarget(smoothCycle->getMotionSkeletonName(),character->getSkeleton()->getName());
	smoothCycle->connect(sk);
	smoothCycle->apply(t);//,SkMotion::Linear, 0, retarget);
	for(int i = 0; i < sk->getNumJoints(); i++)
	{
			SmartBody::SBJoint* joint = sk->getJoint(i);	
			int chanId = _context->channels().search(joint->getMappedJointName(), SkChannel::Quat);
			if (chanId < 0)
				continue;
			int index = _context->toBufferIndex(chanId);
			SrQuat quat = joint->quat()->rawValue();
			SrQuat retargetQ =  quat; 
			if (retarget)
				retargetQ = retarget->applyRetargetJointRotation(joint->getMappedJointName(),quat);			
			if(index<0)
				continue;
			buffer[index + 0] = retargetQ.w;
			buffer[index + 1] = retargetQ.x;
			buffer[index + 2] = retargetQ.y;
			buffer[index + 3] = retargetQ.z;
	}
	smoothCycle->disconnect();
}


void MeCtNewLocomotion::writeToSkeleton(SrBuffer<float>& buffer)
{
	for(int i = 0; i < sk->getNumJoints(); i++)
	{
			SmartBody::SBJoint* joint = sk->getJoint(i);	
			int chanId = _context->channels().search(joint->getMappedJointName(), SkChannel::Quat);
			if (chanId < 0)
				continue;
			int index = _context->toBufferIndex(chanId);
			if(index<0)
				continue;
			SrQuat temp(buffer[index + 0], buffer[index + 1], buffer[index + 2], buffer[index + 3] );
			joint->quat()->value(temp);
	}
}

bool MeCtNewLocomotion::addEffectorJointPair( const char* effectorName, const char* effectorRootName, const SrVec& pos, const SrQuat& rot, 
	                                          ConstraintType cType, ConstraintMap& posCons, ConstraintMap& rotCons)
{
	MeCtIKTreeNode* node = ik_scenario.findIKTreeNode(effectorName);
	MeCtIKTreeNode* rootNode = ik_scenario.findIKTreeNode(effectorRootName);	
	if (!node)
		return false;

	std::string rootName = effectorRootName;

	if (!rootNode)
		rootName = ik_scenario.ikTreeRoot->getNodeName();

	// separate position & rotation constraint
	ConstraintMap& jEffectorMap = (cType == CONSTRAINT_ROT) ? rotCons : posCons;

	std::string str = effectorName;

	ConstraintMap::iterator ci = jEffectorMap.find(str);
	if (ci != jEffectorMap.end())
	{
		EffectorConstantConstraint* cons = dynamic_cast<EffectorConstantConstraint*>((*ci).second);
		cons->rootName = rootName;
		cons->targetPos = pos;
		cons->targetRot = rot;
		
	}
	else // add effector-joint pair
	{
		// initialize constraint
		EffectorConstantConstraint* cons = new EffectorConstantConstraint();
		cons->efffectorName = effectorName;
		cons->rootName = rootName;
		cons->targetPos = pos;
		cons->targetRot = rot;
		jEffectorMap[str] = cons;
	}
	return true;
}
void MeCtNewLocomotion::controller_map_updated() 
{		
	
}

void MeCtNewLocomotion::controller_start()
{
	LeftFading.controlRestart();
	RightFading.controlRestart();
}

void MeCtNewLocomotion::addPawn(SrVec& pos, std::string name)
{
	SmartBody::SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn(name);
	float maxerror = 0.30f;
	if(pawn == NULL)
		pawn = SmartBody::SBScene::getScene()->createPawn(name);
	pawn->setStringAttribute("collisionShape","sphere");
	pawn->setVec3Attribute("collisionShapeScale",0.025f,0.025f,0.025f);
	if(errorSum/maxerror<1.0f )
	{
		//pos.y= max(1.0f - errorSum/maxerror,0.0f);
		pawn->setVec3Attribute("color",0.0f,max(1.0f - errorSum/maxerror,0.0f),min(errorSum/maxerror, 1.0f));
	}
	else if (errorSum > 99.0f)
	{
		pawn->setVec3Attribute("color",1.0f,0.0f,0.0f);
	}
	else
	{
		pawn->setVec3Attribute("color",1.0f,1.0f,0.0f);
	}
	pawn->setPosition(pos);
}

void MeCtNewLocomotion::updateConstraints(float t)
{
	if(useIKRt)
	{
		if(t > rplant[1] && t < rplant[2] && RightFading.fadeMode == Fading::FADING_MODE_OFF )
		{
			RightFading.setFadeOut(fadeout);
		}
	}
	else if(((t > rplant[2] && t < rplant[3] )|| t > rplant[0] && t < rplant[1])&& RightFading.fadeMode == Fading::FADING_MODE_OFF)
	{	
		SmartBody::SBSkeleton *sk2 = character->getSkeleton();
		sk2->update_global_matrices();
		SmartBody::SBJoint* rootJoint = sk2->getJointByName(hipjoint);
		ik_scenario.ikGlobalMat = rootJoint->parent()->gmat();		
		SmartBody::SBJoint* joint = sk2->getJointByName(rend);	
		SrVec tv = joint->gmat().get_translation();
		SrQuat tq = SrQuat(joint->gmat());
		ConstraintType cType = CONSTRAINT_ROT;
		addEffectorJointPair(rend.c_str(), hipjoint.c_str(), tv, tq, cType, posConsRt, rotConsRt);
		cType = CONSTRAINT_POS;
		addEffectorJointPair(rend.c_str(), hipjoint.c_str(), tv, tq, cType, posConsRt, rotConsRt);
		RightFading.setFadeIn(fadein);
	}
	if(useIKLf)
	{
		if(t > lplant[1] && LeftFading.fadeMode == Fading::FADING_MODE_OFF )
		{
			LeftFading.setFadeOut(fadeout);
		}
	}
	else if(t > lplant[0] && t < lplant[1] && LeftFading.fadeMode == Fading::FADING_MODE_OFF)
	{		
		SmartBody::SBSkeleton *sk2 = character->getSkeleton();
		sk2->update_global_matrices();
		SmartBody::SBJoint* rootJoint = sk2->getJointByName(hipjoint);
		ik_scenario.ikGlobalMat = rootJoint->parent()->gmat();		
		SmartBody::SBJoint* joint = sk2->getJointByName(lend);	
		SrVec tv = joint->gmat().get_translation();
		SrQuat tq = SrQuat(joint->gmat());
		ConstraintType cType = CONSTRAINT_ROT;
		addEffectorJointPair(lend.c_str(), hipjoint.c_str(), tv, tq, cType, posConsLf, rotConsLf);
		cType = CONSTRAINT_POS;
		addEffectorJointPair(lend.c_str(), hipjoint.c_str(), tv, tq, cType, posConsLf, rotConsLf);
		LeftFading.setFadeIn(fadein);
	}
}

void MeCtNewLocomotion::updateWorldOffset(SrBuffer<float>& buffer, SrQuat& rot, SrVec& pos )
{	
	JointChannelId baseChanID, baseBuffId;
	baseChanID.x = _context->channels().search(SbmPawn::WORLD_OFFSET_JOINT_NAME, SkChannel::XPos);
	baseChanID.y = _context->channels().search(SbmPawn::WORLD_OFFSET_JOINT_NAME, SkChannel::YPos);
	baseChanID.z = _context->channels().search(SbmPawn::WORLD_OFFSET_JOINT_NAME, SkChannel::ZPos);
	baseChanID.q = _context->channels().search(SbmPawn::WORLD_OFFSET_JOINT_NAME, SkChannel::Quat);

	baseBuffId.x = _context->toBufferIndex(baseChanID.x);
	baseBuffId.y = _context->toBufferIndex(baseChanID.y);
	baseBuffId.z = _context->toBufferIndex(baseChanID.z);	
	baseBuffId.q = _context->toBufferIndex(baseChanID.q);	

	buffer[baseBuffId.x] = pos[0];
	buffer[baseBuffId.y] = pos[1];
	buffer[baseBuffId.z] = pos[2];	
	for (int k = 0; k < 4; k++)
		buffer[baseBuffId.q + k] = rot.getData(k);
}


void  MeCtNewLocomotion::notify(SmartBody::SBSubject* subject)
{
	SmartBody::SBAttribute* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		const std::string& name = attribute->getName();
		bool check_attributes=false;
		for(unsigned int i = 0; i< attributes_names.size(); i++)
			if (name == attributes_names[i])
				check_attributes=true;
		if(check_attributes)
			setup();
	}
}

void MeCtNewLocomotion::reset(bool resetPos)
{	
	scootSpd = 0.0f;
	movingSpd = 0.0f;
	turningSpd = 0.0f;
	_valid = false;
	startTime = -1.0f;
	_prevTime = -1.0f;
	_lastTime = -1.0f;
	errorSum = 0.0f; 
	useIKRt = false;
	useIKLf = false;
	posConsRt.clear();
	rotConsRt.clear();
	posConsLf.clear();
	rotConsLf.clear();
	LeftFading.controlRestart();
	RightFading.controlRestart();
	if(resetPos)
	{
		character->set_world_offset(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		setDesiredHeading(0.0f);
	}
}

bool MeCtNewLocomotion::capsule_collision(SrVec  mp1, SrVec mp2, SrVec np1, SrVec np2)
{
    SrVec   u = mp2 - mp1;
    SrVec   v = np2 - np1;
    SrVec   w = mp1 - np1;
    float    a = dot(u,u);         // always >= 0
    float    b = dot(u,v);
    float    c = dot(v,v);         // always >= 0
    float    d = dot(u,w);
    float    e = dot(v,w);
    float    D = a*c - b*b;        // always >= 0
    float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
    float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

    // compute the line parameters of the two closest points
    if (D < 0.0001f) { // the lines are almost parallel
        sN = 0.0;         // force using point P0 on segment S1
        sD = 1.0;         // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                 // get the closest points on the infinite lines
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
            sN = 0.0;
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
        tN = 0.0;
        // recompute sc for this edge
        if (-d < 0.0)
            sN = 0.0;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    }
    else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
        tN = tD;
        // recompute sc for this edge
        if ((-d + b) < 0.0)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d +  b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (fabs(sN) < 0.0001f ? 0.0f : sN / sD);
    tc = (fabs(tN) < 0.0001f ? 0.0f : tN / tD);

    // get the difference of the two closest points
    SrVec   dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)
    return dP.norm()< 0.175f;
}

void MeCtNewLocomotion::check_collision()
 {
	sk->update_global_matrices();
	SmartBody::SBJoint* Rhip = sk->getJointByName("r_hip");
	SmartBody::SBJoint* Rknee = sk->getJointByName("r_knee");
	SmartBody::SBJoint* Rfoot = sk->getJointByName("r_ankle");
	SmartBody::SBJoint* Rend = sk->getJointByName("r_forefoot");
	SmartBody::SBJoint* Lhip = sk->getJointByName("l_hip");
	SmartBody::SBJoint* Lknee = sk->getJointByName("l_knee");
	SmartBody::SBJoint* Lfoot = sk->getJointByName("l_ankle");
	SmartBody::SBJoint* Lend = sk->getJointByName("l_forefoot");
    SrVec r[4]={Rhip->gmat().get_translation(), Rknee->gmat().get_translation(), Rfoot->gmat().get_translation(), Rend->gmat().get_translation()};
    SrVec l[4]={Lhip->gmat().get_translation(), Lknee->gmat().get_translation(), Lfoot->gmat().get_translation(), Lend->gmat().get_translation()};
    bool collide=false;
    for(int i=0;i<3;i++)
        for(int j=0; j<3; j++)
        {
            bool temp=capsule_collision(r[i], r[i+1], l[i], l[i+1]);
            collide=collide||temp;
        }
    if(collide)
	{
        errorSum+=100.0f;
	}
}
//------------------------------------------------------------------------------------------------------//
//-----------------------------------------FADING-------------------------------------------------------//
//------------------------------------------------------------------------------------------------------//
Fading::Fading()
{
	fadeMode = FADING_MODE_OFF;
	blendWeight = 0.0;
	prev_time = 0.0;
	restart = false;
}

bool Fading::updateFading( float dt )
{
	const float FADE_EPSILON = 0.001f;
	bool finishFadeOut = false;
	if (fadeMode)
	{
		fadeRemainTime -= dt;
		if (fadeRemainTime <= 0.0)
			fadeRemainTime = 0.0;

		if (fadeMode == FADING_MODE_IN)
		{			
			float fadeNormal = 1.f - (float)fadeRemainTime/fadeInterval;
			blendWeight = fadeNormal;
			if (blendWeight > 1.0 - FADE_EPSILON)
			{
				blendWeight = 1.0;
				fadeMode = FADING_MODE_OFF;
			}						
		}
		else
		{
			float fadeNormal = fadeRemainTime/fadeInterval;
			blendWeight = fadeNormal;
			if (blendWeight < FADE_EPSILON)
			{
				blendWeight = 0.0;
				fadeMode = FADING_MODE_OFF;
				finishFadeOut = true;
			}	
		}
	}
	return finishFadeOut;
}

void Fading::setFadeIn( float interval )
{
	if (blendWeight == 1.0 && fadeMode == FADING_MODE_OFF)
		return;

	fadeInterval = interval;
	fadeRemainTime = interval;
	fadeMode = FADING_MODE_IN;
}

void Fading::setFadeOut( float interval )
{
	if (blendWeight == 0.0 && fadeMode == FADING_MODE_OFF)
		return;

	fadeInterval = interval;
	fadeRemainTime = interval;
	fadeMode = FADING_MODE_OUT;
}

void Fading::controlRestart()
{
	fadeMode = FADING_MODE_OFF;
	blendWeight = 0.0;
	prev_time = 0.0;
	restart = true;
}

void Fading::updateDt( float curTime )
{
	if (restart)
	{
		dt = 0.f;
		restart = false;
	}
	else
	{
		dt = curTime - prev_time;
	}	
	prev_time = curTime;
}