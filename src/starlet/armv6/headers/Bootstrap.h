#pragma once
#include "Task.h"

class Bootstrap : public IpcKernelPort
{
public:
	Bootstrap(Task* task);

	bool Process(IpcKernMsg* message, IpcKernMsg* reply);

protected:
	typedef std::map<std::string, IpcName*> PortMap;
	typedef PortMap::iterator PortIter;

	PortMap m_portMap;
	Task* m_task;
};