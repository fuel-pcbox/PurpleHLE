#pragma once
#include "Class.h"

class NSObject : public BaseClass
{
public:
	void Release();
	void Retain();
	void Init();

	virtual FuncMPtr FindFunc(std::string& name)
	{
		if (name == "init")
			return (FuncMPtr) &NSObject::Init;
		else if (name == "retain")
			return (FuncMPtr) &NSObject::Retain;
		else if (name == "release")
			return (FuncMPtr) &NSObject::Release;
		else
			return NULL;
	}
protected:
	u32 m_refCnt;
};


