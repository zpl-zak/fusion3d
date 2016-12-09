# Copyright (C) 2016 Marek Kraus
# Copyright (C) 2014 Barry Deeney
# Copyright (C) 2014 Benny Bobaganoosh
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

################
# CMAKE RULES! #
################
# Huge thanks to mxaddict for setting up the CMake build system!

# we need features that are 2.6 >= dependent
cmake_minimum_required(VERSION 2.6)
set (CMAKE_CXX_STANDARD 11)

# lets name the project
project(Fusion3D)

# add the -c and -Wall flags
add_definitions(
	-c
	-Wall
)

if ( CMAKE_BUILD_TYPE STREQUAL "" )
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	set(
		CMAKE_BUILD_TYPE
			"Debug"
		CACHE STRING
			"Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) \"Debug\" \"Release\" \"RelWithDebInfo\" \"MinSizeRel\"."
		FORCE
	)
endif ( CMAKE_BUILD_TYPE STREQUAL "" )

if ( CMAKE_BUILD_TYPE STREQUAL "Release" )
add_definitions( -O2 )
endif ( CMAKE_BUILD_TYPE STREQUAL "Release" )

# Lets LOAD app our headers!
file(GLOB_RECURSE HDRS ${Fusion3D_SOURCE_DIR}/engine/src/*.h)

# Lets LOAD app our sources!
file(GLOB_RECURSE SRCS
	${Fusion3D_SOURCE_DIR}/engine/src/*.cpp
	${Fusion3D_SOURCE_DIR}/engine/src/*.c
)

# Define the executable
add_executable(Fusion3D ${HDRS} ${SRCS})

# We need a CMAKE_DIR with some code to find external dependencies
SET(Fusion3D_CMAKE_DIR "${Fusion3D_SOURCE_DIR}/cmake")

#######################################
# LOOK for the packages that we need! #
#######################################

# OpenGL
find_package(OpenGL REQUIRED)

# GLEW
INCLUDE(${Fusion3D_CMAKE_DIR}/FindGLEW.cmake)

# SDL2
INCLUDE(${Fusion3D_CMAKE_DIR}/FindSDL2.cmake)

# ASSIMP
INCLUDE(${Fusion3D_CMAKE_DIR}/FindASSIMP.cmake)

# BULLET
INCLUDE(${Fusion3D_CMAKE_DIR}/FindBULLET.cmake)

MESSAGE(STATUS "DIR: " ${Fusion3D_SOURCE_DIR})

# Define the include DIRs
include_directories(
	${Fusion3D_SOURCE_DIR}/headers
	${Fusion3D_SOURCE_DIR}/sources
	${Fusion3D_SOURCE_DIR}/engine/src/staticLibs
	${OPENGL_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
	${ASSIMP_INCLUDE_DIRS}
	${BULLET_INCLUDE_DIRS}
)

MESSAGE(STATUS "OMG: " ${BULLET_INCLUDE_DIRS})

# Define the link libraries
target_link_libraries( Fusion3D
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${SDL2_LIBRARIES}
	${ASSIMP_LIBRARIES}
	${BULLET_LIBRARIES}
)

