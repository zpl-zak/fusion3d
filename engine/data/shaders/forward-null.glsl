/*
 * Copyright (C) 2015 Dominik Madarasz
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common.glh"

varying vec3 color0;

#if defined(VS_BUILD)
attribute vec3 position;
attribute vec3 color;

uniform mat4 T_MVP;
uniform vec3 matColor;

void main()
{
    gl_Position = T_MVP * vec4(position, 1.0);
	color0 = color;

}
#elif defined(FS_BUILD)
#include "sampling.glh"

DeclareFragOutput(0, vec4);
void main()
{
	SetFragOutput(0, vec4(color0,1));
}
#endif
