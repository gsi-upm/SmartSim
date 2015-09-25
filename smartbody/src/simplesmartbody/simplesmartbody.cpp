#include "vhcl.h"
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBPython.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBBmlProcessor.h>
#include <sb/SBSceneListener.h>

class SimpleListener : public SmartBody::SBSceneListener
{
   public:
	   SimpleListener() {}
	   ~SimpleListener() {}
     
	  virtual void OnLogMessage( const std::string & message )
	  {
#ifdef WIN32
		LOG("%s", message.c_str());
#else
		std::cout << message << std::endl;
#endif
	  }
};

int main( int argc, char ** argv )
{
	// set the relative path from the location of the simplesmartbody binary to the data directory
	// if you are downloading the source code from SVN, it will be ../../../../data
	//std::string mediaPath = "../../../../data";
	// if you're using the SDK, this path will be ../data
	std::string mediaPath = "../data";

	// add a message logger to stdout
	vhcl::Log::StdoutListener* stdoutLog = new vhcl::Log::StdoutListener();
	vhcl::Log::g_log.AddListener(stdoutLog);

	LOG("Loading Python...");

	// initialize the Python libraries
	initPython("../Python27/Libs");

	// get the scene object
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	SimpleListener listener;
	scene->addSceneListener(&listener);

	// set the mediapath which dictates the top-level asset directory
	scene->setMediaPath(mediaPath);

	// indicate where different assets will be located
	// "motion" = animations and skeletons
	// "script" = Python scripts to be executed
	// "mesh" = models and textures
	scene->addAssetPath("motion", "ChrBrad");

	// load the assets from the indicated locations
	LOG("Loading Assets...");
	scene->loadAssets();
	int numMotions = scene->getNumMotions();
	LOG("Loaded %d motions...", numMotions);

	// create a character
	LOG("Creating the character...");
	SmartBody::SBCharacter* character = scene->createCharacter("mycharacter", "");

	// load the skeleton from one of the available skeleton types
	SmartBody::SBSkeleton* skeleton = scene->createSkeleton("ChrBrad.sk");

	// attach the skeleton to the character
	character->setSkeleton(skeleton);

	// create the standard set of controllers (idling, gesture, nodding, etc.)
	character->createStandardControllers();

	// get the simulation object 
	SmartBody::SBSimulationManager* sim = scene->getSimulationManager();

	// if you want to use a real-time clock do the following:
	bool useRealTimeClock = true;
	if (useRealTimeClock)
	{
		sim->setupTimer();
	}
	else
	{
		// otherwise, the time will run according
		sim->setTime(0.0);
	}

	// make the character do something
	scene->getBmlProcessor()->execBML("mycharacter", "<body posture=\"ChrBrad@Idle01\"/>");
	
	LOG("Starting the simulation...");
	double lastPrint = 0;
	sim->start();
	while (sim->getTime() < 100.0) // run for 100 simulation seconds
	{
		scene->update();
		if (!useRealTimeClock)
			sim->setTime(sim->getTime() + 0.16); // update at 1/60 of a second when running in simulated time
		else
			sim->updateTimer();
		

		if (sim->getTime() > lastPrint)
		{
			LOG("Simulation is at time: %5.2f\n", sim->getTime());
			lastPrint = sim->getTime() + 10;
		}

		const std::vector<std::string>& characterNames = scene->getCharacterNames();
		for (size_t c = 0; c < characterNames.size(); c++)
		{
			SmartBody::SBCharacter* character = scene->getCharacter(characterNames[c]);
			std::string jointName = "JtRoot";
			SmartBody::SBJoint* joint = character->getSkeleton()->getJointByName(jointName);
			if (joint)
			{
				SrVec position = joint->getPosition();
				LOG("Character %s joint %s is at position (%f, %f, %f)", character->getName().c_str(), jointName.c_str(), position.x, position.y, position.z);
			}
		}
	}

	sim->stop();

	
	
	return 0;
}
