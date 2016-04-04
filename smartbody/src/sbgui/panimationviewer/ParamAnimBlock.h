/*
 *  ParamAnimBlock.h - part of SmartBody-lib's Test Suite
 *  Copyright (C) 2009  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Yuyu Xu, USC
 */

#ifndef _PARAMANIMBLOCK_
#define _PARAMANIMBLOCK_

#include "nle/NonLinearEditor.h"

class CorrespondenceMark : public nle::Mark
{
	public:
		CorrespondenceMark();
		~CorrespondenceMark();

		void attach(CorrespondenceMark* mark);
		std::vector<CorrespondenceMark*>& getAttachedMark();

		std::vector<CorrespondenceMark*> _attached;
};


class FootstepMark : public nle::Mark
{
	public:
		FootstepMark();		
};

class ParamAnimBlock : public nle::Block
{
	public:
		ParamAnimBlock();
		
};

class ParamAnimTrack : public nle::Track
{
	public:
		ParamAnimTrack();
};

class IntervalMark : public nle::Mark
{
	public:
		IntervalMark();

};


#endif
