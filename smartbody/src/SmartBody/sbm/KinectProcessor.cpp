#include "KinectProcessor.h"
#include <sb/SBSkeleton.h>
#include <sb/SBJoint.h>
#include <sb/SBScene.h>
#include <sb/SBAssetManager.h>
#include <sb/SBRetargetManager.h>
#include <sb/SBRetarget.h>

KinectProcessor::KinectProcessor()
{
	//Note: everything is based on below assumptions
	//20 to 24 bones, using kinect sdk standard, the joint order is also kinect sdk order as following
	//index		openNI					kinecSDK				SmartBody
	//0			XN_SKEL_WAIST			HipCenter				base
	//1			XN_SKEL_TORSO			Spine					spine2
	//2			XN_SKEL_NECK			ShoulderCenter			spine3
	//3			XN_SKEL_HEAD			Head					skullbase
	//4			XN_SKEL_LEFT_SHOULDER	ShoulderLeft			l_shoulder
	//5			XN_SKEL_LEFT_ELBOW		ElbowLeft				l_elbow
	//6			XN_SKEL_LEFT_WRIST		WristLeft				l_wrist	
	//7			XN_SKEL_LEFT_HAND		HandLeft				l_index1
	//8			XN_SKEL_RIGHT_SHOULDER	ShoulderRight			r_shoulder
	//9			XN_SKEL_RIGHT_ELBOW		ElbowRight				r_elbow
	//10		XN_SKEL_RIGHT_WRIST		WristRight				r_wrist
	//11		XN_SKEL_RIGHT_HAND		HandRight				r_index1
	//12		XN_SKEL_LEFT_HIP		HipLeft					l_hip		
	//13		XN_SKEL_LEFT_KNEE		KneeLeft				l_knee			
	//14		XN_SKEL_LEFT_ANKLE		AnkleLeft				l_ankle		
	//15		XN_SKEL_LEFT_FOOT		FootLeft				l_forefoot
	//16		XN_SKEL_RIGHT_HIP		HipRight				r_hip
	//17		XN_SKEL_RIGHT_KNEE		KneeRight				r_knee
	//18		XN_SKEL_RIGHT_ANKLE		AnkleRight				r_ankle
	//19		XN_SKEL_RIGHT_FOOT		FootRight				r_forefoot
	//20		XN_SKEL_LEFT_COLLAR								l_acromioclavicular
	//21		XN_SKEL_RIGHT_COLLAR							r_acromioclavicular
	//22		XN_SKEL_LEFT_FINGERTIP									
	//23		XN_SKEL_RIGHT_FINGERTIP							

	numOfKinectJoints = 20; // use only 20 joints
	// default mapping, can be user defined too
	boneMapping.push_back("base");			
	boneMapping.push_back("spine2");	// should it be spine2 or base
	boneMapping.push_back("spine3");	// should it be spine4?
	boneMapping.push_back("spine5");
	boneMapping.push_back("l_shoulder");
	boneMapping.push_back("l_elbow");
	boneMapping.push_back("l_wrist");
	boneMapping.push_back("l_index1");			// no mapping for hand
	boneMapping.push_back("r_shoulder");
	boneMapping.push_back("r_elbow");
	boneMapping.push_back("r_wrist");
	boneMapping.push_back("r_index1");			// no mapping for hand
	boneMapping.push_back("l_hip");
	boneMapping.push_back("l_knee");
	boneMapping.push_back("l_ankle");
	boneMapping.push_back("l_forefoot");
	boneMapping.push_back("r_hip");
	boneMapping.push_back("r_knee");
	boneMapping.push_back("r_ankle");
	boneMapping.push_back("r_forefoot");

	boneMapping.push_back("l_acromioclavicular");
	boneMapping.push_back("r_acromioclavicular");
	boneMapping.push_back("");
	boneMapping.push_back("");

	parentIndexMap.clear();
	for (unsigned int i=0;i<boneMapping.size();i++)
	{
		if (i == 4 || i==8)
			parentIndexMap[i] = 2;
		else if (i == 12 || i== 16)			
			parentIndexMap[i] = 0;
		else if (i >= 20)
			parentIndexMap[i] = 0;
		else
			parentIndexMap[i] = i-1;		
	}
	filterSize = 10;
	rotationBuffer.resize(24);
	for (int i = 0; i < 24; i++)
		rotationBuffer[i].resize(24);
}

