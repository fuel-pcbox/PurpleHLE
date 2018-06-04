#pragma once
#include "Task.h"
#include "Waitable.h"
#include "Thread.h"

class Semaphore : public IpcKernelPort, public Waitable
{
public:
	Semaphore(int policy, int initValue) : m_policy(policy), m_value(initValue) {}

	void Signal() 
	{ 
		m_value++; 
		if (ResumeOne())
			m_value--;
	}
	void SignalThread(Thread* thread) 
	{ 
		bool signalled = thread ? ResumeSpecific(thread) : ResumeOne();
		if (!signalled)
			throw MachErr::NotWaiting;
	}
	void Wait() 
	{ 
		if (m_value > 0)
			m_value--;
		else
			Waitable::Wait();
	}
	void TimedWait(s64 time) 
	{ 
		if (m_value > 0)
			m_value--;
		else
			Waitable::WaitTime(time);
	}


	static void Signal(u32 name) 
	{ 
		GetSemaphore(name)->Signal(); 
	}
	static void SignalThread(u32 name, u32 thread) 
	{ 
		GetSemaphore(name)->SignalThread(GetThread(thread)); 
	}
	static void Wait(u32 name) 
	{ 
		GetSemaphore(name)->Wait(); 
	}
	static void WaitSignal(u32 wait, u32 signal) 
	{ 
		if (signal)
			GetSemaphore(signal)->Signal(); 
		GetSemaphore(wait)->Wait();
	}
	static void TimedWaitSignal(u32 wait, u32 signal, s64 time) 
	{ 
		if (signal)
			GetSemaphore(signal)->Signal(); 
		GetSemaphore(wait)->TimedWait(time);
	}

	bool Process(IpcKernMsg* message, IpcKernMsg* reply)
	{
		switch (message->ID())
		{
		default:
			Fail(message->ID());
		}
		return true;
	}

	static Semaphore* GetSemaphore(u32 name)
	{
		IpcName* ipcName = g_task->nameSpace->Name(name);
		if (!ipcName || !ipcName->SendRight())
			throw MachErr::InvalidArgument;
		Semaphore* sema = dynamic_cast<Semaphore*>(ipcName->SendRight()->Port());
		if (!sema)
			throw MachErr::InvalidArgument;
		return sema;
	}

	static Thread* GetThread(u32 name) //really not the place!
	{
		if (name == 0)
			return NULL;
		IpcName* ipcName = g_task->nameSpace->Name(name);
		if (!ipcName || !ipcName->SendRight())
			throw MachErr::InvalidArgument;
		Thread* thread = dynamic_cast<Thread*>(ipcName->SendRight()->Port());
		if (!thread)
			throw MachErr::InvalidArgument;
		return thread;
	}

private:


	int m_policy;
	int m_value;
};
