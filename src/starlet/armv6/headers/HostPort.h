#pragma once
#include "Task.h"
#include "ClockPort.h"
#include "IoServicePort.h"

class HostPort : public IpcKernelPort
{
	static const int GET_HOST_INFO = 200;
	static const int GET_PAGE_SIZE = 202;
	static const int GET_IO_MASTER = 205;
	static const int GET_CLOCK_SERVICE = 206;

public:
	HostPort(Task* task) : m_task(task)
	{}

	bool Process(IpcKernMsg* message, IpcKernMsg* reply)
	{
		switch (message->ID())
		{
		case GET_HOST_INFO:
			{
				INPUT(u32, type);
				INPUT(u32, wordSize);

				const int BASIC_SIZE = 12;
				const int PRIORITY_SIZE = 8;

				if (type == 1 && wordSize >= BASIC_SIZE)
				{
					u32 maxMem = 128 * 1024 * 1024; //whatever
					OUTPUT_STRUCT(u32 retCode; u32 size; u32 maxCpus; u32 availCpus; u32 maxMem;
							      u32 cpuType; u32 cpuSubType; u32 threading; u32 pcpus; u32 pcpusMax;
								  u32 lcpus; u32 lcpusMax; u64 maxMemReally,
								  0, BASIC_SIZE, 1, 1, maxMem, 0xc, 0x6, 0, 1, 1, 1, 1, maxMem);
				}
				else if (type == 5 && wordSize >= PRIORITY_SIZE)
				{
					OUTPUT_STRUCT(u32 retCode; u32 size; u32 kernelP; u32 systemP; u32 serverP; u32 userP;
								  u32 depressP; u32 idleP; u32 minP; u32 maxP,
								  0, PRIORITY_SIZE, 100, 90, 80, 60, 40, 20, 1, 100);				}
				else
					Fail(type);
			}
			break;

		case GET_PAGE_SIZE:
			{
				OUTPUT(u32, 0); //retCode 
				OUTPUT(u32, VirtualMem::PageSize);
			}
			break;

		case GET_IO_MASTER:
			{
				OUTPUT_PORT(IoServicePort::GetMasterPort(), MsgRight::MakeSend); 
			}
			break;

		case GET_CLOCK_SERVICE:
			{
				INPUT(u32, service);

				OUTPUT_PORT(ClockPort::GetPort(service), MsgRight::MakeSend); 
			}
			break;

		default:
			Fail(message->ID());
		}
		return true;
	}

private:
	Task* m_task;
};
