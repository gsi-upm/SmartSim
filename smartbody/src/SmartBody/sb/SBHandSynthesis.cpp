/*
SBHandSynthesis.cpp - implementation for the algorithm
*/

#include <sb/SBHandSynthesis.h>
#include <sb/SBScene.h>
#include <sb/SBSkeleton.h>

#include <algorithm>
#include <fstream>

#include <boost/config.hpp>
 
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/properties.hpp>
#include <boost/algorithm/string.hpp>
 
#include <boost/property_map/property_map.hpp>

#include <sb/SBMotion.h>
#include <sb/SBScene.h>
#include <sb/SBJoint.h>
#include <sb/SBAssetManager.h>
#include <sb/SBHandConfigurationManager.h>
#include <sb/SBHandConfiguration.h>

#include <string>
#include <iostream>
#include <fstream>
#include <ostream>


using namespace boost;

namespace SmartBody{

/*
   These inital type defs are used to define the lists , graphs , vertices, edges etc
*/
typedef adjacency_list < listS, vecS, directedS,
no_property, property < edge_weight_t, int > > graph_t;
typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
typedef graph_traits < graph_t >::edge_descriptor edge_descriptor;
typedef std::pair<int, int> Edge;

// default constructor
SBHandSynthesis::SBHandSynthesis(SmartBody::SBSkeleton* skeleton, SkChannelArray& channels)
{
	// log here
//	LOG ( " \n \n \n SBHandSynthesis started \n \n \n " );

	// initailize variables
	_leftDb = new MotionDatabase();
	_rightDb = new MotionDatabase();
	_selectDb = _rightDb;
	_bodyMotion = NULL;
	_sk = skeleton;
	_skCopy = new SmartBody::SBSkeleton();
	_skCopy->copy(_sk);
	_maxLevels = 3;
	_k = 3;
	_channels = channels;
	_shouldStop = false;
	_configName = "";
	_printDebug = false;

	// initialize names
	_leftDb->setJointName("l_wrist");
	_rightDb->setJointName("r_wrist");

	// set the base joint
	_baseJoint = _sk->getJointByName("base");

	// add the two motions to asset manager
	SmartBody::SBAssetManager* assetManager = SmartBody::SBScene::getScene()->getAssetManager();
	
	// set the names
	_leftDb->getFinalMotion()->setName("_leftMotion");
	_rightDb->getFinalMotion()->setName("_rightMotion");

	// add the two motions to assets (TODO: mb add thees with the character name)
	//assetManager->addMotion(_leftDb->getFinalMotion());
	//assetManager->addMotion(_rightDb->getFinalMotion());

	// segment the database in the start
	segmentDatabase();

	//segmentDatabase();
}

// set name of the configuration
void SBHandSynthesis::setConfigurationName(std::string configName)
{
	_configName = configName;
}

// set the print debug value
void SBHandSynthesis::setDebug(bool debug)
{
	_printDebug = debug;
}

// destructor
SBHandSynthesis::~SBHandSynthesis()
{
	clearData();
}

// chooses between different states
void SBHandSynthesis::changeState(HandState state)
{
	// switch between two states
	if (state == LEFT_HAND)
	{
		_selectDb = _leftDb;
	}
	else if (state == RIGHT_HAND)
	{
		_selectDb = _rightDb;
	}
}

// break motion and add to database
void SBHandSynthesis::addDatabaseMotion(SmartBody::SBMotion* dbMotion)
{
	// check if motion exists or not 
	if (dbMotion == NULL)
	{
		LOG("Unable to add motion (its NULL)");
		return;
	}

	// make new motions for hand and body
	SmartBody::SBMotion *handMotion = new SmartBody::SBMotion();
	SmartBody::SBMotion *bodyMotion = new SmartBody::SBMotion();

	// copy the body motion
	handMotion->setMotion(*dbMotion);
	bodyMotion->setMotion(*dbMotion);

	// trim motion channels
	std::vector<std::string> jointNames;
	jointNames.push_back("l_wrist"); jointNames.push_back("r_wrist");

	// make body motion by removing channels
	bodyMotion->removeMotionChannelsByEndJoints("ChrBrad.sk",jointNames);

	// make hand motion by pertaining
	handMotion->pertainMotionChannelsByEndJoints("ChrBrad.sk",jointNames);

	// give it a new name
	std::vector<std::string> tokens;
	boost::split(tokens, dbMotion->getName(), is_any_of("."));

	// get the first string and make the two names
	std::string handFileName = tokens[0] + "_hand_"+"_" + boost::lexical_cast<std::string>(_bodyDbMotion.size());
	std::string bodyFileName = tokens[0] + "_body_"+"_" + boost::lexical_cast<std::string>(_bodyDbMotion.size());

	//std::cout << " hand file name is " << handFileName << std::endl;
	//std::cout << " body file name is " << bodyFileName << std::endl;

	// set the file names
	handMotion->setName(handFileName);
	bodyMotion->setName(bodyFileName);

	// use these motions instead
	_handDbMotion.push_back(handMotion);
	_bodyDbMotion.push_back(bodyMotion);
}

// load the database motions
bool SBHandSynthesis::loadDatabase()
{
	// check what the hand configuration is
	if (_configName.compare("") == 0)
	{

		return false;
	}

	// config name to be loaded
	if (_printDebug)
		LOG("Going to use hand configuration %s", _configName.c_str());

	// instead get the database from the configuration manager
	SBHandConfiguration* config = SmartBody::SBScene::getScene()->getHandConfigurationManager()->getHandConfiguration(_configName);

	if (!config)
		return false;

	// get all the motions and add them
	for (int i = 0 ; i < config->getNumMotions(); i++)
	{
		// find the motion
		SmartBody::SBMotion* motion = config->getMotion(i);

		// add the motion to database
		addDatabaseMotion(motion);
	}

	return true;
}

// generate the database segments for each hand
void SBHandSynthesis::segmentDatabase()
{
	// load the database from file here
	if (!loadDatabase() )
	{
		return;
	}

	// generate for left hand
	changeState(LEFT_HAND);
	generateDatabaseSegments();

	// generate for right hand
	changeState(RIGHT_HAND);
	generateDatabaseSegments();
}

// generate segments for a motion
void SBHandSynthesis::generateDatabaseSegments()
{
	// time step
	float dt = 0.1f , speed = 0.0f , threshold = 0.2f ;
	float minLength = 0.33f, maxLength = 2.0f;

	// print db stats
	//LOG ("body database size is %d" , _bodyDbMotion.size());
	//LOG ("hand database size is %d" , _handDbMotion.size());
	
	for (size_t i = 0 ; i < _bodyDbMotion.size() ; i++ ) 
	{
		SmartBody::SBMotion* curBodyDbMotion = _bodyDbMotion[i];
		SmartBody::SBMotion* curHandDbMotion = _handDbMotion[i];

		// connect motion to skeleton
		curBodyDbMotion->connect(_sk);

		// get the framerate and the wrist joint name
		float frameRate = (float)curBodyDbMotion->getFrameRate();
		SmartBody::SBJoint* wristJoint = _sk->getJointByName(_selectDb->getJointName());
		bool added = false;
		float lastOffset = 0.0f;

		// print some stats here 
		if (_printDebug)
		{
			LOG( "Processing body database named %s ", curBodyDbMotion->getName().c_str());
			LOG( "Time of original body motion is %f ", curBodyDbMotion->duration() );
		}

		// make segments
		for (float t=dt ; t < curBodyDbMotion->duration() ; t=t+dt)
		{
			speed = curBodyDbMotion->getJointSpeed(wristJoint,t-dt,t);
 		
			// TODO: check for conditions here and generate segments
			if (t-lastOffset <= minLength)
				continue;
			else if ( ( (t-lastOffset) >= maxLength) || (speed > threshold))
			{

				// create segments
				SmartBody::SBMotion* bodySegment = createSegment(curBodyDbMotion,lastOffset,t,frameRate);
				SmartBody::SBMotion* handSegment = createSegment(curHandDbMotion,lastOffset,t,frameRate);

				// add to db
				_selectDb->addBodyDbSegment(bodySegment);
				_selectDb->addHandDbSegment(handSegment);

				// set the names for body and hand db segments
				bodySegment->setName(curBodyDbMotion->getName()+"_body_db_seg_"+_selectDb->getJointName()+"_" +boost::lexical_cast<std::string>(_selectDb->getBodyDbSegments().size()));
				handSegment->setName(curHandDbMotion->getName()+"_hand_db_seg_"+_selectDb->getJointName()+"_" +boost::lexical_cast<std::string>(_selectDb->getHandDbSegments().size()));

				// update variables
				lastOffset = t;
				added = true;

				// creating a new segment
				//LOG("Creating new segment of time %f", bodySegment->duration());
 			}
		}
	
		// if length smaller 
		if (!added){
			// copy the motion and give them new names
			SmartBody::SBMotion* bodySegment = new SmartBody::SBMotion();
			SmartBody::SBMotion* handSegment = new SmartBody::SBMotion();

			// set motions
			bodySegment->setMotion(*curBodyDbMotion);
			handSegment->setMotion(*curHandDbMotion);

			// simply add to database
			_selectDb->addHandDbSegment(handSegment);
			_selectDb->addBodyDbSegment(bodySegment);

			// set new names 
			bodySegment->setName(curBodyDbMotion->getName()+"_body_db_seg_"+_selectDb->getJointName()+"_"+ boost::lexical_cast<std::string>(_selectDb->getBodyDbSegments().size()));
			handSegment->setName(curHandDbMotion->getName()+"_hand_db_seg_"+_selectDb->getJointName()+"_"+ boost::lexical_cast<std::string>(_selectDb->getHandDbSegments().size()));			
		
			// log results
			//LOG("No segment generated. Adding motion with time %f", bodySegment->duration());
		}

		// disconnect motion from skeleton
		curBodyDbMotion->disconnect();
	}

	// finished processsing. output results	
	//LOG( "Number of segments are %d" , _selectDb->getBodyDbSegments().size() );
	//LOG( "Number of segments are %d" , _selectDb->getHandDbSegments().size() );

	// check if _k is more than the size
	if (_k > _selectDb->getBodyDbSegments().size() )
		_k = _selectDb->getBodyDbSegments().size();
}


// clear all data
void SBHandSynthesis::clearData()
{
	_selectDb = NULL;
	
	_leftDb->clearDb();
	_rightDb->clearDb();

	_bodyMotion = NULL;
}

// synthesize the hand motion
void SBHandSynthesis::synthesizeHands(SmartBody::SBMotion* bodyMotion, int maxLevels, bool useRandom)
{
	// check if database is present
	if (_leftDb->getBodyDbSegments().size() == 0 && _rightDb->getBodyDbSegments().size() == 0 )
	{
		LOG ("No body database selected. unable to generate hand motion");
		return;
	}

		
	// clear all data
	clearData();

	// log here
	if (_printDebug)
		LOG (" Synthesizing hand motion in SBHandSynthesis " );

	// set max levels
	_maxLevels = maxLevels;

	// set body motion
	_bodyMotion = bodyMotion;

	// generate body motion segments
	changeState(RIGHT_HAND);
	if (useRandom)
		synthesizeRandomHandMotion();
	else
		synthesizeHandMotion();

	// do the same for the second hand
	changeState(LEFT_HAND);
	if (useRandom)
		synthesizeRandomHandMotion();
	else
		synthesizeHandMotion();
	
}

// synthesize hand motion for one hand
void SBHandSynthesis::synthesizeHandMotion()
{
	// print here
	LOG("Generating motion segments ... ");

	// generate motion segments
	generateMotionSegments();

	// print next
	LOG("Finding similar segments ...");

	// find similar segments
	findSimilarSegments();

	// print graph
	LOG("Building graph ...");

	// build the graph and solve it
	buildGraph();

	// print final
	LOG("Creating Final Motion ...");
	
	// create the final motion
	createFinalMotion();

	// print the results
	printResults();
}

// synthesize random hand motion
void SBHandSynthesis::synthesizeRandomHandMotion()
{
	// print first
	LOG("Generating motion segments ... ");

	// generate the motion segments
	generateMotionSegments();

	// assign random segments
	fillRandomHandSegments();

	// create the final motion
	createFinalMotion();

	// print the results
	printResults();
}

// generate segments for the custom body motion
void SBHandSynthesis::generateMotionSegments()
{
	// initalize variables
	float dt = 0.1f , speed = 0.0f, lastOffset = 0.0f, threshold = 0.2f ;
	float frameRate = (float)_bodyMotion->getFrameRate(), minLength = 0.33f, maxLength = 2.0f;
	

	// connect to skeleton here
	_bodyMotion->connect(_sk);

	// get the joint from joint name
	SmartBody::SBJoint* wristJoint = _sk->getJointByName(_selectDb->getJointName());

	// boolean for ensuring the motion gets added
	bool isAdded = false;

	// get the wrist join position at a certain time
	for (float t = dt ; t < _bodyMotion->duration() ; t = t+dt)
	{
		speed = _bodyMotion->getJointSpeed(wristJoint,t-dt,t);
 		
		// TODO: check for conditions here and generate segments
		if (t-lastOffset <= minLength)
			continue;
		else if (t-lastOffset >= maxLength || (speed > threshold))
		{

			SmartBody::SBMotion* bodySegment = createSegment(_bodyMotion,lastOffset,t,frameRate);
			_selectDb->addMotionSegment(bodySegment);

			bodySegment->setName(_bodyMotion->getName()+"_motion_seg_"+_selectDb->getJointName()+"_" +boost::lexical_cast<std::string>(_selectDb->getMotionSegments().size()));

			lastOffset = t;
			isAdded = true;
 		}
	}

	// if not added, add the whole motion copy
	if (!isAdded)
	{
		SmartBody::SBMotion* copyMotion = new SmartBody::SBMotion();
		copyMotion->setMotion(*_bodyMotion);
		_selectDb->addMotionSegment(copyMotion);
		copyMotion->setName(_bodyMotion->getName()+"_motion_seg_"+_selectDb->getJointName()+"_" +boost::lexical_cast<std::string>(_selectDb->getMotionSegments().size()));
	}

	// print out some stats
	if (_printDebug)
		LOG( "Body motion segments are %d" , _selectDb->getMotionSegments().size() );

	// disconnect skeleton
	_bodyMotion->disconnect();
}

// create a segment given a motion
SmartBody::SBMotion* SBHandSynthesis::createSegment(SmartBody::SBMotion* motion, float tStart, float tEnd, float frameRate)
{
	SmartBody::SBMotion* segment = new SmartBody::SBMotion();
	segment->setMotion(*motion);

	int trimFront = (int)(tStart/frameRate);
	int trimBack = (int)((motion->getDuration()-tEnd)/frameRate);
		
	segment->trim(trimFront, trimBack);

	return segment;
}

// sort function used in similar segment calculation
bool sortFunction(std::pair<int,float> a, std::pair<int,float> b)
{
	return a.second < b.second;
}

// fill with random hand segments from body Db
void SBHandSynthesis::fillRandomHandSegments()
{
	for (size_t i = 0 ; i < _selectDb->getMotionSegments().size() ; i++)
	{
		int randomSegIndex = std::rand()%_selectDb->getHandDbSegments().size();

		_selectDb->addMotionIndex(randomSegIndex);
	}
}

// find similar segments here
void SBHandSynthesis::findSimilarSegments()
{
	// find the number of levels 
	int num_levels = 0;
	if (_maxLevels < 0 || _maxLevels > _selectDb->getMotionSegments().size())
		num_levels = _selectDb->getMotionSegments().size();
	else 
		num_levels =  _maxLevels;

	// get number of body segments
	int numBodySegments = _selectDb->getBodyDbSegments().size();

	// inital variables , k is the number of segments to find
	float diffFactor = 1.5; 
	std::vector<std::pair<int,float> > costVector;

	// do this for the full number of body segments

	// find similar segments for each segment
	for (int i=0; i < num_levels ; i++)
	{
		costVector.clear();

		SmartBody::SBMotion* segment = _selectDb->getMotionSegments()[i];

		// connect to a skeleton
		segment->connect(_sk);

		// compare with fragments in database 
		for (size_t j=0;j<_selectDb->getBodyDbSegments().size();j++)
		{
			SmartBody::SBMotion* databaseSegment = _selectDb->getBodyDbSegments()[j];

			//if ((databaseSegment->duration() <=0) || (segment->duration() <= 0))
				//cout<<"starting Segment of zero length"<<endl;
			
			// if length greater than the factor continue
			if ((databaseSegment->duration()/segment->duration()) > 1.5)
			{
				continue;
			}

			// else compare
			float cost  = compareSegments(_selectDb->getJointName(), segment, databaseSegment);

			// add to cost vector
			costVector.push_back( std::pair<int,float>(j,cost));

		}

		// sort the costs to find the top k segments
		std::sort(costVector.begin(), costVector.end(), sortFunction);
		CostList similarSegmentVector;

		// put the top k into the array
		for (int j=0;j<_k;j++)
		{
			similarSegmentVector.push_back(costVector[j]);

			// cout<<"Index at "<<i<<" is "<< costVector[j].first << endl;
			// cout<<"Cost at "<<i<< " is "<< costVector[j].second<<endl;
		}

		// put the array into the main list
		_selectDb->addCostList(similarSegmentVector);

	}

	if (_printDebug)
	{
		LOG ("Done comparing all segments" );

		// print out all the similar segments 
		for (int i = 0 ; i < num_levels ; i++ )
		{
			CostList list = _selectDb->getSimilarSegments()[i];

			LOG ( " Level no %d" , i );

			for (int j = 0 ; j < _k ; j++)
			{
				LOG ( " Value at index %d is %d with cost %f " , j , list[j].first , list[j].second );
			}	
		}

		LOG( " Done printing segments " );
	}
}

// find comparison cost of segments
float SBHandSynthesis::compareSegments(std::string wristJointName, SmartBody::SBMotion* segmentInput, SmartBody::SBMotion* segmentDb)
{

	// apply dynamic time warping to get them to the same length
	float factor = segmentInput->duration()/segmentDb->duration();
	segmentDb->retime(factor);

	// compare both segments to find cost
	float w_p = 1, w_r = 0.5, costPosition = 0, costRotation = 0, noFrames = 0;

	
	// loop through the frames
	for (float t=0; t< segmentInput->duration(); t ++)
	{
		// get positions 
		SmartBody::SBJoint* joint = _sk->getJointByName(wristJointName);

		// get wrist positions
		segmentInput->connect(_sk);
		SrVec positionInput = segmentInput->getJointPositionFromBase(joint, _baseJoint, t);
		segmentInput->disconnect();

		// get wrist position for db
		segmentDb->connect(_sk);
		SrVec positionDb 	= segmentDb->getJointPositionFromBase(joint, _baseJoint, t);
		segmentDb->disconnect();

		// get wrist rotations
		segmentInput->connect(_sk);
		SrQuat rotationInput = segmentInput->getJointRotation(joint, t);
		segmentInput->disconnect();

		// get the joint rotations
		segmentDb->connect(_sk);
		SrQuat rotationDb 	 = segmentDb->getJointRotation(joint, t);
		segmentDb->disconnect();

		// compute squared distances
		float distPosition = (positionInput-positionDb).norm2();
		float distRotation = (rotationInput.getEuler()-rotationDb.getEuler()).norm2();

		// add to cost
		costPosition += distPosition;
		costRotation += distRotation;

		// add frames
		noFrames++;
	}

	// this is just a hack to go around the memory leaks
	segmentDb->retime(1.0f/factor);

	// disconnect the motions
	//segmentInput->disconnect();
	//segmentDb->disconnect();

	// return cost
	return (w_p*costPosition + w_r*costRotation)/ (noFrames);
}

// builds the transition graph to be solved using dijkstras algorithm
void SBHandSynthesis::buildGraphAlternate()
{
	// calculate the number of levels to be used 
	int num_levels = 0;
	if (_maxLevels < 0 || _maxLevels > _selectDb->getMotionSegments().size())
		num_levels = _selectDb->getMotionSegments().size();
	else 
		num_levels =  _maxLevels;

	// number of nodes
	int num_edges = _k*_k*(num_levels-1) + _k*2 
						, num_nodes = _k*num_levels + 2 
						, START = 0
						, END = num_nodes -1;
	float w_s = 1 ,  w_t = 0.5;

	// printing stuff
	if (_printDebug)
		LOG (" \n Building the graph now \n");


	// create the edge array
	Edge *edge_array = new Edge[num_edges];
	int *weights = new int[num_edges];

	// populate the node from the START
	for (int i=0 ; i < _k ; i++)
	{
		edge_array[i] = Edge(START,i+1);
		weights[i] = 0;
	}
	
	// calculate transition and make edges. doing it only for two levels right now
	for (int i=0 ; i < num_levels - 1 ; i++)
	{
		CostList list = _selectDb->getSimilarSegments()[i];
		CostList list_b = _selectDb->getSimilarSegments()[i+1];

		if (_printDebug)
			LOG("Starting a new level ");

		// go through each level and create graph
		for (int x = 0; x < _k ; x++)
		{
			if (_printDebug)
				LOG(" New node for level %d at index  " , x );

			// go through the next level
			for (int y = 0; y < _k ; y++)
			{

				// adding _k to account for the start node
				int currIndex = ( i * _k * _k ) + ( x * _k ) + y + (_k); 

				// using x + 1 and y + 1 because of the start node
				edge_array[ currIndex ] = Edge( i*_k + x + 1, (i+1)*_k + y + 1 );

				// calculate cost and add as weight
				float c_s =  list[x].second , c_t = calcTransitionCost(_selectDb->getHandDbSegments()[list[x].first],_selectDb->getHandDbSegments()[list_b[y].first],_selectDb->getJointName());
				weights[ currIndex ] = w_s * c_s + w_t * c_t ;
			}
		}

		// make the last remaining edges
		if ( i == num_levels - 2)
		{
			if (_printDebug)
				LOG( "Got to the end " );

			for (int n=0 ; n < _k ; n++ )
			{
				int currIndex =  ( (num_levels-1) * _k * _k ) + _k + n; 
				edge_array [ currIndex ] = Edge( (num_levels-1)*_k + 1 + n, END);
				weights [ currIndex ] = w_s * list_b[n].second;
			}

			if (_printDebug)
				LOG( " End is done " );
		}
	}

	if (_printDebug)
		LOG( " creating a graph " );

	// creating graph from list of edges
	graph_t g(edge_array, edge_array + num_edges , weights, num_nodes);

	// create property map from edges to weights
	property_map<graph_t , edge_weight_t >::type weightmap = get(edge_weight, g);

	// create vectors to store the predecessors p and the distances d from the root
	std::vector<vertex_descriptor> p(num_vertices(g));
	std::vector<int> d(num_vertices(g));

	// create a discriptor from the source node
	vertex_descriptor s = vertex(START, g);

	if (_printDebug)
		LOG( " going to find shortest paths " );

	// evaluate dijkstras on graph g with source s, predecessor_map p and distance_map d
	dijkstra_shortest_paths(g, s, predecessor_map(&p[0]).distance_map(&d[0]));

	// getting back the graphs and everything else
	if (_printDebug)
	{	
		LOG( "distances and parents:" );
		graph_traits < graph_t >::vertex_iterator vi, vend;
		for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {

			LOG ( " distance(%d) = %d , parent(%d) = %d ", *vi, d[*vi], *vi, p[*vi]);
	  	}
	  	LOG (" ");
	}

	// getting the final motion strip
	bool done = false; int currNode = p[END] , currLevel = num_levels; 
	// add nodes backward
	while (!done)
	{
		// find the actual segment (subtracting one to account for the index which starts at zero ,
		// whereas the node numbers start from 1 . basically subtracting the start node )
		int segNumber = currNode - ((currLevel - 1) * _k) - 1;

		// get the list and the motion
		CostList list = _selectDb->getSimilarSegments()[currLevel-1];
		int index = list[segNumber].first;

		// save the index in a list that can be inverted later
		_selectDb->addMotionIndex(index);

		if (_printDebug)
		{
			LOG ("the curr node is %d" , currNode );
			LOG ("the segment number is %d" , segNumber);
			LOG ("The computed index is %d" , index);
		}

		// set the new parent and level
		currNode = p[currNode]; currLevel--;

		if (currLevel <= 0)
			done = true;
	}

	// reverse it here
	std::reverse(_selectDb->getMotionIndices().begin(), _selectDb->getMotionIndices().end());

	// print the vector 
	if (_printDebug)
	{
	  LOG ("myvector contains:");
	  for (std::vector<int>::iterator it=_selectDb->getMotionIndices().begin(); it!=_selectDb->getMotionIndices().end(); ++it)
		LOG( " %d " , *it);
	  LOG ("");
	}

	// outputting the graph in a dot file
	std::ofstream dot_file("dijkstra-eg.dot");

	// header for the dot file
	dot_file << "digraph D {\n"
	<< "  rankdir=LR\n"
	<< "  size=\"4,3\"\n"
	<< "  ratio=\"fill\"\n"
	<< "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";

	// outputting into a dot file
	graph_traits < graph_t >::edge_iterator ei, ei_end;
	for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
	graph_traits < graph_t >::edge_descriptor e = *ei;
	graph_traits < graph_t >::vertex_descriptor
		u = source(e, g), v = target(e, g);
	dot_file << u << " -> " << v
		<< "[label=\"" << get(weightmap, e) << "\"";
	if (p[v] == u)
		dot_file << ", color=\"black\"";
	else
		dot_file << ", color=\"grey\"";
	dot_file << "]";
	}
	dot_file << "}";

