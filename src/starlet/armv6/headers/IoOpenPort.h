#pragma once
#include <map>
#include "Task.h"
#include "Plist.h"

class IoServicePort;

class IoIterator : public IpcKernelPort
{
public:
	IoIterator(std::vector<IoServicePort*> contents) : m_contents(contents), m_index(0) {}

	bool Process(IpcKernMsg* message, IpcKernMsg* reply);

protected:
	std::vector<IoServicePort*> m_contents;
	u32 m_index;
};

struct Interest
{
	IpcName* port;
	u32 refs[8];
};

class IoOpenPort : public IpcKernelPort
{
public:
	IoOpenPort() { m_task = NULL; m_notifyPort = NULL; m_parent = NULL; }

	void Open(Task* task) { m_task = task; }

	bool Process(IpcKernMsg* message, IpcKernMsg* reply);

	virtual u32 IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
						 macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
						 u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol) = 0;

	void SetParent(IoServicePort* port) { m_parent = port; }
protected:
	IpcName* m_notifyPort;
	Task* m_task;
	IoServicePort* m_parent;
};

#define SIZES(is64, is8, isool, os64, os8, osool) MACRO( \
	MakeSure(inSize64 == (is64) && inSize8 == (is8) && inSizeOol == (isool) && \
			 *outSize64 == (os64) && *outSize8 == (os8) && *outSizeOol == (osool)); )

#define SIZES_64(is64, os64) SIZES(is64, 0, 0, os64, 0, 0)
#define SIZES_8(is8, os8) SIZES(0, is8, 0, 0, os8, 0)