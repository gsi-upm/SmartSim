#locomotion mirror
if scene.getMotion('ChrMarine@WalkCircleLf01') == None:
	#locomotion smooth cycle
	print 'locomotion smooth cycle'
	smoothMotion = scene.getMotion("ChrMarine@RunCircleRt01")
	smoothMotion.smoothCycle("ChrMarine@RunCircleRt01_smooth",0.1);
	smoothMotion = scene.getMotion("ChrMarine@WalkCircleRt01")
	smoothMotion.smoothCycle("ChrMarine@WalkCircleRt01_smooth",0.1);
	smoothMotion = scene.getMotion("ChrMarine@WalkTightCircleRt01")
	smoothMotion.smoothCycle("ChrMarine@WalkTightCircleRt01_smooth",0.1);
	smoothMotion = scene.getMotion("ChrMarine@StrafeFastRt01")
	smoothMotion.smoothCycle("ChrMarine@StrafeFastRt01_smooth",0.1);
	smoothMotion = scene.getMotion("ChrMarine@Turn180Rt01")
	smoothMotion.smoothCycle("ChrMarine@Turn180Rt01_smooth",0.1);

	#locomotion mirror
	#print 'locomotion mirror'
	mirrorMotion = scene.getMotion("ChrMarine@WalkCircleRt01")
	mirrorMotion.mirror("ChrMarine@WalkCircleLf01", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@WalkTightCircleRt01")
	mirrorMotion.mirror("ChrMarine@WalkTightCircleLf01", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@StrafeFastRt01")
	mirrorMotion.mirror("ChrMarine@StrafeFastLf01", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@StrafeSlowRt01")
	mirrorMotion.mirror("ChrMarine@StrafeSlowLf01", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@RunCircleRt01")
	mirrorMotion.mirror("ChrMarine@RunCircleLf01", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@RunTightCircleRt01")
	mirrorMotion.mirror("ChrMarine@RunTightCircleLf01", "ChrBackovic.sk")
	
	#idle turn
	#print 'idle turn mirror'
	mirrorMotion = scene.getMotion("ChrMarine@Turn90Rt01")
	mirrorMotion.mirror("ChrMarine@Turn90Lf01", "ChrBackovic.sk")
	mirrorMotion1 = scene.getMotion("ChrMarine@Turn180Rt01_smooth")
	mirrorMotion1.mirror("ChrMarine@Turn180Lf01_smooth", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@Turn360Rt01")
	mirrorMotion.mirror("ChrMarine@Turn360Lf01", "ChrBackovic.sk")

	#mirroring for smooth cycle motion 
	mirrorMotion = scene.getMotion("ChrMarine@WalkCircleRt01_smooth")
	mirrorMotion.mirror("ChrMarine@WalkCircleLf01_smooth", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@WalkTightCircleRt01_smooth")
	mirrorMotion.mirror("ChrMarine@WalkTightCircleLf01_smooth", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@StrafeFastRt01_smooth")
	mirrorMotion.mirror("ChrMarine@StrafeFastLf01_smooth", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@RunCircleRt01_smooth")
	mirrorMotion.mirror("ChrMarine@RunCircleLf01_smooth", "ChrBackovic.sk")

	
	#starting mirror
	mirrorMotion = scene.getMotion("ChrMarine@Idle01_ToWalk01")
	mirrorMotion.mirror("ChrMarine@Idle01_ToWalkLf01", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@Idle01_ToWalk01_Turn90Rt01")
	mirrorMotion.mirror("ChrMarine@Idle01_ToWalk01_Turn90Lf01", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@Idle01_ToWalk01_Turn180Rt01")
	mirrorMotion.mirror("ChrMarine@Idle01_ToWalk01_Turn180Lf01", "ChrBackovic.sk")

	#step mirror
	#print 'step mirror'
	mirrorMotion = scene.getMotion("ChrMarine@Idle01_StepBackwardsRt01")
	mirrorMotion.mirror("ChrMarine@Idle01_StepBackwardsLf01", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@Idle01_StepForwardRt01")
	mirrorMotion.mirror("ChrMarine@Idle01_StepForwardLf01", "ChrBackovic.sk")
	mirrorMotion = scene.getMotion("ChrMarine@Idle01_StepSidewaysRt01")
	mirrorMotion.mirror("ChrMarine@Idle01_StepSidewaysLf01", "ChrBackovic.sk")

	



