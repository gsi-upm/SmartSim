#ifndef _SBASSETHANDLERAMC_H_
#define _SBASSETHANDLERAMC_H_

#include <sb/SBTypes.h>
#include <vector>
#include <sb/SBAsset.h>
#include <sb/SBAssetHandler.h>

namespace SmartBody {

class SBAssetHandlerAmc : public SBAssetHandler
{
	public:
		SBAPI SBAssetHandlerAmc();
		SBAPI virtual ~SBAssetHandlerAmc();

		SBAPI virtual std::vector<SBAsset*> getAssets(const std::string& path);

};


}

#endif