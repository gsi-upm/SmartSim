#include "SBAssetHandlerAsf.h"
#include <vhcl.h>
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>
#include <sbm/ParserASFAMC.h>

namespace SmartBody {

SBAssetHandlerAsf::SBAssetHandlerAsf()
{
	assetTypes.push_back("asf");
}

SBAssetHandlerAsf::~SBAssetHandlerAsf()
{
}

std::vector<SBAsset*> SBAssetHandlerAsf::getAssets(const std::string& path)
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
	if (SmartBody::SBScene::getScene()->getAttribute("globalSkeletonScale"))
		scale = SmartBody::SBScene::getScene()->getDoubleAttribute("globalSkeletonScale");

	SmartBody::SBSkeleton* skeleton = new SmartBody::SBSkeleton();
	bool ok = ParserASFAMC::parseAsf(*skeleton, filestream, float(scale));
	if (ok)
	{
		skeleton->setName(fileName + extension);
		skeleton->setFileName(convertedPath);
		assets.push_back(skeleton);
	}
	else
	{
		delete skeleton;
		LOG("Could not load skeleton from file %s", convertedPath.c_str());
	}

	return assets;
}

};
