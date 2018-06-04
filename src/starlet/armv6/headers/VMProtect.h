#pragma once
#include "types.h"

class VMProtect
{
public:
	VMProtect() : m_value(None) {}
	VMProtect(u32 value) : m_value(value) {}
	operator u32() const { return m_value; }

	static const int None = 0;

	static const int Read = 1;
	static const int Write = 2;
	static const int Execute = 4;

	static const int All = 7;

	bool CanRead() const { return !!(m_value & Read); }
	bool CanWrite() const { return !!(m_value & Write); }
	bool CanExecute() const { return !!(m_value & Execute); }
	bool CanAccess() const { return (m_value != None); }

	bool RestrictedBy(VMProtect other) const { return (m_value & ~other) == 0; }

private:
	u32 m_value;
};