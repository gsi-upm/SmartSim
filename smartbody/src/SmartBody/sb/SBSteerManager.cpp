#include "SBSteerManager.h"

#include <sb/SBSimulationManager.h>
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBAttribute.h>
#include <PPRAgent.h>
#include <sb/SBSteerAgent.h>
#include <sbm/PPRAISteeringAgent.h>
#include <sbm/SteerSuiteEngineDriver.h>
#include <SteerLib.h>

namespace SmartBody {

SBSteerManager::SBSteerManager() : SBService()
{
	setName("steering");
	_driver = new SteerSuiteEngineDriver();
#ifdef WIN32
			createStringAttribute("aimodule", "pprAI", true, "Basic", 60, false, false, false, "Agent module library");
#endif
#ifdef __linux__
			createStringAttribute("aimodule", "libpprAI", true, "Basic", 60, false, false, false, "Agent module library");
#endif
#ifdef __APPLE__
			createStringAttribute("aimodule", "libpprAI", true, "Basic", 60, false, false, false, "Agent module library");
#endif

	createStringAttribute("engineOptions.testCaseSearchPath", "../../../../core/smartbody/steersuite-1.3/testcases/", true, "Basic", 60, false, false, false, "Path to find agent shared libraries");
	createStringAttribute("engineOptions.moduleSearchPath", "../../../../core/smartbody/sbgui/bin/", true, "Basic", 60, false, false, false, "Path to find test cases");
	createDoubleAttribute("gridDatabaseOptions.gridSizeX", 35, true, "Basic", 60, false, false, false, "Size of grid in x dimension.");
	createDoubleAttribute("gridDatabaseOptions.gridSizeZ", 35, true, "Basic", 60, false, false, false, "Size of grid in z dimension.");
	createIntAttribute("gridDatabaseOptions.numGridCellsX", 70, true, "Basic", 60, false, false, false, "Number of grid cells in x dimension.");
	createIntAttribute("gridDatabaseOptions.numGridCellsZ", 70, true, "Basic", 60, false, false, false, "Number of grid cells in z dimension.");
	createIntAttribute("gridDatabaseOptions.maxItemsPerGridCell", 7, true, "Basic", 60, false, false, false, "Max units per grid cell. If agent density is high, make sure increase this value.");
	createDoubleAttribute("initialConditions.radius", 0.4, true, "Basic", 60, false, false, false, "Initial radius of agents in meters.");
	createBoolAttribute("addBoundaryWalls", true, true, "Basic", 60, false, false, false, "Adds boundaries around the perimeter of the grid to prevent agents from leaving grid area.");
	createBoolAttribute("useEnvironmentCollisions", true, true, "Basic", 60, false, false, false, "Determines whether to include the environment (pawns) when determining steering path. If set to false, objects in the environment will be ignored.");
	createDoubleAttribute("maxUpdateFrequency", 60.0, true, "Basic", 60, false, false, false, "Maximum frequency of steering updates.");	
}

SBSteerManager::~SBSteerManager()
{
	std::map<std::string, SBSteerAgent*>::iterator iter = _steerAgents.begin();
	for (; iter != _steerAgents.end(); iter++)
	{
		delete iter->second;
	}
	_steerAgents.clear();

	delete _driver;

	// TODO: boundaryObstacles
}

SteerSuiteEngineDriver* SBSteerManager::getEngineDriver()
{
	return _driver;
}

void SBSteerManager::setEnable(bool enable)
{
	SBService::setEnable(enable);
	if (enable)
		start();
	else
		stop();
	SmartBody::SBAttribute* attribute = getAttribute("enable");
	if (attribute)
	{
		SmartBody::BoolAttribute* enableAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
		enableAttribute->setValueFast(enable);
	}
}

void SBSteerManager::beforeUpdate(double time)
{
	
}

void SBSteerManager::update(double time)
{
	

	SBScene* scene = SmartBody::SBScene::getScene();
	if (getEngineDriver()->isInitialized())
	{
		if (!getEngineDriver()->isDone())
		{

			if (getEngineDriver()->getStartTime() == 0.0)
			{
				getEngineDriver()->setStartTime(SmartBody::SBScene::getScene()->getSimulationManager()->getTime());
				getEngineDriver()->setLastUpdateTime(SmartBody::SBScene::getScene()->getSimulationManager()->getTime() - _maxUpdateFrequency - .01);
			}

			double maxFrequency = getDoubleAttribute("maxUpdateFrequency");
			if (maxFrequency != 0.0)
				_maxUpdateFrequency = 1.0 / maxFrequency;
			else
				_maxUpdateFrequency = .016;
			double timeDiff = SmartBody::SBScene::getScene()->getSimulationManager()->getTime() - getEngineDriver()->getLastUpdateTime();
			const std::vector<std::string>& characterNames = SmartBody::SBScene::getScene()->getCharacterNames();
			for (std::vector<std::string>::const_iterator iter = characterNames.begin();
				iter != characterNames.end();
				iter++)
			{
				SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(*iter);
				SmartBody::SBSteerAgent* steerAgent = getSteerAgent(character->getName());
				if (steerAgent)
					steerAgent->evaluate(timeDiff);
			}

			if (timeDiff >= _maxUpdateFrequency)
			{ // limit steering to 60 fps
				getEngineDriver()->setLastUpdateTime(SmartBody::SBScene::getScene()->getSimulationManager()->getTime());
			
				bool running = getEngineDriver()->_engine->update(false, true, (float) (SmartBody::SBScene::getScene()->getSimulationManager()->getTime() - getEngineDriver()->getStartTime()));
				if (!running)
					getEngineDriver()->setDone(true);
			}

		}
	}
}

void SBSteerManager::afterUpdate(double time)
{
}

void SBSteerManager::start()
{
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();

	
	if (scene->getSteerManager()->getEngineDriver()->isInitialized())
	{
		LOG("STEERSIM ALREADY STARTED");
		return;
	}

	SteerLib::SimulationOptions* steerOptions = new SteerLib::SimulationOptions();
	steerOptions->moduleOptionsDatabase["testCasePlayer"]["testcase"] = "3-way-confusion-1.xml";
	std::string ai = dynamic_cast<SmartBody::StringAttribute*>( SmartBody::SBScene::getScene()->getSteerManager()->getAttribute("aimodule") )->getValue();

	if (ai == "")
		return;
	steerOptions->moduleOptionsDatabase["testCasePlayer"]["ai"] = ai;
	steerOptions->engineOptions.startupModules.insert("testCasePlayer");
	std::string testCases = dynamic_cast<SmartBody::StringAttribute*>( SmartBody::SBScene::getScene()->getSteerManager()->getAttribute("engineOptions.testCaseSearchPath") )->getValue();
	steerOptions->engineOptions.testCaseSearchPath = testCases;
	std::string moduleSearchPath = dynamic_cast<SmartBody::StringAttribute*>( SmartBody::SBScene::getScene()->getSteerManager()->getAttribute("engineOptions.moduleSearchPath") )->getValue();
	steerOptions->engineOptions.moduleSearchPath = moduleSearchPath;
	double gridSizeX = dynamic_cast<SmartBody::DoubleAttribute*>( SmartBody::SBScene::getScene()->getSteerManager()->getAttribute("gridDatabaseOptions.gridSizeX") )->getValue();
	double gridSizeZ = dynamic_cast<SmartBody::DoubleAttribute*>( SmartBody::SBScene::getScene()->getSteerManager()->getAttribute("gridDatabaseOptions.gridSizeZ") )->getValue();
	steerOptions->gridDatabaseOptions.gridSizeX = float(gridSizeX);
    steerOptions->gridDatabaseOptions.gridSizeZ = float(gridSizeZ);
	int numGridCellsX = dynamic_cast<SmartBody::IntAttribute*> (SmartBody::SBScene::getScene()->getSteerManager()->getAttribute("gridDatabaseOptions.numGridCellsX"))->getValue();
	int numGridCellsZ = dynamic_cast<SmartBody::IntAttribute*> (SmartBody::SBScene::getScene()->getSteerManager()->getAttribute("gridDatabaseOptions.numGridCellsZ"))->getValue();
	int maxItemsPerGridCell = dynamic_cast<SmartBody::IntAttribute*> (SmartBody::SBScene::getScene()->getSteerManager()->getAttribute("gridDatabaseOptions.maxItemsPerGridCell"))->getValue();
	//LOG("max Items per grid cell = %d",maxItemsPerGridCell);
	steerOptions->gridDatabaseOptions.numGridCellsX = numGridCellsX;
	steerOptions->gridDatabaseOptions.numGridCellsZ = numGridCellsZ;
	steerOptions->gridDatabaseOptions.maxItemsPerGridCell = maxItemsPerGridCell;

	bool setBoundaries = SmartBody::SBScene::getScene()->getSteerManager()->getBoolAttribute("addBoundaryWalls");
	if (setBoundaries)
	{
		for (std::vector<SteerLib::BoxObstacle*>::iterator iter = _boundaryObstacles.begin();
			 iter != _boundaryObstacles.end();
			 iter++)
		{
			getEngineDriver()->_engine->removeObstacle((*iter));
			getEngineDriver()->_engine->getSpatialDatabase()->removeObject((*iter), (*iter)->getBounds());
			delete (*iter);
		}
		SteerLib::BoxObstacle* top = new SteerLib::BoxObstacle((float) -gridSizeX / 2.0f, (float) gridSizeX / 2.0f, 0.0f,  1.0f, (float) -gridSizeZ / 2.0f, (float) -gridSizeZ / 2.0f + 1.0f);
		_boundaryObstacles.push_back(top);
		SteerLib::BoxObstacle* bottom = new SteerLib::BoxObstacle((float) -gridSizeX / 2.0f, (float) gridSizeX / 2.0f, 0.0f,  1.0f, (float) gridSizeZ / 2.0f - 1.0f, (float) gridSizeZ / 2.0f);
		_boundaryObstacles.push_back(bottom);
		SteerLib::BoxObstacle* left = new SteerLib::BoxObstacle((float) -gridSizeX / 2.0f, (float) -gridSizeX / 2.0f + 1.0f, 0.0f,  1.0f, (float) -gridSizeZ / 2.0f, (float) gridSizeZ / 2.0f);
		_boundaryObstacles.push_back(left);
		SteerLib::BoxObstacle* right = new SteerLib::BoxObstacle((float) gridSizeX / 2.0f - 1.0f, (float) gridSizeX / 2.0f, 0.0f,  1.0f, (float) -gridSizeZ / 2.0f, (float) gridSizeZ / 2.0f);
		_boundaryObstacles.push_back(right);
	}

	//	customize the item per grid cell
	//	steerOptions->gridDatabaseOptions.maxItemsPerGridCell = maxItemPerCell;


	//LOG("INIT STEERSIM");
	try {
		SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->init(steerOptions);
	} catch (Util::GenericException& ge) {
		LOG("Problem starting steering engine: %s", ge.what()); 
		SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->finish();
		delete steerOptions;
		return;
	} catch (std::exception& e) {
		if (e.what())
			LOG("Problem starting steering engine: %s", e.what()); 
		else
			LOG("Unknown problem starting steering engine: %s", e.what()); 

		SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->finish();
		delete steerOptions;
		return;
	}

	//LOG("LOADING STEERSIM");
	SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->loadSimulation();

	int numSetup = 0;
	// create an agent based on the current characters and positions
	SteerLib::ModuleInterface* pprAIModule = SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->_engine->getModule(ai);
	const std::vector<std::string>& characterNames = SmartBody::SBScene::getScene()->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = characterNames.begin();
		iter != characterNames.end();
		iter++)
	{
		SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(*iter);
		SmartBody::SBSteerManager* steerManager = SmartBody::SBScene::getScene()->getSteerManager();
		SmartBody::SBSteerAgent* steerAgent = steerManager->getSteerAgent(character->getName());
		if (!steerAgent)
		{
			LOG("No steering agent for character %s", character->getName().c_str());
			continue;
		}

		float x, y, z;
		float yaw, pitch, roll;
		character->get_world_offset(x, y, z, yaw, pitch, roll);
		SteerLib::AgentInitialConditions initialConditions;
		initialConditions.position = Util::Point( x * scene->getScale(), 0.0f, z * scene->getScale() );
		Util::Vector orientation = Util::rotateInXZPlane(Util::Vector(0.0f, 0.0f, 1.0f), yaw * 3.14159f / 180.0f);
		initialConditions.direction = orientation;
		double initialRadius = dynamic_cast<SmartBody::DoubleAttribute*>( SmartBody::SBScene::getScene()->getSteerManager()->getAttribute("initialConditions.radius") )->getValue();
		if (initialRadius == 0.0)
			initialConditions.radius = 0.3f;//0.2f;//0.4f;
		else
			initialConditions.radius = (float) initialRadius;
		initialConditions.speed = 0.0f;
		initialConditions.goals.clear();
		initialConditions.name = character->getName();		
		SteerLib::AgentInterface* agent = SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->_engine->createAgent( initialConditions, pprAIModule );			
		PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
		ppraiAgent->setAgent(agent);
		agent->reset(initialConditions, dynamic_cast<SteerLib::EngineInterface*>(pprAIModule));
		LOG("Setting up steering agent for character %s", character->getName().c_str());
		numSetup++;
	}
	if (numSetup == 0)
	{
		//LOG("No characters set up with steering. Steering will need to be restarted when new characters are available.");
	}

