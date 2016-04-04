/*  This file is part of the Pinocchio automatic rigging library.
    Copyright (C) 2007 Ilya Baran (ibaran@mit.edu)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

// Pinocchio.cpp : Defines the entry point for the DLL application.

#include "PinnocchioCallback.h"

PinnocchioCallBackManager* PinnocchioCallBackManager::_singleton = NULL;


PinnocchioCallBackManager* PinnocchioCallBackManager::singletonPtr()
{
	if (!_singleton)
		_singleton = new PinnocchioCallBackManager();
	return _singleton;
}

void PinnocchioCallBackManager::destroy_singleton()
{
	if( _singleton )
		delete _singleton;
	_singleton = NULL;
}

PinnocchioCallBackManager& PinnocchioCallBackManager::singleton()
{
	return *singletonPtr();
}

void PinnocchioCallBackManager::setCallBack( PinnocchioCallBack* pinoCallBack )
{
	callBack = pinoCallBack;
}

void PinnocchioCallBackManager::runCallBack()
{
	if (callBack)
		callBack->callbackFunc();
}

PinnocchioCallBack* PinnocchioCallBackManager::getCallBack()
{
	return callBack;
}

PinnocchioCallBackManager::PinnocchioCallBackManager()
{
	callBack = NULL;
}

PinnocchioCallBackManager::~PinnocchioCallBackManager()
{

}
