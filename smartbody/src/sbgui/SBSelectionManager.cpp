#include "SBSelectionManager.h"
#include <sb/SBObject.h>
#include <sb/SBService.h>
#include <sb/SBMotion.h>
#include <sb/SBSkeleton.h>
#include <sb/SBCharacter.h>
#include <sb/SBPawn.h>
#include <sb/SBScene.h>
#include <sb/SBController.h>
#include <sb/SBServiceManager.h>
#include <sb/SBAssetManager.h>

SBSelectionManager* SBSelectionManager::_selectionManager = NULL;

SelectionListener::SelectionListener()
{
	SBSelectionManager::getSelectionManager()->addSelectionListener(this);
}

SelectionListener::~SelectionListener()
{
	SBSelectionManager::getSelectionManager()->removeSelectionListener(this);
}

void SelectionListener::OnSelect(const std::string& value)
{
}

SBSelectionManager* SBSelectionManager::getSelectionManager()
{
	if (!_selectionManager)
	{
		_selectionManager = new SBSelectionManager();
	}
	return _selectionManager;
}

SBSelectionManager::SBSelectionManager()
{
	_currentSelection = "";
}

SBSelectionManager::~SBSelectionManager()
{
}

void SBSelectionManager::select(const std::string& value)
{
	for (std::vector<SelectionListener*>::iterator iter = _selectionListeners.begin();
		iter != _selectionListeners.end(); 
		iter++)
	{
		(*iter)->OnSelect(value);
	}
}

void SBSelectionManager::addSelectionListener(SelectionListener* listener)
{
	for (std::vector<SelectionListener*>::iterator iter = _selectionListeners.begin();
		 iter != _selectionListeners.end(); 
		 iter++)
	{
		if ((*iter) == listener)
			return;
	}
	_selectionListeners.push_back(listener);
}

void SBSelectionManager::removeSelectionListener(SelectionListener* listener)
{
	for (std::vector<SelectionListener*>::iterator iter = _selectionListeners.begin();
		 iter != _selectionListeners.end(); 
		 iter++)
	{
		if ((*iter) == listener)
		{
			_selectionListeners.erase(iter);
			return;
		}
	}
}

const std::string& SBSelectionManager::getCurrentSelection()
{
	return _currentSelection;
}


