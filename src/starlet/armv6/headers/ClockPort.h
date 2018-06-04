#pragma once
#include "Task.h"

class ClockPort : public IpcKernelPort
{
public:
	static IpcName* GetPort(int index);

	bool Process(IpcKernMsg* message, IpcKernMsg* reply)
	{
		switch (message->ID())
		{
		default:
			Fail(message->ID());
		}
		return true;
	}

private:
	ClockPort() {}

	static IpcName* g_realTime;
};

