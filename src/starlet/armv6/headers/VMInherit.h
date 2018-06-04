#pragma once
#include "types.h"

class VMInherit
{
public:
	VMInherit(u32 value) : m_value(value) {}
	operator u32() { return m_value; }

	static const int Share = 0;
	static const int Copy = 1;
	static const int None = 2;
	static const int CopyAndDelete = 3;

	bool ShouldShare() { return (m_value == Share); }
	bool ShouldCopy() { return (m_value == Copy || m_value == CopyAndDelete); }
	bool ShouldDelete() { return (m_value == CopyAndDelete); }

private:
	u32 m_value;
};
