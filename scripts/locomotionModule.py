class LocomotionModule(SBScript):

	def singleStep(self):

		'''Updates the simulation'''
		
		listdata = ConnectionsModule().getUbikSimRoutes(ubikSimServer)
		
		#Charges the status of every agent in the simulation
		agentStep = ConnectionsModule().getSteps()
		
		for i in range (amountAgents):
			#Gets one character of the simulation
			agent = scene.getCharacter('Agent%s' % i)
			positionVec = agent.getPosition()
			finalPosition = SrVec(listdata[i][len(listdata[i])-1][0], 0, listdata[i][len(listdata[i])-1][1])

			if (modeSimulation == 'gettingSteps'):

				if (behaviors and firstStep[i]):
					bml.execBML(agent.getName(), '<body posture="ChrBrad@126_fear"/>')
					firstStep[i]

				if (agentStep[i]+1 < len(listdata[i])):

					#Checks if the character has reached the position = 
					reachPosition = SrVec(listdata[i][agentStep[i]][0], 0, listdata[i][agentStep[i]][1])

					if (Tools().distance (positionVec, reachPosition) <=1.8):

						#Directs the agent to the next position
						nextPosition = SrVec(listdata[i][agentStep[i]+1][0], 0, listdata[i][agentStep[i]+1][1])

						if (behaviors and Tools().distance(positionVec, positionEmergency)):
							bml.execBML(agent.getName(), '<locomotion speed="10" target="' + Tools().vec2str(nextPosition) + '"/>')
							bml.execBML(agent.getName(), '<body posture="ChrBrad@Idle01_BeatHighBt01"/>')
							#bml.execBML(agent.getName(), 'gritar Help')
						else:
							bml.execBML(agent.getName(), '<locomotion speed="20" target="' + Tools().vec2str(nextPosition) + '"/>')
						agentStep[i] = agentStep[i]+1

			elif (modeSimulation == 'bmlRoute'):

				if (firstStep[i]):
				
					route = ConnectionsModule().getFullRoute(ubikSimServer, i) 
					agent = scene.getCharacter('Agent%s' % i)

					agent.setBoolAttribute("steering.pathFollowingMode", True)

					bml.execBML(agent.getName(), '<locomotion target="'+route+'"/>')

					firstStep[i] = False
					
					if amountLeaders is not None:
						leaderName = ('Agent%s' % i)
						for j in range (amountFollowers):
							follower = scene.getCharacter(leaderName + '%s' % j)

							follower.setBoolAttribute("steering.pathFollowingMode", True)

							bml.execBML(follower.getName(), '<locomotion target="'+route+'"/>')			

			if (Tools().distance (positionVec, finalPosition) <=1.5 and inside[i]):

				timeExit = time.time() - timeStart[0]

				if (len(timeStart) > 1):

					timePaused = 0					

					for i in range (len(timeStart) - 1):

						timePaused = timePaused + (timePause[i] - timeStart[i])

						timeExit = timeExit - timePaused

				print ('Agent%s has reached his final position' % i)
				print ('Agent%s has left the building' % i)
				results.write('Agent%s succesfully exited the building\n' % i)
				results.write('Agent exited the building in %s seconds\n' % timeExit)
				if (behaviors):
					bml.execBML(agent.getName(), '<body posture="ChrBrad@112_happy"/>')

				inside[i] = False

		#Sets the step of the route in which the agent is
		ConnectionsModule().setSteps(agentStep)

	def update(self, time):
		if (simulationStarted):
			LocomotionModule().singleStep()
	
	def closeDocument(self):
		results.close()

	def pauseSimulation(self):
		timeStop.append(time.time())
		global simulationStarted
		simulationStarted = False

	def finishSimulation(self):
		LocomotionModule().pauseSimulation()
		LocomotionModule().closeDocument()

	def playSimulation(self):
		timeStart.append(time.time())
		global simulationStarted
		simulationStarted = True

