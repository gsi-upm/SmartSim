#ifndef _SBASSETHANDLERSASF_H_
#define _SBASSETHANDLERASF_H_

#include <sb/SBTypes.h>
#include <vector>
#include <sb/SBAsset.h>
#include <sb/SBAssetHandler.h>


namespace SmartBody {

class SBAssetHandlerAsf : public SBAssetHandler
{
	public:
		SBAPI SBAssetHandlerAsf();
		SBAPI virtual ~SBAssetHandlerAsf();

		SBAPI virtual std::vector<SBAsset*> getAssets(const std::string& path);

};


}

#endif