include_directories( ./)

include_directories ( ${CMAKE_SOURCE_DIR}/include/vhcl )
include_directories ( ${CMAKE_SOURCE_DIR}/include/vhmsg)
include_directories ( ${CMAKE_SOURCE_DIR}/include/bonebus)
include_directories ( ${CMAKE_SOURCE_DIR}/include/steersuite)
include_directories ( ${CMAKE_SOURCE_DIR}/include/steersuite/external)
include_directories ( ${CMAKE_SOURCE_DIR}/include/steersuite/external/tinyxml)
include_directories ( /usr/include/python2.7 )

add_definitions( "-DdDOUBLE" )

set(SB_EXPORTS 1)
set(SMARTBODY_DLL_EXPORTS 1)
set(SMARTBODY_C_DLL_EXPORTS 1)
set(_COMPILING_STEERLIB 1)

find_library (LIBSNDFILE sndfile )
find_library (ALUT alut )
find_library (OPENAL openal)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set( MORE_LIBS
	${COREAUDIO_LIBRARY}
	${AUDIOUNIT_LIBRARY}
	${CORESERVICES_LIBRARY}
	${LIBSNDFILE}
	${ALUT}
	${OPENAL}
	${GLEW}
	${FESTIVAL}
	${ESTBASE}
	${ESTSTRING}
	${ESTOOLS}
)
else()
set( MORE_LIBS
	${LIBSNDFILE}
	${ALUT}
	${OPENAL}
	rt
)       
endif()

if (${SB_NO_VHMSG})
set( VHMSG_LIB
)
else()
set( VHMSG_LIB
        vhmsg
        activemq-cpp
)
endif()

if (${SB_NO_BONEBUS})
set( BONEBUS_LIB
)
else()
set( BONEBUS_LIB
        bonebus
)
endif()

if (${SB_NO_PYTHON})
set( PYTHON_LIB
)
else()
set( PYTHON_LIB
        boost_python
        python2.7
)
endif()

if (${SB_NO_ODE_PHYSICS})
set( ODE_LIB
)
else()
set( ODE_LIB
        ode
)
endif()

if (${SB_NO_ASSIMP})
set( ASSIMP_LIB
)
else()
include_directories( ../assimp-3.1.1/include)
set( ASSIMP_LIB
        assimp
)
endif()


find_package(OpenGL REQUIRED)

add_custom_target(protoctarget
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/protocols/sbmotion.pb.cc
		COMMAND protoc --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/protocols --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/protocols ${CMAKE_CURRENT_SOURCE_DIR}/protocols/sbmotion.proto 
		
		)


set( SMARTBODY_LIB_EXT_NANOFLANN_SRC

        ./external/nanoflann/nanoflann.hpp

)

set( SMARTBODY_LIB_EXT_RPLY_SRC
        ./external/rply/rply.c
)

set( SMARTBODY_LIB_EXT_PARSER_SRC

        ./external/parser/Bchart.cpp
        ./external/parser/BchartSm.cpp
        ./external/parser/Bst.cpp
        ./external/parser/ChartBase.cpp
        ./external/parser/ClassRule.cpp
        ./external/parser/CntxArray.cpp
        ./external/parser/CombineBests.cpp
        ./external/parser/ECArgs.cpp
        ./external/parser/Edge.cpp
        ./external/parser/EdgeHeap.cpp
        ./external/parser/edgeSubFns.cpp
        ./external/parser/EgsFromTree.cpp
        ./external/parser/ewDciTokStrm.cpp
        ./external/parser/extraMain.cpp
        ./external/parser/FBinaryArray.cpp
        ./external/parser/Feat.cpp
        ./external/parser/Feature.cpp
        ./external/parser/FeatureTree.cpp
        ./external/parser/fhSubFns.cpp
        ./external/parser/Field.cpp
        ./external/parser/FullHist.cpp
        ./external/parser/GotIter.cpp
        ./external/parser/headFinder.cpp
        ./external/parser/headFinderCh.cpp
        ./external/parser/InputTree.cpp
        ./external/parser/Item.cpp
        ./external/parser/Link.cpp
        ./external/parser/MeChart.cpp
        ./external/parser/Params.cpp
        ./external/parser/ParseStats.cpp
        ./external/parser/SentRep.cpp
        ./external/parser/Term.cpp
        ./external/parser/TimeIt.cpp
        ./external/parser/UnitRules.cpp
        ./external/parser/utils.cpp
        ./external/parser/ValHeap.cpp
)

