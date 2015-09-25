#ifndef __COMMAND_LINE_ENGINE_DRIVER_H__
#define __COMMAND_LINE_ENGINE_DRIVER_H__

/// @file SteerSuiteEngineDriver.h
/// @brief Declares the SteerSuiteEngineDriver class
#include <sb/SBTypes.h>
#include <vhcl.h>
#include <SteerLib.h>
#include <PPRAgent.h>
#include <sb/SBObject.h>

class SteerSuiteEngineDriver :  public SmartBody::SBObject, public SteerLib::EngineControllerInterface
{
public:
	SteerSuiteEngineDriver();
	~SteerSuiteEngineDriver();


	void init(SteerLib::SimulationOptions * options);
	SBAPI void finish();
	void run();

	SBAPI bool isInitialized();
	bool isDone();
	void setDone(bool val);
	void setStartTime(double time);
	double getStartTime();
	void setLastUpdateTime(double time);
	double getLastUpdateTime();
	float collisionPenetration( SrVec pos, float radius, SteerLib::AgentInterface* agent );	

	/// @name The EngineControllerInterface
	/// @brief The CommandLineEngineDriver does not support any of the engine controls.
	//@{
	virtual bool isStartupControlSupported() { return false; }
	virtual bool isPausingControlSupported() { return false; }
	virtual bool isPaused() { return false; }
	virtual void loadSimulation();
	virtual void unloadSimulation();
	virtual void startSimulation();
	virtual void stopSimulation();
	virtual void pauseSimulation() { throw Util::GenericException("CommandLineEngineDriver does not support pauseSimulation()."); }
	virtual void unpauseSimulation() { throw Util::GenericException("CommandLineEngineDriver does not support unpauseSimulation()."); }
	virtual void togglePausedState() { throw Util::GenericException("CommandLineEngineDriver does not support togglePausedState()."); }
	virtual void pauseAndStepOneFrame() { throw Util::GenericException("CommandLineEngineDriver does not support pauseAndStepOneFrame()."); }
	//@}

	SteerLib::SimulationEngine * _engine;

		// These functions are kept here to protect us from mangling the instance.
	// Technically the CommandLineEngineDriver is not a singleton, though.
//	SteerSuiteEngineDriver(const SteerSuiteEngineDriver & );  // not implemented, not copyable
//	SteerSuiteEngineDriver& operator= (const SteerSuiteEngineDriver & );  // not implemented, not assignable

protected:
	bool _alreadyInitialized;
	bool _done;
	double _startTime;
	double _lastUpdateTime;
	SteerLib::SimulationOptions * _options;


};


#endif