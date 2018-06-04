#include "Task.h"
#include "HostPort.h"
#include "TaskPort.h"
#include "Bootstrap.h"
#include "Thread.h"
#include "FdSpace.h"
#include <list>

#include "MsgFlags.h"
#include "MsgError.h"

Task::Task()
{
	vMem = new VirtualMem();

	nameSpace = new IpcNameSpace();
	fdSpace = new FdSpace();

	m_taskPort = IpcName::GetName(new TaskPort(this));
	m_hostPort = IpcName::GetName(new HostPort(this));
	m_bootPort = App::Launch() ? NULL : IpcName::GetName(new Bootstrap(this));

	Thread* firstThread = NewThread();
	firstThread->Resume(); //for now, probably
}

Thread* Task::NewThread()
{
	Thread* newThread = new Thread(this);
	m_threads.push_back(newThread);
	return newThread;
}

void Task::RemoveThread(Thread* thread)
{
	m_threads.remove(thread);
}

IpcName* Task::GetSpecialPort(int i)
{
	switch (i)
	{
	case 4:
		return GetBootstrap();
	default:
		Fail(i);
	}
}

void Task::SetSpecialPort(int i, IpcName* port)
{
	switch (i)
	{
	case 4:
		SetBootstrap(port);
		break;
	default:
		Fail(i);
	}
}


int Task::Message(macptr msgAddr, MsgFlags opts, u32 sendSize, u32 recvSize, u32 recvPort, u32 timeOut, u32 notifyPort)
{
	try
	{
		s64 timeOutAbs = TimeUtils::IntervalMsec(timeOut);

		if (opts.ShouldSend())
		{
			IpcMessage* msg = new IpcMessage();
			msg->LoadFrom(msgAddr, sendSize);
			msg->Send(opts.TimeoutSend(), timeOutAbs); 
		}
		if (opts.ShouldReceive())
		{			
			IpcName* recvName = g_task->nameSpace->Name(recvPort);
			IpcMessage* msg = IpcMessage::Receive(recvName, opts.TimeoutReceive(), timeOutAbs);
			msg->StoreTo(msgAddr, recvSize, opts.GetTrailerNum());
			delete msg;
		}
		return MsgError::NoError;
	}
	catch (MsgError::Type& err)
	{
		return err;
	}
}


Task* g_task = NULL;

