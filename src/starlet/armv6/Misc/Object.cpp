#include "Object.h"

using namespace Interpreter;

void NSObject::Init()
{ 
	m_refCnt = 1;
	Ret();
}
void NSObject::Retain()
{ 
	m_refCnt++;
	Ret();
}
void NSObject::Release()
{ 
	m_refCnt--; 
	if (m_refCnt == 0) 
		delete this;
	Ret();
}