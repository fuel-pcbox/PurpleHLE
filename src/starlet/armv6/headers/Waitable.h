#pragma once
#include <deque>
#include "types.h"
#include "Timer.h"

class StopThread
{
};

class Thread;

class Waitable
{
public:
	void Wait(u32 userData = 0);
	void WaitTime(s64 time, u32 userData = 0);

	void ResumeAll();
	bool ResumeOne();
	bool ResumeSpecific(Thread* thread);

	void TimeOut(Thread* thread);
	void RemoveWaiting(Thread* thread);

	static void TimeOut(void* ptr)
	{
		HelperStruct* helper = static_cast<HelperStruct*>(ptr);
		(helper->waitable)->TimeOut(helper->thread);
	}

private:
	struct WaitStruct
	{
		WaitStruct(Thread* thread, Timer* timeout) : thread(thread), timeout(timeout) {}
		Thread* thread;
		Timer* timeout;
	};

	struct HelperStruct
	{
		HelperStruct(Waitable* waitable, Thread* thread) : waitable(waitable), thread(thread) {}
		Waitable* waitable;
		Thread* thread;
	};

	void DoWait(WaitStruct waitStruct, u32 userData);
	void DoResume(WaitStruct thread, bool success);

	std::deque<WaitStruct> g_waitingThreads;
	typedef std::deque<WaitStruct>::iterator Iter;
};