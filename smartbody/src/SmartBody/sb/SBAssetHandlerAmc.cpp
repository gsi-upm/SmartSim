#include "SBAssetHandlerAmc.h"
#include <vhcl.h>
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBAssetManager.h>
#include <sbm/ParserASFAMC.h>

namespace SmartBody {

SBAssetHandlerAmc::SBAssetHandlerAmc()
{
	assetTypes.push_back("amc");
}

SBAssetHandlerAmc::~SBAssetHandlerAmc()
{
}

std::vector<SBAsset*> SBAssetHandlerAmc::getAssets(const std::string& path)
{
	std::vector<SBAsset*> assets;

	boost::filesystem::path pathname(path);
	if( !boost::filesystem::exists( pathname ) )
	{
#if (BOOST_VERSION > 104400)
		LOG("Asset path \"%s\" not found.",  pathname.string().c_str());
#else
		LOG("Asset path \"%s\" not found.", pathname.native_file_string().c_str());
#endif
		return assets;
	}

	if( boost::filesystem::is_directory( pathname ) ) // path indicates a directory
	{
		#if (BOOST_VERSION > 104400)
		LOG("Asset path \"%s\" is a directory.",  pathname.string().c_str());
#else
		LOG("Asset path \"%s\" is a directory.", pathname.native_file_string().c_str());
#endif
		return assets;
	}

	std::string convertedPath = pathname.string();
#ifdef WIN32
	boost::replace_all(convertedPath, "\\", "/");
#endif

	boost::filesystem::path p(convertedPath);
	std::string fileName = boost::filesystem::basename( p );
	std::string extension =  boost::filesystem::extension( p );


	std::ifstream filestream(convertedPath.c_str());

	double scale = 1.0;
	if (SmartBody::SBScene::getScene()->getAttribute("globalMotionScale"))
		scale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalMotionScale");

	SmartBody::SBSkeleton* skeleton = NULL;
	// find a skeleton with a prefix that matches the first part of this motion file
	int pos = fileName.find_first_of("_");
	if (pos != std::string::npos)
	{
		std::string skelPrefix = fileName.substr(0, pos);
		// find a skeleton with that name
		std::string skelName = skelPrefix + ".asf";
		skeleton = SmartBody::SBScene::getScene()->getAssetManager()->getSkeleton(skelName);
		if (!skeleton)
		{
			LOG("No skeleton with name %s could be find to accompany .asf file, %s not loaded.", skelName.c_str(), convertedPath.c_str());
			return assets;
		}
	}

	if (!skeleton)
	{
		LOG(".amc files need accompanying .asf skeleton, %s not loaded.", convertedPath.c_str());
		return assets;
	}

	
	SmartBody::SBMotion* motion = new SmartBody::SBMotion();
	bool ok = ParserASFAMC::parseAmc(*motion, skeleton, filestream, float(scale));
	if (ok)
	{
		motion->setName(fileName);
		assets.push_back(motion);
	}
	else
	{
		delete motion;
		LOG("Could not load .amc file %s", convertedPath.c_str());
	}

	return assets;
}

};
