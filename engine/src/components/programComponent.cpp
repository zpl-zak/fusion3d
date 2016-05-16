/*
 * Copyright (C) 2015-2016 Subvision Studio
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

#include "programComponent.h"

double ProgramHoster::lastTime = 0;
double ProgramHoster::timeCounter = 0;
bool ProgramHoster::isUpdating = true;

ProgramHoster::ProgramHoster(): m_input(0), m_update(0), m_render(0), m_verbose(0), m_program(nullptr), m_delayedInit(false), m_loaded(false)
{
}
