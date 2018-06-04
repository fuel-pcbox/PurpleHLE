#pragma once
#include "Task.h"
#include "IpcPort.h"

class Thread;

class ThreadPort : public IpcKernelPort
{
	static const int THREAD_TERMINATE = 3600;
	static const int THREAD_GET_STATE = 3603;
	static const int THREAD_SET_STATE = 3604;
	static const int THREAD_RESUME = 3606;
	static const int THREAD_POLICY = 3616;
	static const int THREAD_POLICY_SET = 3617;

public:
	ThreadPort(Thread* thread) : m_thread(thread) {}

	bool Process(IpcKernMsg* message, IpcKernMsg* reply);

private:
	Thread* m_thread;
};
