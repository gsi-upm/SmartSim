/*
 *  ParamAnimBlock.cpp - part of SmartBody-lib's Test Suite
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

#include "vhcl.h"
#include "ParamAnimBlock.h"

CorrespondenceMark::CorrespondenceMark()
{
	setBoundToBlock(false);
	setInterval(false);
}

CorrespondenceMark::~CorrespondenceMark()
{
}


void CorrespondenceMark::attach(CorrespondenceMark* mark)
{
	_attached.push_back(mark);
}

std::vector<CorrespondenceMark*>& CorrespondenceMark::getAttachedMark()
{
	return _attached;
}

FootstepMark::FootstepMark()
{
}

ParamAnimBlock::ParamAnimBlock()
{
}

ParamAnimTrack::ParamAnimTrack()
{
}

IntervalMark::IntervalMark()
{
	setInterval(true);
}


