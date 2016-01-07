/*
 * Copyright (C) 2015 Subvision Studio
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
