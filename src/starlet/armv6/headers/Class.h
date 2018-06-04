#pragma once
#include "MemBlock.h"
#include "CpuState.h"
#include <vector>
#include <string>
#include "FuncExec.h"

class BaseClass;
typedef void (BaseClass::*FuncMPtr)();

class BaseClass /*: public IClass*/
{
public:
	BaseClass();
	virtual ~BaseClass() {};
	u32 GetPtr() { return m_ptr; }

	void Execute(u32 msgAddr);
	virtual FuncMPtr FindFunc(std::string& name) = 0;

protected:
	u32 m_ptr;
};
