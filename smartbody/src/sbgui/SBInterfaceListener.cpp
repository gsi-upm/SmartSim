#include "SBInterfaceListener.h"
#include <sb/SBNavigationMesh.h>
#include <sb/SBScene.h>
#include "SBSelectionManager.h"
#include <sr/sr_plane.h>
#include <sr/sr_camera.h>

SBInterfaceListener::SBInterfaceListener()
{
}

SBInterfaceListener::~SBInterfaceListener()
{
}

void SBInterfaceListener::onStart()
{
}

bool SBInterfaceListener::onMouseClick(int x, int y, int button)
{
	return false;
}

bool SBInterfaceListener::onMouseMove(int x, int y)
{
	return false;
}

bool SBInterfaceListener::onMouseRelease(int x, int y, int button)
{
	return false;
}

bool SBInterfaceListener::onMouseDrag(int x, int y)
{
	return false;
}

bool SBInterfaceListener::onKeyboardPress(char c)
{
	return false;
}

bool SBInterfaceListener::onKeyboardRelease(char c)
{
	return false;
}

void SBInterfaceListener::onEnd()
{
}





SBInterfaceManager* SBInterfaceManager::_interfaceManager = NULL;

SBInterfaceManager::SBInterfaceManager()
{
}

SBInterfaceManager::~SBInterfaceManager()
{
	std::vector<SBInterfaceListener*> listeners = getInterfaceListeners();
	for (size_t l = 0; l < listeners.size(); l++)
	{
		this->removeInterfaceListener(listeners[l]);
	}
}

void SBInterfaceManager::addInterfaceListener(SBInterfaceListener* listener)
{
	for (std::vector<SBInterfaceListener*>::iterator iter = _interfaceListeners.begin();
		 iter != _interfaceListeners.end(); 
		 iter++)
	{
		if ((*iter) == listener)
			return;
	}
	_interfaceListeners.push_back(listener);
	listener->onStart();
}

void SBInterfaceManager::removeInterfaceListener(SBInterfaceListener* listener)
{
	for (std::vector<SBInterfaceListener*>::iterator iter = _interfaceListeners.begin();
		 iter != _interfaceListeners.end(); 
		 iter++)
	{
		if ((*iter) == listener)
		{
			(*iter)->onEnd();
			_interfaceListeners.erase(iter);
			return;
		}
	}
}

std::vector<SBInterfaceListener*> SBInterfaceManager::getInterfaceListeners()
{
	std::vector<SBInterfaceListener*> listeners;
	for (std::vector<SBInterfaceListener*>::iterator iter = _interfaceListeners.begin();
		 iter != _interfaceListeners.end(); 
		 iter++)
	{
		listeners.push_back(*iter);
	}

	return listeners;
}

SrVec SBInterfaceManager::convertScreenSpaceTo3D(int x, int y, SrVec ground, SrVec upVector)
{
	SrVec p1;
	SrVec p2;
	SmartBody::SBScene::getScene()->getActiveCamera()->get_ray(x, y, p1, p2);
	bool intersectGround = true;
	SrVec dest, src;				
	SrPlane plane(ground, upVector);
	dest = plane.intersect(p1, p2);
	return dest;
}

std::string SBInterfaceManager::getSelectedObject()
{
	return SBSelectionManager::getSelectionManager()->getCurrentSelection();
}

SBInterfaceManager* SBInterfaceManager::getInterfaceManager()
{
	if (SBInterfaceManager::_interfaceManager == NULL)
	{
		SBInterfaceManager::_interfaceManager = new SBInterfaceManager();
	}

	return SBInterfaceManager::_interfaceManager;
}



