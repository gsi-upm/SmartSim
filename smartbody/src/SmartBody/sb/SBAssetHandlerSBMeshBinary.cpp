#include "SBAssetHandlerSBMeshBinary.h"
#include <vhcl.h>
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sbm/sbm_deformable_mesh.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>

namespace SmartBody {

	SBAssetHandlerSBMeshBinary::SBAssetHandlerSBMeshBinary()
	{
		assetTypes.push_back("smb");
		assetTypes.push_back("dmb");
	}

	SBAssetHandlerSBMeshBinary::~SBAssetHandlerSBMeshBinary()
	{
	}

	std::vector<SBAsset*> SBAssetHandlerSBMeshBinary::getAssets(const std::string& path)
	{
		std::vector<SBAsset*> assets;

		std::string convertedPath = checkPath(path);
		if (convertedPath == "")
			return assets;

		boost::filesystem::path p(convertedPath);
		std::string fileName = boost::filesystem::basename(p);
		std::string extension =  boost::filesystem::extension(p);

#if !defined (__ANDROID__) && !defined(SB_IPHONE) &&  !defined(__FLASHPLAYER__) && !defined(__native_client__)
		SbmDeformableMeshGPU* mesh = new SbmDeformableMeshGPU();
#else
		DeformableMesh* mesh = new DeformableMesh();
#endif
		if (extension == ".smb")	// load in 
		{
			bool ok = mesh->readFromSmb(convertedPath);
			if (ok)
				assets.push_back(mesh);
			else
				delete mesh;
		}
		else if (extension == ".dmb")
		{
			bool ok = mesh->readFromDmb(convertedPath);
			if (ok)
				assets.push_back(mesh);
			else
				delete mesh;
		}
		return assets;
	}

};
