#pragma once
#include "types.h"

class MMFlag
{
public:
	MMFlag(u32 value) : m_value(value) { MakeSure(value == (value & 0x1013)); }
	operator u32() { return m_value; }

	static const int Shared = 0x1;
	static const int Private = 0x2;

	static const int Fixed = 0x10;

	static const int Anon = 0x1000;
	
	bool IsShared() { return !!(m_value & Shared); }
	bool IsFixed() { return !!(m_value & Fixed); }
	bool IsAnonymous() { return !!(m_value & Anon); }

private:
	u32 m_value;
};
