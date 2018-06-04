#include "Swi.h"
#include "types.h"
#include "Memory.h"
#include "BsdFile.h"
#include "BsdRegFile.h"
#include "BsdSocket.h"
#include "Task.h"
#include "MemMap.h"
#include "SysCtl.h"
#include "Thread.h"
#include "Waitable.h"
#include "ErrNo.h"
#include "FdSpace.h"
#include "Semaphore.h"
#include "Pthread.h"
#include "TimerPort.h"
#include "BsdKqueue.h"

#include "SwiCall.h"

using namespace Interpreter;

u64& Long(u32& low, u32& high)
{
	Assert(&low + 1 == &high);
	return *(u64*)&low;
}

void SwiMach(u32* args, u32* rets, u32 syscall)
{
	try
	{
		switch (syscall) //function
		{
		case MACH_ABSOLUTE_TIME:
			Long(rets[0], rets[1]) = TimeLine::GetTime();
			return;

		case MACH_REPLY_PORT:
			rets[0] = g_task->NewUserReplyPort()->Name(); //new one every time
			return;

		case THREAD_SELF_TRAP:
			rets[0] = IpcName::MakeSend(ActiveThreads::Current()->GetThreadSelf())->Name();
			return;

		case TASK_SELF_TRAP:
			rets[0] = IpcName::MakeSend(g_task->GetTaskSelf())->Name();
			return;
			
		case HOST_SELF_TRAP:
			rets[0] = IpcName::MakeSend(g_task->GetHostSelf())->Name();
			return;

		case MACH_MSG_TRAP:
			rets[0] = g_task->Message(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
			return;

		case SEMAPHORE_SIGNAL_TRAP:
			Semaphore::Signal(args[0]);
			rets[0] = 0;
			return;

		case SEMAPHORE_SIGNAL_THREAD_TRAP:
			Semaphore::SignalThread(args[0], args[1]);
			rets[0] = 0;
			return;
			
		case SEMAPHORE_WAIT_TRAP:
			Semaphore::Wait(args[0]);
			rets[0] = 0;
			return;
			
		case SEMAPHORE_WAIT_SIGNAL_TRAP:
			Semaphore::WaitSignal(args[0], args[1]);
			rets[0] = 0;
			return;

		case SWTCH_PRI:
			rets[0] = 1; //hint-ish function. returns 0 if there are no more threads (but eh.)
			return;

		case SYSCALL_THREAD_SWITCH:
			rets[0] = 0; //hint function (should be implemented)
			return;

		case MACH_TIMEBASE_INFO_TRAP:
			Ram<u32>(args[0]) = Ram<u32>(args[0] + 4) = 1;
			rets[0] = 0;
			return;

		case MACH_WAIT_UNTIL_TRAP:
			ActiveThreads::Current()->WaitUntil(Long(args[0], args[1]));
			rets[0] = 0;
			return;

		case MK_TIMER_CREATE_TRAP:
			rets[0] = TimerPort::New();
			return;

		case MK_TIMER_ARM_TRAP:
			rets[0] = TimerPort::Arm(args[0], Long(args[1], args[2]));
			return;

		case MK_TIMER_CANCEL_TRAP:
			rets[0] = TimerPort::Cancel(args[0], args[1]);
			return;

		case MK_TIMER_DESTROY_TRAP:
			rets[0] = TimerPort::Destroy(args[0]);
			return;

		default:
			rets[0] = 0;
			Fail(syscall);
		}
	}
	catch (MachErr::Type& machErr)
	{
		Warn("syscall Mach Err set to %d (cpu.ip == %d)", machErr, syscall);		
		rets[0] = machErr;
	}
}

void SwiBsd(u32* args, u32* rets, u32 syscall)
{
	FdSpace* fds = g_task->fdSpace;

	cpu.cf = 0; //no error
	try
	{
		switch (syscall)
		{

		case OPEN:
		case OPEN_NOCANCEL:
			rets[0] = fds->NewFd(BsdFile::OpenFile(RamString(args[0]), args[1], args[2]));
			return;
		case CLOSE:
		case CLOSE_NOCANCEL:
			rets[0] = fds->DelFd(args[0]);
			return;

		case ACCESS:
			rets[0] = BsdFile::Access(RamString(args[0]), args[1]);
			return;
		case DUP:
			rets[0] = fds->NewFd(fds->GetFd(args[0]));
			return;

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
			
		case WRITE:
		case WRITE_NOCANCEL:
			rets[0] = fds->GetFd(args[0])->Write(args[1], args[2]);
			return;
		case PWRITE:
		case PWRITE_NOCANCEL:
			rets[0] = fds->GetFd(args[0])->WriteP(args[1], args[2], Long(args[3], args[4]));
			return;
		case WRITEV:
		case WRITEV_NOCANCEL:
			rets[0] = fds->GetFd(args[0])->WriteVec(args[1], args[2]);
			return;

		case FCNTL:
		case FCNTL_NOCANCEL:
			rets[0] = fds->GetFd(args[0])->Control(args[0], args[1], args[2]);
			return;
		case IOCTL:
			rets[0] = fds->GetFd(args[0])->IoControl(args[0], args[1], args[2]);
			return;
		case LSEEK:
			Long(rets[0], rets[1]) = fds->GetFd(args[0])->Seek(Long(args[1], args[2]), args[3]);
			return;

		case STATFS:
			rets[0] = BsdFile::StatFS(RamString(args[0]), args[1]);
			return;
		case FSTATFS:
			rets[0] = fds->GetFd(args[0])->StatFS(args[1]);
			return;

		case PATHCONF:
			rets[0] = BsdFile::PathConf(RamString(args[0]), args[1]);
			return;
		case FPATHCONF:
			rets[0] = fds->GetFd(args[0])->PathConf(args[1]);
			return;

		case STAT:
			rets[0] = BsdFile::Stat(RamString(args[0]), args[1]);
			return;
		case FSTAT:
			rets[0] = fds->GetFd(args[0])->Stat(args[1]);
			return;
		case LSTAT:
			rets[0] = BsdFile::LinkStat(RamString(args[0]), args[1]);
			return;

		case STATFS64:
			rets[0] = BsdFile::StatFS64(RamString(args[0]), args[1]);
			return;
		case FSTATFS64:
			rets[0] = fds->GetFd(args[0])->StatFS64(args[1]);
			return;

		case STAT64:
			rets[0] = BsdFile::Stat64(RamString(args[0]), args[1]);
			return;
		case FSTAT64:
			rets[0] = fds->GetFd(args[0])->Stat64(args[1]);
			return;
		case LSTAT64:
			rets[0] = BsdFile::LinkStat64(RamString(args[0]), args[1]);
			return;

		case SELECT:
			rets[0] = BsdFile::Select(args[0], args[1], args[2], args[3], args[4]);
			return;

		case READLINK:
			rets[0] = ReadLink(RamString(args[0]), args[1], args[2]);
			return;

		case GETDIRENTRIES:
			rets[0] = fds->GetFd(args[0])->GetDirEntries(args[1], args[2], args[3]);
			return;
		case GETDIRENTRIES64:
			rets[0] = fds->GetFd(args[0])->GetDirEntries64(args[1], args[2], args[3]);
			return;

		case GETDTABLESIZE:
			rets[0] = FdSpace::FdLimit;
			return;

		case FSYNC: //..
			rets[0] = 0;
			return;

		case CHDIR:
			MakeSure(RamString(args[0]) == "/");
			rets[0] = 0;
			return;

		case MKDIR:
			rets[0] = MakeDir(RamString(args[0]), args[1]);
			return;

		case RENAME:
			rets[0] = Rename(RamString(args[0]), RamString(args[1]));
			return;

		case CHMOD:
		case FCHMOD:
		case CHOWN:
			rets[0] = 0; //not emulated
			return;

		case GETPID:
		case GETUID:
		case GETEUID:
		case GETEGID:
		case GETGID:
		case GETPPID:
		case SETSID:
			rets[0] = 1; //w/e (0 or 1?)
			return;

		case SETLOGIN:
		case CHROOT:
		case MOUNT:
			Warn("LAUNCH-TYPE-STUFF: %d", syscall);
			rets[0] = 0;
			return;

		case SOCKET:
			rets[0] = fds->NewFd(BsdSocket::NewSocket(args[0], args[1], args[2]));
			return;
		case SOCKETPAIR:
			rets[0] = BsdSocket::NewSocketPair(args[0], args[1], args[2], args[3]);
			return;

		case CONNECT:
		case CONNECT_NOCANCEL:
			rets[0] = 0; //BsdSocket::GetFd(args[0])->Connect(args[1], args[2]);
			return;
		case BIND:
			Warn("socket bind(!) %s", RamString(args[1] + 2).c_str());
			rets[0] = BsdSocket::GetFd(args[0])->Bind(args[1], args[2]);
			return;
		case LISTEN:
			rets[0] = BsdSocket::GetFd(args[0])->Listen(args[1]);
			return;
		case ACCEPT:
		case ACCEPT_NOCANCEL:
			rets[0] = BsdSocket::GetFd(args[0])->Accept(args[1], args[2]);
			return;

		case SENDMSG:
		case SENDMSG_NOCANCEL:
			rets[0] = 0; //BsdSocket::GetFd(args[0])->SendMsg(args[1], args[2]);
			return;
		case SENDTO:
		case SENDTO_NOCANCEL:
			rets[0] = 0; //BsdSocket::GetFd(args[0])->SendTo(args[1], args[2], args[3], args[4], args[5]);
			Warn("to socket: %s", RamString(args[1]).c_str());
			return;
		/*case SEND:
		case SEND_NOCANCEL:
			rets[0] = BsdSocket::GetFd(args[0])->Send(args[1], args[2], args[3]);
			return;*/
			
		case RECVMSG:
		case RECVMSG_NOCANCEL:
			rets[0] = 0; //BsdSocket::GetFd(args[0])->RecvMsg(args[1], args[2]);
			return;
		case RECVFROM:
		case RECVFROM_NOCANCEL:
			rets[0] = 0; //BsdSocket::GetFd(args[0])->RecvFrom(args[1], args[2], args[3], args[4], args[5]);
			return;
		/*case RECV:
		case RECV_NOCANCEL:
			rets[0] = BsdSocket::GetFd(args[0])->Recv(args[1], args[2], args[3]);
			return;*/

		case SETSOCKOPT:
			rets[0] = 0; //..........
			return;

		case SIGACTION:
		case SIGPROCMASK:
			rets[0] = 0;//SigAction(args[0], args[1], args[2]);
			//rets[0] = SigMask(args[0], args[1], args[2]);
			return;

		case KQUEUE:
			rets[0] = fds->NewFd(new BsdKqueue());
			return;
		case KEVENT:
			rets[0] = BsdKqueue::Kevent(args[0], args[1], args[2], args[3], args[4], args[5]);
			return;

		case MUNMAP:
			rets[0] = MemUnmap(args[0], args[1]);
			return;
		case MADVICE:
			rets[0] = 0; //some memory advice.
			return;
		case MMAP:
			rets[0] = MemMap(args[0], args[1], args[2], args[3], args[4], Long(args[5], args[6]));
			return;

		case SYSCTL:
			rets[0] = SysCtl(args[0], args[1], args[2], args[3], args[4], args[5]);
			return;

		case SHARED_REGION_CHECK_NP:
			rets[0] = 1; //args[0] - some pointer. 0 - cache already mapped. 
			return;
		case SHARED_REGION_MAP_NP:
		case SHARED_REGION_MAP_NP_v2:
			rets[0] = MemMapShared(args[0], args[1], args[2]);
			return;

		case ISSETUGID:
			rets[0] = 0;
			return;

		case GETTIMEOFDAY:
			Long(rets[0], rets[1]) = GetTimeOfDay(args[0], args[1]);
			return;

		case __PTHREAD_CANCELED:
			rets[0] = SetCancel(args[0]);
			return;
		case __DISABLE_THREADSIGNAL:
			rets[0] = 0; //not implemented
			return;

		case __SEMWAIT_SIGNAL:
		case __SEMWAIT_SIGNAL_v2:
			rets[0] = SemWaitSignal(args[0], args[1], args[2], args[3], args[4], args[5]);
			return;

		default:
			rets[0] = 0;
			Fail(syscall);
		}
	}
	catch (ErrNo::Type& errNo)
	{
		switch (cpu.ip)
		{
		case OPEN:
		case OPEN_NOCANCEL:
		case STAT:
		case STAT64:
		case LSTAT:
		case LSTAT64:
		case ACCESS:
		case READLINK:
			Warn("Failed access to %s (cpu.ip == %d, ErrNo = %d)", RamString(args[0]).c_str(), syscall, errNo);
			break;

		default:
			Warn("Bsd ErrNo set to %d (cpu.ip == %d)", errNo, syscall);
		}
		
		rets[0] = errNo;
		cpu.cf = 1; //error!
	}
}

void SwiSys(u32* args, u32 func)
{
	switch (func)
	{
	case SYS_ICACHE_INVALIDATE:
		break;

	case __PTHREAD_SET_SELF:
		cpu.threadData = args[0];
		break;

	default:
		Fail(0);
	}
	//should this return stuff?
}

void Swi(u32 swi) //for ARM code
{
	try
	{
		if (swi == 0x80) 
		{
			if ((s32)cpu.ip > 0)
				SwiBsd(&cpu.a1, &cpu.a1, cpu.ip);
			else if ((s32)cpu.ip == 0)
				SwiBsd(&cpu.a2, &cpu.a1, cpu.a1);
			else if (cpu.ip != 0x80000000)
				SwiMach(&cpu.a1, &cpu.a1, cpu.ip);
			else
				SwiSys(&cpu.a1, cpu.a4);
		}
		else
			Fail(swi);
	}
	catch (StopThread& wait)
	{
		ActiveThreads::RemoveCurrent();
	}
}