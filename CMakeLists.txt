cmake_minimum_required (VERSION 3.0)

project(168-project)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wno-parentheses")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -g0 -fno-exceptions")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g")

FILE (GLOB_RECURSE Sources src/*.cpp)

include_directories(src godot-cpp/include godot-cpp/include/core godot-cpp/include/gen godot-cpp/godot_headers)
link_directories(godot-cpp/bin)

add_library(168-project SHARED ${Sources})

target_link_libraries(168-project -s)

if (CMAKE_SYSTEM_NAME MATCHES Linux) 
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-local-addr")
	if (CMAKE_BUILD_TYPE MATCHES Debug)
		target_link_libraries(168-project godot-cpp.linux.debug.64)
	elseif(CMAKE_BUILD_TYPE MATCHES Release)
		target_link_libraries(168-project godot-cpp.linux.release.64)
	endif()
endif()

if (CMAKE_SYSTEM_NAME MATCHES Darwin)
	if (CMAKE_BUILD_TYPE MATCHES Debug)
		target_link_libraries(168-project libgodot-cpp.osx.debug.64.a)
	elseif(CMAKE_BUILD_TYPE MATCHES Release)
		target_link_libraries(168-project godot-cpp.osx.release.64)
	endif()
endif()

if (CMAKE_SYSTEM_NAME MATCHES Windows)
	set_target_properties(168-project PROPERTIES PREFIX "")
	target_link_libraries(168-project --static -Wl,--no-undefined -static-libgcc -static-libstdc++ -Wl,-Bstatic -lpthread)
	if (CMAKE_BUILD_TYPE MATCHES Debug)
		target_link_libraries(168-project godot-cpp.windows.debug.64)
	elseif(CMAKE_BUILD_TYPE MATCHES Release)
		target_link_libraries(168-project godot-cpp.windows.release.64)
	endif()
endif()