	dot_file.close();

	// pick the final motion segment

}


// alternate graph for floats
void SBHandSynthesis::buildGraph()
{
	typedef float Weight;
	typedef boost::property<boost::edge_weight_t, Weight> WeightProperty;
	typedef boost::property<boost::vertex_name_t, std::string> NameProperty;
 
	typedef boost::adjacency_list < boost::listS, boost::vecS, boost::directedS,
	NameProperty, WeightProperty > Graph;
 
	typedef boost::graph_traits < Graph >::vertex_descriptor Vertex;
 
	typedef boost::property_map < Graph, boost::vertex_index_t >::type IndexMap;
	typedef boost::property_map < Graph, boost::vertex_name_t >::type NameMap;
 
	typedef boost::iterator_property_map < Vertex*, IndexMap, Vertex, Vertex& > PredecessorMap;
	typedef boost::iterator_property_map < Weight*, IndexMap, Weight, Weight& > DistanceMap;

	// calculate the number of levels to be used 
	int num_levels = 0;
	if (_maxLevels < 0 || _maxLevels > _selectDb->getMotionSegments().size())
		num_levels = _selectDb->getMotionSegments().size();
	else 
		num_levels =  _maxLevels;

	// number of nodes
	int num_edges = _k*_k*(num_levels-1) + _k*2 
						, num_nodes = _k*num_levels + 2 
						, START = 0
						, END = num_nodes -1;
	float w_s = 1 ,  w_t = 0.5;

	// printing stuff
	if (_printDebug)
		LOG (" \n Building the graph now \n");

	// create a graph 
	Graph g;

	// keep a vertex array (TODO : delete it later)
	std::vector<Vertex> vertex_array;

	// add all the vertices in the start
	for (int i = 0 ; i < num_nodes ; i++)
	{
		// add a vertex and store in an array
		std::string name = boost::lexical_cast<std::string>(i);
		
		// adding vertex here
		Vertex v = boost::add_vertex(std::string(name),g);
		vertex_array.push_back(v);
	}

	// populate edges from the start
	for ( int i = 0 ; i < _k ; i++)
	{
		// add edge with weight 0
		boost::add_edge(vertex_array[START],vertex_array[i+1],0,g);
	}

	// calculate transition and add edges
	for (int i=0 ; i< num_levels - 1 ; i++)
	{
		// get cost lists 
		CostList list = _selectDb->getSimilarSegments()[i];
		CostList list_b = _selectDb->getSimilarSegments()[i+1];

		if (_printDebug)
			LOG ("Starting new level with index %d " , i );

		// go through each level and create a graph
		for (int x = 0 ; x < _k ; x++)
		{
			// can possibley log here too
			for (int y = 0 ; y < _k ; y++)
			{
				// adding _k to accoutn for start node
				int currIndex = ( i * _k * _k ) + ( x * _k ) + y + (_k); 

				// calculate weight
				float c_s =  list[x].second , c_t = calcTransitionCost(_selectDb->getHandDbSegments()[list[x].first],_selectDb->getHandDbSegments()[list_b[y].first],_selectDb->getJointName());
				
				// add the edge from va to vb with weight
				boost::add_edge( vertex_array[i*_k + x + 1], vertex_array[(i+1)*_k + y + 1], w_s * c_s + w_t * c_t , g);

				// output the cost here
				if (_printDebug)
					LOG ("x is %d, y is %d , i is %d, c_s is %f, c_t is %f, and total cost from node %d to %d is %f", x , y, i , c_s, c_t ,  i*_k + x + 1, (i+1)*_k + y + 1 , w_s * c_s + w_t * c_t);
			}
		}

		
	}

	// make the last remaining edges
	//if ( i == num_levels - 2)
	{

		if (_printDebug)
			LOG( "Got to the end " );	

		CostList list_b = _selectDb->getSimilarSegments()[num_levels-1];

		for (int n=0 ; n < _k ; n++ )
		{
			// find the index
			int currIndex =  ( (num_levels-1) * _k * _k ) + _k + n; 

			// create the edge
			boost::add_edge( vertex_array[(num_levels-1)*_k + 1 + n], vertex_array[END],w_s*list_b[n].second, g);
		}

		if (_printDebug)
			LOG( " End is done " );
	}

	if (_printDebug)
			LOG( " creating a graph " );

	// Create things for Dijkstra
	std::vector<Vertex> predecessors(boost::num_vertices(g)); // To store parents
	std::vector<Weight> distances(boost::num_vertices(g)); // To store distances
 
	IndexMap indexMap = boost::get(boost::vertex_index, g);
	PredecessorMap predecessorMap(&predecessors[0], indexMap);
	DistanceMap distanceMap(&distances[0], indexMap);

	// Compute shortest paths from v0 to all vertices, and store the output in predecessors and distances
	// boost::dijkstra_shortest_paths(g, v0, boost::predecessor_map(predecessorMap).distance_map(distanceMap));
	// This is exactly the same as the above line - it is the idea of "named parameters" - you can pass the
	// prdecessor map and the distance map in any order.
	boost::dijkstra_shortest_paths(g, vertex_array[0], boost::distance_map(distanceMap).predecessor_map(predecessorMap));

	// Output results
	if (_printDebug)
			std::cout << "distances and parents:" << std::endl;

	// create the name map	
	NameMap nameMap = boost::get(boost::vertex_name, g);
 
	if (_printDebug)
	{
		BGL_FORALL_VERTICES(v, g, Graph)
		{
			LOG ("distance(%d,%d) = %f , predecessor(%d) = %d )" , indexMap[START], indexMap[v], distanceMap[v], indexMap[v], indexMap[predecessorMap[v]]);
		}
	}

	// Extract a shortest path
	std::cout << std::endl;
 
	typedef std::vector<Graph::edge_descriptor> PathType;
 
	PathType path;
 
	Vertex v = vertex_array[END]; // We want to start at the destination and work our way back to the source
	for(Vertex u = predecessorMap[v]; // Start by setting 'u' to the destintaion node's predecessor
		u != v; // Keep tracking the path until we get to the source
		v = u, u = predecessorMap[v]) // Set the current vertex to the current predecessor, and the predecessor to one level up
	{
		std::pair<Graph::edge_descriptor, bool> edgePair = boost::edge(u, v, g);
		Graph::edge_descriptor edge = edgePair.first;
 
		path.push_back( edge );
	}


	// Write shortest path
	if (_printDebug)
		std::cout << "Shortest path from START to END:" << std::endl;
	
	float totalDistance = 0; int currLevel = 1;
	for(PathType::reverse_iterator pathIterator = path.rbegin(); pathIterator != path.rend(); ++pathIterator)
	{
		if (_printDebug)
			std::cout << nameMap[boost::source(*pathIterator, g)] << " -> " << nameMap[boost::target(*pathIterator, g)]
				<< " = " << boost::get( boost::edge_weight, g, *pathIterator ) << std::endl;


		// get the current target vertex
		int target_vertex_index = indexMap[boost::target(*pathIterator, g)];

		// find the actual segment (subtracting one to account for the index which starts at zero ,
		// whereas the node numbers start from 1 . basically subtracting the start node )
		int segNumber = target_vertex_index - ((currLevel - 1) * _k) - 1;


		// get the list and the motion
		CostList list = _selectDb->getSimilarSegments()[currLevel-1];
		int index = list[segNumber].first;


		_selectDb->addMotionIndex(index);

		if (_printDebug)
		{	
			LOG ("the current node is %d" , target_vertex_index);
			LOG( "the segment number is %d" , segNumber);
			LOG ("The computed index is %d" , index );
		}

		currLevel++;
		if (currLevel > num_levels)
			break;
	}
 
	if (_printDebug)
	{
		std::cout << std::endl;
		std::cout << "Distance: " << distanceMap[vertex_array[END]] << std::endl;
	}
}

