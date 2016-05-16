// (C) 2015-2016 Subvision Studio, MIT; see LICENSE.

#ifndef TIME_H
#define TIME_H

namespace Time
{
	double GetTime();
	int HasElapsed(int x);
};

class DelayTimer
{
public:
	DelayTimer() :
		m_delay(0),
		m_lastTime(Time::GetTime()),
		m_timer(0)
	{}

	DelayTimer(double delay) :
		m_delay(delay),
		m_lastTime(Time::GetTime()),
		m_timer(0)
	{}

	double GetDelay() const { return m_delay; }
	void SetDelay(double delay) { m_delay = delay; }

	int HasElapsed()
	{
		m_timer += Time::GetTime() - m_lastTime;

		int ret = (m_timer >= m_delay);
		if (ret)
		{
			m_timer = 0;
		}

		m_lastTime = Time::GetTime();
		return ret;
	}

private:
	double m_delay, m_lastTime, m_timer;
};

#endif
