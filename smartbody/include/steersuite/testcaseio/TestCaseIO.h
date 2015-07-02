//
// Copyright (c) 2009-2010 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __STEERLIB_TEST_CASE_IO_H__
#define __STEERLIB_TEST_CASE_IO_H__

/// @file TestCaseIO.h
/// @brief Defines the public interfaces for reading/writing SteerSuite test cases.

#include "Globals.h"
#include "TestCaseIOPrivate.h"
#include "util/XMLParser.h"

namespace SteerLib {

	/**
	 * @brief The public interface for reading SteerSuite XML test cases.
	 *
	 * This class reads a SteerSuite XML test case, and initializes a list of agents and obstacles to their initial conditions.
	 *
	 * <h3> How to use this class </h3>
	 *
	 * Specify the desired SteerSuite XML test case using #readTestCaseFromFile().  This class automatically parses and
	 * loads all initial conditions.  Then, the rest of the functionality of the class can be used to query information
	 * about the test case.  In particular, #getAgentInitialConditions() and #getObstacleInitialConditions() allow users
	 * to query the initial conditions of each individual agent and obstacle.
	 *
	 * Most likely your initialization will have a loop that iterates over all agents, using #getAgentInitialConditions() for
	 * each agent, and a similar loop for obstacles.  For each agent, then, you would transfer the initial conditions of
	 * our data structure into your data structures.
	 *
	 * <h3> Notes </h3>
	 *
	 * The XML test cases support the definition of agent and obstacle "regions"; these regions are automatically expanded into
	 * their individual agents in #readTestCaseFromFile().  The XML test cases also support the definition of random initial
	 * conditions, these random initial conditions are also resolved in #readTestCaseFromFile(), using the Mersenne Twister random
	 * number generator.  Goals can also be random, but are NOT resolved here, since we cannot predict run-time conditions to
	 * determine valid random goals at intialization.
	 *
	 * If the test case is large, this class may consume a large amount of memory.  It is a good idea to de-allocate it
	 * as soon as you finish initializing your own data.
	 *
	 * @see
	 *  - Documentation of AgentInitialConditions, which is the return value of #getAgentInitialConditions()
	 *  - Documentation of ObstacleInitialConditions, which is the return value of #getObstacleInitialConditions()
	 *  - Documentation of CameraView, which is the return value of #getCameraView()
	 */
	class STEERLIB_API TestCaseReader : public TestCaseReaderPrivate {
	public:
		TestCaseReader();
		/// Parses the specified XML test case; after this function returns the class contains all initialized information about the test case.
		void readTestCaseFromFile( const std::string & testCaseFilename );

		/// @name General queries about the test case
		//@{
		/// Returns the total number of agents specified by the test case.
		inline unsigned int getNumAgents() const { return _initializedAgents.size(); }
		/// Returns the total number of obstacles specified by the test case.
		inline unsigned int getNumObstacles() { return _initializedObstacles.size(); }
		/// Returns the total number of suggested camera views specified by the test case.
		inline unsigned int getNumCameraViews() { return _cameraViews.size(); }
		/// Returns the test case name (not the filename) specified by the test case.
		inline const std::string & getTestCaseName() { return _header.name; }
		/// Returns the description specified by the test case.
		inline const std::string & getDescription() { return _header.description; }
		/// Returns a string indicating the version of this test case.
		inline const std::string & getVersion() { return _header.version; }
		/// Returns a human-readable string desciribing the criteria for passing a particular test case; In the future this criteria may become more elaborate and automated.
		inline const std::string & getPassingCriteria() { return _header.passingCriteria; }
		/// Returns a data structure containing information about one suggested camera view.
		inline const CameraView & getCameraView(unsigned int cameraIndex) { return _cameraViews[cameraIndex]; }
		/// Returns the world boundaries specified by the test case.
		inline const Util::AxisAlignedBox & getWorldBounds() const { return _header.worldBounds; }
		//@}

		/// @name Queries about the initial conditions of the test case.
		//@{
		/// Returns a data structure containing the agent's initial conditions for the parsed test case
		inline const AgentInitialConditions & getAgentInitialConditions(unsigned int agentIndex) { return _initializedAgents.at(agentIndex); }
		/// Returns a data structure containing the obstacle's initial conditions for the parsed test case
		inline const ObstacleInitialConditions & getObstacleInitialConditions(unsigned int obstacleIndex) { return _initializedObstacles.at(obstacleIndex); }
		//@}
	};

} // end namespace SteerLib

#endif
