
add_subdirectory (vhcl)
if (${SB_NO_VHMSG})
else()
add_subdirectory (vhmsg)
endif()


if (${SB_NO_BONEBUS})
else()
add_subdirectory (bonebus)
endif()

add_subdirectory (steerlib)
add_subdirectory (pprAI)

if (${SB_NO_ASSIMP})
else()
add_subdirectory (assimp-3.1.1)
endif()

add_subdirectory (SmartBody)
add_subdirectory (sbgui)
add_subdirectory (simplesmartbody)

# uncomment these lines to build the Ogre and/or Irrlicht samples
if (${BUILD_OGRE})
add_subdirectory (ogresmartbody)
endif()

if (${BUILD_IRRLICHT})
add_subdirectory (irrlichtsmartbody)
endif()

