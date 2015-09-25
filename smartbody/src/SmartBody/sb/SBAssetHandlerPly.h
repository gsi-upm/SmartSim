#ifndef _SBASSETHANDLERSPLY_H_
#define _SBASSETHANDLERSPLY_H_

#include <sb/SBTypes.h>
#include <vector>
#include <sb/SBAsset.h>
#include <sb/SBAssetHandler.h>


namespace SmartBody {

class SBAssetHandlerPly: public SBAssetHandler
{
	public:
		SBAPI SBAssetHandlerPly();
		SBAPI virtual ~SBAssetHandlerPly();

		SBAPI virtual std::vector<SBAsset*> getAssets(const std::string& path);

};


}

#endif