	bool useEnvironment = getBoolAttribute("useEnvironmentCollisions");

	if (useEnvironment)
	{
		// adding obstacles to the steering space
		const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
		for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
			pawnIter != pawns.end();
			pawnIter++)
		{
			SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn((*pawnIter));
			SBCharacter* character = dynamic_cast<SBCharacter*>(pawn);
			if (character) continue; // do not set obstacle for the character, it will mess up the steering
// 			if ((*iter).second->getGeomObject())
// 				(*iter).second->initSteeringSpaceObject();
			if (pawn && pawn->getGeomObject() && pawn->getGeomObject()->geomType() != "null")
				pawn->initSteeringSpaceObject();
		}

	}

	// add any boundary walls, if applicable
	for (std::vector<SteerLib::BoxObstacle*>::iterator iter = _boundaryObstacles.begin();
			 iter != _boundaryObstacles.end();
			 iter++)
	{
		getEngineDriver()->_engine->addObstacle((*iter));
		getEngineDriver()->_engine->getSpatialDatabase()->addObject((*iter), (*iter)->getBounds());
	}

	//LOG("STARTING STEERSIM");
	SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->startSimulation();
	SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->setStartTime(0.0f);

	double maxFrequency = getDoubleAttribute("maxUpdateFrequency");
	if (maxFrequency != 0.0)
		_maxUpdateFrequency = 1.0 / maxFrequency;
	else
		_maxUpdateFrequency = .016;
}

