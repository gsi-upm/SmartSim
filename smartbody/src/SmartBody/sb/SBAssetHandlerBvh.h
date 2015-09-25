#ifndef _SBASSETHANDLERBVH_H_
#define _SBASSETHANDLERBVH_H_

#include <sb/SBTypes.h>
#include <vector>
#include <sb/SBAsset.h>
#include <sb/SBAssetHandler.h>


namespace SmartBody {

class SBAssetHandlerBvh : public SBAssetHandler
{
	public:
		SBAPI SBAssetHandlerBvh();
		SBAPI virtual ~SBAssetHandlerBvh();

		SBAPI virtual std::vector<SBAsset*> getAssets(const std::string& path);

};


}

#endif