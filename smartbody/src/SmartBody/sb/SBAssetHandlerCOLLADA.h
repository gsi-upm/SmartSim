#ifndef _SBASSETHANDLERCOLLADA_H_

#include <sb/SBTypes.h>
#include <vector>
#include <sb/SBAsset.h>
#include <sb/SBAssetHandler.h>

namespace SmartBody {

class SBAssetHandlerCOLLADA : public SBAssetHandler
{
	public:
		SBAPI SBAssetHandlerCOLLADA();
		SBAPI virtual ~SBAssetHandlerCOLLADA();

		SBAPI virtual std::vector<SBAsset*> getAssets(const std::string& path);

};


}

#endif