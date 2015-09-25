cmake_minimum_required (VERSION 2.8)
project(vhmsg)

include_directories ( ../../include/vhcl )
include_directories ( ../../include/vhmsg )


if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions(-fPIC)
endif()



if (UNIX)
   include_directories(/usr/local/include/activemq-cpp-3.8.3)
endif (UNIX)

if (APPLE)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-value")
endif ()

if (MSVC)
   add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif (MSVC)


set ( SRC
   ./HttpUtility.cpp
   ./vhmsg.cpp
   ./vhmsg-tt.cpp
)


add_library( vhmsg
   STATIC 
   ${SRC}
)


if (UNIX)
   target_link_libraries(vhmsg vhcl)
endif (UNIX)