void KinectProcessor::inferJointOffsets( std::vector<SrVec>& gPos, std::vector<SrQuat>& gRot, std::vector<SrVec>& out, std::map<int, SrQuat>& bonePreRotMap )
{
	out.resize(numOfKinectJoints);
	out.assign(numOfKinectJoints,SrVec(0,0,0)); // set to zero vector by default	
	for (int i=0;i<numOfKinectJoints;i++)
	{
		int pindex = parentIndexMap[i];
		if (pindex >= 0)
		{
			SrVec offset = gPos[i] - gPos[pindex]; // segment offset			
			SrQuat quat = gRot[i];//*gRot[1].inverse();
			SrQuat prot = gRot[i]*gRot[pindex].inverse();		
			
			//quat.x *= -1;
			//SrVec newOffset = offset*gRot[pindex].inverse();//.inverse();
			SrVec newOffset = offset*quat.inverse();//.inverse();
			if (boneMapping[i] == "l_shoulder" || boneMapping[i] == "r_shoulder")
			{
				newOffset = newOffset*prot;
				bonePreRotMap[i] = prot;
			}
			else if (boneMapping[i] == "l_hip" || boneMapping[i] == "r_hip")
			{
				newOffset = newOffset*prot;
				bonePreRotMap[i] = prot;
			}
			else if (boneMapping[i] == "spine2")
			{
				newOffset = newOffset*prot;
				bonePreRotMap[i] = prot;
			}
// 			else if (i>=5 && i<=7) // l_elbow, l_wrist, l_hand
// 			{
// 				newOffset = newOffset*bonePreRotMap[4];
// 			}
// 			else if (i>=9 && i<=11) // r_elbow, r_wrist, r_hand
// 			{
// 				newOffset = newOffset*bonePreRotMap[8];
// 			}
// 			else if (i>=13 && i<=15) // l_elbow, l_wrist, l_hand
// 			{
// 				newOffset = newOffset*bonePreRotMap[12];
// 			}
// 			else if (i>=17 && i<=19) // l_elbow, l_wrist, l_hand
// 			{
// 				newOffset = newOffset*bonePreRotMap[16];
// 			}
			out[i] = newOffset;
		}
	}
}

void KinectProcessor::initKinectSkeleton(std::vector<SrVec>& gPos, std::vector<SrQuat>& gRot)
{
	SmartBody::SBSkeleton* kinectSk = new SmartBody::SBSkeleton();
	kinectSk->setName("kinect.sk");
	kinectSk->setFileName("kinect.sk");
	// only use the first 20 joints to build the skeleton
	std::vector<SrVec> offsetVec;
	std::map<int, SrQuat> bonePreRotMap;
	inferJointOffsets(gPos,gRot,offsetVec,bonePreRotMap);
	for (int i=0;i<numOfKinectJoints;i++)
	{
		std::string jname = boneMapping[i];
		//SmartBody::SBJoint* sbJoint = dynamic_cast<SmartBody::SBJoint*>(kinectSk->add_joint(SkJoint::TypeQuat, parentIndexMap[i]));
		SmartBody::SBJoint* sbJoint = dynamic_cast<SmartBody::SBJoint*>(kinectSk->add_joint(SkJoint::TypeQuat,  parentIndexMap[i]));
		if (i==0) // root joint
			sbJoint->quat()->prerot(gRot[i].inverse());
		else if (bonePreRotMap.find(i) != bonePreRotMap.end())
			sbJoint->quat()->prerot(bonePreRotMap[i]);		
		// set joint data
		sbJoint->quat()->activate();
		sbJoint->name(jname);
		sbJoint->extName(jname);
		sbJoint->extID(jname);
		sbJoint->extSID(jname);			
		kinectSk->channels().add(jname, SkChannel::XPos);
		kinectSk->channels().add(jname, SkChannel::YPos);
		kinectSk->channels().add(jname, SkChannel::ZPos);
		sbJoint->pos()->limits(SkVecLimits::X, false);
		sbJoint->pos()->limits(SkVecLimits::Y, false);
		sbJoint->pos()->limits(SkVecLimits::Z, false);
		kinectSk->channels().add(jname, SkChannel::Quat);
		//sbJoint->offset(SrVec(0,0,10.f)); // default dummy value		
		sbJoint->offset(offsetVec[i]);				

		// add root joint
		if (jname == "base")
			kinectSk->root(sbJoint);
	}		
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	assetManager->addSkeleton(kinectSk);	
}

