/*
 *  ParserFBX.h - part of Motion Engine and SmartBody-lib
 *  Copyright (C) 2008  University of Southern California
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
 *      Adam Reilly, USC
 */

#ifndef _PARSER_FBX_H_
#define _PARSER_FBX_H_


#if ENABLE_FBX_PARSER   // defined in the project settings
#define ONE_OVER_THIRTY (1.0f / 30.0f)

#include <map>

#include "sk/sk_skeleton.h"
#include "sk/sk_motion.h"


#include "fbxsdk.h"


class ParserFBX
{
   public:
      // main function to be called to parse an fbx file. Parses skeleton and animations
      static bool parse(SkSkeleton& skeleton, SkMotion& motion, const std::string& fileName, float scale);

      // parses skin info
      static bool parseSkin(const std::string& fileName, const char* char_name, float scaleFactor, std::string& jointPrefix, mcuCBHandle* mcu_p);
   private:
      // struct that stores animation data until it is converted into the sbm structs
      struct FBXAnimData
      {
         std::string channelName; // i.e. l_elbow Quat
         SkChannel::Type channelType; // i.e. SkChannel::XPos:
         std::map<kLongLong, float> keyFrameData; // key = key frame time, value = position or rotation at that time
         std::map<int, float> keyFrameDataFrame; //key = frame #, value = position or rotation at that frame
      };

      // stores data specific to sbm animations that we manually parse out of the fbx
      struct FBxMetaData
      {
         fbxDouble1 readyTime;
         fbxDouble1 strokeStart;
         fbxDouble1 emphasisTime;
         fbxDouble1 strokeTime;
         fbxDouble1 relaxTime;

         FBxMetaData() { readyTime = strokeStart = emphasisTime = strokeTime = relaxTime = 0; }
         FBxMetaData operator *=(fbxDouble1 value)
         {
            readyTime *= value;
            strokeStart *= value;
            emphasisTime *= value;
            strokeTime *= value;
            relaxTime *= value;
            return *this;
         }

         FBxMetaData operator -=(fbxDouble1 value)
         {
            readyTime -= value;
            strokeStart -= value;
            emphasisTime -= value;
            strokeTime -= value;
            relaxTime -= value;
            return *this;
         }

         bool operator ==(fbxDouble1 value) const
         {
            return (readyTime == value && strokeStart == value 
               && emphasisTime == value && strokeTime == value && relaxTime == value);
         }
      };

      // starts up the fbx sdk
      static bool Init(KFbxSdkManager** pSdkManager, KFbxImporter** pImporter, KFbxScene** pScene, const std::string& fileName);

      // shuts down the fbx sdk
      static void Shutdown(KFbxSdkManager* pSdkManager, KFbxImporter* pImporter);

      // skin parsing functions
      static void parseSkinRecursive(KFbxNode* pNode, const char* char_name, float scaleFactor,
         std::string& jointPrefix, mcuCBHandle* mcu_p, SbmCharacter* char_p, std::vector<SrModel*>& meshModelVec);

      // joint parsing functions
      static void parseJoints(KFbxNode* pNode, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order,
         FBxMetaData& metaData, SkJoint* parent = NULL);
      static SkJoint* createJoint(KFbxNode* pNode, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, SkJoint* parent);
      static void parseLibraryAnimations(KFbxNode* pNode, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order);
      static void animationPostProcessByChannels(SkSkeleton& skeleton, SkMotion& motion, SkChannelArray& channels);

      // meta data parsing functions
      static void ParseMetaData(KFbxNode* pNode, FBxMetaData& out_metaData);

      // animation parsing functions
      static void ParseKeyData(const KFbxAnimCurve* pCurve, const SkChannel::Type type, const std::string& jointName,
         SkMotion& motion, std::vector<FBXAnimData*>& fbxAnimData);
      static void AddAnimation(KFbxScene* pScene, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, const FBxMetaData& metaData);
      static void AddAnimationRecursive(KFbxAnimStack* pAnimStack, KFbxNode* pNode,/*, FBXFrame * pFrame*/SkSkeleton& skeleton,
         SkMotion& motion,float scale, int& order, std::vector<FBXAnimData*>& fbxAnimData);
      static void AddAnimationRecursive(KFbxAnimLayer* pAnimLayer, KFbxNode* pNode, std::string &takeName,
         SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, std::vector<FBXAnimData*>& fbxAnimData);

      // utility functions
      static void ConvertfbxDouble3ToSrVec(const fbxDouble3& fbx, SrVec& sbm);
      static void ConvertKFbxVector4ToSrPnt(const KFbxVector4& fbx, SrPnt& sbm);
      static void ConvertKFbxVector2ToSrPnt2(const KFbxVector2& fbx, SrPnt2& sbm);
      static void ConvertfbxAnimToSBM(const std::vector<FBXAnimData*>& fbxAnimData, SkSkeleton& skeleton, 
         SkMotion& motion, float scale, int& order, const FBxMetaData& metaData);
      static bool HasSmartbodyChannel(KFbxNode* pNode, const char* pChannelName, bool& out_ChannelValue);
};

#endif  // ENABLE_FBX_PARSER

#endif // _PARSER_FBX_H_
