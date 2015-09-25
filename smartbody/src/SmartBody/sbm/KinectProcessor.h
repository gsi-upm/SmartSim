#ifndef _KINECT_PROCESSOR_H_
#define _KINECT_PROCESSOR_H_

#include <string>
#include <vector>
#include <list>
#include <map>
#include <sr/sr_quat.h>
class KinectProcessor
{
	public:
		KinectProcessor();
		~KinectProcessor();
		int getNumBones();
		const char* getSBJointName(int i);
		void setSBJointName(int i, const char* jName);
		static void processGlobalRotation(std::vector<SrQuat>& quats);
		void processRetargetRotation(std::string targetSkelName, std::vector<SrQuat>& inQuats, std::vector<SrQuat>& outQuat);
		void processRetargetPosition(std::string targetSkelName, SrVec& inPos, SrVec& outPos);
		void filterRotation(std::vector<SrQuat>& quats);
		// routines to reconstruct a kinect skeleton from joint data
		void inferJointOffsets(std::vector<SrVec>& gPos, std::vector<SrQuat>& gRot, std::vector<SrVec>& out, std::map<int, SrQuat>& bonePreRotMap);
		void initKinectSkeleton(std::vector<SrVec>& gPos, std::vector<SrQuat>& gRot);
		void refineKinectSkeleton(std::vector<SrVec>& gPos, std::vector<SrQuat>& gRot);

	private:
		std::vector<std::string>			boneMapping;
		std::vector<std::list<SrQuat> >		rotationBuffer;
		std::map<int,int>                   parentIndexMap;
		int									filterSize;
		int                                 numOfKinectJoints;
};
#endif
