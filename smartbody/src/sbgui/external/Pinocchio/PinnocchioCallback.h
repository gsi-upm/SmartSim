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

#ifndef PINNOCCHIOCALLBACK_H_INCLUDED
#define PINNOCCHIOCALLBACK_H_INCLUDED
#include <math.h>
#include <cstddef>
#include "Pinocchio.h"
#include "mesh.h"
#include "attachment.h"

class PinnocchioCallBack
{
public:
	PINOCCHIO_API PinnocchioCallBack() {}
	PINOCCHIO_API ~PinnocchioCallBack() {}
	PINOCCHIO_API virtual void callbackFunc() = 0;
	PINOCCHIO_API virtual void skeletonCompleteCallBack(std::vector<Vector3>& embedding) = 0;
};

class PinnocchioCallBackManager
{
public:
	PINOCCHIO_API PinnocchioCallBackManager();
	PINOCCHIO_API ~PinnocchioCallBackManager();
	PINOCCHIO_API void setCallBack(PinnocchioCallBack* pinoCallBack);
	PINOCCHIO_API PinnocchioCallBack* getCallBack();
	PINOCCHIO_API void runCallBack();
	PINOCCHIO_API static PinnocchioCallBackManager* _singleton;
public:
	PINOCCHIO_API static PinnocchioCallBackManager& singleton();

	PINOCCHIO_API static PinnocchioCallBackManager* singletonPtr();

	PINOCCHIO_API static void destroy_singleton();	
protected:
	PinnocchioCallBack* callBack;
};
#endif //UTILS_H_INCLUDED
