#include "Timer.h"
#include "Memory.h"

Timer::Timer(s64 time, TimerFunc func, void* data) : m_func(func), m_data(data), m_time(time)
{
	TimeLine::AddTimer(this);
}

void TimeLine::AddTimer(Timer* timer)
{
	for (Iter it = g_timeLine.begin(); it != g_timeLine.end(); it++)
	{
		if ((*it)->GetTime() > timer->GetTime())
		{
			g_timeLine.insert(it, timer);
			return;
		}
	}
	g_timeLine.push_back(timer);
}

bool TimeLine::RemoveTimer(Timer* timer)
{
	for (Iter it = g_timeLine.begin(); it != g_timeLine.end(); it++)
	{
		if (*it == timer)
		{
			g_timeLine.erase(it);
			return true;
		}
	}
	return false;
}

void TimeLine::AdvanceLineBy(s64 delta)
{
	g_curTime += delta;

	Iter it, next;
	for (it = g_timeLine.begin(); it != g_timeLine.end(); it = next)
	{
		next = it; next++;
		if ((*it)->GetTime() <= g_curTime)
			(*it)->Expire();
		else
			break;
	}
	g_timeLine.erase(g_timeLine.begin(), it);
}

void TimeLine::AdvanceToNext()
{
	Assert(!g_timeLine.empty());

	s64 delta = g_timeLine.front()->GetTime() - GetTime();
	if (delta < 0) delta = 0; //no need to travel backwards in time
	AdvanceLineBy(delta); //expire first timer
}

std::list<Timer*> TimeLine::g_timeLine;
s64 TimeLine::g_curTime = 0;


s64 TimeUtils::IntervalTime(macptr timePtr)
{
	s64 time = Ram<u32>(timePtr) * SecToNsec + Ram<u32>(timePtr + 4) * UsecToNsec; //??
	return TimeLine::GetTime() + time;
}

s64 TimeUtils::IntervalMsec(u32 time)
{
	return TimeLine::GetTime() + (s64)time * MsecToNsec;
}