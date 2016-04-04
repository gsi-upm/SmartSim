#include "SBAssetHandlerObj.h"
#include <vhcl.h>
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sr/sr_model.h>

namespace SmartBody {

SBAssetHandlerObj::SBAssetHandlerObj()
{
#ifdef SB_NO_ASSIMP
	assetTypes.push_back("obj");
#endif
}

SBAssetHandlerObj::~SBAssetHandlerObj()
{
}

std::vector<SBAsset*> SBAssetHandlerObj::getAssets(const std::string& path)
{
	std::vector<SBAsset*> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath == "")
		return assets;

	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename( p );
	std::string extension =  boost::filesystem::extension( p );

	SrModel* model = new SrModel();
	bool loadSuccess = model->import_obj(convertedPath.c_str());	
	if (!loadSuccess)
	{
		delete model;
	}
	else
	{
#if !defined (__ANDROID__) && !defined(SB_IPHONE) &&  !defined(__FLASHPLAYER__) && !defined(__native_client__)
		SbmDeformableMeshGPU* mesh = new SbmDeformableMeshGPU();
#else
		DeformableMesh* mesh = new DeformableMesh();
#endif
		mesh->setName(fileName + extension);
		
		//float factor = 1.0f;
		//for (int j = 0; j < model->V.size(); j++)
		//{
		//	model->V[j] *= factor;
		//}
		
		if (model->Fn.size() == 0)
		{
			model->computeNormals();
		}

		SrSnModel* srSnModelStatic = new SrSnModel();
		srSnModelStatic->shape(*model);
		srSnModelStatic->shape().name = model->name;
		mesh->dMeshStatic_p.push_back(srSnModelStatic);
		srSnModelStatic->ref();

		SrSnModel* srSnModelDynamic = new SrSnModel();
		srSnModelDynamic->shape(*model);
		srSnModelDynamic->changed(true);
		srSnModelDynamic->visible(false);
		srSnModelDynamic->shape().name = model->name;
		mesh->dMeshDynamic_p.push_back(srSnModelDynamic);
		srSnModelDynamic->ref();

		delete model;

		assets.push_back(mesh);
	}

	return assets;
}

};
