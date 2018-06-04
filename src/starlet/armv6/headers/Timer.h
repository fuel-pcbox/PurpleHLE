#pragma once
#include <list>
#include "types.h"

typedef void (*TimerFunc)(void* data);

class Timer;

class TimeLine
{
public:
	static void AddTimer(Timer* timer);
	static bool RemoveTimer(Timer* timer);
	static void AdvanceLineBy(s64 delta);
	static void AdvanceToNext();

	static s64 GetTime() { return g_curTime; }

private:
	static std::list<Timer*> g_timeLine;
	typedef std::list<Timer*>::iterator Iter;

	static s64 g_curTime;
};

class Timer
{
public:
	Timer(s64 time, TimerFunc func, void* data);
	~Timer() { Cancel(); }
	s64 GetTime() { return m_time; }
	void Expire() { m_func(m_data); }
	bool Cancel() { return TimeLine::RemoveTimer(this); }

private:
	TimerFunc m_func;
	void* m_data;
	s64 m_time;
};

class TimeUtils
{
public:
	static s64 IntervalMsec(u32 time);
	static s64 IntervalTime(macptr timePtr);

private:
	static const s32 UsecToNsec = (s32)1e3;
	static const s32 MsecToNsec = (s32)1e6;
	static const s32 SecToNsec = (s32)1e9;
};