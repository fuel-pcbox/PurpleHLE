#pragma once
#include "CpuState.h"
#include "Objc.h"
#include "MiscFunc.h"

const u32 FUNC_EXEC_HACK = 0xffeeddcc; //undefined ARM instruction

u32 GetFuncPtr(std::string& name);
u32 GetClassPtr(std::string& name);

typedef void (*FuncPtr)();

void INLINE FuncExec(u32 data)
{
	if (data >= 0x80000000)
		((FuncPtr)(data - 0x80000000))();
	else
	{
		const char *name = (const char *)data;
		Fail(0);
	}
}
