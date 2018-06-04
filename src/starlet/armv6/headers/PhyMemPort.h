#pragma once
#include "Task.h"
#include "PhysicalMem.h"

class PhyMemPort : public IpcKernelPort //TEMP. should be one with PhysicalMem
{
public:
	PhyMemPort(Ref<PhysicalMem> phyMem) : m_phyMem(phyMem) {}

	bool Process(IpcKernMsg* message, IpcKernMsg* reply)
	{
		switch (message->ID())
		{
		default:
			Fail(message->ID());
		}
		return true;
	}

	Ref<PhysicalMem> PhyMem() { return m_phyMem; }

private:
	Ref<PhysicalMem> m_phyMem;
};
