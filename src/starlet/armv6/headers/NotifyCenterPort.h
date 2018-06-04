#pragma once
#include "Task.h"

class NotifyCenterPort : public IpcKernelPort
{
public:
	bool Process(IpcKernMsg* message, IpcKernMsg* reply)
	{
		switch (message->ID())
		{
		case 0x04b49d89: //register mach port
			{
				INPUT_OOL(name);
				INPUT_PORT(taskPort);
				INPUT_PORT(userPort); //?
				INPUT(u32, index); //?
				INPUT(u32, in_unk); //???

				OUTPUT(u32, 0); //retcode
				OUTPUT_STRUCT(u32 unk1; u32 unk2, 0, 0);
				break;
			}

		case 0x04b49d86: //register check?
			{
				INPUT_OOL(name);
				INPUT_PORT(taskPort);
				INPUT(u32, index); //?

				OUTPUT(u32, 0); //retcode
				OUTPUT_STRUCT(u32 unk1; u32 unk2; u32 unk3; u32 unk4, 1, 1, 1, 1);
				break;
			}

		case 0x04b49d85: //register plain?
			{
				INPUT_OOL(name);
				INPUT_PORT(taskPort);
				INPUT(u32, index); //?

				OUTPUT(u32, 0); //retcode
				OUTPUT_STRUCT(u32 unk1; u32 unk2, 0, 0);
				break;
			}

		case 0x04b49d8f: //cancel
			{
				OUTPUT(u32, 0); //retcode
				break;
			}

		case 0x04b49d91: //get state?
			{
				INPUT(u32, something); //unk1 of register plain

				OUTPUT(u32, 0); //retcode
				OUTPUT_STRUCT(u32 unk1; u32 unk2; u32 unk3, 0, 0, 0);
				break;
			}

		default:
			Fail(message->ID());
			break;
		}
		return true;
	}
};