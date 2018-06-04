#pragma once
#include "Class.h"

class NSAutoreleasePool : public NSObject
{
public:
	virtual FuncMPtr FindFunc(std::string& name)
	{
		return NSObject::FindFunc(name);
	}
};
