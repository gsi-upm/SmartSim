#include "SBAssetHandlerCOLLADA.h"
#include <vhcl.h>
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBMotion.h>
#include <sb/SBSkeleton.h>
#include <sb/SBScene.h>
#include "sbm/ParserCOLLADAFast.h"
#include "sbm/ParserOpenCOLLADA.h"
#include <sbm/GPU/SbmDeformableMeshGPU.h>


namespace SmartBody {

SBAssetHandlerCOLLADA::SBAssetHandlerCOLLADA()
{
//#ifdef SB_NO_ASSIMP
	assetTypes.push_back("dae");
//#endif
}

SBAssetHandlerCOLLADA::~SBAssetHandlerCOLLADA()
{
}

std::vector<SBAsset*> SBAssetHandlerCOLLADA::getAssets(const std::string& path)
{
	std::vector<SBAsset*> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath == "")
		return assets;

	boost::filesystem::path p(path);
	std::string extension = boost::filesystem::extension(p);
	std::string basename = boost::filesystem::basename(p);
	
	double skeletonScale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalSkeletonScale");
	double motionScale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalMotionScale");

	bool useFastParsing = SmartBody::SBScene::getScene()->getBoolAttribute("useFastCOLLADAParsing");
	if (useFastParsing)
	{
		rapidxml::xml_document<> doc;
		rapidxml::file<char>* rapidFile = ParserCOLLADAFast::getParserDocumentFile(convertedPath, &doc);
		if (!rapidFile)
		{
			LOG("Problem parsing file '%s'", convertedPath.c_str());
			return assets;
		}
		rapidxml::xml_node<>* colladaNode = doc.first_node("COLLADA");
		if (!colladaNode)
		{
			LOG("Problem parsing file '%s': not a COLLADA file.", convertedPath.c_str());
			delete rapidFile;
			return assets;
		}
		bool zaxis = false;
		rapidxml::xml_node<>* assetNode = ParserCOLLADAFast::getNode("asset", colladaNode, 0, 1);
		if (assetNode)
		{
			rapidxml::xml_node<>* upNode = ParserCOLLADAFast::getNode("up_axis", assetNode);
			if (upNode)
			{
				std::string upAxisName = upNode->value();
				if (upAxisName == "Z_UP" || upAxisName == "z_up")
				{
					// rotate the skeleton by -90 around the x-axis
					zaxis = true;
				}

			}
		}

		rapidxml::xml_node<>* geometryNode = ParserCOLLADAFast::getNode("library_geometries", colladaNode, 0, 1);
		rapidxml::xml_node<>* visualSceneNode = ParserCOLLADAFast::getNode("library_visual_scenes", colladaNode, 0, 1);
		if (geometryNode || visualSceneNode)
		{
			// first from library visual scene retrieve the material id to name mapping (TODO: needs reorganizing the assets management)
			std::map<std::string, std::string> materialId2Name;
			rapidxml::xml_node<>* visualSceneNode = ParserCOLLADAFast::getNode("library_visual_scenes", colladaNode, 0, 1);
			if (!visualSceneNode)
				LOG(" .dae file %s doesn't contain correct geometry information.", convertedPath.c_str());
			SBSkeleton* skeleton = new SBSkeleton();
			skeleton->setName(basename + extension);
			SBMotion* motion = new SBMotion();
			motion->setName(basename + extension);
			int order;
			ParserCOLLADAFast::parseLibraryVisualScenes(visualSceneNode, *skeleton, *motion, 1.0, order, materialId2Name);

			if (zaxis)
			{				
				// get the root node
				SkJoint* root = skeleton->root();
				if (root)
				{
					if (root->quat())
					{
						SrQuat prerot = root->quat()->prerot();
						SrVec xaxis(1, 0, 0);
						SrQuat adjust(xaxis, 3.14159f / -2.0f);
						SrQuat adjustY(SrVec(0,1,0), 3.14159f );
						//SrQuat final = adjustY * adjust * prerot; 
						SrQuat final = adjust * prerot;
						//LOG("before = %f %f %f %f", prerot.w, prerot.x, prerot.y, prerot.z);
						//LOG("after = %f %f %f %f", final.w, final.x, final.y, final.z);
						root->quat()->prerot(final);
						root->offset(root->offset()*adjust);
					}
				}
			}		

			rapidxml::xml_node<>* skmNode = ParserCOLLADAFast::getNode("library_animations", colladaNode, 0, 1);
			if (skmNode)
			{
				ParserCOLLADAFast::parseLibraryAnimations(skmNode, *skeleton, *motion, 1.0, order, false);
			}

			if (skeleton->getNumJoints() == 0)
				delete skeleton;
			else
				assets.push_back(skeleton);


			
		

			if (motion->getNumFrames() == 0)
				delete motion;
			else
				assets.push_back(motion);


			// get picture id to file mapping
			std::map<std::string, std::string> pictureId2File;
			std::map<std::string, std::string> pictureId2Name;
			rapidxml::xml_node<>* imageNode = ParserCOLLADAFast::getNode("library_images", colladaNode, 0, 1);
			if (imageNode)
				ParserCOLLADAFast::parseLibraryImages(imageNode, pictureId2File, pictureId2Name);

			// start parsing mateiral
			std::map<std::string, std::string> effectId2MaterialId;
			rapidxml::xml_node<>* materialNode = ParserCOLLADAFast::getNode("library_materials", colladaNode, 0, 1);
			if (materialNode)
				ParserCOLLADAFast::parseLibraryMaterials(materialNode, effectId2MaterialId);

			// start parsing effect
			SrArray<SrMaterial> M;
			SrStringArray mnames;
			std::map<std::string,std::string> mtlTextMap;
			std::map<std::string,std::string> mtlTextBumpMap;
			std::map<std::string,std::string> mtlTextSpecularMap;
			rapidxml::xml_node<>* effectNode = ParserCOLLADAFast::getNode("library_effects", colladaNode, 0, 1);
			if (effectNode)
			{
				ParserCOLLADAFast::parseLibraryEffects(effectNode, effectId2MaterialId, materialId2Name, pictureId2File, pictureId2Name, M, mnames, mtlTextMap, mtlTextBumpMap, mtlTextSpecularMap);
			}
			// parsing geometry
#if !defined (__ANDROID__) && !defined(SB_IPHONE) &&  !defined(__FLASHPLAYER__) && !defined(__native_client__)
			SbmDeformableMeshGPU* mesh = new SbmDeformableMeshGPU();
#else
			DeformableMesh* mesh = new DeformableMesh();
#endif
			boost::filesystem::path meshPath(path);
			std::string meshBaseName = boost::filesystem::basename(meshPath);
			std::string extension = boost::filesystem::extension(meshPath);
			mesh->setName(meshBaseName + extension);
			std::vector<SrModel*> meshModelVec;
			if (geometryNode)
				ParserCOLLADAFast::parseLibraryGeometries(geometryNode, convertedPath.c_str(), M, mnames, materialId2Name, mtlTextMap, mtlTextBumpMap, mtlTextSpecularMap, meshModelVec, 1.0f);

			float factor = 1.0f;

			rapidxml::xml_node<>* controllerNode = ParserCOLLADAFast::getNode("library_controllers", colladaNode, 0, 2);		
			if (!controllerNode)
			{
				LOG("mcu_character_load_skinweights ERR: no binding info contained");
			}
			else
			{
				const char* jointNamePrefix = "";
				ParserCOLLADAFast::parseLibraryControllers(controllerNode, mesh, factor, jointNamePrefix);
			}	
			
			for (unsigned int skinCounter = 0; skinCounter < mesh->skinWeights.size(); skinCounter++)
			{
				SkinWeight* skinWeight = mesh->skinWeights[skinCounter];		
				for (size_t j = 0; j < skinWeight->infJointName.size(); j++)
				{
					std::string& jointName = skinWeight->infJointName[j];
					SkJoint* curJoint = skeleton->search_joint(jointName.c_str());					
					if (curJoint)
					{
						skinWeight->infJointName[j] = curJoint->getName();
					}
					//skinWeight->infJoint.push_back(curJoint); // NOTE: If joints are added/removed during runtime, this list will contain stale data
				}				
			}
	
			std::map<std::string, std::vector<std::string> >::iterator morphTargetIter;
			// handling morph targets
			for (unsigned int i = 0; i < meshModelVec.size(); i++)
			{
				for (int j = 0; j < meshModelVec[i]->V.size(); j++)
				{
					meshModelVec[i]->V[j] *= factor;
				}
		
// 				if (meshModelVec[i]->Fn.size() == 0)
// 				{
// 					meshModelVec[i]->computeNormals();
// 				}
				meshModelVec[i]->computeNormals();

				SrSnModel* srSnModelStatic = new SrSnModel();
				srSnModelStatic->shape(*meshModelVec[i]);
				srSnModelStatic->shape().name = meshModelVec[i]->name;
				
				SrSnModel* srSnModelDynamic = new SrSnModel();
				srSnModelDynamic->shape(*meshModelVec[i]);
				srSnModelDynamic->changed(true);
				srSnModelDynamic->visible(false);
				srSnModelDynamic->shape().name = meshModelVec[i]->name;
				
				bool isBlendShape = false;
				bool isBaseShape = false;
				std::string baseShape = "";

				for (morphTargetIter = mesh->morphTargets.begin(); morphTargetIter != mesh->morphTargets.end(); ++morphTargetIter)	
				{
					if (morphTargetIter->second.size() < 1)
						continue;

					baseShape = morphTargetIter->second[0];
					for (size_t c = 0; c < morphTargetIter->second.size(); ++c)
					{
						if (strcmp(morphTargetIter->second[c].c_str(), meshModelVec[i]->name) == 0)
						{
							if (c == 0)
								isBaseShape = true;
							else
								isBlendShape = true;
						}
					}
				}

				if (isBlendShape || isBaseShape)
				{
					if (mesh->blendShapeMap.find(baseShape) == mesh->blendShapeMap.end())		
					{
						mesh->blendShapeMap.insert(std::make_pair(baseShape, std::vector<SrSnModel*>()));
					}
					mesh->blendShapeMap[baseShape].push_back(srSnModelStatic);
					srSnModelStatic->ref();
					delete srSnModelDynamic;
					//LOG("Insert blend shape %s with base shape %s", (const char*)meshModelVec[i]->name, baseShape.c_str());
				}

				if (isBaseShape)
				{
					SrSnModel* srSnModelStaticBase = new SrSnModel();
					srSnModelStaticBase->shape(*meshModelVec[i]);
					srSnModelStaticBase->shape().name = meshModelVec[i]->name;

					SrSnModel* srSnModelDynamicBase = new SrSnModel();
					srSnModelDynamicBase->shape(*meshModelVec[i]);
					srSnModelDynamicBase->changed(true);
					srSnModelDynamicBase->visible(false);
					srSnModelDynamicBase->shape().name = meshModelVec[i]->name;

					mesh->dMeshStatic_p.push_back(srSnModelStaticBase);
					srSnModelStaticBase->ref();
					mesh->dMeshDynamic_p.push_back(srSnModelDynamicBase);
					srSnModelDynamicBase->ref();
					//LOG("Insert base mesh %s", (const char*)meshModelVec[i]->name);
				}

				if (!isBlendShape && !isBaseShape)
				{
					mesh->dMeshStatic_p.push_back(srSnModelStatic);
					srSnModelStatic->ref();
					mesh->dMeshDynamic_p.push_back(srSnModelDynamic);
					srSnModelDynamic->ref();
					//LOG("Insert mesh %s", (const char*)meshModelVec[i]->name);
				}
			
				/* perform this when the character wants to attach to the mesh
				SrSnGroup* meshGroup = new SrSnGroup();
				meshGroup->separator(true);
				meshGroup->add(srSnModelStatic);
				// find the group of the root joint
				SrSn* node = pawn->scene_p->get(0);
				if (node)
				{
					SrSnGroup* srSnGroup = dynamic_cast<SrSnGroup*>(node);
					if (srSnGroup)
						srSnGroup->add(meshGroup);
				}
				*/
				delete meshModelVec[i];
			}
			assets.push_back(mesh);

			if (rapidFile)
				delete rapidFile;
		}
		else
		{
			LOG( "Could not load mesh from file '%s'", convertedPath.c_str());
			if (rapidFile)
				delete rapidFile;
			return assets;
		}
	}

	return assets;
}

};