// calculate the transition cost from one segment to the next
float SBHandSynthesis::calcTransitionCost(SmartBody::SBMotion* segmentA, SmartBody::SBMotion* segmentB, std::string wristJointName)
{
	// main parameters
	float w_j = 1, w_w = 0.5, costRotation = 0.0f , costVelocity = 0.0f;

	//segmentA->connect(_sk);
	//segmentB->connect(_skCopy);

	
	// find the cost
	SmartBody::SBJoint* wristJoint = _sk->getJointByName(wristJointName);
	std::vector<SmartBody::SBJoint*> descendants = wristJoint->getDescendants();

	// go through all the descendants
	for (int i = 0; i < descendants.size(); i++ ) 
	{
		SmartBody::SBJoint* curJoint = descendants[i];

		// get rotation for A at the last frame and B at the first frame
		segmentA->connect(_sk);
		SrQuat rotA = segmentA->getJointRotation(curJoint,segmentA->duration());
		segmentA->disconnect();

		// get rotation of B
		segmentB->connect(_sk);
		SrQuat rotB = segmentB->getJointRotation(curJoint,0);
		segmentB->disconnect();

		SrQuat diff = rotB*rotA.inverse();
		diff.normalize();

		// get the angular velocities (this is speed. need to get velocities)
		segmentA->connect(_sk);
		SrVec velA = segmentA->getJointAngularVelocity(curJoint,segmentA->duration()-1,segmentA->duration());
		segmentA->disconnect();

		// get angular velocity
		segmentB->connect(_sk);
		SrVec velB = segmentB->getJointAngularVelocity(curJoint,0,1);
		segmentB->disconnect();

		// calculate the costs
		costRotation += diff.axisAngle().norm2();
		//costVelocity += abs(velA-velB);		// TODO : needs to be changed to velocities !!
		costVelocity += (velB-velA).norm2();
	}

	//segmentA->disconnect();
	//segmentB->disconnect();

	return ((w_j*costRotation + w_w*costVelocity)/descendants.size());
}