KinectProcessor::~KinectProcessor()
{
	boneMapping.clear();
}


int KinectProcessor::getNumBones()
{
	return boneMapping.size();
}

const char* KinectProcessor::getSBJointName(int i)
{
	if (i >= 0 && i < 24)
		return boneMapping[i].c_str();
	else
		return "";
}

void KinectProcessor::setSBJointName(int i, const char* jName)
{
	if (i >= 0 && i < 24)
		boneMapping[i] = jName;
}

void KinectProcessor::processGlobalRotation(std::vector<SrQuat>& quats)
{
	if (quats.size() != 24)
		return;

	// process: get local, and reverse x-axis
	std::vector<SrQuat> tempQuats(24);
	tempQuats = quats;

#if 0
	tempQuats[1] = quats[1] * quats[0].inverse();
	tempQuats[2] = quats[2] * quats[1].inverse();
	tempQuats[3] = quats[3] * quats[2].inverse();
	tempQuats[4] = quats[4] * quats[2].inverse();
	tempQuats[5] = quats[5] * quats[4].inverse();
	tempQuats[6] = quats[6] * quats[5].inverse();
	tempQuats[7] = quats[7] * quats[6].inverse();
	tempQuats[8] = quats[8] * quats[2].inverse();
	tempQuats[9] = quats[9] * quats[8].inverse();
	tempQuats[10] = quats[10] * quats[9].inverse();
	tempQuats[11] = quats[11] * quats[10].inverse();
	tempQuats[12] = quats[12] * quats[0].inverse();
	tempQuats[13] = quats[13] * quats[12].inverse();
	tempQuats[14] = quats[14] * quats[13].inverse();
	tempQuats[15] = quats[15] * quats[14].inverse();
	tempQuats[16] = quats[16] * quats[0].inverse();
	tempQuats[17] = quats[17] * quats[16].inverse();
	tempQuats[18] = quats[18] * quats[17].inverse();
	tempQuats[19] = quats[19] * quats[18].inverse();
#endif

	// base
	quats[0] = quats[1];

	for (int i = 0; i < 24; i++)
	{
		if (quats[i].w == 0)
		{
			tempQuats[i] = SrQuat(0, 0, 0, 0);
			quats[i] = SrQuat();
		}
		else
		{
			if (i == 0)
				tempQuats[i] = quats[i];
			else if (i == 4)		// l_shoulder
				tempQuats[i] = quats[i] * quats[2].inverse();
			else if (i == 8)		// r_shoulder
				tempQuats[i] = quats[i] * quats[2].inverse();
			else if (i == 12)		// l_hip
				tempQuats[i] = quats[i] * quats[0].inverse();
			else if (i == 16)		// r_hip
				tempQuats[i] = quats[i] * quats[0].inverse();
			else if (i == 20)		// l_acromioclavicular
				tempQuats[i] = quats[i] * quats[0].inverse();
			else if (i == 21)		// r_acromioclavicular
				tempQuats[i] = quats[i] * quats[0].inverse();
			else
				tempQuats[i] = quats[i] * quats[i - 1].inverse();
		}
	}
	quats = tempQuats;

	for (unsigned int i = 0; i < 24; i++)
		quats[i].x *= -1.0f;
}



