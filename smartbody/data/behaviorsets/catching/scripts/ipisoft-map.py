

def ipisoftmap():

    # Mapping from ipiSoft skeleton to SmartBody skeleton

    jointMapManager = scene.getJointMapManager()
    ipisoftMap = jointMapManager.getJointMap("ipisoft")
    if (ipisoftMap == None):
	ipisoftMap = jointMapManager.createJointMap('ipisoft')

    # Core
    ipisoftMap.setMapping("Hip", "base")
    ipisoftMap.setMapping("LowerSpine", "spine1")
    ipisoftMap.setMapping("MiddleSpine", "spine2")
    ipisoftMap.setMapping("Chest", "spine3")
    #ipisoftMap.setMapping("", "spine4")
    ipisoftMap.setMapping("Neck", "spine5")
    ipisoftMap.setMapping("Head", "skullbase")

    # Arm, left
    ipisoftMap.setMapping("LClavicle", "l_sternoclavicular")
    ipisoftMap.setMapping("LShoulder", "l_shoulder")
    #ipisoftMap.setMapping("", "l_upperarm1")
    #ipisoftMap.setMapping("", "l_upperarm2")
    ipisoftMap.setMapping("LForearm", "l_elbow")
    #ipisoftMap.setMapping("", "l_forearm1")
    #ipisoftMap.setMapping("", "l_forearm2")
    ipisoftMap.setMapping("LHand", "l_wrist")
    ipisoftMap.setMapping("LFinger0", "l_thumb1")
    ipisoftMap.setMapping("LFinger01", "l_thumb2")
    ipisoftMap.setMapping("LFinger02", "l_thumb3")
    ipisoftMap.setMapping("LFinger02EndSite", "l_thumb4")
    ipisoftMap.setMapping("LFinger1", "l_index1")
    ipisoftMap.setMapping("LFinger11", "l_index2")
    ipisoftMap.setMapping("LFinger12", "l_index3")
    ipisoftMap.setMapping("LFinger12EndSite", "l_index4")
    ipisoftMap.setMapping("LFinger2", "l_middle1")
    ipisoftMap.setMapping("LFinger21", "l_middle2")
    ipisoftMap.setMapping("LFinger22", "l_middle3")
    ipisoftMap.setMapping("LFinger22EndSite", "l_middle4")
    ipisoftMap.setMapping("LFinger3", "l_ring1")
    ipisoftMap.setMapping("LFinger31", "l_ring2")
    ipisoftMap.setMapping("LFinger32", "l_ring3")
    ipisoftMap.setMapping("LFinger32EndSite", "l_ring4")
    ipisoftMap.setMapping("LFinger4", "l_pinky1")
    ipisoftMap.setMapping("LFinger41", "l_pinky2")
    ipisoftMap.setMapping("LFinger42", "l_pinky3")
    ipisoftMap.setMapping("LFinger42EndSite", "l_pinky4")

    # Arm, right
    ipisoftMap.setMapping("RClavicle", "r_sternoclavicular")
    ipisoftMap.setMapping("RShoulder", "r_shoulder")
    #ipisoftMap.setMapping("", "r_upperarm1")
    #ipisoftMap.setMapping("", "r_upperarm2")
    ipisoftMap.setMapping("RForearm", "r_elbow")
    #ipisoftMap.setMapping("", "r_forearm1")
    #ipisoftMap.setMapping("", "r_forearm2")
    ipisoftMap.setMapping("RHand", "r_wrist")
    ipisoftMap.setMapping("RFinger0", "r_thumb1")
    ipisoftMap.setMapping("RFinger01", "r_thumb2")
    ipisoftMap.setMapping("RFinger02", "r_thumb3")
    ipisoftMap.setMapping("RFinger02EndSite", "r_thumb4")
    ipisoftMap.setMapping("RFinger1", "r_index1")
    ipisoftMap.setMapping("RFinger11", "r_index2")
    ipisoftMap.setMapping("RFinger12", "r_index3")
    ipisoftMap.setMapping("RFinger12EndSite", "r_index4")
    ipisoftMap.setMapping("RFinger2", "r_middle1")
    ipisoftMap.setMapping("RFinger21", "r_middle2")
    ipisoftMap.setMapping("RFinger22", "r_middle3")
    ipisoftMap.setMapping("RFinger22EndSite", "r_middle4")
    ipisoftMap.setMapping("RFinger3", "r_ring1")
    ipisoftMap.setMapping("RFinger31", "r_ring2")
    ipisoftMap.setMapping("RFinger32", "r_ring3")
    ipisoftMap.setMapping("RFinger32EndSite", "r_ring4")
    ipisoftMap.setMapping("RFinger4", "r_pinky1")
    ipisoftMap.setMapping("RFinger41", "r_pinky2")
    ipisoftMap.setMapping("RFinger42", "r_pinky3")
    ipisoftMap.setMapping("RFinger42EndSite", "r_pinky4")

    # Leg, left
    ipisoftMap.setMapping("LThigh", "l_hip")
    ipisoftMap.setMapping("LShin", "l_knee")
    ipisoftMap.setMapping("LFoot", "l_ankle")
    ipisoftMap.setMapping("LToe", "l_forefoot")
    ipisoftMap.setMapping("LToeEndSite", "l_toe")

    # Leg, right
    ipisoftMap.setMapping("RThigh", "r_hip")
    ipisoftMap.setMapping("RShin", "r_knee")
    ipisoftMap.setMapping("RFoot", "r_ankle")
    ipisoftMap.setMapping("RToe", "r_forefoot")
    ipisoftMap.setMapping("RToeEndSite", "r_toe")

    # Head, left
    #ipisoftMap.setMapping("JtEyeLf", "eyeball_left")
    #ipisoftMap.setMapping("JtEyelidUpperLf", "upper_eyelid_left")
    #ipisoftMap.setMapping("JtEyelidLowerLf", "lower_eyelid_left")

    # Head, right
    #ipisoftMap.setMapping("JtEyeRt", "eyeball_right")
    #ipisoftMap.setMapping("JtEyelidUpperRt", "upper_eyelid_right")
    #ipisoftMap.setMapping("JtEyelidLowerRt", "lower_eyelid_right")

    #ipisoftMap.setMapping("eyeJoint_R", "eyeball_right")
    #ipisoftMap.setMapping("eyeJoint_L", "eyeball_left")



ipisoftmap()
