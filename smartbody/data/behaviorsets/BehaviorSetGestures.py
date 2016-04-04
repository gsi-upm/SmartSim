scene.run("BehaviorSetCommon.py")

def setupBehaviorSet():
	print "Setting up behavior set for gestures..."
	scene.addAssetPath("script", "behaviorsets/Gestures2/scripts")
	
	assetManager = scene.getAssetManager()	
	motionPath = "behaviorsets/Gestures2/motions/"
	skel = scene.getSkeleton("ChrBrad2.sk")
	if skel == None:
		scene.loadAssetsFromPath("behaviorsets/Gestures2/skeletons")
	# map the zebra2 skeleton
	scene.run("zebra2-map.py")
	zebra2Map = scene.getJointMapManager().getJointMap("zebra2")
	bradSkeleton = scene.getSkeleton("ChrBrad2.sk")
	zebra2Map.applySkeleton(bradSkeleton)
	
	gestureMotions = StringVec()
	
	gestureMotions.append("ChrBrad@Idle01")
	gestureMotions.append("ChrBrad@Idle01_ArmStretch01")
	gestureMotions.append("ChrBrad@Idle01_BeatFistMidLf01")
	gestureMotions.append("ChrBrad@Idle01_BeatFistMidLf02")
	gestureMotions.append("ChrBrad@Idle01_BeatHighBt01")
	gestureMotions.append("ChrBrad@Idle01_BeatHighBt02")
	gestureMotions.append("ChrBrad@Idle01_BeatHighBt03")
	gestureMotions.append("ChrBrad@Idle01_BeatHighLf01")
	gestureMotions.append("ChrBrad@Idle01_BeatLowBt01")
	gestureMotions.append("ChrBrad@Idle01_BeatLowBt02")
	gestureMotions.append("ChrBrad@Idle01_BeatLowLf01")
	gestureMotions.append("ChrBrad@Idle01_BeatLowLf02")
	gestureMotions.append("ChrBrad@Idle01_BeatLowRt01")
	gestureMotions.append("ChrBrad@Idle01_BeatMidBt01")
	gestureMotions.append("ChrBrad@Idle01_BeatMidLf01")
	gestureMotions.append("ChrBrad@Idle01_BeatPointMidLf01")
	gestureMotions.append("ChrBrad@Idle01_BesidesLf01")
	gestureMotions.append("ChrBrad@Idle01_ChopBoth01")
	gestureMotions.append("ChrBrad@Idle01_ChopLf01")
	gestureMotions.append("ChrBrad@Idle01_ChopLf02")
	gestureMotions.append("ChrBrad@Idle01_ChopRt02")
	gestureMotions.append("ChrBrad@Idle01_Contemplate01")
	gestureMotions.append("ChrBrad@Idle01_ExampleLf01")
	gestureMotions.append("ChrBrad@Idle01_Guitar01")
	gestureMotions.append("ChrBrad@Idle01_HereBt01")
	gestureMotions.append("ChrBrad@Idle01_HereBt02")
	gestureMotions.append("ChrBrad@Idle01_HoweverLf01")
	gestureMotions.append("ChrBrad@Idle01_InclusivityNegBt01")
	gestureMotions.append("ChrBrad@Idle01_InclusivityNegBt02")
	gestureMotions.append("ChrBrad@Idle01_InclusivityPosBt01")
	gestureMotions.append("ChrBrad@Idle01_InclusivityPosBt02")
	gestureMotions.append("ChrBrad@Idle01_IndicateLeftBt01")
	gestureMotions.append("ChrBrad@Idle01_IndicateLeftLf01")
	gestureMotions.append("ChrBrad@Idle01_IndicateRightBt01")
	gestureMotions.append("ChrBrad@Idle01_IndicateRightRt01")
	gestureMotions.append("ChrBrad@Idle01_MeLf01")
	gestureMotions.append("ChrBrad@Idle01_MeLf02")
	gestureMotions.append("ChrBrad@Idle01_NegativeBt01")
	gestureMotions.append("ChrBrad@Idle01_NegativeBt02")
	gestureMotions.append("ChrBrad@Idle01_NegativeBt03")
	gestureMotions.append("ChrBrad@Idle01_NegativeRt01")
	gestureMotions.append("ChrBrad@Idle01_OfferBoth01")
	gestureMotions.append("ChrBrad@Idle01_OfferLf01")
	gestureMotions.append("ChrBrad@Idle01_PleaBt02")
	gestureMotions.append("ChrBrad@Idle01_PointLf01")
	gestureMotions.append("ChrBrad@Idle01_PointLf02")
	gestureMotions.append("ChrBrad@Idle01_PointRt01")
	gestureMotions.append("ChrBrad@Idle01_ReceiveRt01")
	gestureMotions.append("ChrBrad@Idle01_SafeLf01")
	gestureMotions.append("ChrBrad@Idle01_ScratchChest01")
	gestureMotions.append("ChrBrad@Idle01_ScratchHeadLf01")
	gestureMotions.append("ChrBrad@Idle01_ScratchTempleLf01")
	gestureMotions.append("ChrBrad@Idle01_ShoulderStretch01")
	gestureMotions.append("ChrBrad@Idle01_Shrug01")
	gestureMotions.append("ChrBrad@Idle01_Shrug02")
	gestureMotions.append("ChrBrad@Idle01_SqueezeHand01")
	gestureMotions.append("ChrBrad@Idle01_StopBt01")
	gestureMotions.append("ChrBrad@Idle01_StopBt02")
	gestureMotions.append("ChrBrad@Idle01_SurroundBt01")
	gestureMotions.append("ChrBrad@Idle01_SurroundBt02")
	gestureMotions.append("ChrBrad@Idle01_Think01")
	gestureMotions.append("ChrBrad@Idle01_ToIdle02")
	gestureMotions.append("ChrBrad@Idle01_ToIdle03")
	gestureMotions.append("ChrBrad@Idle01_ToLocIdle01")
	gestureMotions.append("ChrBrad@Idle01_TouchHands01")
	gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn180Rt01")
	gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn360Rt01")
	gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn90Rt01")
	gestureMotions.append("ChrBrad@Idle01_WeightShift01")
	gestureMotions.append("ChrBrad@Idle01_WeightShift02")
	gestureMotions.append("ChrBrad@Idle01_WeRt01")
	gestureMotions.append("ChrBrad@Idle01_WeRt02")
	gestureMotions.append("ChrBrad@Idle01_WhenBt01")
	gestureMotions.append("ChrBrad@Idle01_WhenBt02")
	gestureMotions.append("ChrBrad@Idle01_YouLf01")
	gestureMotions.append("ChrBrad@Idle01_YouLf02")
	gestureMotions.append("ChrBrad@Idle01_YouLf03")
	gestureMotions.append("ChrBrad@Idle01_YouPointLf01")
	gestureMotions.append("ChrBrad@Idle01_YouRt01")
	
	
	for i in range(0, len(gestureMotions)):
		motion = scene.getMotion(gestureMotions[i])		
		if motion == None:
			assetManager.loadAsset(motionPath+gestureMotions[i]+'.skm')
			motion = scene.getMotion(gestureMotions[i])
		#print 'motionName = ' + locoMotions[i]
		if motion != None:
			motion.setMotionSkeletonName("ChrBrad2.sk")
			zebra2Map.applyMotion(motion)				
		
	print "| BradGestureMap.py |"

	# establish gesture data set
	gMapManager = getScene().getGestureMapManager()

	# Brad's gestures
	#neutral
	gMapB1 = gMapManager.createGestureMap("BradGesture")

	gMapB1.addGestureMapping("ChrBrad@Idle01_ChopLf01", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_ChopRt02", "METAPHORIC", "OBLIGATION", "RIGHT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_ChopBoth01", "METAPHORIC", "OBLIGATION", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "BEAT", "RHYTHM", "RIGHT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatHighLf01", "DEICTIC", "LEFT", "LEFT_HAND", "high", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidLf01", "DEICTIC", "LEFT", "LEFT_HAND", "high", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_HoweverLf01", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_IndicateLeftLf01", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01")

	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "DEICTIC", "RIGHT", "RIGHT_HAND", "high", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_IndicateRightRt01", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrBrad@Idle01")

	# gMapB1.addGestureMapping("ChrBrad@Idle01_HereBt02", "DEICTIC", "HERE", "BOTH_HANDS", "high", "ChrBrad@Idle01")
	# gMapB1.addGestureMapping("ChrBrad@Idle01_ChopBoth01", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01")
	# gMapB1.addGestureMapping("ChrBrad@Idle01_Frame", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB1.addGestureMapping("ChrBrad@Idle01_NegativeBt02", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_NegativeRt01", "METAPHORIC", "NEGATION", "RIGHT_HAND", "", "ChrBrad@Idle01")

	gMapB1.addGestureMapping("ChrBrad@Idle01_SafeLf01", "METAPHORIC", "ASIDE", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BesidesLf01", "METAPHORIC", "ASIDE", "LEFT_HAND", "", "ChrBrad@Idle01")


	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidBt01", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidLf01", "METAPHORIC", "QUESTION", "LEFT_HAND", "noheadtilt", "ChrBrad@Idle01")

	gMapB1.addGestureMapping("ChrBrad@Idle01_YouLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_OfferBoth01", "DEICTIC", "YOU", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_OfferLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_ReceiveRt01", "DEICTIC", "YOU", "RIGHT_HAND", "", "ChrBrad@Idle01")

	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidBt01", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB1.addGestureMapping("ChrBrad@Idle01_MeLf01", "DEICTIC", "ME", "LEFT_HAND", "", "ChrBrad@Idle01")

	gMapB1.addGestureMapping("ChrBrad@Idle01_WeRt01", "DEICTIC", "WE", "RIGHT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_WeRt02", "DEICTIC", "WE", "RIGHT_HAND", "", "ChrBrad@Idle01")


	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "METAPHORIC", "CONTRAST", "RIGHT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_IndicateRightR01", "METAPHORIC", "CONTRAST", "RIGHT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_HoweverLf01", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01")

	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatHighBt01", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatHighBt02", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidBt01", "METAPHORIC", "QUESTION", "BOTH_HANDS", "noheadtilt", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidLf01", "METAPHORIC", "QUESTION", "LEFT_HAND", "noheadtilt", "ChrBrad@Idle01")


	gMapB1.addGestureMapping("ChrBrad@Idle01_Shrug01", "METAPHORIC", "UNCERTAINTY", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_Shrug01", "METAPHORIC", "INDIFFERENCE", "BOTH_HANDS", "", "ChrBrad@Idle01")


	#metaphoric inclusivity
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatMidBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "mid", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_InclusivityPosBt01", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "low", "ChrBrad@Idle01")


	#metaphoric quantity_nothing
	gMapB1.addGestureMapping("ChrBrad@Idle01_NegativeBt01", "METAPHORIC", "EMPTY", "BOTH_HANDS", "high", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_NegativeBt02", "METAPHORIC", "EMPTY", "BOTH_HANDS", "high", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_NegativeRt01", "METAPHORIC", "EMPTY", "RIGHT_HAND", "high", "ChrBrad@Idle01")

	gMapB1.addGestureMapping("ChrBrad@Idle01_SurroundBt01", "METAPHORIC", "SURROUND", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_StopBt01", "METAPHORIC", "STOP", "RIGHT_HAND", "", "ChrBrad@Idle02")

	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "EMBLEM", "GREETING", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB1.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "EMBLEM", "GREETING", "RIGHT_HAND", "", "ChrBrad@Idle01")


	# #metaphoric "UNCERTAINTY" to add (low energy shrug)
	# #metaphoric "INDIFFERENCE" to add (low energy shrug)
	# #add gesture "EMPTY"
	# #emphatic
	gMapB2 = gMapManager.createGestureMap("BradGestureAngry")

	gMapB2.addGestureMapping("ChrBrad@Idle01_YouLf02", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_YouPointLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_MeLf02", "DEICTIC", "ME", "LEFT_HAND", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_WeRt02", "DEICTIC", "WE", "RIGHT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatMidBt01", "DEICTIC", "WE", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_PointLf02", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighLf01", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_PointRt01", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatLowBt01", "DEICTIC", "CENTER", "BOTH_HANDS", "", "ChrBrad@Idle01")
	#gMapB2.addGestureMapping("ChrBrad@Idle01_HereBt02", "DEICTIC", "HERE", "BOTH_HANDS", "", "ChrBrad@Idle01")
	#gMapB2.addGestureMapping("ChrBrad@Idle01_HereBt02", "DEICTIC", "CENTER", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_InclusivityNegBt02", "METAPHORIC", "NEGATION", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighBt02", "METAPHORIC", "INTENSIFIER_POSITIVE", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_PointRt01", "METAPHORIC", "CONTRAST", "RIGHT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_PointLf02", "METAPHORIC", "CONTRAST", "LEFT_HAND", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "QUESTION", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_PointRt01", "METAPHORIC", "QUESTION", "RIGHT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_PointLf02", "METAPHORIC", "QUESTION", "LEFT_HAND", "", "ChrBrad@Idle01")



	gMapB2.addGestureMapping("ChrBrad@Idle01_Shrug02", "METAPHORIC", "UNCERTAINTY", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "UNCERTAINTY", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_Shrug02", "METAPHORIC", "INDIFFERENCE", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatHighBt03", "METAPHORIC", "INDIFFERENCE", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_ChopLf02", "METAPHORIC", "INCLUSIVITY", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_InclusivityNegBt02", "METAPHORIC", "EMPTY", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_BesidesLf01", "METAPHORIC", "ASIDE", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_BesidesLf01", "METAPHORIC", "SURROUND", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_InclusivityNegBt02", "METAPHORIC", "STOP", "BOTH_HANDS", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_ChopLf02", "METAPHORIC", "OBLIGATION", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_ChopRt02", "METAPHORIC", "OBLIGATION", "RIGHT_HAND", "", "ChrBrad@Idle01")

	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatLowBt02", "BEAT", "RHYTHM", "BOTH_HANDS", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatPointMidLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatFistMidLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_BeatFistMidLf02", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_ChopLf02", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB2.addGestureMapping("ChrBrad@Idle01_YouPointLf01", "BEAT", "RHYTHM", "LEFT_HAND", "", "ChrBrad@Idle01")

	 #sad
	gMapB3 = gMapManager.createGestureMap("BradGestureSad")
	gMapB3.addGestureMapping("ChrBrad@Idle01_YouLf03", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "DEICTIC", "YOU", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "DEICTIC", "YOU", "RIGHT_HAND", "", "ChrBrad@Idle01")

	gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "DEICTIC", "LEFT", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "DEICTIC", "RIGHT", "RIGHT_HAND", "", "ChrBrad@Idle01")

	gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf01", "METAPHORIC", "NEGATION", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowRt01", "METAPHORIC", "NEGATION", "RIGHT_HAND", "", "ChrBrad@Idle01")

	gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "QUESTION", "LEFT_HAND", "", "ChrBrad@Idle01")

	gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "INDIFFERENCE", "LEFT_HAND", "", "ChrBrad@Idle01")
	gMapB3.addGestureMapping("ChrBrad@Idle01_BeatLowLf02", "METAPHORIC", "UNCERTAINTY", "LEFT_HAND", "", "ChrBrad@Idle01")

		
		
		
		