void KinectProcessor::processRetargetPosition( std::string targetSkelName, SrVec& inPos, SrVec& outPos )
{
	std::string kinectSkName = "kinect.sk";
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	SmartBody::SBSkeleton* kinectSk = assetManager->getSkeleton(kinectSkName);
	if (!kinectSk)
	{
		LOG("No Kinect skeleton found when processing translations.");
		return;
	}

	SmartBody::SBRetargetManager* retargetManager = SmartBody::SBScene::getScene()->getRetargetManager();
	SmartBody::SBRetarget* retarget = retargetManager->getRetarget(kinectSkName, targetSkelName);

	
	if (!assetManager->getSkeleton(targetSkelName))
		return; // don't do anything

	if (!retarget)
	{
		retarget = retargetManager->createRetarget(kinectSkName,targetSkelName);
		std::vector<std::string> endJoints;
		endJoints.push_back("l_forefoot");
		endJoints.push_back("l_toe");
		endJoints.push_back("l_wrist");
		endJoints.push_back("r_forefoot");
		endJoints.push_back("r_toe");	
		endJoints.push_back("r_wrist");
		std::vector<std::string> relativeJoints;
		relativeJoints.push_back("spine1");
		relativeJoints.push_back("spine2");
		relativeJoints.push_back("spine3");
		relativeJoints.push_back("spine4");
		relativeJoints.push_back("spine5");
		relativeJoints.push_back("r_sternoclavicular");
		relativeJoints.push_back("l_sternoclavicular");
		relativeJoints.push_back("r_acromioclavicular");
		relativeJoints.push_back("l_acromioclavicular");
		retarget->initRetarget(endJoints,relativeJoints);
	}

	for (int k=0;k<3;k++)
		outPos[k] = retarget->applyRetargetJointTranslation(getSBJointName(0),inPos[k]);

}


void KinectProcessor::processRetargetRotation(std::string targetSkelName, std::vector<SrQuat>& quats, std::vector<SrQuat>& outQuat )
{	
	std::string kinectSkName = "kinect.sk";
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	SmartBody::SBSkeleton* kinectSk = assetManager->getSkeleton(kinectSkName);
	if (!kinectSk)
	{
		LOG("No Kinect skeleton found when processing rotations.");
		return;
	}

	SmartBody::SBRetargetManager* retargetManager = SmartBody::SBScene::getScene()->getRetargetManager();
	SmartBody::SBRetarget* retarget = retargetManager->getRetarget(kinectSkName, targetSkelName);
	
	if (!assetManager->getSkeleton(targetSkelName))
		return; // don't do anything

	if (!retarget)
	{
		retarget = retargetManager->createRetarget(kinectSkName,targetSkelName);
		std::vector<std::string> endJoints;
		endJoints.push_back("l_forefoot");
		endJoints.push_back("l_toe");
		endJoints.push_back("l_wrist");
		endJoints.push_back("r_forefoot");
		endJoints.push_back("r_toe");	
		endJoints.push_back("r_wrist");
		std::vector<std::string> relativeJoints;
		relativeJoints.push_back("spine1");
		relativeJoints.push_back("spine2");
		relativeJoints.push_back("spine3");
		relativeJoints.push_back("spine4");
		relativeJoints.push_back("spine5");
		relativeJoints.push_back("r_sternoclavicular");
		relativeJoints.push_back("l_sternoclavicular");
		relativeJoints.push_back("r_acromioclavicular");
		relativeJoints.push_back("l_acromioclavicular");
		retarget->initRetarget(endJoints,relativeJoints);
	}	

	//quats[0] = quats[1];
	outQuat.resize(quats.size());
	for (int i = 0; i < numOfKinectJoints; i++)
	{
		int pindex = parentIndexMap[i];
		if (pindex < 0 || pindex == 0 || pindex == 2)
			continue;		
		
		outQuat[pindex] = retarget->applyRetargetJointRotation(getSBJointName(pindex),quats[i]);
		//outQuat[i] = retarget->applyRetargetJointRotation(getSBJointName(i),SrQuat());
	}		

	SmartBody::SBJoint* rootJoint = kinectSk->getJointByName(getSBJointName(0));
	SrQuat q = rootJoint->quat()->prerot()*quats[0];
	outQuat[0] = retarget->applyRetargetJointRotation(getSBJointName(0), q);
}

void KinectProcessor::filterRotation(std::vector<SrQuat>& quats)
{
	while ((int)rotationBuffer[0].size() > filterSize)
	{
		for (int i = 0; i < numOfKinectJoints; i++)
			rotationBuffer[i].pop_front();
	}
	for (int i = 0; i < numOfKinectJoints; i++)
		rotationBuffer[i].push_back(quats[i]);

	float weight = 1.0f / float(filterSize);
	for (int i = 0; i < numOfKinectJoints; i++)
	{
		std::list<SrQuat>::iterator iter = rotationBuffer[i].begin();
		SrQuat temp = *iter;
		for (int j = 1; j < filterSize; j++)
		{
			iter++;
			SrQuat q = *iter;
			SrQuat interpolate = slerp(q, temp, 0.5);
			temp = interpolate;
		}
		quats[i] = temp;
	}
}
