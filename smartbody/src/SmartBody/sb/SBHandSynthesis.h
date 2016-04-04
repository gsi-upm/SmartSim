#pragma once

#ifndef _SB_HAND_SYNTHESIS_H_
#define _SB_HAND_SYNTHESIS_H_
/*
SBHandSynthesis.h -- contains classes to implement
the algorithm for automatic hand synthesis
from body motion. 
*/
#include <stdio.h>
#include <sb/SBMotion.h>

namespace SmartBody{

class MotionDatabase;
typedef std::vector<std::pair<int,float> > CostList;

// This class contains the main algorithm
// for computing hand motion
class SBHandSynthesis {

public:
	// ENUM for both hands
	enum HandState{
		RIGHT_HAND, LEFT_HAND
	};

	// constructor and destructor
	SBHandSynthesis(SmartBody::SBSkeleton* skeleton, SkChannelArray& channels);
	~SBHandSynthesis();

	// function chooses data for different states
	void changeState(HandState state);

	// function to load the database motion files
	bool loadDatabase();

	// add motion to database after breaking
	void addDatabaseMotion(SBMotion* motion);

	// do the one time database segmentation
	void segmentDatabase();

	// generat the database segments . has to be done only once
	void generateDatabaseSegments();

	// creates segment from a given motion
	SmartBody::SBMotion* createSegment(SmartBody::SBMotion* motion, float tStart, float tEnd, float frameRate);
	
	// generate motion segments
	void generateMotionSegments();

	// starts algorithm
	void synthesizeHands(SmartBody::SBMotion* bodyMotion, int maxLevels, bool useRandom);

	// synthesize hand motion for one hand
	void synthesizeHandMotion();

	// synthesize hand motion with random picks
	void synthesizeRandomHandMotion();

	// fill random hand segments
	void fillRandomHandSegments();

	// find similar segments
	void findSimilarSegments();

	// compares two segments and calculates cost diff
	float compareSegments(std::string wirstJointName, SmartBody::SBMotion* segmentInput, SmartBody::SBMotion* segmentDb);

	// build the main graph and solve it
	void buildGraph();

	// alternative graph building for floats
	void buildGraphAlternate();

	// calculate transition cost for moving from one segment to another
	float calcTransitionCost(SmartBody::SBMotion* segmentA, SmartBody::SBMotion* segmentB, std::string wristJointName);

	// clear the data
	void clearData();

	// create the final motion
	void createFinalMotion();

	// combines two motions
	void combineMotion(SmartBody::SBMotion* destMotion, SmartBody::SBMotion* srcMotion, std::string wristJointName);

	// get right database 
	MotionDatabase* getRightDb();

	// get left database
	MotionDatabase* getLeftDb();

	// print all the results
	void printResults();

	// set the name of the configuration
	void setConfigurationName(std::string configName);

	// set the debug value
	void setDebug(bool debug);

private:
	// motion database for left and right hand
	MotionDatabase* _leftDb;
	MotionDatabase* _rightDb;

	// pointer to choose between databases
	MotionDatabase* _selectDb;

	// name of the hand configuration
	std::string _configName;

	// body and hand motion for the database
	std::vector<SmartBody::SBMotion*> _handDbMotion;
	std::vector<SmartBody::SBMotion*> _bodyDbMotion;

	// the actual body motion to use
	SmartBody::SBMotion* _bodyMotion;

	// the skeleton to use
	SmartBody::SBSkeleton *_sk, *_skCopy;

	// variable for base joint
	SmartBody::SBJoint* _baseJoint;

	// maximum levels and number of similar segments
	int _maxLevels , _k;

	// channel array
	SkChannelArray _channels;

	// for debugging
	bool _shouldStop, _printDebug;
};



/*
MotionDatbase - contains data for the motion database
*/
class MotionDatabase{
public:

	// initialize the motion datbase
	MotionDatabase();
	~MotionDatabase();

	// clear the database
	void clearDb();
	
	// get the segments
	std::vector<SBMotion*>& getBodyDbSegments() ;
	std::vector<SBMotion*>& getHandDbSegments() ;
	std::vector<SBMotion*>& getMotionSegments() ;
	std::vector<std::vector<std::pair<int,float> > >& getSimilarSegments();
	std::vector<int>&  		getMotionIndices()	;
	SmartBody::SBMotion* 	getFinalMotion();

	// get the segment at a certina location
	SmartBody::SBMotion* getBodyDbSegment(int i) ;
	SmartBody::SBMotion* getHandDbSegments(int i ) ;

	// add to database
	void addBodyDbSegment(SmartBody::SBMotion* segment) ;
	void addHandDbSegment(SmartBody::SBMotion* segment) ;
	void addMotionSegment(SmartBody::SBMotion* segment) ;
	void addCostList(CostList costList);
	void addMotionIndex(int index);

	// set joint names
	void setJointName(std::string jointName);
	std::string getJointName();

	// print the database 
	void printDatabase(std::ofstream& file);
	void printMotion(SBMotion* motion, std::ofstream& file);

private:
	// segments of the database motions
	std::vector<SBMotion*> _bodyDatabaseSegments;
	std::vector<SBMotion*> _handDatabaseSegments;

	// wrist joint name for this database
	std::string _jointName;

	// contains segments of the input motion
	std::vector<SmartBody::SBMotion*> _motionSegments;

	// variable for similar segments
	std::vector<std::vector<std::pair<int,float> > > _similarSegments;

	// vector for storing final motion
	std::vector<int> _finalMotionIndices;

	// final motion
	SmartBody::SBMotion* _finalMotion;

	// final motion name
	std::string motionName;
};

}

#endif