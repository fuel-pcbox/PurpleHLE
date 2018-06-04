#include "FuncExec.h"
#include "CpuState.h"
#include "MemBlock.h"
#include "MetaClass.h"
#include "Object.h"
#include "MiscClass.h"

using namespace Interpreter;

FuncPtr SelectFunc(std::string& name)
{
	if (name == "_UIApplicationMain")
		return &UIApplicationMain;
	else if (name == "_objc_msgSend")
		return &objc_msgSend;
	else
		return NULL;
}


void* SelectClass(std::string& name)
{
	if (name == "_OBJC_CLASS_$_NSAutoreleasePool")
		return new MetaClass<NSAutoreleasePool>();
	else
		return NULL;
}


u32 GetFuncPtr(std::string& name)
{
	FuncPtr func = SelectFunc(name);
	if (func)
		return 0x80000000 + (u32)func;
	else
		return (u32)name.data();
}
u32 GetClassPtr(std::string& name)
{
	void* func = SelectClass(name);
	if (func)
		return 0x80000000 + (u32)func;
	else
		return (u32)name.data();
}