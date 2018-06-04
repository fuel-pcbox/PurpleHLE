#include "Thread.h"
#include "ThreadPort.h"
#include "TimerPort.h"

using namespace Interpreter;

Thread::Thread(Task* task) : m_task(task)
{
	m_threadPort = IpcName::GetName(new ThreadPort(this));
	m_resume = NULL;
	m_normalResume = false;
	m_waitUntil = new TimerPort(false);
	m_waitingOn = NULL;
}

void Thread::Terminate()
{
	if (m_waitingOn)
		m_waitingOn->RemoveWaiting(this);

	m_threadPort->DeleteAll();
	m_task->RemoveThread(this);
	if (!ActiveThreads::RemoveThreadNotCurrent(this))
		throw StopThread();
	//doesn't delete self, leak!
}

void Thread::Resume()
{
	if (!ActiveThreads::HasThread(this)) 
		ActiveThreads::AddThread(this);
}

int Thread::GetContext(int flavor, u32* state, int size)
{
	Assert(flavor == 1 && size >= 17);

	memcpy(state, m_context.gpr, 15 * sizeof(u32));
	state[15] = m_context.nia;
	
	state[16] = (m_context.nf << 31) | (m_context.zf << 30) | (m_context.cf << 29) | (m_context.vf << 28) |
	            (m_context.qf << 27) | (m_context.ge << 16) | (m_context.thumb << 5) | 0x10;

	return size;
}

void Thread::SetContext(int flavor, u32* state, int size)
{
	Assert(flavor == 1 && size >= 17);

	memcpy(m_context.gpr, state, 15 * sizeof(u32));
	m_context.nia = state[15];
	
	m_context.nf = BITS(state[16], 31, 1); 
	m_context.zf = BITS(state[16], 30, 1);
	m_context.cf = BITS(state[16], 29, 1);
	m_context.vf = BITS(state[16], 28, 1);
	m_context.qf = BITS(state[16], 27, 1);
	m_context.ge = BITS(state[16], 16, 4);
	m_context.thumb = BITS(state[16], 5, 1);
}

void Thread::WaitUntil(s64 time)
{
	m_waitUntil->Arm(time);
	m_waitUntil->Wait();
}

void ActiveThreads::UpdateContext(Thread* oldThread)
{
	Thread* newThread = Current();

	if (oldThread != newThread)
	{
		oldThread->SetContext(cpu); //store cpu
		newThread->GetContext(cpu); //load cpu

		Interpreter::cpu.exclusive = false; //needs to be done better
	}
}

void ActiveThreads::ChangeThread()
{
	Thread* oldThread = Current();

	g_currentThread++;
	if (g_currentThread == g_activeThreads.end())
		g_currentThread = g_activeThreads.begin();

	UpdateContext(oldThread);
	(*g_currentThread)->CallResumeFunc();
}

void ActiveThreads::ChangeThreadRemove()
{
	Iter oldIter = g_currentThread;

	g_currentThread++;
	if (g_currentThread == g_activeThreads.end())
		g_currentThread = g_activeThreads.begin();

	UpdateContext(*oldIter);

	Assert(*oldIter != Current());
	g_activeThreads.erase(oldIter);

	(*g_currentThread)->CallResumeFunc();
}

void ActiveThreads::RemoveCurrent()
{
	if (g_activeThreads.size() == 1) //this is the only thread, need special care
	{
		Thread* oldThread = *g_currentThread;
		g_activeThreads.erase(g_currentThread);
		g_currentThread = g_activeThreads.end();
		
		while (g_activeThreads.size() == 0)
			TimeLine::AdvanceToNext();
		
		g_currentThread = g_activeThreads.begin();
		UpdateContext(oldThread);
		(*g_currentThread)->CallResumeFunc();
		return;
	}

	ChangeThreadRemove();
}

void ActiveThreads::AddThread(Thread* thread)
{
	for (Iter i = g_activeThreads.begin(); i != g_activeThreads.end(); i++)
		Assert(thread != *i);

	g_activeThreads.push_back(thread);
	if (g_activeThreads.size() == 1)
		g_currentThread = g_activeThreads.begin();
}

bool ActiveThreads::HasThread(Thread* thread)
{
	for (Iter i = g_activeThreads.begin(); i != g_activeThreads.end(); i++)
		if (thread == *i)
			return true;
	return false;
}

bool ActiveThreads::RemoveThreadNotCurrent(Thread* thread)
{
	if (thread == *g_currentThread)
		return false;
	g_activeThreads.remove(thread);
	return true;
}


Thread* ActiveThreads::Current()
{ 
	Assert(g_currentThread != g_activeThreads.end()); 
	return *g_currentThread; 
}

std::list<Thread*> ActiveThreads::g_activeThreads;
std::list<Thread*>::iterator ActiveThreads::g_currentThread;
