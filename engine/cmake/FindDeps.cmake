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