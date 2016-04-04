class ScenarioModule():

	def addScenario(self, scenarioMesh):

		scene.loadAssetsFromPath("scene/" + scenarioMesh)
		scenario = scene.createPawn("scenario")
		scenario.setVec3Attribute("meshScale", .01, .01, .01)
		scenario.setStringAttribute("mesh", scenarioMesh)
		scene.getPawn('scenario').setStringAttribute("displayType", "GPUmesh")

	def addEmergency(self, ubikSimServer):
		
		emergencyPosition = ConnectionsModule().getUbikSimEmergency(ubikSimServer)

		emergency = scene.createPawn("emergency")
		emergency.setStringAttribute('collisionShape','box')
		emergency.setVec3Attribute('collisionShapeScale',1.0,1.0,1.0)
		emergency.setPosition(emergencyPosition)
