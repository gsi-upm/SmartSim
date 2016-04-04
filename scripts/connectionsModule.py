class ConnectionsModule():

	def getUbikSimRoutes(self, ubikSimServer):

		'''Gets agents routes from UbikSim'''

		conn = httplib.HTTPConnection(ubikSimServer)
		conn.request('GET', '/UbikSimMOSI-AGIL-Server/ubiksim?position=goals')
		data = conn.getresponse()
		data = data.read()
		jsondata = json.loads(data)
		listdata = jsondata.items()
		
		step = []
		agentRoute = []
		routes = []

		for i in range (len(listdata)):
			for j in range (len(listdata[i][1]['goalPath'])):
				xStep = Tools().xFromVec(listdata[i][1]['goalPath'][j]) * 0.3
				yStep = Tools().yFromVec(listdata[i][1]['goalPath'][j]) * 0.3
				step.append(xStep)
				step.append(yStep)
				agentRoute.append(step)
				step = []
			routes.append(agentRoute)
			agentRoute = []

		return routes

	def getUbikSimPositions(self, ubikSimServer):

		'''Gets agents initial positions from UbikSim'''

		conn = httplib.HTTPConnection(ubikSimServer)
		conn.request('GET', '/UbikSimMOSI-AGIL-Server/ubiksim?position=people')
		data = conn.getresponse()
		data = data.read()
		jsondata = json.loads(data)
		listdata = jsondata.items()
		
		for i in range (len(listdata)):
			listdata[i][1]["positionX"] = listdata[i][1]["positionX"] * 0.3
			listdata[i][1]["positionY"] = listdata[i][1]["positionY"] * 0.3

		return listdata

	def getUbikSimEmergency(self, ubikSimServer):
		
		global positionEmergency
		conn = httplib.HTTPConnection(ubikSimServer)
		conn.request('GET', '/UbikSimMOSI-AGIL-Server/ubiksim?position=emergency')
		data = conn.getresponse()
		data = data.read()

		emergencyUbikSim = (data[data.index('('):data.index(')')+1])
		
		xVector = Tools().xFromVec(emergencyUbikSim) * 0.3
		yVector = Tools().yFromVec(emergencyUbikSim) * 0.3

		vectorEmergency = SrVec(xVector, 0, yVector)

		return vectorEmergency

	def getFullRoute(self, ubikSimServer, agentIndex):
		
		agentsSteps = ConnectionsModule().getUbikSimRoutes(ubikSimServer)

		fullRoute = ''

		for i in range (len(agentsSteps[agentIndex])):
		
			xRoute = str(agentsSteps[agentIndex][i][0])
			yRoute = str(agentsSteps[agentIndex][i][1])

			fullRoute = fullRoute + xRoute + ' ' + yRoute + ' '

		return fullRoute

	def getMaxScenario(self, ubikSimServer):
		
		conn = httplib.HTTPConnection(ubikSimServer)
		conn.request('GET', '/UbikSimMOSI-AGIL-Server/ubiksim?position=map')
		data = conn.getresponse()
		data = data.read()
		jsondata = json.loads(data)
		listdata = jsondata.items()

		maxX = listdata[0][1] * 0.3
		maxY = listdata[2][1] * 0.3

		vectorMax = [maxX, maxY]

		return vectorMax

	def initSteps(self, amountAgents):

		'''Creates a list that records how many steps of the route has an agent passed'''
	
		for i in range (amountAgents):
			agentStep.append(0)

	def getSteps(self):

		'''Returns the steps list'''

		return agentStep

	def getStepIndex(self, index):

		'''Return the step of the route which an agent is passing'''

		return agentStep[index]

	def setSteps(self, newArray):

		'''Changes the steps list'''		

		agentStep = newArray

	def setStepIndex(self, index, value):

		'''Sets the step in which an agent is'''

		agentStep[index] = value
