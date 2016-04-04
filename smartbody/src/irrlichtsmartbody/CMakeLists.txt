include_directories( ${CMAKE_SOURCE_DIR}/src/SmartBody )
include_directories( ${CMAKE_SOURCE_DIR}/include/vhcl )
include_directories( ${CMAKE_SOURCE_DIR}/include/vhmsg )
include_directories( ${CMAKE_SOURCE_DIR}/include/bonebus )
include_directories( ${CMAKE_SOURCE_DIR}/include/steersuite )
include_directories( ${CMAKE_SOURCE_DIR}/include/steersuite/external )
include_directories( ${CMAKE_SOURCE_DIR}/include/steersuite/external/tinyxml )
include_directories( /usr/include/python2.7 )
include_directories( ${CMAKE_SOURCE_DIR}/irrlicht-1.8.3/include)

find_library(IRRLICHTLIB Irrlicht ${CMAKE_SOURCE_DIR}/irrlicht-1.8.3/lib/Linux)

add_executable( irrlichtsmartbody
	main.cpp
	irrlichtsmartbodylistener.cpp
)

target_link_libraries( irrlichtsmartbody
	SmartBody
	${IRRLICHTLIB}
	Xxf86vm
	Xext
	X11
	GLEW
)

set( EXEC_LIST

	irrlichtsmartbody
)

set_target_properties( 

	${EXEC_LIST} 

	PROPERTIES 

## link with install tree:
	INSTALL_RPATH			${CMAKE_SOURCE_DIR}/lib:.	

	BUILD_WITH_INSTALL_RPATH	TRUE
)

install(

  	TARGETS ${EXEC_LIST}
	RUNTIME DESTINATION ./bin
)