set( SMARTBODY_LIB_EXT_ZLIB_SRC
	
	./external/zlib-1.2.5/adler32.c
	./external/zlib-1.2.5/compress.c
	./external/zlib-1.2.5/crc32.c
	./external/zlib-1.2.5/deflate.c
	./external/zlib-1.2.5/example.c
	./external/zlib-1.2.5/gzclose.c
	./external/zlib-1.2.5/gzlib.c
	./external/zlib-1.2.5/gzread.c
	./external/zlib-1.2.5/gzwrite.c
	./external/zlib-1.2.5/infback.c
	./external/zlib-1.2.5/inffast.c
	./external/zlib-1.2.5/inflate.c
	./external/zlib-1.2.5/inftrees.c
	./external/zlib-1.2.5/ioapi.c
	./external/zlib-1.2.5/trees.c
	./external/zlib-1.2.5/uncompr.c
	./external/zlib-1.2.5/unzip.c
	./external/zlib-1.2.5/zip.c
	./external/zlib-1.2.5/zutil.c
)

set( SMARTBODY_LIB_EXT_RECAST_SRC

        ./external/recast/Recast.cpp
        ./external/recast/RecastAlloc.cpp
        ./external/recast/RecastArea.cpp
        ./external/recast/RecastContour.cpp
        ./external/recast/RecastFilter.cpp
        ./external/recast/RecastLayers.cpp
        ./external/recast/RecastMesh.cpp
        ./external/recast/RecastMeshDetail.cpp
        ./external/recast/RecastRasterization.cpp
        ./external/recast/RecastRegion.cpp
        ./external/recast/DetourAlloc.cpp
        ./external/recast/DetourCommon.cpp
        ./external/recast/DetourNavMesh.cpp
        ./external/recast/DetourNavMeshBuilder.cpp
        ./external/recast/DetourNavMeshQuery.cpp
        ./external/recast/DetourNode.cpp
)

set( SMARTBODY_LIB_EXT_SOIL_SRC

        ./external/SOIL/image_DXT.c
        ./external/SOIL/image_helper.c
        ./external/SOIL/SOIL.c
        ./external/SOIL/stb_image_aug.c
		./external/SOIL/stbi_DDS_aug_c.h
)

set( SMARTBODY_LIB_EXT_PERLIN_SRC

        ./external/perlin/perlin.cpp
)

set( SMARTBODY_LIB_ME_SRC

        ./controllers/me_controller_context.cpp
        ./controllers/me_controller_context_proxy.cpp
        ./controllers/me_controller.cpp
        ./controllers/me_controller_tree_root.cpp
        ./controllers/me_ct_adshr_envelope.cpp
        ./controllers/me_ct_blend.cpp
        ./controllers/me_ct_channel_writer.cpp
        ./controllers/me_ct_container.cpp
        ./controllers/me_ct_curve_writer.cpp
        ./controllers/me_ct_interpolator.cpp
        ./controllers/me_ct_motion.cpp
        ./controllers/me_ct_periodic_replay.cpp
        ./controllers/me_ct_pose.cpp
        ./controllers/me_ct_scheduler2.cpp
        ./controllers/me_ct_time_shift_warp.cpp
        ./controllers/me_ct_unary.cpp
        ./controllers/me_default_prune_policy.cpp
        ./controllers/me_spline_1d.cpp
        ./controllers/me_ct_pose_postprocessing.cpp
)

set( SMARTBODY_LIB_SK_SRC

        ./sk/sk_channel_array.cpp
        ./sk/sk_channel.cpp
        ./sk/sk_joint.cpp
        ./sk/sk_joint_euler.cpp
        ./sk/sk_joint_pos.cpp
        ./sk/sk_joint_quat.cpp
        ./sk/sk_joint_swing_twist.cpp
        ./sk/sk_motion.cpp
        ./sk/sk_motion_io.cpp
        ./sk/sk_posture.cpp
        ./sk/sk_scene.cpp
        ./sk/sk_skeleton.cpp
        ./sk/sk_skeleton_io.cpp
        ./sk/sk_vec_limits.cpp
)