// create the final motion
void SBHandSynthesis::createFinalMotion()
{
	// final motion variables
	int index = 0;
	int curIndex = _selectDb->getMotionIndices()[index];
	
	// set to the new motion
	_selectDb->getFinalMotion()->setMotion(*_selectDb->getHandDbSegments()[curIndex]);

	// retime the motion
	float factor = _selectDb->getMotionSegments()[index]->duration()/ _selectDb->getHandDbSegments()[curIndex]->duration();
	_selectDb->getFinalMotion()->retime(factor);

	// print some stuff
	if (_printDebug)
		LOG("Starting to combine motions");
	

	// go through the final motion vector 
	for (index = 1 ; index < _selectDb->getMotionIndices().size() ; index++)
	{
		// get the index
		curIndex = _selectDb->getMotionIndices()[index];

		// get the database motion and retime it
		SmartBody::SBMotion* dbMotion = _selectDb->getHandDbSegments()[curIndex];

		// retime the motion and retime it later 
		float factor = _selectDb->getMotionSegments()[index]->duration() / dbMotion->duration();
		dbMotion->retime(factor);

		// pick the two motions and combine them together
		combineMotion(_selectDb->getFinalMotion(), dbMotion, _selectDb->getJointName());

		// convert dbMotion back 
		dbMotion->retime(1.0f/factor);
	}
}

