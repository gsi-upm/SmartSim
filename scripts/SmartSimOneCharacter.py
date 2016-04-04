
import io
import httplib
import time
import ConfigParser
import math
import json

agentStep = []
inside = []
firstStep = []
timeStart = []
timeStop = []
typesCharacters = []
amountLeaders = None
behaviors = False

results = io.open('SmartSimResults', 'wb')

simulationStarted = False

config = ConfigParser.RawConfigParser()
config.read('SmartSimSettings.cfg')	

amountAgents = 1
ubikSimServer = config.get('Settings', 'ubikSimServer')
meshScenario = config.get('Settings', 'meshScenario')
modeSimulation = config.get('Settings', 'modeSimulation')

print amountAgents
print ubikSimServer
print meshScenario
print modeSimulation

print "|--------------------------------------------|"
print "|            Starting SmartSim               |"
print "|--------------------------------------------|"
print ""

scene.addAssetPath('script', 'scripts')
scene.loadAssets()

scene.run('configureModule.py')
scene.run('agentsCreationModule.py')
scene.run('tools.py')
scene.run('connectionsModule.py')
scene.run('scenarioModule.py')
scene.run('locomotionModule.py')

print "Initiating Scene"	
ConfigureModule().init()

print "Initiating global variables"
ConfigureModule().initGlobalVariables(amountAgents)

print "Setting scene limits"
vectorLimits = ConfigureModule().autoSetLimits(ubikSimServer)
xLimitScene = vectorLimits[0]
yLimitScene = vectorLimits[1]
ConfigureModule().setLimits(xLimitScene, yLimitScene)

print "Setting scenario"
ScenarioModule().addScenario(meshScenario)
ScenarioModule().addEmergency(ubikSimServer)
	
print "Creating agents"
AgentsCreationModule().settleAgents(amountAgents, ubikSimServer)

print "Configuring camera settings"
cameraCenter=ConfigureModule().autoSetCamera(amountAgents)
cameraEye = [cameraCenter[0]+5, 5, cameraCenter[2]+2]
cameraPosition = cameraEye
ConfigureModule().camera(cameraEye, cameraCenter, cameraPosition)
	
print "Getting routes"
ConnectionsModule().initSteps(amountAgents)
	
steerManager = scene.getSteerManager()
steerManager.setEnable(False)
steerManager.setEnable(True)
	
print "Scene settled"
	
scene.removeScript('locomotion')
locomotion = LocomotionModule()
scene.addScript('locomotion', locomotion)

def play():
	LocomotionModule().playSimulation()

def stop():
	LocomotionModule().finishSimulation()
