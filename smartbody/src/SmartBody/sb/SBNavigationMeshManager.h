#ifndef _SBNAVIGATIONMESHMANAGER_H_
#define _SBNAVIGATIONMESHMANAGER_H_

#include <sb/SBTypes.h>
#include <string>
#include <map>
#include <vector>

namespace SmartBody {

class SBNavigationMesh;

typedef std::pair<std::string,std::string> StringPair;

class SBNavigationMeshManager
{
	public:
		SBAPI SBNavigationMeshManager();
		SBAPI ~SBNavigationMeshManager();	

		SBAPI SBNavigationMesh* createNavigationMesh(const std::string& naviMeshName);
		SBAPI SBNavigationMesh* getNavigationMesh(const std::string& naviMeshName);		
		SBAPI std::vector<std::string> getNavigationMeshNames();

	protected:
		std::map<std::string, SBNavigationMesh*> _navigationMeshMap;
};

}
#endif