// combine two motions here
void SBHandSynthesis::combineMotion(SmartBody::SBMotion* destMotion, SmartBody::SBMotion* srcMotion, std::string wristJointName)
{
	// find the mean offset first
	SmartBody::SBJoint* wristJoint = _sk->getJointByName(wristJointName);
	std::vector<SmartBody::SBJoint*> descendants = wristJoint->getDescendants();

	// get the last and the first frame
	std::vector<float> frameLast = destMotion->getFrameData(destMotion->getNumFrames()-1);
	std::vector<float> frameFirst = srcMotion->getFrameData(0);
	SkChannelArray& channels = srcMotion->channels();
	std::vector<SrQuat> offsets;
	float invFrameRate = srcMotion->duration() / srcMotion->getNumFrames() ;
	float destMotionDuration = destMotion->duration();
	float threshold = 0.087266;
	bool addOffset = true;

	//cout << " Inverse frame rate is " << invFrameRate << endl;

	// print out some stuff here
	//LOG(" Duration of the destMotion in start is %f", destMotion->duration());
	//LOG(" Number of frames in destMotion in start is %d", destMotion->getNumFrames());

	//LOG(" Duration of the srcMotion in start is  %f", srcMotion->duration());
	//LOG(" Number of frames in srcMotion in start is %d", srcMotion->getNumFrames());

	// find all the offsets 
	for (size_t i = 0 ; i < descendants.size() ; i++)
	{
		// get the channel id 
		int chanIdx = channels.search(descendants[i]->getMappedJointName(), SkChannel::Quat);
		int floatIdx = channels.float_position(chanIdx);

		// store the offsets
		SrQuat rotLast  = SrQuat(frameLast[floatIdx], frameLast[floatIdx+1], frameLast[floatIdx+2], frameLast[floatIdx+3]);
		SrQuat rotFirst  = SrQuat(frameFirst[floatIdx], frameFirst[floatIdx+1], frameFirst[floatIdx+2], frameFirst[floatIdx+3]);
		
		// find the offset
		SrQuat offset = rotFirst.inverse()*rotLast;

		// normalize the quaternion
		offset.normalize();

		// store this offset
		offsets.push_back(offset);
	
	}

	// print all offsets here
	/*for ( int i = 0 ; i < offsets.size() ; i++)
	{
		LOG ("offset is %s ", offsets[i].toString().c_str());
	}*/

	// find the mean angular difference
	float total = 0;
	for ( int i = 0 ; i < offsets.size() ; i++)
	{
		SrQuat offset = offsets[i];
		total += offset.angle();
	}

	// print out the mean angle
	float mean = total / (float)offsets.size(); 
	//LOG ("the mean angle is %f " , mean);

	// chek if mean offset is more than 5 degrees ( or 0.087266 radians)
	if ( mean > threshold )
	{
		// set addoiffset to false
		//LOG ("Mean angle is higher than threshold, interpolating between frames ");
		addOffset = false;

		// find the frames from 90% to 100%
		int startFrame =  destMotion->getNumFrames() - 25;

		if (startFrame < 0 )
			startFrame = 0;

		int endFrame   = destMotion->getNumFrames();
		float destMotionFrameRate = destMotion->duration() / destMotion->getNumFrames();

		// clear the offsets
		offsets.clear();

		// create new offsets  
		for (int i = 0 ; i < descendants.size() ; i++)
		{
			// get the channel id 
			int chanIdx = channels.search(descendants[i]->getMappedJointName(), SkChannel::Quat);
			int floatIdx = channels.float_position(chanIdx);

			// store the offsets
			SrQuat rotLast  = SrQuat(frameLast[floatIdx], frameLast[floatIdx+1], frameLast[floatIdx+2], frameLast[floatIdx+3]);
			SrQuat rotFirst  = SrQuat(frameFirst[floatIdx], frameFirst[floatIdx+1], frameFirst[floatIdx+2], frameFirst[floatIdx+3]);
			
			// find the offset
			SrQuat offset = rotLast.inverse()*rotFirst;

			// normalize the quaternion
			offset.normalize();

			// store this offset
			offsets.push_back(offset);
		}

		// add blend to the first motion
		for ( int i = startFrame ; i < endFrame ; i++ )
		{
			// get the frame first
			std::vector<float> frameCurr = destMotion->getFrameData(i);
			float* frameBuf = destMotion->posture(i);

			// for all desendants
			for (int j = 0 ; j < descendants.size() ; j++ )
			{
				// get the channel id 
				int chanIdx = channels.search(descendants[j]->getMappedJointName(), SkChannel::Quat);
				int floatIdx = channels.float_position(chanIdx);

				// find quaternions
				SrQuat rot = SrQuat(frameCurr[floatIdx], frameCurr[floatIdx+1], frameCurr[floatIdx+2], frameCurr[floatIdx+3]);

				// multiply with the offset 
				float weight = ( (float)( i - startFrame) / (float)(endFrame - startFrame));
				//LOG ("weight is %f ", weight);
				SrQuat identity(1,0,0,0);
				SrQuat newOffset = slerp( identity, offsets[j], weight);
				//LOG("new offset is %s ", newOffset.toString().c_str());
				SrQuat newRot = rot * newOffset;

				// put the rotation back 
				frameBuf[floatIdx] =  newRot.getData(0);
				frameBuf[floatIdx + 1] =  newRot.getData(1);
				frameBuf[floatIdx + 2] =  newRot.getData(2);
				frameBuf[floatIdx + 3] =  newRot.getData(3);
			}
		}
	}
	
	// get the frames and put it in the first motion with offset
	for (int i = 0 ; i < srcMotion->getNumFrames() ; i++ )
	{
		// get the data
		std::vector<float> frameCurr = srcMotion->getFrameData(i);

		// add offset to all the joints
		for (int j = 0 ; j < descendants.size() ; j++ )
		{
			// get the channel id 
			int chanIdx = channels.search(descendants[j]->getMappedJointName(), SkChannel::Quat);
			int floatIdx = channels.float_position(chanIdx);

			// find quaternions
			SrQuat rot = SrQuat(frameCurr[floatIdx], frameCurr[floatIdx+1], frameCurr[floatIdx+2], frameCurr[floatIdx+3]);

			// multiply with the offset
			SrQuat newRot;
			if (addOffset)
				newRot = rot * offsets[j];
			else
				newRot = rot;

			// put the rotation back 
			frameCurr[floatIdx] =  newRot.getData(0);
			frameCurr[floatIdx + 1] =  newRot.getData(1);
			frameCurr[floatIdx + 2] =  newRot.getData(2);
			frameCurr[floatIdx + 3] =  newRot.getData(3);
		}

		// put in the new frameTime
		float frameTime = invFrameRate * i +  destMotionDuration; 

		// put the frame back
		destMotion->addFrame( frameTime, frameCurr);
	}

	// print out some stuff 
	if (_printDebug)
	{
		LOG(" Num frames and Duration of the motion %s is %d/%f", destMotion->getName().c_str(), destMotion->getNumFrames(), destMotion->duration());
	}

	//LOG(" Combining motions completed ");
	//LOG(" ");
}

