#ifndef _SBASSETHANDLERSBMESHBINARY_H_
#define _SBASSETHANDLERSBMESHBINARY_H_


#include <sb/SBTypes.h>
#include <vector>
#include <sb/SBAsset.h>
#include <sb/SBAssetHandler.h>


namespace SmartBody 
{
	class SBAssetHandlerSBMeshBinary : public SBAssetHandler
	{
	public:
		SBAPI SBAssetHandlerSBMeshBinary();
		SBAPI virtual ~SBAssetHandlerSBMeshBinary();

		SBAPI virtual std::vector<SBAsset*> getAssets(const std::string& path);

	};
}

#endif