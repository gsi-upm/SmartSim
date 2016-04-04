#ifndef SB_NO_ASSIMP
#include "SBAssetHandlerAssimp.h"
#include <vhcl.h>
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sk/sk_channel.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h> 
#include <sbm/GPU/SbmTexture.h>
#include <exception>

struct materialTextures {
	std::vector<std::string> diffuseTextures;
	std::vector<std::string> ambientTextures;
	std::vector<std::string> specularTextures;
	std::vector<std::string> emissiveTextures;
	std::vector<std::string> heightTextures;
	std::vector<std::string> shininessTextures;
	std::vector<std::string> normalsTextures;
	std::vector<std::string> opacityTextures;
	std::vector<std::string> displacementTextures;
	std::vector<std::string> lightmapTextures;
	std::vector<std::string> reflectionTextures;
	std::vector<std::string> unknownTextures;
};


namespace SmartBody {

SBAssetHandlerAssimp::SBAssetHandlerAssimp()
{
	assetTypes.push_back("fbx");
	assetTypes.push_back("blend");
	//assetTypes.push_back("dae");
	assetTypes.push_back("obj");
//	assetTypes.push_back("ply");
	assetTypes.push_back("mesh");
}

SBAssetHandlerAssimp::~SBAssetHandlerAssimp()
{
}

void recurseNode(aiNode* node, std::map<std::string, std::string>& hierarchyMap, std::map<int, aiNode*>& meshParents)
{
	//LOG("%s", node->mName.C_Str());
	int numMeshes = node->mNumMeshes;
		
	if (node->mNumMeshes > 0)
	{
		for (int m = 0; m < numMeshes; m++)
		{
			// add a mapping from the mesh to the node parent
			meshParents.insert(std::pair<int, aiNode*>(node->mMeshes[m], node));

		}
	}
	int numChildren = node->mNumChildren;
	for (int c = 0; c < numChildren; c++)
	{
		aiNode* child = node->mChildren[c];
		hierarchyMap.insert(std::pair<std::string, std::string>(child->mName.C_Str(), node->mName.C_Str()));
		recurseNode(child, hierarchyMap, meshParents);
	}
}

std::string recurseFindParent(std::string& child, std::map<std::string, std::string>& hierarchyMap, std::set<std::string>& skeletonBones)
{
	std::map<std::string, std::string>::iterator iter = hierarchyMap.find(child);
	if (iter == hierarchyMap.end())
	{
		return "";
	}

	std::string curParent = (*iter).second;
	if (curParent == "")
		return "";
	
	std::set<std::string>::iterator boneIter = skeletonBones.find(curParent);
	if (boneIter != skeletonBones.end())
	{
		return curParent;
	}
	else
	{
		return recurseFindParent(curParent, hierarchyMap, skeletonBones);
	}

}

void recurseAddMissing(aiNode* node, std::set<std::string>& existingHierarchy)
{
	int numChildren = node->mNumChildren;
	for (int c = 0; c < numChildren; c++)
	{
		aiNode* child = node->mChildren[c];
		std::string childName = child->mName.C_Str();
		size_t loc = childName.find("_$AssimpFbx$_"); // ignore parser-created nodes
		if (loc == std::string::npos)
		{
			std::set<std::string>::iterator iter = existingHierarchy.find(childName);
			if (iter == existingHierarchy.end())
			{
				existingHierarchy.insert(childName);
				//LOG("ADDED BONE %s NOT DIRECTLY CONTROLLED BY MESH", childName.c_str());
			}
		}
		recurseAddMissing(child, existingHierarchy);
	}
}

int recurseNodeLevel(aiNode* rootNode, aiNode* curNode, int level)
{
	if (curNode == rootNode)
		return level;
	aiNode* parent = curNode->mParent;
	return recurseNodeLevel(rootNode, parent, level + 1);
}

bool has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}


