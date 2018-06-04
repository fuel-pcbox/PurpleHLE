#include "Pthread.h"
#include "Semaphore.h"
#include "MachErr.h"
#include "Timer.h"

int SemWaitSignal(u32 waitPort, u32 signalPort, int timeout, int relative, u32 timeSec, u32 timeUsec) //or Nsec?
{
	try
	{
		MakeSure(!timeout);
		Semaphore::WaitSignal(waitPort, signalPort); //TODO: timeout, etc.
		return 0;
	}
	catch (MachErr::Type& err)
	{
		Fail(0); //no idea
	}
}

int SetCancel(u32 type)
{
	//type - 1 for enable canceling (when?), 2 for disabling it.
	return 0;
}

s64 GetTimeOfDay(macptr time, macptr zone)
{
	if (time)
	{
		Ram<u32>(time) = LOW32(TimeLine::GetTime());
		Ram<u32>(time + 4) = HIGH32(TimeLine::GetTime());
	}
	if (zone)
	{
		MakeSure(0);
		Ram<u32>(zone) = 0; //minutes west from greenwich
		Ram<u32>(zone + 4) = 0; //dst
	}
	return TimeLine::GetTime(); //bug?
}