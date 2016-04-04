//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_REC_FILE_PLAYER_MODULE_H__
#define __STEERLIB_REC_FILE_PLAYER_MODULE_H__

/// @file RecFilePlayerModule.h
/// @brief Declares the RecFilePlayerModule built-in module.

#include "interfaces/ModuleInterface.h"
#include "interfaces/EngineInterface.h"
#include "obstacles/BoxObstacle.h"

namespace SteerLib {

	class ReplayAgent : public SteerLib::AgentInterface {
	public:
		void reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo) { }
		void updateAI(float timeStamp, float dt, unsigned int frameNumber) { }
		void draw();

		bool enabled() { return _enabled; }
		Util::Point& position() { return _position; }
		Util::Vector& forward() { return _forward; }
		float radius() { return _radius; }
		const SteerLib::AgentGoalInfo & currentGoal() { return _currentGoal; }
		void addGoal(const SteerLib::AgentGoalInfo & newGoal) { throw Util::GenericException("addGoals() not implemented yet for ReplayAgent"); }
		void clearGoals() { throw Util::GenericException("clearGoals() not implemented yet for ReplayAgent"); }

		bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsCircle2D(_position, _radius, r, t); }
		bool overlaps(const Util::Point & p, float radius) { return Util::circleOverlapsCircle2D( _position, _radius, p, radius); }
		float computePenetration(const Util::Point & p, float radius) { return Util::computeCircleCirclePenetration2D( _position, _radius, p, radius); }

		// native functionality
		void setPosition(const Util::Point & newPosition) { _position = newPosition; }
		void setForward(const Util::Vector & newForward) { _forward = newForward; }
		void setEnabled(const bool newEnabled) { _enabled = newEnabled; }
		void setRadius(const float newRadius) { _radius = newRadius; }
		void setCurrentGoal(const SteerLib::AgentGoalInfo & newGoal) { _currentGoal = newGoal; }


	protected:
		Util::Point _position;
		Util::Vector _forward;
		bool _enabled;
		float _radius;
		SteerLib::AgentGoalInfo _currentGoal;

	};



	class RecFilePlayerModule : public SteerLib::ModuleInterface
	{
	public:
		std::string getDependencies() { return ""; }
		std::string getConflicts() { return "testCasePlayer"; }
		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );
		void finish();
		void processKeyboardInput(int key, int action );
		void initializeSimulation();
		void cleanupSimulation();
		void preprocessFrame(float timeStamp, float dt, unsigned int frameNumber);
	protected:
		SteerLib::EngineInterface * _engine;
		SteerLib::RecFileReader * _simulationReader;
		double _playbackSpeed;
		double _simulationStartTime;
		double _simulationStopTime;
		double _currentTimeToPlayback;

		double _fixedTimeStep;

		std::vector<SteerLib::BoxObstacle *> _obstacles;
		std::string _recFilename;

	};

} // end namespace SteerLib

#endif
