#include "SBRigNode.h"
#include <vhcl.h>

namespace SmartBody {

SBRigNode::SBRigNode() 
{
}

SBRigNode::~SBRigNode()
{
}

SBSetDrivenKeyNode::SBSetDrivenKeyNode() : SBRigNode()
{
}

SBSetDrivenKeyNode::~SBSetDrivenKeyNode()
{
}

void SBSetDrivenKeyNode::setRange(double range)
{
	_range = range;
}

double SBSetDrivenKeyNode::getValue(double value)
{
	if (_range == 0.0)
		return 0.0;
	return value / _range;
}

SBSetDrivenKeySequenceNode::SBSetDrivenKeySequenceNode() : SBRigNode()
{
}

SBSetDrivenKeySequenceNode::~SBSetDrivenKeySequenceNode()
{
}

void SBSetDrivenKeySequenceNode::setRange(double range, int numValues)
{
	_range = range;
	_numValues = numValues;
}

double SBSetDrivenKeySequenceNode::getValue(double value, int index)
{
	if (_range == 0.0)
		return 0.0;

	if (_numValues == 0)
		return 0.0;

	if (index >= _numValues)
	{
		LOG("Cannot retrieve key for node %d, only %d available.", index, _numValues);
		return 0.0;
	}
	if (index < 0)
	{
		LOG("Bad key value %d, only %d available.", index, _numValues);
		return 0.0;
	}

	// how big are the partitions
	double partitionSize = _range / (double) _numValues;

	// which partition are we in?
	int whichPart = (int) (value / partitionSize);

	// in the i_th part, bring the i_th value up
	if (index == whichPart)
	{
		return (value - ((double) whichPart * partitionSize)) / partitionSize;
	}

	// in the i_th part, bring the (i-1)_th value down
	if (index == whichPart - 1)
	{
		return 1.0 - (value - ((double) whichPart * partitionSize)) / partitionSize;
	}

	return 0.0;
}

}