std::vector<SBAsset*> SBAssetHandlerAssimp::getAssets(const std::string& path)
{
	std::vector<SBAsset*> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath == "")
		return assets;

	boost::filesystem::path p(path);
	std::string extension = boost::filesystem::extension(p);
	std::string basename = boost::filesystem::basename(p);
	boost::filesystem::path parentPath = p.parent_path();

	////////////////////////////////////////////
	try {
	Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile( convertedPath, 
		aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_SortByPType);
  

	std::vector<SrMaterial*> allMaterials;
	std::vector<std::string> allMaterialNames;

	std::vector< materialTextures > allTextureSets;

	std::set<std::string> skeletonBones;
	
	// If the import failed, report it
	if( !scene)
	{
		LOG(importer.GetErrorString());
		return assets;
	}
	else
	{
		// get the materials
		if (scene->HasMaterials())
		{
			int numMaterials = scene->mNumMaterials;
			for (int m = 0; m < numMaterials; m++)
			{
				SrMaterial* material = new SrMaterial();

				aiColor4D ambient(0.0f, 0.0f, 0.0f, 1.0f);
				scene->mMaterials[m]->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
				material->ambient = SrColor(ambient.r, ambient.g, ambient.b, ambient.a);
				//LOG("AMBIENT COLOR %x %x %x %x", material->ambient.r, material->ambient.g, material->ambient.b, material->ambient.a);

				aiColor4D diffuse(0.0f, 0.0f, 0.0f, 1.0f);
				scene->mMaterials[m]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
				material->diffuse = SrColor(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
				//LOG("DIFFUSE COLOR %x %x %x %x", material->diffuse.r, material->diffuse.g, material->diffuse.b, material->diffuse.a);

				aiColor4D emmissive(0.0f, 0.0f, 0.0f, 1.0f);
				scene->mMaterials[m]->Get(AI_MATKEY_COLOR_EMISSIVE , emmissive);
				material->emission = SrColor(emmissive.r, emmissive.g, emmissive.b, emmissive.a);
				//LOG("EMMISSIVE COLOR %x %x %x %x", material->emission.r, material->emission.g, material->emission.b, material->emission.a);

				float shininess = 0.0;
				float tmpShininess = 0.0;
				unsigned int max = 1;
				int ret1 = aiGetMaterialFloatArray(scene->mMaterials[m], AI_MATKEY_SHININESS, &shininess, &max);
				if(ret1 == AI_SUCCESS)
				{
    				max = 1;
					float strength = 0.0f;
    				int ret2 = aiGetMaterialFloatArray(scene->mMaterials[m], AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
					if(ret2 == AI_SUCCESS)
					{
						tmpShininess = shininess * strength;
					}
					else
					{
						tmpShininess = shininess;
					}
				}
				// convert to byte
				int tempIntShininess = int(tmpShininess);
				material->shininess =  tempIntShininess & 0x000000ff;
				//LOG("SHININESS COLOR %x", material->shininess);

				aiColor4D specular(0.0f, 0.0f, 0.0f, 1.0f);
				scene->mMaterials[m]->Get(AI_MATKEY_COLOR_SPECULAR, specular);
				material->specular = SrColor(specular.r, specular.g, specular.b);
				//LOG("SPECULAR COLOR %x %x %x %x", material->specular.r, material->specular.g, material->specular.b, material->specular.a);

				aiColor4D transparency(0.0f, 0.0f, 0.0f, 1.0f);
				scene->mMaterials[m]->Get(AI_MATKEY_COLOR_TRANSPARENT, transparency);
				//LOG("TRANSPARENCY %x %x %x %x", transparency.r, transparency.g, transparency.b, transparency.a);

				material->transparency = (transparency.r + transparency.g  + transparency.b) / 3.0;
				if (material->transparency < .99)
					material->useAlphaBlend = false;
				else
					material->useAlphaBlend = true;
				material->diffuse.a = (srbyte) ( material->transparency * 255.0f );
				//LOG("TRANSPARENCY COLOR %.2f ", material->transparency);

				float matOpacity = 1.f;
				scene->mMaterials[m]->Get(AI_MATKEY_OPACITY, matOpacity);
				if (matOpacity < 1.f)
				{
					material->useAlphaBlend = true;
					material->diffuse.a = matOpacity;
				}
				else
					material->useAlphaBlend = false;
				
				float opacity = 1.0f;
				scene->mMaterials[m]->Get(AI_MATKEY_OPACITY, opacity);
				//LOG("OPACITY %.2f ", opacity);
				if (opacity < .99)
					material->useAlphaBlend = true;
				else
					material->useAlphaBlend = false;
				material->transparency = opacity;
				material->diffuse.a = (srbyte) ( opacity * 255.0f );
				
				aiString matName;
				scene->mMaterials[m]->Get(AI_MATKEY_NAME, matName);
				allMaterialNames.push_back(matName.C_Str());
				//LOG("mat name = %s, opacity = %f, transparency = %f %f %f", matName.C_Str(), matOpacity, transparency.r, transparency.g, transparency.b);


				SbmTextureManager& texManager = SbmTextureManager::singleton();
				materialTextures matTex;
				aiString texturePath;
                unsigned int numDiffuseTextures = scene->mMaterials[m]->GetTextureCount(aiTextureType_DIFFUSE);
				std::vector<std::string> texturePaths;
				for (int t = 0; t < numDiffuseTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, t, &texturePath);
					boost::filesystem::path p(texturePath.C_Str());
					std::string extension = boost::filesystem::extension(p);
					std::string basename = boost::filesystem::basename(p);
					std::string tName = basename + extension;
					boost::filesystem::path loadPath = parentPath;
					loadPath /= tName;
					matTex.diffuseTextures.push_back(loadPath.string());
					texManager.loadTexture(SbmTextureManager::TEXTURE_DIFFUSE, loadPath.string().c_str(), loadPath.string().c_str());	

					LOG("Found diffuse texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				unsigned int numSpecularTextures = scene->mMaterials[m]->GetTextureCount(aiTextureType_SPECULAR);
				for (int t = 0; t < numSpecularTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture(aiTextureType_SPECULAR, t, &texturePath);
					boost::filesystem::path p(texturePath.C_Str());
					std::string extension = boost::filesystem::extension(p);
					std::string basename = boost::filesystem::basename(p);
					std::string tName = basename + extension;
					boost::filesystem::path loadPath = parentPath;
					loadPath /= tName;
					matTex.specularTextures.push_back(loadPath.string());
					texManager.loadTexture(SbmTextureManager::TEXTURE_SPECULARMAP, loadPath.string().c_str(), loadPath.string().c_str());	

					LOG("Found specular texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				unsigned int numAmbientTextures = scene->mMaterials[m]->GetTextureCount(aiTextureType_AMBIENT );
				for (int t = 0; t < numAmbientTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture(aiTextureType_AMBIENT , t, &texturePath);
					LOG("Found ambient texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				unsigned int numEmissiveTextures = scene->mMaterials[m]->GetTextureCount(aiTextureType_EMISSIVE );
				for (int t = 0; t < numEmissiveTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture(aiTextureType_EMISSIVE , t, &texturePath);
					LOG("Found emissive texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				unsigned int numHeightTextures = scene->mMaterials[m]->GetTextureCount(aiTextureType_HEIGHT );
				for (int t = 0; t < numHeightTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture(aiTextureType_HEIGHT , t, &texturePath);
					LOG("Found height texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				unsigned int numNormalTextures = scene->mMaterials[m]->GetTextureCount(aiTextureType_NORMALS);
				for (int t = 0; t < numNormalTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture(aiTextureType_NORMALS, t, &texturePath);
					boost::filesystem::path p(texturePath.C_Str());
					std::string extension = boost::filesystem::extension(p);
					std::string basename = boost::filesystem::basename(p);
					std::string tName = basename + extension;
					boost::filesystem::path loadPath = parentPath;
					loadPath /= tName;
					matTex.normalsTextures.push_back(loadPath.string());
					texManager.loadTexture(SbmTextureManager::TEXTURE_NORMALMAP, loadPath.string().c_str(), loadPath.string().c_str());	
					LOG("Found normal texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				unsigned int numShininessTextures = scene->mMaterials[m]->GetTextureCount(aiTextureType_SHININESS);
				for (int t = 0; t < numShininessTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture(aiTextureType_SHININESS, t, &texturePath);
					LOG("Found normal texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				unsigned int numOpacityTextures = scene->mMaterials[m]->GetTextureCount( aiTextureType_OPACITY);
				for (int t = 0; t < numOpacityTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture( aiTextureType_OPACITY, t, &texturePath);
					LOG("Found opacity texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				unsigned int numDisplacementTextures = scene->mMaterials[m]->GetTextureCount( aiTextureType_DISPLACEMENT );
				for (int t = 0; t < numDisplacementTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture( aiTextureType_DISPLACEMENT, t, &texturePath);
					LOG("Found displacement texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				unsigned int numLightmapTextures = scene->mMaterials[m]->GetTextureCount( aiTextureType_LIGHTMAP );
				for (int t = 0; t < numLightmapTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture( aiTextureType_LIGHTMAP, t, &texturePath);
					LOG("Found lightmap texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				unsigned int numReflectionTextures = scene->mMaterials[m]->GetTextureCount( aiTextureType_REFLECTION );
				for (int t = 0; t < numReflectionTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture( aiTextureType_REFLECTION , t, &texturePath);
					LOG("Found reflection texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				unsigned int numUnknownTextures = scene->mMaterials[m]->GetTextureCount( aiTextureType_UNKNOWN );
				for (int t = 0; t < numUnknownTextures; t++)
				{
					aiString texturePath;
					scene->mMaterials[m]->GetTexture( aiTextureType_UNKNOWN , t, &texturePath);
					LOG("Found unknown texture with material name %s in path %s", matName.C_Str(), texturePath.C_Str());
				}

				//for (int p = 0; p < scene->mMaterials[m]->mNumProperties; p++)
				//{
				//	std::string keyStr = scene->mMaterials[m]->mProperties[p]->mKey.C_Str();
				//	LOG("Property: %s", keyStr.c_str());
				//}

				allTextureSets.push_back(matTex);
				allMaterials.push_back(material);
			}
		}

		// get the node hierarchy and determine the parent-child relationship
		std::map<std::string, std::string> hierarchyMap;
		hierarchyMap.insert(std::pair<std::string, std::string>(scene->mRootNode->mName.C_Str(), ""));
		std::map<int, aiNode*> meshParents;
		recurseNode(scene->mRootNode, hierarchyMap, meshParents);
		std::map<std::string, aiMatrix4x4> jointTransformMap;
		std::map<std::string, int> boneLevelMap;
		std::vector<std::string> topmostNodeNames;
		int topMostNodeLevel = 999999999;

		std::map<int, SrMat> allMeshTransforms;

		SBSkeleton* lastSkeleton = NULL;

		if (1) //scene->HasMeshes())
		{
#if !defined (__ANDROID__) && !defined(SB_IPHONE) &&  !defined(__FLASHPLAYER__) && !defined(__native_client__)
			SbmDeformableMeshGPU* mesh = new SbmDeformableMeshGPU();
#else
			DeformableMesh* mesh = new DeformableMesh();
#endif
			LOG("CREATING MODEL WITH NAME %s", basename.c_str());
			mesh->setName(basename + extension);

			for (int m = 0; m < scene->mNumMeshes; m++)
			{
				LOG("FOUND MESH WITH %d VERTICES, %d FACES", scene->mMeshes[m]->mNumVertices, scene->mMeshes[m]->mNumFaces);

				SrModel* model = new SrModel();
				std::string sceneMeshName = scene->mMeshes[m]->mName.C_Str();
				if (sceneMeshName != "") // if mesh name exist, use it directly
				{
					//model->name = mesh->getName().c_str();
					model->name = scene->mMeshes[m]->mName.C_Str();
				}
				else if (scene->mNumMeshes == 1) // otherwise, if there is only one single mesh exist, use the file name
				{
					model->name = scene->mRootNode->mName.C_Str();
					scene->mMeshes[m]->mName = model->name;
				}
				else
				{
					//model->name = scene->mMeshes[m]->mName.C_Str();
					std::stringstream strstr;
					strstr << m;
					model->name = strstr.str().c_str();
					scene->mMeshes[m]->mName = model->name;
				}

			
				// get the node parent of the mesh
				std::map<int, aiNode*>::iterator meshParentIter = meshParents.find(m);
				
				aiMatrix4x4 meshTransform;
				if (meshParentIter != meshParents.end())
				{
					aiNode* curNode = (*meshParentIter).second;
					while (curNode)
					{
						if (!curNode->mTransformation.IsIdentity())
						{
							// traverse from this node to the root and multiply all matrices
							meshTransform = curNode->mTransformation * meshTransform;
						}
						curNode = curNode->mParent;
					}
				}
				SrMat meshMat;
				
				bool useTransform = !meshTransform.IsIdentity();

				if (useTransform)
				{
					// use this matrix to modify the vertices and normals of the mesh 
					// if there is a non-identity transform
					meshMat.setl1(meshTransform.a1,  meshTransform.b1,  meshTransform.c1,  meshTransform.d1); 
					meshMat.setl2(meshTransform.a2,  meshTransform.b2,  meshTransform.c2,  meshTransform.d2); 
					meshMat.setl3(meshTransform.a3,  meshTransform.b3,  meshTransform.c3,  meshTransform.d3); 
					meshMat.setl4(meshTransform.a4,  meshTransform.b4,  meshTransform.c4,  meshTransform.d4); 


				}
				meshMat.transpose();
				SrMat meshRot;
				SrQuat meshOrient(meshMat);
				meshOrient.normalize();

				allMeshTransforms.insert(std::pair<int, SrMat>(m, meshMat));

				// extract vertices and normals
				int numVertices = scene->mMeshes[m]->mNumVertices;
				model->V.size(numVertices);
				model->N.size(numVertices);
				model->T.size(numVertices);
				bool hasNormal = false;
				for (int v = 0; v < numVertices; v++)
				{

					model->V[v].x = scene->mMeshes[m]->mVertices[v].x;
					model->V[v].y = scene->mMeshes[m]->mVertices[v].y;
					model->V[v].z = scene->mMeshes[m]->mVertices[v].z;

					if (scene->mMeshes[m]->mNormals)
					{
						hasNormal = true;
						model->N[v].x = scene->mMeshes[m]->mNormals[v].x;
						model->N[v].y = scene->mMeshes[m]->mNormals[v].y;
						model->N[v].z = scene->mMeshes[m]->mNormals[v].z;
					}
					else
					{
						model->N[v].x = 0.0;
						model->N[v].y = 1.0;
						model->N[v].z = 0.0;
					}
					
					if (useTransform)
					{
						SrVec point = meshMat * model->V[v];
						SrVec normal = model->N[v] * meshOrient;
						model->V[v] = point;
						model->N[v] = normal;
					}
					

					if (scene->mMeshes[m]->HasTextureCoords(0))
					{ 
						//if (scene->mMeshes[m]->mNumUVComponents >= 2)
						{
							model->T[v].x = scene->mMeshes[m]->mTextureCoords[0][v].x;
							model->T[v].y = scene->mMeshes[m]->mTextureCoords[0][v].y;
							//LOG("%.2f %.2f", model->T[v].x, model->T[v].y);
						}
					}
				}
			
			

				// extract faces
				int numFaces = scene->mMeshes[m]->mNumFaces;
				model->F.size(numFaces);
				model->Fm.size(numFaces);
				model->Ft.size(numFaces);
				model->Fn.size(numFaces);
				for (int f = 0; f < numFaces; f++)
				{
					int numIndices = scene->mMeshes[m]->mFaces[f].mNumIndices;
					if (numIndices == 3)
					{
						model->F[f].a = scene->mMeshes[m]->mFaces[f].mIndices[0];
						model->F[f].b = scene->mMeshes[m]->mFaces[f].mIndices[1];
						model->F[f].c = scene->mMeshes[m]->mFaces[f].mIndices[2];

						model->Ft[f].a = model->F[f].a;
						model->Ft[f].b = model->F[f].b;
						model->Ft[f].c = model->F[f].c;

						model->Fn[f].a = model->F[f].a;
						model->Fn[f].b = model->F[f].b;
						model->Fn[f].c = model->F[f].c;
					}
					else if (numIndices == 2)
					{
						model->F[f].a = scene->mMeshes[m]->mFaces[f].mIndices[0];
						model->F[f].b = scene->mMeshes[m]->mFaces[f].mIndices[1];
						model->F[f].c = scene->mMeshes[m]->mFaces[f].mIndices[1];

						model->Ft[f].a = model->F[f].a;
						model->Ft[f].b = model->F[f].b;
						model->Ft[f].c = model->F[f].c;

						model->Fn[f].a = model->F[f].a;
						model->Fn[f].b = model->F[f].b;
						model->Fn[f].c = model->F[f].c;
					}
					else
					{
						LOG("Face has %d vertices, instead of 3.");
					}
				}

				// get the material
				int materialIndex = scene->mMeshes[m]->mMaterialIndex;
				if (materialIndex < allMaterials.size())
				{
					model->M.size(1);
					model->M[0] = SrMaterial(*allMaterials[materialIndex]);
					model->mtlnames.push(allMaterialNames[0].c_str());
					model->Fm.size(numFaces);
					for (int fm = 0; fm < numFaces; fm++)
					{
						model->Fm[fm] = 0;
					}
					if (allTextureSets[materialIndex].diffuseTextures.size() > 0)
					{
						model->mtlTextureNameMap.insert(std::pair<std::string, std::string>(allMaterialNames[0].c_str(), allTextureSets[materialIndex].diffuseTextures[0]));
					}
					if (allTextureSets[materialIndex].normalsTextures.size() > 0)
					{
						model->mtlNormalTexNameMap.insert(std::pair<std::string, std::string>(allMaterialNames[0].c_str(), allTextureSets[materialIndex].diffuseTextures[0]));
					}
					if (allTextureSets[materialIndex].specularTextures.size() > 0)
					{
						model->mtlSpecularTexNameMap.insert(std::pair<std::string, std::string>(allMaterialNames[0].c_str(), allTextureSets[materialIndex].diffuseTextures[0]));
					}

				}
				else
				{
					model->M.size(1);
					model->M[0] = SrMaterial();
					model->M[0].diffuse.r = 1.0;
					model->mtlnames.push("unknown");
				}

				if (!hasNormal)
					model->computeNormals();
				//if (!scene->mMeshes[m]->mNormals)
				//	model->computeNormals();

				SrSnModel* srSnModelStatic = new SrSnModel();
				srSnModelStatic->shape(*model);
				if (model->name.len() > 0)
					srSnModelStatic->shape().name = model->name;
				mesh->dMeshStatic_p.push_back(srSnModelStatic);
				srSnModelStatic->ref();

				SrSnModel* srSnModelDynamic = new SrSnModel();
				srSnModelDynamic->shape(*model);
				srSnModelDynamic->changed(true);
				srSnModelDynamic->visible(false);
				if (model->name.len() > 0)
					srSnModelDynamic->shape().name = model->name;
				mesh->dMeshDynamic_p.push_back(srSnModelDynamic);
				srSnModelDynamic->ref();

				// seed the bones needed for dynamic mesh
				int numBones = scene->mMeshes[m]->mNumBones;
				
				for (int b = 0; b < numBones; b++)
				{
					std::string boneName = scene->mMeshes[m]->mBones[b]->mName.C_Str();
					skeletonBones.insert(boneName);
				}

				
			}


			// there may be bones that are not directly controlled by the mesh (but directly
			// controlled by other bones) that need to be part of the skeleton
			for (std::set<std::string>::iterator bonesIter = skeletonBones.begin();
				 bonesIter != skeletonBones.end();
				 bonesIter++)
			{ 
				// only need to send bones from root (the following is inefficent since we are sending every bone)
				aiNode* node = scene->mRootNode->FindNode((*bonesIter).c_str());
				recurseAddMissing(node, skeletonBones);
			}



			// determine the skeleton
			// create a new skeleton hierarchy consisting only of bones used for models
			std::vector<std::string> currentNodes;
			std::map<std::string, std::string> reducedHierarchy;
			for (std::set<std::string>::iterator iter = skeletonBones.begin(); 
				 iter != skeletonBones.end();
				 iter++)
			{
				std::string child = (*iter);
				std::string parent = recurseFindParent(child, hierarchyMap, skeletonBones);
				if (parent == "")
					currentNodes.push_back(child); // keep track of nodes at/under the root
				reducedHierarchy.insert(std::pair<std::string, std::string>((*iter), parent));
				//LOG("%s -> %s", (*iter).c_str(), parent.c_str());
			}

			// determine the bones with empty parents
			std::vector<std::string> currentTopMostBones;
			for (std::map<std::string, std::string>::iterator iter = reducedHierarchy.begin();
				 iter != reducedHierarchy.end();
				 iter++)
			{
				if ((*iter).second == "")
					currentTopMostBones.push_back((*iter).first);
			}

			while (currentTopMostBones.size() > 1)
			{
				std::vector<std::string> tmpNodes;
				// continue to add to hierarchy until there is only one top node
				for (size_t t = 0; t < currentTopMostBones.size(); t++)
				{
					aiNode* node = scene->mRootNode->FindNode(currentTopMostBones[t].c_str());
					std::string nodeName = node->mName.C_Str();
					aiNode* parent = node->mParent;
					std::string parentName = parent->mName.C_Str();
					int pos = parentName.find("_$Assimp");
					while (pos != std::string::npos)
					{ // skip nodes automatically added by assimp
						parent = parent->mParent;
						parentName = parent->mName.C_Str();
						pos = parentName.find("_$Assimp");
					}

					std::set<std::string>::iterator iter = skeletonBones.find(parentName);
					if (iter == skeletonBones.end())
					{
						skeletonBones.insert(parentName);
						tmpNodes.push_back(parentName);
					}
				
					reducedHierarchy[currentTopMostBones[t]] = parentName;
				}
				currentTopMostBones = tmpNodes;
			}

			if (currentTopMostBones.size() == 1)
			{
				skeletonBones.insert(currentTopMostBones[0]);
				reducedHierarchy.insert(std::pair<std::string, std::string>(currentTopMostBones[0], ""));
			}

			for (std::set<std::string>::iterator iter = skeletonBones.begin(); 
				 iter != skeletonBones.end();
				 iter++)
			{
				// assign a node level
				aiNode* node = scene->mRootNode->FindNode((*iter).c_str());
				std::string boneName = node->mName.C_Str();
				int nodeLevel = recurseNodeLevel(scene->mRootNode, node, 0);
				boneLevelMap[boneName] = nodeLevel;
				if (nodeLevel < topMostNodeLevel)
				{
					topMostNodeLevel = nodeLevel;
					topmostNodeNames.clear();
					topmostNodeNames.push_back(boneName);
				}
				else if (nodeLevel == topMostNodeLevel)
				{
					topmostNodeNames.push_back(boneName);
				}
			}

			// get a list of all node trees that do not have parents


			//for (std::vector<std::string>::iterator iter = topmostNodeNames.begin();
			//	 iter != topmostNodeNames.end();
			//	 iter++)
			//{
			//	LOG("TOPMOST = %s", (*iter).c_str());
			//}

			
			if (reducedHierarchy.size() > 0)
			{
				SBSkeleton* skeleton = new SBSkeleton();
				skeleton->setName(basename + extension);

				SBJoint* rootJoint = NULL;
				
				std::map<std::string, SBJoint*> nameJointMap;
				for (std::set<std::string>::iterator iter = skeletonBones.begin(); 
					 iter != skeletonBones.end();
					 iter++)
				{
					SBJoint* joint = NULL;
					std::string jointName = (*iter);
					if (jointName == topmostNodeNames[0])
					{
						rootJoint = skeleton->createJoint(topmostNodeNames[0], NULL);
						joint = rootJoint;
					}
					else
					{
						joint = new SBJoint();
					}
					joint->setName(jointName);
					joint->setName(jointName);
					joint->name(jointName);
					joint->extName(jointName);
					joint->setSkeleton(skeleton);

					joint->rot_type ( SkJoint::TypeQuat );
					joint->quat()->activate();
					joint->pos()->limits(0, false);
					joint->pos()->limits(1, false);
					joint->pos()->limits(2, false);
					joint->pos()->value(0, 0);
					joint->pos()->value(1, 0);
					joint->pos()->value(2, 0);

					// get the local transformation
					aiNode* bone = scene->mRootNode->FindNode(jointName.c_str());
					aiMatrix4x4 transform = bone->mTransformation;
					SrMat mat(transform.a1, 
							  transform.b1, 
							  transform.c1, 
							  transform.d1, 
							  transform.a2, 
							  transform.b2, 
							  transform.c2, 
							  transform.d2, 
							  transform.a3, 
							  transform.b3, 
							  transform.c3, 
							  transform.d3, 
							  transform.a4, 
							  transform.b4, 
							  transform.c4, 
							  transform.d4);
					SrQuat quat(mat);
					joint->setPrerotation(quat);
					joint->setOffset(mat.get_translation());
					//LOG("%s PREROT (%4.2f, %4.2f, %4.2f, %4.2f)", jointName.c_str(), quat.w, quat.x, quat.y, quat.z);

					/*
					* RotationPivot
					* RotationOffset
					* PreRotation
					* PostRotation
					* ScalingPivot
					* ScalingOffset
					* Translation
					* Scaling
					* Rotation
					*/
					
					std::stringstream strstr;
					
					/*
					aiNode* n = NULL;
					strstr << jointName << "_$AssimpFbx$_RotationOffset";
					n = scene->mRootNode->FindNode(strstr.str().c_str());
					if (n)
					{
						LOG("FOUND %s", strstr.str().c_str());
					}
					strstr.str(std::string());

					strstr << jointName << "_$AssimpFbx$_PreRotation";
					n = scene->mRootNode->FindNode(strstr.str().c_str());
					if (n)
					{
						LOG("FOUND %s", strstr.str().c_str());
					}
					strstr.str(std::string());

					strstr << jointName << "_$AssimpFbx$_PostRotation";
					 n = scene->mRootNode->FindNode(strstr.str().c_str());
					if (n)
					{
						LOG("FOUND %s", strstr.str().c_str());
					}
					strstr.str(std::string());

					strstr << jointName << "_$AssimpFbx$_ScalingPivot";
					n = scene->mRootNode->FindNode(strstr.str().c_str());
					if (n)
					{
						LOG("FOUND %s", strstr.str().c_str());
					}
					strstr.str(std::string());

					strstr << jointName << "_$AssimpFbx$_ScalingOffset";
					n = scene->mRootNode->FindNode(strstr.str().c_str());
					if (n)
					{
						LOG("FOUND %s", strstr.str().c_str());
					}
					strstr.str(std::string());

					strstr << jointName << "_$AssimpFbx$_Translation";
					n = scene->mRootNode->FindNode(strstr.str().c_str());
					if (n)
					{
						LOG("FOUND %s", strstr.str().c_str());
					}
					strstr.str(std::string());

					strstr << jointName << "_$AssimpFbx$_Scaling";
					 n = scene->mRootNode->FindNode(strstr.str().c_str());
					if (n)
					{
						LOG("FOUND %s", strstr.str().c_str());
					}
					strstr.str(std::string());

					strstr << jointName << "_$AssimpFbx$_Rotation";
					n = scene->mRootNode->FindNode(strstr.str().c_str());
					if (n)
					{
						LOG("FOUND %s", strstr.str().c_str());
					}
					strstr.str(std::string());
					*/






					// does a translation node exist?
					SrVec offset;
					strstr.str(std::string());
					strstr << jointName << "_$AssimpFbx$_Translation";
					aiNode* translationNode = scene->mRootNode->FindNode(strstr.str().c_str());
					if (translationNode)
					{
						aiMatrix4x4 transmat = translationNode->mTransformation;
						SrMat translationMat(
							  transmat.a1, 
							  transmat.b1, 
							  transmat.c1, 
							  transmat.d1, 
							  transmat.a2, 
							  transmat.b2, 
							  transmat.c2, 
							  transmat.d2, 
							  transmat.a3, 
							  transmat.b3, 
							  transmat.c3, 
							  transmat.d3, 
							  transmat.a4, 
							  transmat.b4, 
							  transmat.c4, 
							  transmat.d4);
						offset = translationMat.get_translation();
						joint->offset(offset);
					}

					// does a PreRotation node exist?
					strstr.str(std::string());
					strstr << jointName << "_$AssimpFbx$_PreRotation";
					aiNode* preRotationNode = scene->mRootNode->FindNode(strstr.str().c_str());
					if (preRotationNode)
					{
						aiMatrix4x4 transmat = preRotationNode->mTransformation;
						SrMat preRotationMat(
							  transmat.a1, 
							  transmat.b1, 
							  transmat.c1, 
							  transmat.d1, 
							  transmat.a2, 
							  transmat.b2, 
							  transmat.c2, 
							  transmat.d2, 
							  transmat.a3, 
							  transmat.b3, 
							  transmat.c3, 
							  transmat.d3, 
							  transmat.a4, 
							  transmat.b4, 
							  transmat.c4, 
							  transmat.d4);
						SrQuat quat(preRotationMat);
						joint->setPrerotation(quat);
						//LOG("%s PREROT (%4.2f, %4.2f, %4.2f, %4.2f)", jointName.c_str(), quat.w, quat.x, quat.y, quat.z);
					}

					nameJointMap[(*iter)] = joint;
				}

				

				for (std::set<std::string>::iterator iter = skeletonBones.begin(); 
					 iter != skeletonBones.end();
					 iter++)
				{
					// get the joint
					std::map<std::string, SBJoint*>::iterator childIter = nameJointMap.find((*iter));

					// get the name of the parent joint
					std::map<std::string, std::string>::iterator parentNameIter = reducedHierarchy.find((*iter));

					SBJoint* parentJoint = NULL;
					// if the name of the parent is blank, this is the root joint
					if ((*parentNameIter).second == "")
					{
						// root joint, ignore
					}
					else
					{
						std::map<std::string, SBJoint*>::iterator parentIter = nameJointMap.find((*parentNameIter).second);
						parentJoint = (*parentIter).second;
						// connect the child to parent
						(*childIter).second->setParent(parentJoint);
						parentJoint->addChild((*childIter).second);
					}
				}
				skeleton->refresh_joints();
				lastSkeleton = skeleton;

				assets.push_back(skeleton);


				// add in skinweights
				for (int m = 0; m < scene->mNumMeshes; m++)
				{
					std::vector<std::vector<std::pair<int,float> > > jointNameIndexWeightMap;
					jointNameIndexWeightMap.resize(scene->mMeshes[m]->mNumVertices);
					SkinWeight* skinWeight = new SkinWeight();
					skinWeight->sourceMesh = scene->mMeshes[m]->mName.C_Str();
					int numBones = scene->mMeshes[m]->mNumBones;
					int nTotalCount = 0;
					for (int b = 0; b < numBones; b++)
					{
						std::string boneName = scene->mMeshes[m]->mBones[b]->mName.C_Str();
						skinWeight->infJointName.push_back(boneName);
						// find the corresponding SBJoint
						SBJoint* joint = skeleton->getJointByName(boneName);
						//skinWeight->infJoint.push_back(joint);
						int numWeights = scene->mMeshes[m]->mBones[b]->mNumWeights;
						aiMatrix4x4 transmat =  scene->mMeshes[m]->mBones[b]->mOffsetMatrix;
						SrMat mat(
							  transmat.a1, 
							  transmat.b1, 
							  transmat.c1, 
							  transmat.d1, 
							  transmat.a2, 
							  transmat.b2, 
							  transmat.c2, 
							  transmat.d2, 
							  transmat.a3, 
							  transmat.b3, 
							  transmat.c3, 
							  transmat.d3, 
							  transmat.a4, 
							  transmat.b4, 
							  transmat.c4, 
							  transmat.d4);

						// if there was a mesh transform, get it and extract the scale
#if 1
						std::map<int, SrMat>::iterator iter = allMeshTransforms.find(m);
						if (iter != allMeshTransforms.end())
						{
							SrMat& meshTransform = (*iter).second;
							SrVec translation = mat.get_translation();
							SrVec scaledTranslation(	translation.x * meshTransform.e11(), 
														translation.y * meshTransform.e22(),
														translation.z * meshTransform.e33());
							mat.set_translation(scaledTranslation);
						}
#endif
						skinWeight->bindPoseMat.push_back(mat);
						
						
				
						int numVerts = scene->mMeshes[m]->mBones[b]->mNumWeights;
						//skinWeight->numInfJoints.push_back(numVerts);
						for (int v = 0; v < numVerts; v++)
						{
							int vIdx = scene->mMeshes[m]->mBones[b]->mWeights[v].mVertexId;
							jointNameIndexWeightMap[vIdx].push_back(std::pair<int,float>(b,scene->mMeshes[m]->mBones[b]->mWeights[v].mWeight));
							//skinWeight->bindWeight.push_back(scene->mMeshes[m]->mBones[b]->mWeights[v].mWeight);
							//skinWeight->weightIndex.push_back(scene->mMeshes[m]->mBones[b]->mWeights[v].mVertexId);
							//skinWeight->jointNameIndex.push_back(b);
						}
						nTotalCount += numVerts;
					}
					skinWeight->bindWeight.resize(nTotalCount);
					skinWeight->weightIndex.resize(nTotalCount);
					skinWeight->jointNameIndex.resize(nTotalCount);
					int idxCount = 0;
					for (unsigned int v=0;v<jointNameIndexWeightMap.size();v++)
					{
						std::vector<std::pair<int,float> >& idxWeightList = jointNameIndexWeightMap[v];
						skinWeight->numInfJoints.push_back(idxWeightList.size());
						for (unsigned int idx = 0; idx < idxWeightList.size(); idx++)
						{
							std::pair<int,float>& idxWeight = idxWeightList[idx];
							skinWeight->bindWeight[idxCount] = idxWeight.second;
							skinWeight->weightIndex[idxCount] = idxCount;
							skinWeight->jointNameIndex[idxCount] = idxWeight.first;
							idxCount++;
						}
					}
					mesh->skinWeights.push_back(skinWeight);
				}
				/*
x	std::vector<std::string>	infJointName;	// name array
x	std::vector<SkJoint*>		infJoint;         // corresponding joint for each infJointName
x	std::vector<float>			bindWeight;		// weight array
x	std::vector<SrMat>			bindPoseMat;	// each joint, binding pose transformation matrix
	SrMat						bindShapeMat;	// overall bind shape transformation matrix
x	std::string					sourceMesh;		// skin Source Name
x	std::vector<unsigned int>	numInfJoints;	// number of influenced joints for very vertex
x	std::vector<unsigned int>	weightIndex;	// looking up the weight according to this index
	std::vector<unsigned int>	jointNameIndex;	// looking up the joint name according to this index
				*/
			}
			
			
			// check for animations
			int numMotionChannels = 0;
			int numAnimations = scene->mNumAnimations;
			std::stringstream strstr;
			for (int a = 0; a < numAnimations; a++)
			{
				aiAnimation* animation = scene->mAnimations[a];
				SBMotion* motion = new SBMotion();
				strstr.str("");
				
				motion->setName(basename + extension + animation->mName.C_Str());
				if (motion->getName() == "")
				{
					strstr << mesh->getName() << a;
					motion->setName(strstr.str());
				}
				

				// first pass, collect the channels
				int numChannels = animation->mNumChannels;
				for (int c = 0; c < numChannels; c++)
				{
					aiNodeAnim* nodeAnim = animation->mChannels[c];
					std::string channelName = nodeAnim->mNodeName.C_Str();

					if (channelName.find("_$AssimpFbx$_") != std::string::npos)
					{
						// ignore keys on auto-created bones
						// this may cause problems... :(
						//continue;
					}


					if (nodeAnim->mNumPositionKeys > 0)
					{
						motion->addChannel(channelName, "XPos");
						motion->addChannel(channelName, "YPos");
						motion->addChannel(channelName, "ZPos");
						numMotionChannels += 3;
					}
					if (nodeAnim->mNumRotationKeys > 0)
					{
						motion->addChannel(channelName, "Quat");
						numMotionChannels += 1;
					}
					if (nodeAnim->mNumScalingKeys > 0)
					{
						// ignore scaling in animations
					}
				}			
				
				// second pass, create the animations
				for (int c = 0; c < numChannels; c++)
				{
					aiNodeAnim* nodeAnim = animation->mChannels[c];
					std::string channelName = nodeAnim->mNodeName.C_Str();

					if (channelName.find("_$AssimpFbx$_") != std::string::npos)
					{
						// ignore keys on auto-created bones
						// this may cause problems... :(
						//continue;
					}

					// get the joint and subtract the joint offset
					SBJoint* joint = NULL;
					SrVec offset;
					if (lastSkeleton)
					{
						joint = lastSkeleton->getJointByName(channelName);
						if (joint)
							offset = joint->getOffset();
					}

					int numPosKeys = nodeAnim->mNumPositionKeys;
					for (int k = 0; k < numPosKeys; k++)
					{
						// translation
						aiVectorKey& key = nodeAnim->mPositionKeys[k];
						motion->addKeyFrameChannel(channelName, "XPos", key.mTime / animation->mTicksPerSecond, key.mValue.x - offset.x);
						motion->addKeyFrameChannel(channelName, "YPos", key.mTime / animation->mTicksPerSecond, key.mValue.y - offset.y);
						motion->addKeyFrameChannel(channelName, "ZPos", key.mTime / animation->mTicksPerSecond, key.mValue.z - offset.z);
					}
					
					int numRotKeys = nodeAnim->mNumRotationKeys;
					for (int k = 0; k < numRotKeys; k++)
					{
						aiQuatKey& key = nodeAnim->mRotationKeys[k];
						motion->addKeyFrameQuat(channelName, "Quat", key.mTime / animation->mTicksPerSecond, SrQuat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z));
					}
					if (nodeAnim->mNumScalingKeys > 0)
					{
						// ignore scaling in animations
					}
				}

				// bake the animation
				motion->bakeFrames(animation->mTicksPerSecond);

				assets.push_back(motion);
			}	

			if (scene->HasMeshes())
				assets.push_back(mesh);
			else
				delete mesh;
		}
		
 		
		  // extract animations

	  }
	} catch (std::exception& e) {
		LOG("Could not load asset %s: %s", path.c_str(), e.what());
	}
	return assets;

}



};

#endif
