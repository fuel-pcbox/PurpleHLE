#include "Class.h"
#include "CpuState.h"

using namespace Interpreter;

BaseClass::BaseClass()
{
	m_ptr = memBlock->Alloc(sizeof(int)); //no support for custom classes and variables yet (will need redoing stuff)
	Ram<u32>(m_ptr) = 0x80000000 + (u32)this;
}

void BaseClass::Execute(u32 msgAddr)
{
	//for now, no caching
	std::string name = std::string((const char*) &Ram<u8>(msgAddr));
	FuncMPtr func = FindFunc(name);
	if (func)
		(this->*func)();
	else
		Fail(0);
}






/*void CustomClass::CalcInstSize()
{
	// Get superclass's size
	if (m_super)
		m_instSize = m_super->m_instSize;
	else
		m_instSize = 0;

	m_instSize += 
}

void CustomClass::InitClass()
{
	CalcInstSize();
}*/