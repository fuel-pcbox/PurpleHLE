#pragma once
#include "Task.h" 
#include "Memory.h"

class TaskPort : public IpcKernelPort
{
	static const int PORT_TYPE = 3201;
	static const int PORT_ALLOCATE = 3204;
	static const int PORT_DESTROY = 3205;
	static const int PORT_DEALLOCATE = 3206;
	static const int PORT_MOD_REFS = 3208;
	static const int PORT_GET_SET_STATUS = 3211;
	static const int PORT_MOVE_MEMBER = 3212;
	static const int PORT_REQUEST_NOTIFICATION = 3213;
	static const int PORT_INSERT_RIGHT = 3214;
	static const int PORT_GET_ATTRIBUTES = 3217;
	static const int PORT_SET_ATTRIBUTES = 3218;
	static const int PORT_INSERT_MEMBER = 3226;
	static const int PORT_EXTRACT_MEMBER = 3227;

	static const int TASK_THREADS = 3402;
	static const int PORTS_LOOKUP = 3404;
	static const int TASK_GET_SPECIAL_PORT = 3409;
	static const int TASK_SET_SPECIAL_PORT = 3410;
	static const int THREAD_CREATE = 3411;
	static const int THREAD_CREATE_RUNNING = 3412;
	static const int SEMAPHORE_CREATE = 3418;

	static const int VM_ALLOCATE = 3801;
	static const int VM_DEALLOCATE = 3802;
	static const int VM_PROTECT = 3803;
	static const int VM_MAP = 3812;
	static const int VM_PURGABLE_CONTROL = 3830;

	static const int MACH_MAKE_MEMORY_ENTRY_64 = 3825;

	static const int MACH_VM_ALLOCATE = 4800;
	static const int MACH_VM_DEALLOCATE = 4801;
	static const int MACH_VM_PROTECT = 4802;
	static const int MACH_VM_MAP = 4811;
	static const int MACH_VM_REGION = 4816;

public:
	TaskPort(Task* task) : m_task(task)
	{}

	Task* GetTask() { return m_task; }

	bool Process(IpcKernMsg* message, IpcKernMsg* reply);
	
private:
	Task* m_task;
};
