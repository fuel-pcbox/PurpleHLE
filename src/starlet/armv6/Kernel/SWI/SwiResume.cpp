#include "Swi.h"
#include "TimerPort.h"
#include "Semaphore.h"
#include "Pthread.h"
#include "MsgError.h"
#include "CpuState.h"
#include "ErrNo.h"
#include "FdSpace.h"
#include "BsdSocket.h"

#include "SwiCall.h"

using namespace Interpreter;

void SwiMachResume(u32* args, u32* rets, u32 syscall, bool success, u32 userData)
{
	try
	{
		switch (syscall) //function
		{

		case MACH_MSG_TRAP:
			//curently: only if receive waited, so:
			{
				if (userData == MsgFlags::MessageSend)
				{
					if (success)
						rets[0] = g_task->Message(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
					else
						rets[0] = MsgError::SendTimedOut;
				}
				else //if (userData == MsgFlags::MessageReceive)
				{
					if (success)
						rets[0] = g_task->Message(args[0], args[1] &~ MsgFlags::MessageSend,
												  args[2], args[3], args[4], args[5], args[6]);
					else
						rets[0] = MsgError::ReceiveTimedOut;
				}
			}
			return;
			
		case SEMAPHORE_WAIT_TRAP:
			Assert(success);
			rets[0] = 0;
			return;
			
		case SEMAPHORE_WAIT_SIGNAL_TRAP:
			Assert(success);
			rets[0] = 0;
			return;

		case MACH_WAIT_UNTIL_TRAP:
			Assert(success);
			rets[0] = 0;
			return;

		default:
			Fail(syscall);
		}
	}
	catch (MachErr::Type& machErr)
	{
		Warn("syscall Mach Err set to %d (cpu.ip == %d)", machErr, syscall);		
		rets[0] = machErr;
	}
}

void SwiBsdResume(u32* args, u32* rets, u32 syscall, bool success, u32 userData)
{
	FdSpace* fds = g_task->fdSpace;

	cpu.cf = 0; //no error
	try
	{
		switch (syscall)
		{
			case READ:
			case READ_NOCANCEL:
				rets[0] = fds->GetFd(args[0])->Read(args[1], args[2]);
				return;
			case PREAD:
			case PREAD_NOCANCEL:
				rets[0] = fds->GetFd(args[0])->ReadP(args[1], args[2], Long(args[3], args[4]));
				return;
			case READV:
			case READV_NOCANCEL:
				rets[0] = fds->GetFd(args[0])->ReadVec(args[1], args[2]);
				return;

			case ACCEPT:
			case ACCEPT_NOCANCEL:
				rets[0] = BsdSocket::GetFd(args[0])->Accept(args[1], args[2]);
				return;
			case RECVMSG:
			case RECVMSG_NOCANCEL:
				rets[0] = BsdSocket::GetFd(args[0])->RecvMsg(args[1], args[2]);
				return;
			case RECVFROM:
			case RECVFROM_NOCANCEL:
				rets[0] = BsdSocket::GetFd(args[0])->RecvFrom(args[1], args[2], args[3], args[4], args[5]);
				return;
			/*case RECV:
			case RECV_NOCANCEL:
				rets[0] = BsdSocket::GetFd(args[0])->Recv(args[1], args[2], args[3]);
				return;*/

			case CONNECT:
			case CONNECT_NOCANCEL:
				rets[0] = 0;
				return;

			case __SEMWAIT_SIGNAL:
			case __SEMWAIT_SIGNAL_v2:
				Assert(success); //timeout not yet here
				rets[0] = 0;
				break;

			default:
				Fail(syscall);
		}
	}
	catch (ErrNo::Type& errNo)
	{
		Warn("Bsd ErrNo set to %d (cpu.ip == %d)", errNo, syscall);		
		rets[0] = errNo;
		cpu.cf = 1; //error!
	}
}

void SwiSysResume(u32* args, u32 func, bool success, u32 userData)
{
	Fail(0);
}

void SwiResume(bool success, u32 userData) //for ARM code, for int 80h (assumed)
{
	try
	{
		if ((s32)cpu.ip > 0)
			SwiBsdResume(&cpu.a1, &cpu.a1, cpu.ip, success, userData);
		else if ((s32)cpu.ip == 0)
			SwiBsdResume(&cpu.a2, &cpu.a1, cpu.a1, success, userData);
		else if (cpu.ip != 0x80000000)
			SwiMachResume(&cpu.a1, &cpu.a1, cpu.ip, success, userData);
		else
			SwiSysResume(&cpu.a1, cpu.a4, success, userData);
	}
	catch (StopThread& wait)
	{
		ActiveThreads::RemoveCurrent();
	}
}