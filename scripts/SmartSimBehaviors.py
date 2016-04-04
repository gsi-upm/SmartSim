
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
typesCharacters=[]

amountLeaders = None

results = io.open('SmartSimResults', 'wb')

behaviors = True
simulationStarted = False

config = ConfigParser.RawConfigParser()
config.read('SmartSimSettings.cfg')	

amountAgents = config.getint('Settings', 'amountAgents')
ubikSimServer = config.get('Settings', 'ubikSimServer')
meshScenario = config.get('Settings', 'meshScenario')
modeSimulation = 'gettingSteps'

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

positionEmergency=ConnectionsModule().getUbikSimEmergency(ubikSimServer)
	
print "Creating agents"
AgentsCreationModule().settleAgents(amountAgents, ubikSimServer)

print "Configuring camera settings"
cameraCenter=ConfigureModule().autoSetCamera(amountAgents)
cameraEye = [cameraCenter[0], 30, cameraCenter[2]+40]
cameraPosition = cameraEye
ConfigureModule().camera(cameraEye, cameraCenter, cameraPosition)
	
print "Getting routes"
ConnectionsModule().initSteps(amountAgents)

print 'Steering'
	
steerManager = scene.getSteerManager()
steerManager.setEnable(False)
steerManager.setEnable(True)
	
print "Scene settled"

def play():
	LocomotionModule().playSimulation()

def stop():
	LocomotionModule().finishSimulation()

def createAgent(name, x, y):
	AgentsCreationModule().addAgent(name, SrVec(x, 0, y))
	
scene.removeScript('locomotion')
locomotion = LocomotionModule()
scene.addScript('locomotion', locomotion)

