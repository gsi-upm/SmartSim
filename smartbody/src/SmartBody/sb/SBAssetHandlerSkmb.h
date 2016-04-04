#ifndef _SBASSETHANDLERSSKMB_H_
#define _SBASSETHANDLERSSKMB_H_

#include <sb/SBTypes.h>
#include <vector>
#include <sb/SBAsset.h>
#include <sb/SBAssetHandler.h>


namespace SmartBody {

class SBAssetHandlerSkmb: public SBAssetHandler
{
	public:
		SBAPI SBAssetHandlerSkmb();
		SBAPI virtual ~SBAssetHandlerSkmb();

		SBAPI virtual std::vector<SBAsset*> getAssets(const std::string& path);

};


}

#endif