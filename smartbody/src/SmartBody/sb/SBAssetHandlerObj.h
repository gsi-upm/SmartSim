#ifndef _SBASSETHANDLERSOBJ_H_
#define _SBASSETHANDLERSOBJ_H_

#include <sb/SBTypes.h>
#include <vector>
#include <sb/SBAsset.h>
#include <sb/SBAssetHandler.h>


namespace SmartBody {

class SBAssetHandlerObj: public SBAssetHandler
{
	public:
		SBAPI SBAssetHandlerObj();
		SBAPI virtual ~SBAssetHandlerObj();

		SBAPI virtual std::vector<SBAsset*> getAssets(const std::string& path);

};


}

#endif