set( SMARTBODY_LIB_SR_SRC

        ./sr/sr_alg.cpp
        ./sr/sr_array.cpp
        ./sr/sr_box.cpp
        ./sr/sr_buffer.cpp
        ./sr/sr_camera.cpp
        ./sr/sr_color.cpp
        ./sr/sr.cpp
        ./sr/sr_cylinder.cpp
        ./sr/sr_euler.cpp
        ./sr/sr_event.cpp
        ./sr/sr_geo2.cpp
        ./sr/sr_gl.cpp
        ./sr/sr_gl_render_funcs.cpp
        ./sr/sr_hash_table.cpp
        ./sr/sr_input.cpp
        ./sr/sr_light.cpp
        ./sr/sr_line.cpp
        ./sr/sr_lines.cpp
        ./sr/sr_mat.cpp
        ./sr/sr_material.cpp
        ./sr/sr_model.cpp
        ./sr/sr_model_export_iv.cpp
        ./sr/sr_model_import_obj.cpp
	./sr/sr_model_import_ply.cpp
        ./sr/sr_output.cpp
        ./sr/sr_path_array.cpp
        ./sr/sr_plane.cpp
        ./sr/sr_points.cpp
        ./sr/sr_polygon.cpp
        ./sr/sr_polygons.cpp
        ./sr/sr_quat.cpp
        ./sr/sr_sa_bbox.cpp
        ./sr/sr_sa.cpp
        ./sr/sr_random.cpp
        ./sr/sr_sa_event.cpp
        ./sr/sr_sa_gl_render.cpp
        ./sr/sr_sa_render_mode.cpp
        ./sr/sr_shared_ptr.cpp
        ./sr/sr_sn.cpp
		./sr/sr_sn_editor.cpp
        ./sr/sr_sn_group.cpp
        ./sr/sr_sn_matrix.cpp
        ./sr/sr_sn_shape.cpp
        ./sr/sr_sn_colorsurf.cpp
        ./sr/sr_sphere.cpp
        ./sr/sr_spline.cpp
        ./sr/sr_string_array.cpp
        ./sr/sr_string.cpp
        ./sr/sr_timer.cpp
        ./sr/sr_trackball.cpp
        ./sr/sr_tree.cpp
        ./sr/sr_triangle.cpp
        ./sr/sr_vec2.cpp
        ./sr/sr_vec.cpp
        ./sr/sr_viewer.cpp
        ./sr/planner/sk_blend_cfg.cpp
        ./sr/planner/sk_pos_planner.cpp
        ./sr/planner/sk_blend_cfg_manager.cpp
        ./sr/planner/sr_cfg_planner.cpp
        ./sr/planner/sk_blend_planner.cpp
        ./sr/planner/sr_cfg_manager.cpp
        ./sr/planner/sk_pos_cfg.cpp
        ./sr/planner/sr_cfg_planner_lazy.cpp
        ./sr/planner/sr_cfg_path.cpp
        ./sr/planner/sr_cfg_tree.cpp
        ./sr/planner/sk_pos_cfg_manager.cpp
	./sr/jpge.cpp
)

