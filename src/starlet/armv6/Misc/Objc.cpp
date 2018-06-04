#include "Objc.h"
#include "CpuState.h"
#include "MemBlock.h"
#include "Class.h"

using namespace Interpreter;

void objc_msgSend()
{
	u32 address = cpu.a1;
	u32 isa = Ram<u32>(address);
	if (isa < 0x80000000) //custom class 
		Fail(0);
	else //exec class
	{
		BaseClass* classptr = (BaseClass*)(isa - 0x80000000);
		classptr->Execute(cpu.a2); //performs whatever is needed (including Ret)
	}
}