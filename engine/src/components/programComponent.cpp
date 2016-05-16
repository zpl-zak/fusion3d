// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#include "programComponent.h"

double ProgramHoster::lastTime = 0;
double ProgramHoster::timeCounter = 0;
bool ProgramHoster::isUpdating = true;

ProgramHoster::ProgramHoster(): m_input(0), m_update(0), m_render(0), m_verbose(0), m_program(nullptr), m_delayedInit(false), m_loaded(false)
{
}