void SBSteerManager::stop()
{
	
	if (SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->isInitialized())
	{
		SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->stopSimulation();
		SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->unloadSimulation();
		SmartBody::SBScene::getScene()->getSteerManager()->getEngineDriver()->finish();

		const std::vector<std::string>& characterNames = SmartBody::SBScene::getScene()->getCharacterNames();
		for (std::vector<std::string>::const_iterator iter = characterNames.begin();
			iter != characterNames.end();
			iter++)
		{
			SmartBody::SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(*iter);
			SmartBody::SBSteerAgent* steerAgent = getSteerAgent(character->getName());
		
			if (steerAgent)
			{
				PPRAISteeringAgent* ppraiAgent = dynamic_cast<PPRAISteeringAgent*>(steerAgent);
				ppraiAgent->setAgent(NULL);
			}
				
		}

		const std::vector<std::string>& pawns = SmartBody::SBScene::getScene()->getPawnNames();
		for (std::vector<std::string>::const_iterator pawnIter = pawns.begin();
			pawnIter != pawns.end();
			pawnIter++)
		{
			SBPawn* pawn = SmartBody::SBScene::getScene()->getPawn((*pawnIter));
			if (pawn->steeringSpaceObj_p)
			{
				delete pawn->steeringSpaceObj_p;
				pawn->steeringSpaceObj_p = NULL;
			}
		}

		for (std::vector<SteerLib::BoxObstacle*>::iterator iter = _boundaryObstacles.begin();
			 iter != _boundaryObstacles.end();
			 iter++)
		{
			delete (*iter);
		}
		_boundaryObstacles.clear();
	}
}

