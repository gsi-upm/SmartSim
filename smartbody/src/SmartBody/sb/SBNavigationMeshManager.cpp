#include "SBNavigationMeshManager.h"
#include <sb/SBNavigationMesh.h>


namespace SmartBody {

	
SBAPI SBNavigationMeshManager::SBNavigationMeshManager()
{

}

SBAPI SBNavigationMeshManager::~SBNavigationMeshManager()
{

}

SBNavigationMesh* SBNavigationMeshManager::createNavigationMesh( const std::string& naviMeshName )
{
	SBNavigationMesh* naviMesh = NULL;
	if (_navigationMeshMap.find(naviMeshName) != _navigationMeshMap.end())
	{
		LOG("Navigation mesh '%s' already exists. Returns the copy in system. ", naviMeshName.c_str());
		return _navigationMeshMap[naviMeshName];
	}

	naviMesh = new SBNavigationMesh();
	naviMesh->setName(naviMeshName);
	_navigationMeshMap[naviMeshName] = naviMesh;
	return naviMesh;
}

SBNavigationMesh* SBNavigationMeshManager::getNavigationMesh( const std::string& naviMeshName )
{
	std::map<std::string, SBNavigationMesh*>::iterator iter = _navigationMeshMap.find(naviMeshName);
	SBNavigationMesh* naviMesh = NULL;
	if (iter != _navigationMeshMap.end())
		naviMesh = iter->second;
	return naviMesh;
}

std::vector<std::string> SBNavigationMeshManager::getNavigationMeshNames()
{
	std::vector<std::string> ret;

	for(std::map<std::string, SBNavigationMesh*>::iterator iter = _navigationMeshMap.begin();
		iter != _navigationMeshMap.end();
		iter++)
	{
		ret.push_back(std::string(iter->first));
	}

	return ret;	
}

}
