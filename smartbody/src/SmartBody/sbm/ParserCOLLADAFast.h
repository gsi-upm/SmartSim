/*
 *  ParserOpenCOLLADA.h - part of Motion Engine and SmartBody-lib
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
 *      Yuyu Xu, USC
 */

#ifndef _PARSER_COLLADAFAST_H_
#define _PARSER_COLLADAFAST_H_

#include <fstream>
#include <sbm/rapidxml_utils.hpp>
#include <sbm/rapidxml.hpp>
#include "sk/sk_skeleton.h"
#include "sk/sk_motion.h"
#include <sr/sr_material.h>

#include "sbm/gwiz_math.h"
#include <sbm/sbm_deformable_mesh.h>

typedef std::map<std::string, std::vector<SrVec> > VecListMap;

struct ColladaFloatArrayFast
{
	std::vector<float> floatArray;
	std::string accessorParam;
	int stride;	 
};

struct ColladaSamplerFast
{
	std::string inputName;
	std::string outputName;
};

struct ColladChannelFast
{
	std::string sourceName;
	std::string targetJointName;
	std::string targetType;
};

class ParserCOLLADAFast
{
	public:
		static void getChildNodes(const std::string& nodeName, rapidxml::xml_node<>* node, std::vector<rapidxml::xml_node<>*>& children );
		static rapidxml::xml_node<>* getNode(const std::string& nodeName, rapidxml::xml_node<>* node, int curDepth, int maximumDepth);
		static rapidxml::xml_node<>* getNode(const std::string& nodeName, rapidxml::xml_node<>* node);

		//static rapidxml::xml_node<>* getNode(const std::string& nodeName, std::string fileName, int maximumDepth);
		//static rapidxml::xml_node<>* getNode(const std::string& nodeName, std::string fileName);

		static rapidxml::file<char>* getParserDocumentFile(std::string fileName, rapidxml::xml_document<>* doc);
		static std::string getNodeAttributeString(rapidxml::xml_node<>* node, std::string attrName);
		static int getNodeAttributeInt(rapidxml::xml_node<>* node, std::string attrName);
		static void nodeStr(const std::string s, std::string& out);

		// parse from files
		static bool parse(SkSkeleton& skeleton, SkMotion& motion, std::string fileName, float scale, bool doParseSkeleton, bool doParseMotion);
		static bool parseStaticMesh(std::vector<SrModel*>& meshModelVecs, std::string fileName);

		// parse nodes
		static void parseLibraryControllers(rapidxml::xml_node<>* node, DeformableMesh* mesh, float scaleFactor, std::string jointPrefix);
		static void parseLibraryVisualScenes(rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, std::map<std::string, std::string>& materialId2Name);
		static void parseJoints(rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, std::map<std::string, std::string>& materialId2Name, SkJoint* parent = NULL, bool hasRootJoint = false);
		static void parseLibraryAnimations(rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order, bool zaxis = false);

		static void parseNodeAnimation(rapidxml::xml_node<>* node1, std::map<std::string, ColladaFloatArrayFast > &floatArrayMap, float scale, std::map<std::string, ColladaSamplerFast > &samplerMap, std::vector<ColladChannelFast> &channelSamplerNameMap, SkSkeleton &skeleton );
		static void parseLibraryAnimations2(rapidxml::xml_node<>* node, SkSkeleton& skeleton, SkMotion& motion, float scale, int& order);
	
		static void animationPostProcess(SkSkeleton& skeleton, SkMotion& motion);
		static void animationPostProcessByChannels(SkSkeleton& skeleton, SkMotion& motion, SkChannelArray& channels);
		static void parseLibraryGeometries(rapidxml::xml_node<>* node, const char* file, SrArray<SrMaterial>& M, SrStringArray& mnames,std::map<std::string, std::string>& materialId2Name, std::map<std::string,std::string>& mtlTexMap, std::map<std::string,std::string>& mtlTexBumpMap, std::map<std::string,std::string>& mtlTexSpecularMap,std::vector<SrModel*>& meshModelVec, float scale);
		static void load_texture(int type, const char* file, const SrStringArray& paths);
		static void parseLibraryMaterials(rapidxml::xml_node<>* node, std::map<std::string, std::string>& effectId2MaterialId);
		static void parseLibraryImages(rapidxml::xml_node<>* node, std::map<std::string, std::string>& pictureId2File, std::map<std::string, std::string>& pictureId2Name);
		static void parseLibraryEffects(rapidxml::xml_node<>* node, std::map<std::string, std::string>&effectId2MaterialId, std::map<std::string, std::string>& materialId2Name, std::map<std::string, std::string>& pictureId2File, std::map<std::string, std::string>& pictureId2Name, SrArray<SrMaterial>& M, SrStringArray& mnames, std::map<std::string,std::string>& mtlTexMap, std::map<std::string,std::string>& mtlTexBumpMap, std::map<std::string,std::string>& mtlTexSpecularMap);

	private:
		static int getMotionChannelId(SkChannelArray& channels, const std::string&  sourceName);
		static int getMotionChannelId(SkChannelArray& channels, std::string& jointName, std::string& targetType);
		static std::string tokenize(std::string& str,const std::string& delimiters = " ", int mode = 1);
		static int getRotationOrder(std::vector<std::string> orderVec);
		static std::string getGeometryType(std::string s);
		static void setModelVertexSource(std::string& sourceName, std::string& semanticName, SrModel* model, VecListMap& vecMap);		
};

#endif