// get right database
MotionDatabase* SBHandSynthesis::getRightDb()
{
	return _rightDb;
}

MotionDatabase* SBHandSynthesis::getLeftDb()
{
	return _leftDb;
}

// print out the results
void SBHandSynthesis::printResults()
{
	// print the current database
	std::string fileName = _bodyMotion->getName()+"results_";

	// generate the file name
	std::string filePath =  SmartBody::SBScene::getScene()->getMediaPath() 
								+ "/Hand/" + fileName + "_" + _selectDb->getJointName() + ".txt";

	if (_printDebug)
		LOG ("Saving to file path %s ", filePath.c_str());

	// open stream
	std::ofstream myFile(filePath.c_str());

	if (!myFile.is_open())
	{
		std::cout << "Unable to open file " << filePath << std::endl;
		return;
	}

	// print wrist joint name first 
	myFile << "Database : " << _selectDb->getJointName() << "\n";

	// print all the database motion names
	myFile << "\n Body Database Motions : \n";
	for ( size_t i = 0 ; i < _bodyDbMotion.size() ; i++)
	{
		_selectDb->printMotion(_bodyDbMotion[i], myFile);
	}

	// print the hand db motions 
	myFile << "\n Hand Database Motions : \n";
	for ( size_t i = 0 ; i < _handDbMotion.size() ; i++)
	{
		_selectDb->printMotion(_handDbMotion[i], myFile);
	}	

	// print the database itself
	_selectDb->printDatabase(myFile);
}

