#pragma once
#include "Class.h"

template <class CT>
class MetaClass : public NSObject
{
public:
	void Alloc()
	{
		cpu.a1 = (new CT())->GetPtr();
		Ret();
	}

	virtual FuncMPtr FindFunc(std::string& name)
	{
		if (name == "alloc")
			return (FuncMPtr) &MetaClass::Alloc;
		else
			return NSObject::FindFunc(name);
	}
};