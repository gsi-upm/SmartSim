//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

/// @file PerformanceProfiler.cpp
/// @brief Implements the Util::PerformanceProfiler class.

#include "util/HighResCounter.h"
#include "util/PerformanceProfiler.h"
#include <iostream>
#include <math.h>

using namespace Util;

PerformanceProfiler::PerformanceProfiler()
{
	reset();
}

void PerformanceProfiler::reset()
{
	_startTick = 0;
	_endTick = 0;
	_totalTicksAccumulated = 0;
	_maxTicks = 0;
	_minTicks = 0;
	_numTimesCalled = 0;
	_minTime = 0.0f; 
	_maxTime = 0.0f;
	_averageTime = 0.0f;
	_totalTime = 0.0f;
	_tickFrequencyInHz = ((float)getHighResCounterFrequency());
}

unsigned int PerformanceProfiler::getNumTimesExecuted()
{
	return _numTimesCalled;
}

float PerformanceProfiler::getMinExecutionTime()
{
	_minTime =  ( (float)_minTicks / _tickFrequencyInHz);
	return _minTime;
}

float PerformanceProfiler::getMaxExecutionTime()
{
	_maxTime =  ( (float)_maxTicks / _tickFrequencyInHz );
	return _maxTime;
}

float PerformanceProfiler::getAverageExecutionTime()
{
	if (_numTimesCalled == 0) return 0.0f;
	_averageTime =  ( ( (float)_totalTicksAccumulated / (float)_numTimesCalled ) / _tickFrequencyInHz );
	return _averageTime;
}

float PerformanceProfiler::getTotalTime()
{
	_totalTime =  ( (float)_totalTicksAccumulated / _tickFrequencyInHz );
	return _totalTime;
}

void PerformanceProfiler::displayStatistics(std::ostream & out)
{
	out << "   Number of times executed: " << _numTimesCalled << std::endl;
	out << "    Total ticks accumulated: " << _totalTicksAccumulated << std::endl;
	out << " Shortest execution (ticks): " << _minTicks << std::endl;
	out << "  Longest execution (ticks): " << _maxTicks << std::endl;
	out << "       Tick Frequency (MHz): " << (_tickFrequencyInHz / 1000000.0f) << std::endl;
	out << "          Fastest execution: " << getMinExecutionTime()*1000.0f << " milliseconds" << std::endl;
	out << "          Slowest execution: " << getMaxExecutionTime()*1000.0f << " milliseconds" << std::endl;
	out << "      Average time per call: " << getAverageExecutionTime()*1000.0f <<  " milliseconds" << std::endl;
	out << "    Total time of all calls: " << getTotalTime() << " seconds" << std::endl;
}

void PerformanceProfiler::_updateStatistics()
{
	unsigned long long ticksForOneStep = _endTick - _startTick;

	_numTimesCalled++;

	if ( ticksForOneStep > _maxTicks )
		_maxTicks = ticksForOneStep;

	if ( ticksForOneStep < _minTicks || _minTicks == 0)
		_minTicks = ticksForOneStep;

	_totalTicksAccumulated += ticksForOneStep;
}
