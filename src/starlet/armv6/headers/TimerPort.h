#pragma once
#include "Task.h"
#include "Timer.h"
#include "Waitable.h"
#include "Memory.h"

class TimerPort : public IpcUserPort
{
public:
	TimerPort(bool sendMsgs = true) : m_timer(NULL), m_wait(NULL), m_sendMsgs(sendMsgs) {}
	virtual ~TimerPort() { Delete(); }

	void Delete()
	{
		if (m_timer) 
		{
			m_timer->Cancel();
			delete m_timer;
			m_timer = NULL;
		}
	}

	int Arm(s64 time)
	{
		if (m_timer)
			Delete();
		m_timer = new Timer(time, TimerPort::Expire, this);
		return 0;
	}

	int Cancel(macptr timeptr)
	{
		s64 time;
		if (m_timer)
		{
			bool canceled = m_timer->Cancel();
			MakeSure(canceled);

			time = m_timer->GetTime();
			Delete();
		}
		if (timeptr)
		{
			Ram<u32>(timeptr) = LOW32(time);
			Ram<u32>(timeptr + 4) = HIGH32(time);
		}
		return 0;
	}

	int Wait() //not tested against actual swi
	{
		Assert(m_timer);
		Assert(m_wait == NULL);

		m_wait = new Waitable();
		m_wait->Wait();
		return 0;
	}

	void Expire()
	{
		Assert(m_timer);
		if (m_sendMsgs)
		{
			//should this use ndr?
			IpcKernMsg* reply = IpcKernMsg::MakeMsg(m_receiveRight->Name(), MsgRight::MakeSendOnce, 0, false);
			//should this have added reserved garbage?
			reply->Send();
		}

		if (m_wait)
		{
			m_wait->ResumeOne();

			delete m_wait;
			m_wait = NULL;
		}
	}

	static u32 New()
	{  
		return IpcName::NewReceive(new TimerPort())->Name();
	}

	static int Destroy(u32 name)
	{
		GetTimerName(name)->DeleteReceiveRight();
		return 0;
	}

	static int Arm(u32 name, s64 time)
	{
		return GetTimer(name)->Arm(time);
	}

	static int Cancel(u32 name, macptr timeptr)
	{
		return GetTimer(name)->Cancel(timeptr);
	}

	static void Expire(void* ptr)
	{
		TimerPort* timerPort = static_cast<TimerPort*>(ptr);
		timerPort->Expire();
	}

private:

	static IpcName* GetTimerName(u32 name)
	{
		return GetTimer(name)->m_receiveRight->Name();
	}

	static TimerPort* GetTimer(u32 name)
	{
		IpcName* ipcName = g_task->nameSpace->Name(name);
		if (!ipcName)
			throw MachErr::InvalidName;
		if (!ipcName->ReceiveRight())
			throw MachErr::InvalidRight;

		TimerPort* timerPort = dynamic_cast<TimerPort*>(ipcName->ReceiveRight()->Port());
		if (!timerPort)
			throw MachErr::InvalidArgument;

		return timerPort;
	}

	Timer* m_timer;
	Waitable* m_wait;
	bool m_sendMsgs;
};