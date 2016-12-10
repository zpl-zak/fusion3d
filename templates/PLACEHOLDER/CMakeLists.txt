# Copyright (C) 2016 Marek Kraus
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

cmake_minimum_required(VERSION 2.6)
set (CMAKE_CXX_STANDARD 11)

project(PLACEHOLDER)

# add the -c and -Wall flags
add_definitions(
	
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

if ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
add_definitions( -g )
endif ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

SET(Fusion3D_SOURCE_DIR $ENV{FUSION3D_ROOT})

file(GLOB_RECURSE HDRS
	${Fusion3D_SOURCE_DIR}/src/*.h
	${PLACEHOLDER_SOURCE_DIR}/src/*.h
)

file(GLOB_RECURSE SRCS
	${Fusion3D_SOURCE_DIR}/src/*.cpp
	${Fusion3D_SOURCE_DIR}/src/*.c
	${PLACEHOLDER_SOURCE_DIR}/src/*.cpp
	${PLACEHOLDER_SOURCE_DIR}/src/*.c
)

add_executable(PLACEHOLDER ${HDRS} ${SRCS})

SET(Fusion3D_CMAKE_DIR "$ENV{FUSION3D_ROOT}/cmake")

INCLUDE(${Fusion3D_CMAKE_DIR}/FindDeps.cmake)

include_directories( 
	${PLACEHOLDER_SOURCE_DIR}/src/
	$ENV{FUSION3D_ROOT}/src/
	${OPENGL_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
	${ASSIMP_INCLUDE_DIRS}
	${BULLET_INCLUDE_DIRS}
)

target_link_libraries( PLACEHOLDER
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${SDL2_LIBRARIES}
	${ASSIMP_LIBRARIES}
	${BULLET_LIBRARIES}
)