set( SMARTBODY_LIB_SBM_SRC

        ./sbm/BMLDefs.cpp
        ./bml/behavior_scheduler_constant_speed.cpp
        ./bml/behavior_scheduler.cpp
        ./bml/behavior_scheduler_fixed.cpp
		./bml/behavior_scheduler_gesture.cpp
        ./bml/behavior_span.cpp
        ./bml/bml_animation.cpp
        ./bml/bml_gesture.cpp
        ./bml/bml_bodyreach.cpp
        ./bml/bml_constraint.cpp
        ./bml/bml.cpp
        ./bml/bml_event.cpp
        ./bml/bml_face.cpp
        ./bml/bml_gaze.cpp
        ./bml/bml_general_param.cpp
        ./bml/bml_grab.cpp
        ./bml/bml_interrupt.cpp
        ./bml/bml_locomotion.cpp
        ./bml/bml_processor.cpp
        ./bml/bml_quickdraw.cpp
        ./bml/bml_reach.cpp
        ./bml/bml_speech.cpp
        ./bml/bml_sync_point.cpp
        ./bml/bml_target.cpp
        ./bml/bml_saccade.cpp
        ./bml/bml_noise.cpp
		./bml/bml_states.cpp
		./bml/BMLAnimationObject.cpp
		./bml/BMLBodyObject.cpp
		./bml/BMLConstraintObject.cpp
		./bml/BMLEventObject.cpp
		./bml/BMLFaceObject.cpp
		./bml/BMLGazeObject.cpp
		./bml/BMLGestureObject.cpp
		./bml/BMLHeadObject.cpp
		./bml/BMLLocomotionObject.cpp
		./bml/BMLObject.cpp
		./bml/BMLReachObject.cpp
		./bml/BMLSpeechObject.cpp
		./bml/BMLSaccadeObject.cpp
		./bml/BMLHandObject.cpp
		./bml/BMLStateObject.cpp
		./bml/BMLNoiseObject.cpp		
		
        ./sbm/GenericViewer.cpp
        ./sbm/gwiz_cmdl.cpp
        ./sbm/gwiz_math.cpp
        ./sbm/gwiz_spline.cpp
        ./sbm/Heightfield.cpp
        ./sbm/lin_win.cpp
        ./sbm/mcontrol_callbacks.cpp
        ./sbm/action_unit.cpp

        ./controllers/me_ct_basic_locomotion.cpp
        ./controllers/me_ct_ccd_IK.cpp
        ./controllers/me_ct_constraint.cpp
        ./controllers/me_ct_data_driven_reach.cpp
        ./controllers/me_ct_data_interpolation.cpp
        ./controllers/me_ct_example_body_reach.cpp
        ./controllers/me_ct_examples.cpp
        ./controllers/me_ct_eyelid.cpp
        ./controllers/me_ct_face.cpp
        ./controllers/me_ct_gaze_alg.cpp
        ./controllers/me_ct_gaze.cpp
        ./controllers/me_ct_gaze_joint.cpp
        ./controllers/me_ct_gaze_keymap.cpp
        ./controllers/me_ct_gaze_target.cpp
        ./controllers/me_ct_IK.cpp
        ./controllers/me_ct_IK_scenario.cpp
        ./controllers/me_ct_jacobian_IK.cpp
        ./controllers/me_ct_limb.cpp
        ./controllers/me_ct_locomotion_func.cpp
        ./controllers/me_ct_motion_example.cpp
        ./controllers/me_ct_motion_parameter.cpp
        ./controllers/me_ct_motion_player.cpp
        ./controllers/me_ct_motion_timewarp.cpp
        ./controllers/me_ct_param_animation.cpp
        ./controllers/me_ct_param_animation_utilities.cpp
        ./controllers/me_ct_quick_draw.cpp
        ./controllers/me_ct_reach.cpp
        ./controllers/me_ct_hand.cpp
        ./controllers/me_ct_motion_profile.cpp
        ./controllers/me_ct_barycentric_interpolation.cpp
        ./controllers/me_ct_inverse_interpolation.cpp
        ./controllers/me_ct_reach_IK.cpp
        ./controllers/me_ct_simple_gaze.cpp
        ./controllers/me_ct_tether.cpp
        ./controllers/me_ct_ublas.cpp
        ./controllers/me_ct_breathing.cpp
        ./controllers/me_ct_breathing_interface.cpp
        ./controllers/me_ct_physics_controller.cpp
        ./controllers/me_ct_noise_controller.cpp
        ./controllers/me_ct_motion_recorder.cpp

        ./controllers/MeCtBlendEngine.cpp
        ./controllers/me_ct_motion_blend.cpp
        ./controllers/me_ct_motion_graph.cpp
        ./controllers/me_ct_generic_hand.cpp
        ./controllers/me_ct_new_locomotion.cpp
        ./controllers/MotionAnalysis.cpp
		./controllers/RealTimeLipSyncController.cpp

        ./sbm/ParserBVH.cpp
        ./sbm/ParserASFAMC.cpp
        ./sbm/ParserFBX.cpp
        ./sbm/ParserOgre.cpp
        ./sbm/ParserCOLLADAFast.cpp
        ./sbm/ParserOpenCOLLADA.cpp
        ./sbm/remote_speech.cpp
        ./sbm/sbm_audio.cpp
        ./controllers/MeCtBodyReachState.cpp
        ./controllers/MeCtReachEngine.cpp
        ./controllers/me_ct_saccade.cpp
        ./sbm/local_speech.cpp
## UNUSED:
##      sbm/sbm_bml.cpp
        ./sbm/sbm_deformable_mesh.cpp
        ./sbm/sbm_speech_audiofile.cpp
        ./sbm/sbm_speech.cpp
        ./sbm/sbm_speech_impl_skeleton.cpp
        ./sbm/sbm_test_cmds.cpp
        ./sbm/sr_arg_buff.cpp
        ./sbm/sr_cmd_line.cpp
        ./sbm/sr_cmd_seq.cpp
        ./sbm/sr_hash_map.cpp
        ./sbm/sr_linear_curve.cpp
        ./sbm/sr_spline_curve.cpp
        ./sbm/sr_synch_points.cpp
        ./sbm/text_speech.cpp
        ./sbm/time_profiler.cpp
        ./sbm/time_regulator.cpp
        ./sbm/xercesc_utils.cpp


        ./sbm/ODEPhysicsSim.cpp

        ./sbm/GPU/SbmTexture.cpp
        ./sbm/GPU/SbmDeformableMeshGPU.cpp
        ./sbm/GPU/TBOData.cpp
        ./sbm/GPU/VBOData.cpp
        ./sbm/GPU/SbmShader.cpp
        ./sbm/GPU/SbmBlendFace.cpp
        ./sbm/PPRAISteeringAgent.cpp
        ./sbm/SteerSuiteEnginerDriver.cpp
        ./sbm/sr_path_list.cpp

        ./sb/sbm_character.cpp
        ./sb/sbm_pawn.cpp

        ./sb/SBAttribute.cpp
        ./sb/SBAttributeManager.cpp
        ./sb/SBObject.cpp
        ./sb/SBObserver.cpp
        ./sb/SBSubject.cpp
        ./sb/DefaultAttributeTable.cpp

        ./sb/SBScene.cpp
        ./sbm/KinectProcessor.cpp
        ./controllers/me_ct_data_receiver.cpp

        ./sb/SBScript.cpp
        ./sb/SBCharacter.cpp
        ./sb/SBPawn.cpp
        ./sb/SBJoint.cpp
        ./sb/SBSkeleton.cpp
        ./sb/SBController.cpp
        ./sb/SBSimulationManager.cpp
        ./sb/SBBmlProcessor.cpp
        ./sb/SBAnimationState.cpp
        ./sb/SBAnimationTransition.cpp
        ./sb/SBAnimationTransitionRule.cpp
        ./sb/SBAnimationStateManager.cpp
        ./sb/SBSteerManager.cpp
        ./sb/SBSteerAgent.cpp
        ./sb/SBReachManager.cpp
        ./sb/SBReach.cpp
        ./sb/SBGestureMapManager.cpp
        ./sb/SBGestureMap.cpp
        ./sb/SBServiceManager.cpp
        ./sb/SBService.cpp
        ./sb/SBMotion.cpp
        ./sb/SBFaceDefinition.cpp
        ./sb/SBPhysicsManager.cpp
        ./sb/SBBoneBusManager.cpp
        ./sb/SBCollisionManager.cpp
        ./sb/SBParser.cpp
        ./sb/SBParseNode.cpp
        ./sb/SBJointMap.cpp
        ./sb/SBJointMapManager.cpp
        ./sb/SBPhoneme.cpp
        ./sb/SBPhonemeManager.cpp
        ./sb/SBBehaviorSet.cpp
        ./sb/SBBehaviorSetManager.cpp
        ./sb/SBMotionBlendBase.cpp
        ./sb/nvbg.cpp
        ./sb/MiniBrain.cpp
        ./sb/SBColObject.cpp
        ./sb/SBPhysicsSim.cpp
        ./sb/SBEvent.cpp
        ./sb/PABlend.cpp
        ./sb/SBRetarget.cpp
        ./sb/SBRetargetManager.cpp
        ./sb/SBNavigationMesh.cpp
        ./sb/SBAsset.cpp
        ./sb/SBAssetManager.cpp
        ./sb/SBAsset.cpp
        ./sb/SBSpeechManager.cpp
        ./sb/SBVHMsgManager.cpp
        ./sb/SBCommandManager.cpp
        ./sb/SBNavigationMeshManager.cpp
        ./sb/SBMotionGraph.cpp

        ./sb/SBPython.cpp
        ./sb/SBPythonAnimation.cpp
        ./sb/SBPythonAttribute.cpp
        ./sb/SBPythonCharacter.cpp
        ./sb/SBPythonMath.cpp
        ./sb/SBPythonMotion.cpp
        ./sb/SBPythonScene.cpp
        ./sb/SBPythonSimulation.cpp
        ./sb/SBPythonSkeleton.cpp
        ./sb/SBPythonSystem.cpp
        ./sb/SBPythonClass.cpp
        ./sb/SBPythonMesh.cpp
		
        ./sb/SBAssetHandler.cpp
        ./sb/SBAssetHandlerCOLLADA.cpp
        ./sb/SBAssetHandlerSk.cpp
        ./sb/SBAssetHandlerSkm.cpp
	./sb/SBAssetHandlerAsf.cpp
	./sb/SBAssetHandlerAmc.cpp
	./sb/SBAssetHandlerOgre.cpp
	./sb/SBAssetHandlerObj.cpp
	./sb/SBAssetHandlerBvh.cpp
	./sb/SBAssetHandlerPly.cpp
	./sb/SBAssetHandlerAssimp.cpp
	./sb/SBAssetHandlerSkmb.cpp
	./sb/SBAssetHandlerSBMeshBinary.cpp
        ./sb/SBBehavior.cpp
        ./sb/SBDebuggerServer.cpp
        ./sb/SBDebuggerClient.cpp
        ./sb/SBDebuggerUtility.cpp
        ./sb/SBHandConfiguration.cpp
        ./sb/SBHandConfigurationManager.cpp
        ./sb/SBHandSynthesis.cpp
        ./sb/SBRigNode.cpp
        ./sb/SBFaceShiftManager.cpp
        ./sb/SBRealtimeManager.cpp
        ./sbm/SteerPath.cpp
        ./sbm/MiscCommands.cpp
		
	./sb/smartbody-c-dll.cpp
		
	./protocols/sbmotion.pb.cc
	./protocols/sbutilities.pb.cc
	./protocols/sbmesh.pb.cc
)

