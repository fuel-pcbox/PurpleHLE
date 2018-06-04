#include "ClockPort.h"

IpcName* ClockPort::GetPort(int index)
{
	switch (index)
	{
	case 0:
		if (!g_realTime) g_realTime = IpcName::GetName(new ClockPort());
		return g_realTime;
	default:
		Fail(index);
	}
}

IpcName* ClockPort::g_realTime = NULL;