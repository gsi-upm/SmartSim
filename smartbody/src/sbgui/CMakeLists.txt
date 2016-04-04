add_subdirectory(external/polyvox) 
add_subdirectory(external/Pinocchio) 

include_directories( ${CMAKE_SOURCE_DIR}/src/sbgui)
include_directories( ${CMAKE_SOURCE_DIR}/src/SmartBody )
include_directories( ${CMAKE_SOURCE_DIR}/include/vhcl )
include_directories( ${CMAKE_SOURCE_DIR}/include/vhmsg )
include_directories( ${CMAKE_SOURCE_DIR}/include/bonebus )
include_directories( ${CMAKE_SOURCE_DIR}/include/steersuite )
include_directories( ${CMAKE_SOURCE_DIR}/include/steersuite/external )
include_directories( ${CMAKE_SOURCE_DIR}/include/steersuite/external/tinyxml )
include_directories( /usr/include/python2.7 )
include_directories( external/Pinocchio )
include_directories( external/polyvox/library/PolyVoxCore/include)

add_definitions(  -std=c++11 )


set (BUILD_OGRE FALSE)
# never build ogre if in osx
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set (BUILD_OGRE FALSE)
	#include_directories( /Users/feng/SmartBodyDev/OgreSDK_1.7/include/ )
	#include_directories( /Users/feng/SmartBodyDev/OgreSDK_1.7/include/Ogre/ )
endif()


find_package(OpenGL REQUIRED)

if (${BUILD_OGRE})
find_library (OGREMAIN Ogre)
find_library (OISLIB OIS)
find_library (RTSHADERLIB OgreRTShaderSystem)
endif()
if (${BUILD_OGRE})
include_directories( /usr/include/OGRE )
set (OGREMAINLIB OgreMain)
set (OISLIB OIS)
set (OGRE_LIBS
	${OGREMAINLIB}	
	${OISLIB}		
	${RTSHADERLIB}		
)



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





set (OGRE_SRCS
	EmbeddedOgre.cpp
	ExampleFrameListener.cpp	
	FLTKOgreViewer.cpp	
	OgreFrameListener.cpp
	SBOgreListener.cpp	
)
else()
set (OGRE_LIBS
)
set (OGRE_SRCS
)
endif()

find_library (LIBSNDFILE sndfile )
find_library (ALUT alut )
find_library (OPENAL openal)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
link_directories(/usr/X11R6/lib)
link_directories(/usr/local/lib)
set( MORE_LIBS
        ${COREAUDIO_LIBRARY}
        ${AUDIOUNIT_LIBRARY}
        ${CORESERVICES_LIBRARY}
	${IOKIT_LIBRARY}
	${FOUNDATION_LIBRARY}
	${COCOA_LIBRARY}
	${X11_LIBRARY}	
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
	fontconfig
	Xft
	GLEW
)

endif()


#if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#	if (${CMAKE_C_COMPILER_VERSION} VERSION_GREATER 4.2.0)
#		add_definitions(-Wno-deprecated-writable-strings)
#		add_definitions(-Wno-unused-value)
#	endif()
#endif()


