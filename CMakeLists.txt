# CMakeList.txt : CMake project for RubikSolver, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(RubikSolver)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")

if (${IS_DEBUG_BUILD})
	add_compile_definitions(__DEBUG__)
endif()

add_definitions(-DDIRECTORY_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\")

include_directories("include")
include_directories("deps/GLM/")
# include_directories(${IMGUI_DIR})

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	include_directories("deps/GLFW/include")
	include_directories("deps/GLEW/include")
	
	link_directories("deps/GLFW/lib-vc2022")
	link_directories("deps/GLEW/lib/Release/x64")

elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
	find_package(glfw3 REQUIRED)
	find_package(GLEW REQUIRED)
	find_package(OpenGL REQUIRED)

	include_directories(${GLFW_INCLUDE_DIRS})
	include_directories(${GLEW_INCLUDE_DIRS})
	include_directories(${OPENGL_INCLUDE_DIRS})

endif()

# Executable definition
add_subdirectory(src)