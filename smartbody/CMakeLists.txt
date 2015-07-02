cmake_minimum_required( VERSION 2.8 )
project( SmartBody )

set( CMAKE_INSTALL_PREFIX "../" )
#debug
#add_definitions(	-g3 -fPIC )
#release
#add_definitions(	-fPIC )
#optimization
add_definitions( -O3	-fPIC )

include_directories(
	/usr/local/include
)

LINK_DIRECTORIES(
	/usr/local/lib
)

add_subdirectory( src )


