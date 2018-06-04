#pragma once
#include "types.h"

class VMFlags
{
public:
	VMFlags(u32 value) : m_value(value) {}
	operator u32() { return m_value; }

	static const int Fixed = 0;
	static const int Anywhere = 1;
	static const int Purgeable = 4;
	static const int Overwrite = 8;

	bool IsAnywhere() { return (m_value & Anywhere) != 0; }
	bool IsPurgeable() { return (m_value & Purgeable) != 0; }
	bool IsOverwrite() { return (m_value & Overwrite) != 0; }

private:
	u32 m_value;
};
