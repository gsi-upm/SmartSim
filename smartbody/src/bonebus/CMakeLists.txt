cmake_minimum_required (VERSION 2.8)
project(bonebus)

include_directories ( ../../include/bonebus )
include_directories ( ../../include/vhcl )


if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions(-fPIC)
endif()


if (MSVC)
   add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif (MSVC)


set ( SRC
   ./bonebus.cpp
   ./bonebus-c.cpp
)


add_library( bonebus
   STATIC 
   ${SRC}
)