def retargetBehaviorSet(charName):
	gestureMotions = StringVec()
	gestureMotions.append("ChrBrad@Idle01")
	gestureMotions.append("ChrBrad@Idle01_ArmStretch01")
	gestureMotions.append("ChrBrad@Idle01_BeatFistMidLf01")
	gestureMotions.append("ChrBrad@Idle01_BeatFistMidLf02")
	gestureMotions.append("ChrBrad@Idle01_BeatHighBt01")
	gestureMotions.append("ChrBrad@Idle01_BeatHighBt02")
	gestureMotions.append("ChrBrad@Idle01_BeatHighBt03")
	gestureMotions.append("ChrBrad@Idle01_BeatHighLf01")
	gestureMotions.append("ChrBrad@Idle01_BeatLowBt01")
	gestureMotions.append("ChrBrad@Idle01_BeatLowBt02")
	gestureMotions.append("ChrBrad@Idle01_BeatLowLf01")
	gestureMotions.append("ChrBrad@Idle01_BeatLowLf02")
	gestureMotions.append("ChrBrad@Idle01_BeatLowRt01")
	gestureMotions.append("ChrBrad@Idle01_BeatMidBt01")
	gestureMotions.append("ChrBrad@Idle01_BeatMidLf01")
	gestureMotions.append("ChrBrad@Idle01_BeatPointMidLf01")
	gestureMotions.append("ChrBrad@Idle01_BesidesLf01")
	gestureMotions.append("ChrBrad@Idle01_ChopBoth01")
	gestureMotions.append("ChrBrad@Idle01_ChopLf01")
	gestureMotions.append("ChrBrad@Idle01_ChopLf02")
	gestureMotions.append("ChrBrad@Idle01_ChopRt02")
	gestureMotions.append("ChrBrad@Idle01_Contemplate01")
	gestureMotions.append("ChrBrad@Idle01_ExampleLf01")
	gestureMotions.append("ChrBrad@Idle01_Guitar01")
	gestureMotions.append("ChrBrad@Idle01_HereBt01")
	gestureMotions.append("ChrBrad@Idle01_HereBt02")
	gestureMotions.append("ChrBrad@Idle01_HoweverLf01")
	gestureMotions.append("ChrBrad@Idle01_InclusivityNegBt01")
	gestureMotions.append("ChrBrad@Idle01_InclusivityNegBt02")
	gestureMotions.append("ChrBrad@Idle01_InclusivityPosBt01")
	gestureMotions.append("ChrBrad@Idle01_InclusivityPosBt02")
	gestureMotions.append("ChrBrad@Idle01_IndicateLeftBt01")
	gestureMotions.append("ChrBrad@Idle01_IndicateLeftLf01")
	gestureMotions.append("ChrBrad@Idle01_IndicateRightBt01")
	gestureMotions.append("ChrBrad@Idle01_IndicateRightRt01")
	gestureMotions.append("ChrBrad@Idle01_MeLf01")
	gestureMotions.append("ChrBrad@Idle01_MeLf02")
	gestureMotions.append("ChrBrad@Idle01_NegativeBt01")
	gestureMotions.append("ChrBrad@Idle01_NegativeBt02")
	gestureMotions.append("ChrBrad@Idle01_NegativeBt03")
	gestureMotions.append("ChrBrad@Idle01_NegativeRt01")
	gestureMotions.append("ChrBrad@Idle01_OfferBoth01")
	gestureMotions.append("ChrBrad@Idle01_OfferLf01")
	gestureMotions.append("ChrBrad@Idle01_PleaBt02")
	gestureMotions.append("ChrBrad@Idle01_PointLf01")
	gestureMotions.append("ChrBrad@Idle01_PointLf02")
	gestureMotions.append("ChrBrad@Idle01_PointRt01")
	gestureMotions.append("ChrBrad@Idle01_ReceiveRt01")
	gestureMotions.append("ChrBrad@Idle01_SafeLf01")
	gestureMotions.append("ChrBrad@Idle01_ScratchChest01")
	gestureMotions.append("ChrBrad@Idle01_ScratchHeadLf01")
	gestureMotions.append("ChrBrad@Idle01_ScratchTempleLf01")
	gestureMotions.append("ChrBrad@Idle01_ShoulderStretch01")
	gestureMotions.append("ChrBrad@Idle01_Shrug01")
	gestureMotions.append("ChrBrad@Idle01_Shrug02")
	gestureMotions.append("ChrBrad@Idle01_SqueezeHand01")
	gestureMotions.append("ChrBrad@Idle01_StopBt01")
	gestureMotions.append("ChrBrad@Idle01_StopBt02")
	gestureMotions.append("ChrBrad@Idle01_SurroundBt01")
	gestureMotions.append("ChrBrad@Idle01_SurroundBt02")
	gestureMotions.append("ChrBrad@Idle01_Think01")
	gestureMotions.append("ChrBrad@Idle01_ToIdle02")
	gestureMotions.append("ChrBrad@Idle01_ToIdle03")
	gestureMotions.append("ChrBrad@Idle01_ToLocIdle01")
	gestureMotions.append("ChrBrad@Idle01_TouchHands01")
	gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn180Rt01")
	gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn360Rt01")
	gestureMotions.append("ChrBrad@Idle01_ToWalk01_Turn90Rt01")
	gestureMotions.append("ChrBrad@Idle01_WeightShift01")
	gestureMotions.append("ChrBrad@Idle01_WeightShift02")
	gestureMotions.append("ChrBrad@Idle01_WeRt01")
	gestureMotions.append("ChrBrad@Idle01_WeRt02")
	gestureMotions.append("ChrBrad@Idle01_WhenBt01")
	gestureMotions.append("ChrBrad@Idle01_WhenBt02")
	gestureMotions.append("ChrBrad@Idle01_YouLf01")
	gestureMotions.append("ChrBrad@Idle01_YouLf02")
	gestureMotions.append("ChrBrad@Idle01_YouLf03")
	gestureMotions.append("ChrBrad@Idle01_YouPointLf01")
	gestureMotions.append("ChrBrad@Idle01_YouRt01")
	
	sbChar = scene.getCharacter(charName)
	if sbChar == None:
		return
	skelName = sbChar.getSkeleton().getName()
	
	motions = StringVec()
	assetManager = scene.getAssetManager()
	for i in range(0, len(gestureMotions)):
		sbMotion = assetManager.getMotion(gestureMotions[i])
		if sbMotion != None:
			sbMotion.setMotionSkeletonName("ChrBrad2.sk")		
			
	
	createRetargetInstance('ChrBrad2.sk', skelName)
	
	#outDir = scene.getMediaPath() + '/retarget/motion/' + skelName + '/';
	#print 'outDir = ' + outDir ;
	#if not os.path.exists(outDir):
	#	os.makedirs(outDir)

	# retarget gestures
	#for n in range(0, len(gestureMotions)):
	#	curMotion = scene.getMotion(gestureMotions[n])
	#	if curMotion is not None:
	#		retargetMotion(gestureMotions[n], 'ChrBrad.sk', skelName, outDir + 'Gestures/');
	#	else:
	#		print "Cannot find motion " + gestureMotions[n] + ", it will be excluded from the gesture setup..."
	
	sbChar.setStringAttribute("gestureMap", "BradGesture")
	sbChar.setStringAttribute("gestureMapAngry", "BradGestureAngry")
	sbChar.setStringAttribute("gestureMapSad", "BradGestureSad")

		