add_library(
	SmartBody
	SHARED 

	${SMARTBODY_LIB_EXT_SOIL_SRC}

	${SMARTBODY_LIB_EXT_ZLIB_SRC}

        ${SMARTBODY_LIB_EXT_PARSER_SRC}

        ${SMARTBODY_LIB_EXT_RECAST_SRC}

        ${SMARTBODY_LIB_EXT_PERLIN_SRC}

	${SMARTBODY_LIB_EXT_NANOFLAN_SRC}

	${SMARTBODY_LIB_EXT_RPLY_SRC}

        ${SMARTBODY_LIB_ME_SRC}

        ${SMARTBODY_LIB_SK_SRC}

        ${SMARTBODY_LIB_SR_SRC}

        ${SMARTBODY_LIB_SBM_SRC}

	

)

if (${GOOGLE_PROFILER})
set (PROFILER_LIBS
	tcmalloc
	profiler
)
else()
set (PROFILER_LIBS "")
endif()

target_link_libraries( SmartBody

	${PROFILER_LIBS}
	${BONEBUS_LIB}
	${VHMSG_LIB}
	${ASSIMP_LIB}
	vhcl
	xerces-c
	boost_filesystem
	boost_system
	boost_regex
	boost_serialization
	boost_python
	boost_serialization
	blas
	lapack
	${OPENGL_LIBRARIES}
	activemq-cpp
	steerlib
	pprAI
	ode
	${PYTHON_LIB}
	ncurses
	${MORE_LIBS}
	protoc
	protobuf
)

add_dependencies(SmartBody protoctarget)

set( EXEC_LIST

       SmartBody
)

set( APP_LIBRARY_SOURCE_PATHS

${PROJECT_SOURCE_DIR}
)       

set_target_properties(
	${EXEC_LIST}
	PROPERTIES
	## link with install tree:
	INSTALL_RPATH                   .
	BUILD_WITH_INSTALL_RPATH        TRUE
)

install(

        TARGETS ${EXEC_LIST}
	DESTINATION ./bin
)

