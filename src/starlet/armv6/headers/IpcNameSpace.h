#pragma once
#include "types.h"
#include <stack>
#include "IpcRight.h"
#include "IpcName.h"

class IpcNameSpace
{
public:
	IpcNameSpace() : m_vacantBarrier(0) {}

	IpcName* NewName(u32 name)
	{
		if (name == 0 || name == DeadName || ContainsName(name))
			return NULL;

		IpcName* newName = new IpcName(this, name);
		m_ipcNames[name] = newName;
		return newName;
	}
	
	IpcName* NewOrGetName(u32 name)
	{
		if (ContainsName(name))
			return Name(name);
		else
			return NewName(name);
	}

	bool ContainsName(u32 name)
	{
		return m_ipcNames[name] != NULL;
	}

	IpcName* Name(u32 name)
	{
		return m_ipcNames[name];
	}

	void DeleteName(u32 name)
	{
		delete m_ipcNames[name];
		m_ipcNames[name] = NULL;
		if (name < m_vacantBarrier)
			m_vacant.push(name);
	}

	u32 GetVacantName()
	{
		if (!m_vacant.empty())
		{
			u32 name = m_vacant.top();
			MakeSure(m_ipcNames[name] == NULL);
			m_vacant.pop();
			return name;
		}

		while (true)
		{
			u32 name = m_vacantBarrier++;
			if (name == 0 || m_ipcNames[name])
				continue;
			else if (name == DeadName)
				Fail(-1);
			else
				return name;
		}
	}
	
	static const u32 DeadName = 0xffffffff;

	static IpcNameSpace* KernelIpcSpace()
	{
		if (!kernelIpcSpace)
			kernelIpcSpace = new IpcNameSpace();
		return kernelIpcSpace;
	}

private:
	static IpcNameSpace* kernelIpcSpace; // for when ports are owned by the kernel

	typedef std::map<u32, IpcName*>::iterator Iter;

	std::map<u32, IpcName*> m_ipcNames;
	std::stack<u32> m_vacant;
	u32 m_vacantBarrier;

};