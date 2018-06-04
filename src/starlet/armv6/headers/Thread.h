#pragma once
#include <list>

#include "types.h"
#include "IpcName.h"
#include "IpcMessage.h"
#include "CpuState.h"

typedef void (*ResumeFunc)(bool normal, u32 data);

class Task;
class TimerPort;
class Waitable;

extern Task* g_task;

class Thread //very temp-ish still
{
public:
	Thread(Task* task);

	IpcName* GetThreadSelf() { return m_threadPort; }

	int GetContext(int flavor, u32* state, int size);
	void SetContext(int flavor, u32* state, int size);

	void GetContext(Interpreter::CpuState& dest) { memcpy(&dest, &m_context, sizeof(Interpreter::CpuState)); }
	void SetContext(const Interpreter::CpuState& src) { memcpy(&m_context, &src, sizeof(Interpreter::CpuState)); }

	void Resume();

	void WaitUntil(s64 time);

	void CallResumeFunc() 
	{ 
		g_task = m_task; //HACK
		if (m_resume) 
		{ 
			ResumeFunc resume = m_resume;
			m_resume = NULL;
			resume(m_normalResume, m_resumeData); 
		} 
	}

	void SetResumeFunc(ResumeFunc resume) { m_resume = resume; }
	void SetResumeSuccess(bool success) { m_normalResume = success; }
	void SetResumeParam(u32 data) { m_resumeData = data; }

	void SetWaitingOn(Waitable* wait) { m_waitingOn = wait; }

	void Terminate();

private:
	Task* m_task;
	IpcName* m_threadPort;

	ResumeFunc m_resume;
	bool m_normalResume;
	u32 m_resumeData;

	TimerPort* m_waitUntil;
	Waitable* m_waitingOn;

	Interpreter::CpuState m_context;
};

class ActiveThreads
{
public:
	static void ChangeThread();
	static void RemoveCurrent();
	static Thread* Current();
	static void AddThread(Thread* thread);
	static bool RemoveThreadNotCurrent(Thread* thread);
	static bool HasThread(Thread* thread);

private:
	static std::list<Thread*> g_activeThreads;
	static std::list<Thread*>::iterator g_currentThread;

	typedef std::list<Thread*>::iterator Iter;

	static void UpdateContext(Thread* oldThread);
	static void ChangeThreadRemove();
};