// initialize the motion datbase
MotionDatabase::MotionDatabase()
{
	_finalMotion = new SmartBody::SBMotion();
}


MotionDatabase::~MotionDatabase()
{
	clearDb();

	// clear body segments
	for (size_t i = 0 ; i < _bodyDatabaseSegments.size() ; i++)
	{
		delete _bodyDatabaseSegments[i];
	}

	// clear hand segments
	for (size_t i = 0 ; i < _handDatabaseSegments.size() ; i++)
	{
		delete _handDatabaseSegments[i];
	}

	// only clear the datbases here
	_handDatabaseSegments.clear();
	_bodyDatabaseSegments.clear();
}

// clear the database
void MotionDatabase::clearDb()
{
	// clear final motion
	if (_finalMotion)
	{
		delete _finalMotion;

		// reinitialize
		_finalMotion = new SmartBody::SBMotion();
	}

	// clera motion segments
	for (size_t i = 0 ; i < _motionSegments.size() ; i++ )
	{
		delete _motionSegments[i];
	}

	// clear the datbases 
	_motionSegments.clear();
	_similarSegments.clear();
	_finalMotionIndices.clear();
}

// get the segments
std::vector<SBMotion*>& MotionDatabase::getBodyDbSegments() 
{ 
	return _bodyDatabaseSegments;
}


