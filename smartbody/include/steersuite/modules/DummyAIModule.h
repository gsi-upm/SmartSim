//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_DUMMY_AI_MODULE_H__
#define __STEERLIB_DUMMY_AI_MODULE_H__

/// @file DummyAIModule.h
/// @brief Declares the DummyAIModule built-in module.

#include "interfaces/ModuleInterface.h"
#include "interfaces/EngineInterface.h"

namespace SteerLib {

	class DummyAgent : public SteerLib::AgentInterface
	{
		void reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo) {

			_position = initialConditions.position;
			_forward = initialConditions.direction;
			_radius = initialConditions.radius;


			if (initialConditions.goals.size() > 0) {
				_currentGoal = initialConditions.goals[0];
			}
			else {
				throw Util::GenericException("No goals were specified!\n");
			}
		}

		/// The DummyAgent does absolutely nothing during each update.
		void updateAI(float timeStamp, float dt, unsigned int frameNumber) { }

		void draw() {
#ifdef ENABLE_GUI
			Util::DrawLib::drawAgentDisc(_position, _forward, _radius, Util::gBlack); 
			if (_currentGoal.goalType == SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET) {
				Util::DrawLib::drawFlag(_currentGoal.targetLocation);
			}
#endif
		}


		bool enabled() { return true; }
		Util::Point& position() { return _position; }
		Util::Vector& forward() { return _forward; }
		float radius() { return _radius; }
		const SteerLib::AgentGoalInfo & currentGoal() { return _currentGoal; }
		void addGoal(const SteerLib::AgentGoalInfo & newGoal) { throw Util::GenericException("addGoals() not implemented yet for DummyAgent"); }
		void clearGoals() { throw Util::GenericException("clearGoals() not implemented yet for DummyAgent"); }

		bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsCircle2D(_position, _radius, r, t); }
		bool overlaps(const Util::Point & p, float radius) { return Util::circleOverlapsCircle2D( _position, _radius, p, radius); }
		float computePenetration(const Util::Point & p, float radius) { return Util::computeCircleCirclePenetration2D( _position, _radius, p, radius); }

	protected:
		Util::Point _position;
		Util::Vector _forward;
		float _radius;
		SteerLib::AgentGoalInfo _currentGoal;
	};



	class DummyAIModule : public SteerLib::ModuleInterface
	{
	public:
		std::string getDependencies() { return "testCasePlayer"; }
		std::string getConflicts() { return ""; }
		void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo ) { }
		void finish() { }

		SteerLib::AgentInterface * createAgent() { return new DummyAgent; }
		void destroyAgent( SteerLib::AgentInterface * agent ) { assert(agent!=NULL);  delete agent;  agent = NULL; }
	};

} // end namespace SteerLib

#endif