set( SBM_FLTK_SRC

	./sbm_main.cpp
	./ppm.cpp
	./pic.cpp
#	./glfont2.cpp
	./CommandWindow.cpp
	./ObjectManipulationHandle.cpp
	./PawnPosControl.cpp
	./PositionControl.cpp
	./RotationControl.cpp
	./RootWindow.cxx
	./ExportWindow.cpp
	./fltk_viewer.cpp
	./SBWindowListener.cpp
	./SBSelectionManager.cpp
	./SBInterfaceListener.cpp
	${OGRE_SRCS}
	./pic.cpp
	./ppm.cpp
	./bmlviewer/BehaviorBlock.cpp
	./bmlviewer/BehaviorEditorWidget.cpp
	./bmlviewer/BehaviorWindow.cpp
	./channelbufferviewer/GlChartView.cpp
	./channelbufferviewer/GlChartViewArchive.cpp
	./channelbufferviewer/GlChartViewCoordinate.cpp
	./channelbufferviewer/channelbufferWindow.cpp
	./nle/NonLinearEditor.cpp
	./nle/NonLinearEditorWidget.cpp
	./panimationviewer/ErrorVisualization.cpp
	./panimationviewer/PanimationWindow.cpp
	./panimationviewer/ParamAnimBlock.cpp
	./panimationviewer/ParamAnimEditorWidget.cpp
	./panimationviewer/ParamAnimRunTimeEditor.cpp
	./panimationviewer/ParamAnimScriptEditor.cpp
	./panimationviewer/ParamAnimStateEditor.cpp
	./panimationviewer/ParamAnimTransitionEditor.cpp
	./panimationviewer/ParamAnimTransitionEditor2.cpp
	./panimationviewer/ParamAnimTransitionCreatorWidget.cpp
	./panimationviewer/ParameterGroup.cpp
	./panimationviewer/ParameterVisualization.cpp
	./panimationviewer/Parameter3DVisualization.cpp
	./panimationviewer/ParamAnimStateCreatorWidget.cpp
	./panimationviewer/ParamAnimAutoMarkingEditor.cpp
	./panimationviewer/ParamAnimParameterEditor.cpp
	./panimationviewer/VisualizationView.cpp
	./CharacterCreatorWindow.cpp
	./bmlcreator/AttributeWindow.cpp
	./bmlcreator/TextEditor.cpp
	./bmlcreator/BMLCreatorWindow.cpp

	./resourceViewer/Fl_Tree_Horizontal.cpp
	./resourceViewer/TreeItemInfoWidget.cpp
	./resourceViewer/ResourceWindow.cpp
	./resourceViewer/TreeInfoObject.cpp
	./resourceViewer/AttributeEditor.cpp

	./visemeviewer/VisemeCurve.cpp
	./visemeviewer/VisemeCurveEditor.cpp
	./visemeviewer/VisemeViewerWindow.cpp
	./visemeviewer/VisemeRunTimeWindow.cpp
	./visemeviewer/VisemeRTCurveViewer.cpp
	./visemeviewer/ImageSequencePlayer.cpp
	
	./retargetviewer/RetargetViewer.cpp
	./retargetcreator/RetargetCreatorWindow.cpp
	./autorig/SBAutoRigManager.cpp
	./autorig/VoxelizerWindow.cpp
	./autorig/AutoRigViewer.cpp

	./monitorviewer/MonitorConnectWindow.cpp 
	./motioneditor/MotionEditorWindow.cpp
	./jointmapviewer/JointMapViewer.cpp
	./jointmapviewer/RetargetStepWindow.cpp
	
	./faceshiftviewer/FaceShiftViewer.cpp

	./faceviewer/FaceViewer.cpp
	./FLTKListener.cpp
	./SBGUIManager.cpp
	
	./flu/Flu_Button.cpp
	./flu/Flu_Collapsable_Group.cpp
	./flu/FluSimpleString.cpp
	
	./pythonbind/SBPythonAutorig.cpp
)

add_executable( sbgui

	${SBM_FLTK_SRC}
)

if(NOT APPLE )
set ( XFT  "")	
else()
set ( XFT "" ) 
endif()

if (${GOOGLE_PROFILER})
set ( PROFILER_LIBS
	tcmalloc
	profiler
)
else()
set ( PROFILER_LIBS "")
endif()

target_link_libraries( sbgui

	${PROFILER_LIBS}
	SmartBody
	${BONEBUS_LIBS}
	${VHMSG_LIBS}
	vhcl
	boost_filesystem
	boost_system
	boost_regex
	boost_python
	${XFT}
	fltk_images 
	fltk 
	fltk_gl 
	${OPENGL_LIBRARIES}
	activemq-cpp
	${PYTHON_LIBS}
    	ncurses	
	pthread
	pinocchio
	polyvoxcore
	${MORE_LIBS}
	${OGRE_LIBS}
	#CEGUIBase
	#CEGUIOpenGLRenderer
	protobuf
	protoc
)

set( EXEC_LIST

	sbgui
)

set( APP_LIBRARY_BINARY_PATHS

	./bin
)

set( APP_LIBRARY_SOURCE_PATHS

	./bin
)

set_target_properties( 

	${EXEC_LIST} 

	PROPERTIES 

## link with binary tree:
#	INSTALL_RPATH				.

## link with install tree:
	INSTALL_RPATH			.:${CMAKE_SOURCE_DIR}/lib	

	BUILD_WITH_INSTALL_RPATH	TRUE
)

install(

  	TARGETS ${EXEC_LIST}
	RUNTIME DESTINATION ./bin
)