// get the hand database segments
std::vector<SBMotion*>& MotionDatabase::getHandDbSegments() 
{ 
	return _handDatabaseSegments;
}

std::vector<SBMotion*>& MotionDatabase::getMotionSegments()
{
	return _motionSegments;
}

// get similar segments
std::vector<std::vector<std::pair<int,float> > >& MotionDatabase::getSimilarSegments()
{
	return _similarSegments;
}

// get final motion indices
std::vector<int>& MotionDatabase::getMotionIndices()
{
	return _finalMotionIndices;
}

// get the segment at a certina location
SmartBody::SBMotion* MotionDatabase::getBodyDbSegment(int i) 
{ 
	return _bodyDatabaseSegments[i];
}

// get hand segment at certain location
SmartBody::SBMotion* MotionDatabase::getHandDbSegments(int i ) 
{ 
	return _handDatabaseSegments[i];
}

// get the final motion
SmartBody::SBMotion* MotionDatabase::getFinalMotion()
{
	return _finalMotion;
}

// add to body database
void MotionDatabase::addBodyDbSegment(SmartBody::SBMotion* segment) 
{
	_bodyDatabaseSegments.push_back(segment);
}

// add to hand database
void MotionDatabase::addHandDbSegment(SmartBody::SBMotion* segment) 
{
	_handDatabaseSegments.push_back(segment);
}

// add motion segment
void MotionDatabase::addMotionSegment(SmartBody::SBMotion* segment)
{
	_motionSegments.push_back(segment);
}

// adds a cost list
void MotionDatabase::addCostList(CostList costList)
{
	_similarSegments.push_back(costList);
}

// add a motion index for final motion
void MotionDatabase::addMotionIndex(int index)
{
	_finalMotionIndices.push_back(index);
}

void MotionDatabase::setJointName(std::string jointName)
{
	_jointName = jointName;
}

std::string MotionDatabase::getJointName()
{
	return _jointName;
}

// print the motion
void MotionDatabase::printMotion(SBMotion *motion, std::ofstream& myFile)
{
	//LOG ("Motion -> {name : %s , duration: %f, frames : %d } ", motion->getName(), motion->getDuration(), motion->getNumFrames());
	myFile << "Motion -> {name : " << motion->getName() << " , duration: "<< motion->getDuration() <<", frames : "<< motion->getNumFrames() << " } \n";
}

// print the database
void MotionDatabase::printDatabase(std::ofstream& myFile)
{

	// first print the body database segments
	myFile << "\n Body Database : \n";
	for (size_t i = 0 ; i < _bodyDatabaseSegments.size() ; i++ )
	{
		printMotion(_bodyDatabaseSegments[i], myFile);
	}

	// print the hand database segments
	myFile << "\n Hand Database : \n";
	for ( size_t i = 0 ; i < _handDatabaseSegments.size() ; i++ )
	{
		printMotion(_handDatabaseSegments[i], myFile);
	}

	// print out all the motion segments
	myFile << "\n Motion Segments : \n";
	for (int i = 0 ; i < _motionSegments.size() ; i++)
	{
		printMotion(_motionSegments[i], myFile);
	}

	// printing segments
	myFile << "\n Similar Segments : \n";
	for (size_t i = 0 ; i < _similarSegments.size() ; i++)
	{
		// retrieve the cost list
		CostList list = _similarSegments[i];

		// print the level 
		myFile << "Cost vector "<< i << " : ";

		// print the cost list 
		for ( int j = 0 ; j < list.size() ; j++)
		{
			// retrieve the item
			std::pair<int, float> item = list[j];

			// print the pair
			myFile << "( " << item.first << "," << item.second << " ) ";
		}

		// end the line
		myFile << " \n";
	}

	// final indices calculated
	myFile << "\n Final motion indices : ";
	for ( size_t i = 0 ; i < _finalMotionIndices.size() ; i++ )
	{
		myFile << _finalMotionIndices[i] << " , ";
	}

	// and finally print the final motion
	myFile << "\n Final Motion : ";
	printMotion(_finalMotion, myFile);

	// close the file 
	myFile.close();
}



}
