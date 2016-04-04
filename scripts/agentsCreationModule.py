class AgentsCreationModule():

	#from connectionsModule import ConnectionsModule

	def settleAgents(self, amountAgents, ubikSimServer):

		'''Adds the desired amount of agents to SmartSim'''
		
		#Getting list of initial positions from UbikSim
		listData = ConnectionsModule().getUbikSimPositions(ubikSimServer)
		
		#Creating the desired amount of agents
		for i in range(amountAgents):
			agentName = 'Agent%s' % i
			characterKind = 'default'
			posX = listData[i][1]["positionX"]
			posZ = listData[i][1]["positionY"]
			agentPosition = SrVec(posX, 0, posZ)
			for typeCharacter in typesCharacters:
				if (i in typeCharacterDict[typeCharacter]):
					characterKind = str(typeCharacter)
			self.addAgent(agentName, agentPosition, characterKind)

		# Set up list of Brads
		agentsList = []
		for name in scene.getCharacterNames():
			if 'Agent' in name:
				agentsList.append(scene.getCharacter(name))

	def addAgent (self, agentName, agentPosition, *characterType):

		'''Adds an agent to SmartSim'''

		#Creation of the agent
		agent = scene.createCharacter(agentName, '')
		
		for character in characterType:
			characterType = character
		characterType = str(characterType)
		
		#Creation of the agent structure
		if (characterType == 'rachel'):
			print 'Hola'
			print characterType
			agentSkeleton = scene.createSkeleton('ChrRachel.sk')
		else:
			print 'Que te den'
			print characterType
			agentSkeleton = scene.createSkeleton('ChrBrad.sk')
		
		agent.setSkeleton(agentSkeleton)

		#Setting up agent position
		agent.setPosition(agentPosition)

		#Creation of standard controllers
		agent.createStandardControllers()
		
		#Adding deformable mesh to agent
		agent.setVec3Attribute('deformableMeshScale', .01, .01, .01)
		if (characterType == 'rachel'):
			agent.setStringAttribute('deformableMesh', 'ChrRachel.dae')
		elif (characterType == 'maarten'):
			agent.setStringAttribute('deformableMesh', 'ChrMaarten.dae')
		else:			
			agent.setStringAttribute('deformableMesh', 'ChrBrad.dae')
		
		if (characterType == 'maarten'):
			scene.run('BehaviorSetMaleMocapLocomotion.py')
			setupBehaviorSet()
			retargetBehaviorSet(agentName)
			scene.run('BehaviorSetGestures.py')
			setupBehaviorSet()
			retargetBehaviorSet(agentName)
		else:
			scene.run('BehaviorSetMaleLocomotion.py')
			setupBehaviorSet()
			retargetBehaviorSet(agentName)
			scene.run('BehaviorSetGestures.py')
			setupBehaviorSet()
			retargetBehaviorSet(agentName)

		#Adding Steer manager to agent
		steerManager = scene.getSteerManager()		
		steerAgent = steerManager.createSteerAgent(agentName)
		steerAgent.setSteerStateNamePrefix("all")
		steerAgent.setSteerType("example")
		agent.setBoolAttribute('steering.pathFollowingMode', False)

		#Setting initial body posture
		bml.execBML(agentName, '<body posture="ChrBrad@Idle01"/>')

		scene.getCharacter(agentName).setStringAttribute("displayType", "mesh")
