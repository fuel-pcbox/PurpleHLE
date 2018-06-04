#include "Waitable.h"
#include "Swi.h"
#include "Thread.h"

void Waitable::Wait(u32 userData)
{
	DoWait(WaitStruct(ActiveThreads::Current(), NULL), userData);
}

void Waitable::WaitTime(s64 time, u32 userData)
{
	HelperStruct* timeOut = new HelperStruct(this, ActiveThreads::Current());
	Timer* timer = new Timer(time, Waitable::TimeOut, timeOut);

	DoWait(WaitStruct(ActiveThreads::Current(), timer), userData);
}

void Waitable::ResumeAll()
{
	for (Iter i = g_waitingThreads.begin() ; i != g_waitingThreads.end(); i++)
		DoResume(*i, true);
	g_waitingThreads.clear();
}

bool Waitable::ResumeOne()
{
	if (g_waitingThreads.size() > 0)
	{
		DoResume(g_waitingThreads.front(), true);
		g_waitingThreads.pop_front();
		return true;
	}
	return false;
}

bool Waitable::ResumeSpecific(Thread* thread)
{
	for (Iter i = g_waitingThreads.begin() ; i != g_waitingThreads.end(); i++)
		if (i->thread == thread)
		{
			DoResume(*i, true);
			g_waitingThreads.erase(i);
			return true;
		}
	return false;
}

void Waitable::TimeOut(Thread* thread)
{
	for (Iter i = g_waitingThreads.begin() ; i != g_waitingThreads.end(); i++)
		if (i->thread == thread)
		{
			DoResume(*i, false);
			g_waitingThreads.erase(i);
			return;
		}
	MakeSure(0);
}

void Waitable::RemoveWaiting(Thread* thread)
{
	for (Iter i = g_waitingThreads.begin() ; i != g_waitingThreads.end(); i++)
		if (i->thread == thread)
		{
			if (i->timeout)
				i->timeout->Cancel();
			g_waitingThreads.erase(i);
			return;
		}
	MakeSure(0);
}

void Waitable::DoWait(WaitStruct waitStruct, u32 userData)
{
	g_waitingThreads.push_back(waitStruct);
	ActiveThreads::Current()->SetResumeFunc(SwiResume);
	ActiveThreads::Current()->SetResumeParam(userData);
	ActiveThreads::Current()->SetWaitingOn(this);
	throw StopThread(); //will RemoveCurrent
}

void Waitable::DoResume(WaitStruct waitStruct, bool success)
{
	waitStruct.thread->SetResumeSuccess(success);
	waitStruct.thread->SetWaitingOn(NULL);
	if (waitStruct.timeout && success)
		waitStruct.timeout->Cancel();
	ActiveThreads::AddThread(waitStruct.thread);
}