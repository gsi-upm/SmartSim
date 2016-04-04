cmake_minimum_required (VERSION 2.8)
project(vhcl)

include_directories( ${CMAKE_SOURCE_DIR}/include/vhcl)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   add_definitions(-fPIC)
endif()


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 
   FIND_PATH(OPENAL_INCLUDE_DIR al.h)
endif()


include_directories(include)


# only for vhcl_crash.cpp
include_directories(./nocturnal/Debug)

# only for vhcl_audio.cpp
if (MSVC)
    include_directories(openal/include)
    include_directories(libsndfile)
endif ()

if (UNIX)
    include_directories(include)
endif ()


if (MSVC)
   add_definitions(-D_CRT_SECURE_NO_WARNINGS)
   add_definitions(/EHa)
endif (MSVC)


set ( SRC
   ./Pow2Assert.cpp
   ./vhcl_audio.cpp
   ./vhcl_crash.cpp
   ./vhcl_log.cpp
   ./vhcl_math.cpp
   ./vhcl_memory.cpp
   ./vhcl_socket.cpp
   ./vhcl_string.cpp
   ./vhcl_timer.cpp
)


if (MSVC)
   set ( SRC
      ${SRC}
      ./nocturnal/Debug/Exception.cpp
      ./nocturnal/Debug/Mail.cpp
      ./nocturnal/Debug/Memory.cpp
      ./nocturnal/Debug/Symbols.cpp
   )
endif (MSVC)


add_library( vhcl
   STATIC 
   ${SRC}
)
