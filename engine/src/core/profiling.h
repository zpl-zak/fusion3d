// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#ifndef PROFILING_H_INCLUDED
#define PROFILING_H_INCLUDED

#include <string>

#define PROFILING_DISABLE_MESH_DRAWING 0
#define PROFILING_DISABLE_SHADING 0
#define PROFILING_SET_1x1_VIEWPORT 0
#define PROFILING_SET_2x2_TEXTURE 0

class ProfileTimer
{
public:
	ProfileTimer() :
		m_numInvocations(0),
		m_totalTime(0.0),
		m_startTime(0) {}

	void StartInvocation();
	void StopInvocation();
	
	double DisplayAndReset(const std::string& message, double divisor = 0, int displayedMessageLength = 40);
	double GetTimeAndReset(double divisor = 0);
protected:
private:
	int    m_numInvocations;
	double m_totalTime;
	double m_startTime;
};

#endif // PROFILING_H_INCLUDED
