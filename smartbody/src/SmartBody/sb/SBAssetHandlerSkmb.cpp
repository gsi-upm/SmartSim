#include "SBAssetHandlerSkmb.h"
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

SBAssetHandlerSkmb::SBAssetHandlerSkmb()
{
	assetTypes.push_back("skmb");
}

SBAssetHandlerSkmb::~SBAssetHandlerSkmb()
{
}

std::vector<SBAsset*> SBAssetHandlerSkmb::getAssets(const std::string& path)
{
	std::vector<SBAsset*> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath == "")
		return assets;

	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename( p );
	std::string extension =  boost::filesystem::extension( p );

	SBMotion* motion = new SBMotion();
	bool ok = motion->readFromSkb(convertedPath);
	if (ok)
		assets.push_back(motion);
	else
		delete motion;

	return assets;
}

};
