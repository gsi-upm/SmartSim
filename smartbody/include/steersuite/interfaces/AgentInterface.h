//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_AGENT_INTERFACE_H__
#define __STEERLIB_AGENT_INTERFACE_H__

/// @file AgentInterface.h
/// @brief Declares the SteerLib::AgentInterface virtual interface.

#include <queue>
#include "Globals.h"
#include "testcaseio/AgentInitialConditions.h"
#include "griddatabase/GridDatabase2D.h"
#include "util/Geometry.h"

namespace SteerLib {

	// forward declaration
	class STEERLIB_API EngineInterface;


	/**
	 * @brief The virtual interface for AI agents.
	 *
	 * This class is the virtual interface for a single AI agent, usually instantiated by a module.  Inherit and 
	 * implement this virtual interface to create your own custom AI agents.  Generally, an instance of SteerLib::ModuleInterface
	 * is responsible for allocating and de-allocating your agent class that inherits this interface.
	 *
	 * During reset(), the agent is given a reference to the engine, which remains valid as long as the agent exists.  Among
	 * other functionality provided by the engine, the agent has access to a spatial database which allows the agent to query
	 * about the environment and other agents.
	 * If you want your implementation to take advantage of this database, then <b>it is your responsibility</b> to add this
	 * agent to the spatial database, to update the database as your agents move around, and to remove the agents from the
	 * database during cleanup.
	 *
	 * @see
	 *  - The SteerLib::EngineInterface provides functionality to modules and agents.
	 *  - The SteerLib::ModuleInterface is the main class to inherit and implement for creating a module
	 *  - The SteerLib::GridDatabase2D is a spatial database that agents may want to use to interact with other agents, such as
	 *    nearest-neighbor queries or ray tracing.
	 *
	 */
	class STEERLIB_API AgentInterface : public SteerLib::SpatialDatabaseItem {
	public:
		virtual ~AgentInterface() { }
		/// @name Core functionality
		//@{
		/// Resets an agent, could be called several times, even during the simulation.
		virtual void reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo) = 0;
		/// Called once per frame by the engine, update the agent here.
		virtual void updateAI(float timeStamp, float dt, unsigned int frameNumber) = 0;
		/// Called once per frame by the engine, use openGL to draw an agent here.
		virtual void draw() = 0;

		/// currently hacked-in
		virtual void updateAgentState(const Util::Point & newPosition,  const Util::Vector & newOrientation, float newSpeed) { }

		//@}

		/// @name Accessors to query info about the agent
		//@{
		/// Returns true if the agent is active/enabled, false if it is inactive/disabled.
		virtual bool enabled() = 0;
		/// Returns a point representing the position of the agent.
		virtual Util::Point& position() = 0;
		/// Returns a vector that points in the direction the agent is facing; this can potentially be different than the direction the agent is moving.
		virtual Util::Vector& forward() = 0;
		/// Returns the radius of the Agent.
		virtual float radius() = 0;
		/// Returns information about the current goal.
		virtual const SteerLib::AgentGoalInfo & currentGoal() = 0;
		//@}

		/// @name Some convenience functions so users can manipulate agents more explicitly
		//@{
		/// Adds a goal to the agent's existing list of goals
		virtual void addGoal(const SteerLib::AgentGoalInfo & newGoal) = 0;
		/// Clears the agent's existing list of goals.
		virtual void clearGoals() = 0;
		//@}

		/// @name The SpatialDatabaseItem interface
		/// @brief Some defaults are given, but can be overridden if desired.
		//@{
		virtual bool isAgent() { return true; }
		virtual bool blocksLineOfSight() { return false; }
		virtual float getTraversalCost() { return 0; }
		virtual bool intersects(const Util::Ray &r, float &t) = 0;
		virtual bool overlaps(const Util::Point & p, float radius) = 0;
		virtual float computePenetration(const Util::Point & p, float radius) = 0;
		//@}
	};


} // end namespace SteerLib

#endif