SBSteerAgent* SBSteerManager::createSteerAgent(std::string name)
{
	
	std::map<std::string, SBSteerAgent*>::iterator iter = _steerAgents.find(name);
	if (iter != _steerAgents.end())
	{
		LOG("Steer agent with name %s already exists.", name.c_str());
		return iter->second;
	}
	SBCharacter* character = SmartBody::SBScene::getScene()->getCharacter(name);
	if (!character)
	{
		LOG("Character named '%s' does not exist, steering agent cannot be constructed.", name.c_str());
		return NULL;
	}
	SBSteerAgent* agent = new PPRAISteeringAgent(character);
	_steerAgents.insert(std::pair<std::string, SBSteerAgent*>(name, agent));
	return agent;
}

void SBSteerManager::removeSteerAgent(std::string name)
{
	std::map<std::string, SBSteerAgent*>::iterator iter = _steerAgents.find(name);
	if (iter != _steerAgents.end())
	{
		_steerAgents.erase(iter);
		return;
	}
	LOG("Steer agent with name %s does not exist.", name.c_str());
}

int SBSteerManager::getNumSteerAgents()
{
	return _steerAgents.size();
}

SBSteerAgent* SBSteerManager::getSteerAgent(std::string name)
{
	std::map<std::string, SBSteerAgent*>::iterator iter = _steerAgents.find(name);
	if (iter == _steerAgents.end())
		return NULL;
	else
		return (*iter).second;
}

std::vector<std::string> SBSteerManager::getSteerAgentNames()
{
	std::vector<std::string> steerAgentNames;

	for (std::map<std::string, SBSteerAgent*>::iterator iter = _steerAgents.begin();
		 iter != _steerAgents.end();
		 iter++)
	{
		steerAgentNames.push_back((*iter).first);
	}

	return steerAgentNames;
}

std::map<std::string, SBSteerAgent*>& SBSteerManager::getSteerAgents()
{
	return _steerAgents;
}

void SBSteerManager::onCharacterDelete(SBCharacter* character)
{
	removeSteerAgent(character->getName());
}

}
