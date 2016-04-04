class ConfigureModule():

	def init(self):

		''' Inits SmartBody'''

		#Add asset paths
		print "Adding assets paths"
		scene.addAssetPath('mesh', 'mesh')
		scene.addAssetPath('motion', 'ChrBrad')
		scene.addAssetPath('motion', 'ChrRachel')
		scene.addAssetPath("script", "behaviorsets")
		scene.addAssetPath('script', 'scripts')
		scene.loadAssets()

		#Set scene scale
		print "Setting scene scale"
		scene.setScale(1.0)
		
		# Set joint map for agents
		print 'Setting up joint map for agents'
		scene.run('zebra2-map.py')
		zebra2Map = scene.getJointMapManager().getJointMap('zebra2')
		bradSkeleton = scene.getSkeleton('ChrBrad.sk')
		zebra2Map.applySkeleton(bradSkeleton)
		zebra2Map.applyMotionRecurse('ChrBrad')
		rachelSkeleton = scene.getSkeleton('ChrRachel.sk')
		zebra2Map.applySkeleton(rachelSkeleton)
		zebra2Map.applyMotionRecurse('ChrRachel')
		
		#Creates scene
		print "Creating scene"
		getViewer().show()

	def initGlobalVariables(self, amountAgents):
		for i in range (amountAgents):
			inside.append(True)
			firstStep.append(True)

	def camera(self, cameraEye, cameraCenter, cameraPosition):

		''' Configures camera'''

		#Gets camera
		getCamera().reset()
		scene.createPawn('camera')
		camera = getCamera()
		
		#Sets camera with provided values
		camera.setEye(cameraEye[0], cameraEye[1], cameraEye[2])
		camera.setCenter(cameraCenter[0], cameraCenter[1], cameraCenter[2])
		scene.getPawn('camera').setPosition(SrVec(cameraPosition[0], cameraPosition[1], cameraPosition[2]))

	def autoSetCamera(self, amountAgents):
		
		xCamera = 0
		yCamera = 0
		
		for i in range (amountAgents):
			
			character = scene.getCharacter(scene.getCharacterNames()[i])
			
			xCamera = xCamera + character.getPosition().getData(0)
			yCamera = yCamera + character.getPosition().getData(2)
		
		xCamera = xCamera/amountAgents
		yCamera = yCamera/amountAgents
		
		centerCamera = [xCamera, 0, yCamera]
		return centerCamera

	def setLimits(self, xLimitScene, yLimitScene):
				
		'''Sets limits of scene grid'''

		steerManager = scene.getSteerManager()

		steerManager.setDoubleAttribute('gridDatabaseOptions.gridSizeX', xLimitScene)
		steerManager.setDoubleAttribute('gridDatabaseOptions.gridSizeZ', yLimitScene)

	def autoSetLimits(self, ubikSimServer):
		vectorLimits = ConnectionsModule().getMaxScenario(ubikSimServer)

		vectorLimits = [vectorLimits[0] + 100, vectorLimits[1] + 100]

		return vectorLimits
