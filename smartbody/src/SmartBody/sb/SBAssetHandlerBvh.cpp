#include "SBAssetHandlerBvh.h"
#include <vhcl.h>
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sbm/ParserBVH.h>


namespace SmartBody {

SBAssetHandlerBvh::SBAssetHandlerBvh()
{
	assetTypes.push_back("bvh");
}

SBAssetHandlerBvh::~SBAssetHandlerBvh()
{
}

std::vector<SBAsset*> SBAssetHandlerBvh::getAssets(const std::string& path)
{
	std::vector<SBAsset*> assets;

	std::string convertedPath = checkPath(path);
	if (convertedPath == "")
		return assets;
	
	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename( p );
	std::string extension =  boost::filesystem::extension( p );
	FILE* myfile = fopen(convertedPath.c_str(), "rt");
	SrInput input(myfile);


	std::ifstream filestream(convertedPath.c_str());

	SmartBody::SBSkeleton* skeleton = new SmartBody::SBSkeleton();
	double scale = 1.0;
	if (SmartBody::SBScene::getScene()->getAttribute("globalSkeletonScale"))
		scale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalSkeletonScale");

	SmartBody::SBMotion* motion = new SmartBody::SBMotion();

	bool ok = ParserBVH::parse(*skeleton, *motion, convertedPath, filestream, float(scale));
	if (!ok)
	{
		delete motion;
		delete skeleton;
		LOG("Could not .bvh file %s", convertedPath.c_str());
	}
	else
	{
		motion->setName(fileName + extension);
		skeleton->setName(fileName + extension);
		skeleton->setFileName(convertedPath);
		assets.push_back(motion);
		assets.push_back(skeleton);
	}
	
	return assets;
}

};
