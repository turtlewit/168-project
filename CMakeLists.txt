cmake_minimum_required (VERSION 3.0)

project(168-project)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror -Wno-parentheses")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g")

FILE (GLOB_RECURSE Sources src/*.cpp)

include_directories(src godot-cpp/include godot-cpp/include/core godot-cpp/include/gen godot-cpp/godot_headers)
link_directories(godot-cpp/bin)

add_library(168-project SHARED ${Sources})

if (CMAKE_BUILD_TYPE MATCHES Debug)
	target_link_libraries(168-project godot-cpp.linux.debug.64)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
	target_link_libraries(168-project godot-cpp.linux.release.64)
endif()
