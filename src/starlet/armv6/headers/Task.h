#pragma once
#include "types.h"

#include "IpcPort.h"
#include "IpcName.h"
#include "IpcMessage.h"
#include "IpcNameSpace.h"
#include "MsgFlags.h"

#include "VirtualMem.h"
#include "StringUtils.h"

class Thread;
class FdSpace;

class Task
{
public:
	Task();

	// return names in kernel namespace
	IpcName* GetTaskSelf() { return m_taskPort; }
	IpcName* GetHostSelf() { return m_hostPort; }
	IpcName* GetBootstrap() { return m_bootPort; }
	IpcName* GetSpecialPort(int i);

	const std::list<Thread*> GetThreads() { return m_threads; }
	Thread* NewThread();
	void RemoveThread(Thread* m_thread);

	void SetBootstrap(IpcName* newPort) { m_bootPort = newPort; }
	void SetSpecialPort(int i, IpcName* newPort);

	IpcName* NewUserReplyPort() { return IpcName::NewReceive(new IpcUserPort()); } //creates in user namespace

	int Message(macptr msgAddr, MsgFlags opts, u32 sendSize, u32 recvSize, u32 recvPort, u32 timeOut, u32 notifyPort);

	VirtualMem* vMem;
	IpcNameSpace* nameSpace;
	FdSpace* fdSpace;

	friend class Bootstrap; //temp silly

private:
	IpcName* m_taskPort;
	IpcName* m_hostPort;
	IpcName* m_bootPort;

	std::list<Thread*> m_threads;
};

extern Task* g_task; 