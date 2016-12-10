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

#############################################
# Try to find BULLET and set the following: #
#                                           #
# BULLET_FOUND                              #
# BULLET_INCLUDE_DIRS                       #
# BULLET_LIBRARIES                          #
#############################################

SET( BULLET_SEARCH_PATHS
	${BULLET_ROOT_DIR}					# ASSIMP!
#	./lib/bullet
	$ENV{PROGRAMFILES}/ASSIMP			# WINDOWS
#	"$ENV{PROGRAMFILES(X86)}/ASSIMP"	# WINDOWS
	~/Library/Frameworks				# MAC
	/Library/Frameworks					# MAC
	/usr/local/include/bullet/
	/usr/local/lib/						# LINUX/MAC/UNIX
	/usr/local							# LINUX/MAC/UNIX
	/usr								# LINUX/MAC/UNIX
	/opt								# LINUX/MAC/UNIX
	/sw									# Fink
	/opt/local							# DarwinPorts
	/opt/csw							# Blastwave
	/usr/local/include/bullet
)

FIND_PATH( BULLET_INCLUDE_DIRS
	NAMES
		btBulletCollisionCommon.h
		btBulletDynamicsCommon.h
	PATHS
		${BULLET_SEARCH_PATHS}
	PATH_SUFFIXES
		bullet
	DOC
		"The directory where bullet/btBulletCollisionCommon.h resides"
)

FIND_LIBRARY( BULLET_LIBRARIES
	NAMES
		BulletCommon
		BulletCollision
		LinearMath
		BulletDynamics
	PATHS
		${BULLET_SEARCH_PATHS}
	PATH_SUFFIXES
		lib
		lib64
		lib/x86
		lib/x64
	DOC
		"The Bullet library"
)

# Check if we found it!
IF ( BULLET_INCLUDE_DIRS AND BULLET_LIBRARIES )
	SET( BULLET_FOUND TRUE )
	MESSAGE(STATUS "Looking for BULLET - found")
ELSE ( BULLET_INCLUDE_DIRS AND BULLET_LIBRARIES )
	SET( BULLET_FOUND FALSE )
	MESSAGE(STATUS "Looking for BULLET - not found")
ENDIF ( BULLET_INCLUDE_DIRS AND BULLET_LIBRARIES )
