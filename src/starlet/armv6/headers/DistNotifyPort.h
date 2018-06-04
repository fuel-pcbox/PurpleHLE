#pragma once
#include "Task.h"

class DistNotifyPort : public IpcKernelPort
{
public:
	bool UsesNdr() { return false; }
	bool Process(IpcKernMsg* message, IpcKernMsg* reply)
	{
		switch (message->ID())
		{
		case 4: 
			{
				//send notification to server
				//there's some user send right in the local name. what for?
				//data (NDR-LESS) : 0(?), size of bplist, bplist
				//no obvious reply
				INPUT(u32, unused);
				INPUT(u32, size);
				INPUT_STRING(bplist, size);
				return false; //no reply
			}

		default:
			Fail(message->ID());
			break;
		}
		return true;
	}
};