#pragma once
#include "types.h"
#include "File.h"

class FSOflag
{
public:
	FSOflag(u32 value) : m_value(value) { MakeSure(value == (value & 0x300e07)); }
	operator u32() { return m_value; }

	static const int ReadOnly = 0x0;
	static const int WriteOnly = 0x1;
	static const int ReadWrite = 0x2;

	static const int NonBlocking = 0x4;
	static const int Append = 0x8;

	//static const int SharedLock = 0x10;
	//static const int ExclusiveLock = 0x20;

	//static const int FlushWrites = 0x40;
	//static const int SendSignal = 0x80;

	static const int LinkNoFollow = 0x100;

	static const int Create = 0x200;
	static const int Truncate = 0x400;
	static const int CreateNew = 0x800;

	static const int Directory = 0x100000;

	//static const int EventsOnly = 0x8000;
	//static const int NoCntlTerminal = 0x20000;
	static const int SymLink = 0x200000;
	
	bool CanRead() { return (AccessMode() != WriteOnly); }
	bool CanWrite() { return (AccessMode() != ReadOnly); }
	bool ShouldAppend() { return !!(m_value & Append); }
	bool CanCreate() { return !!(m_value & Create); }
	bool ShouldTruncate() { return !!(m_value & Truncate); }
	bool CanOnlyCreateNew() { return !!(m_value & CreateNew); }
	bool WantDirectory() { return !!(m_value & Directory); }
	bool DontFollowLinks() { return !!(m_value & LinkNoFollow); }
	bool OpenLinks() { return !!(m_value & SymLink); }
	bool MustNotBlock() { return !!(m_value & NonBlocking); }

	u32 GetAccessMode()
	{
		if (CanRead() && CanWrite())
			return File::ReadWrite;
		else if (CanRead())
			return File::ReadOnly;
		else if (CanWrite())
			return File::WriteOnly;
		else
			Fail(-1);
		return 0;
	}

	u32 GetOpenMode()
	{
		if (CanCreate() && CanOnlyCreateNew()) 
			return File::CreateNew;
		else if (CanCreate() && ShouldTruncate())
			return File::CreateAlways;
		else if (CanCreate())
			return File::OpenAlways;
		else if (ShouldTruncate())
			return File::TruncExisting;
		else
			return File::OpenExisting;
	}

private:
	FSOflag AccessMode() { return (m_value & AccessMask); }

	static const int AccessMask = 0x3;

	u32 m_value;
};
