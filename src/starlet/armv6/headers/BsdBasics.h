#pragma once
#include "types.h"
#include "File.h"
#include "BsdFile.h"
#include "Memory.h"
#include "limits.h"

class BsdCharDev : public BsdFile
{
public:
	int ReadP(macptr memBuffer, u32 size, s64 offset) { return Read(memBuffer, size); } //or error?
	int WriteP(macptr memBuffer, u32 size, s64 offset) { return Write(memBuffer, size); } //or error?
	FSMode GetType() { return FSMode::CharDevice; }
};

class BsdRandom : public BsdCharDev
{
public:
	int Read(macptr memBuffer, u32 size)
	{
		for (u32 i = 0; i < size; i++)
			Ram<u8>(memBuffer + i) = rand();
		return size;
	}
	int Write(macptr memBuffer, u32 size) { return size; }
};

class BsdZero : public BsdCharDev
{
public:
	int Read(macptr memBuffer, u32 size)
	{
		for (u32 i = 0; i < size; i++)
			Ram<u8>(memBuffer + i) = 0;
		return size;
	}
	int Write(macptr memBuffer, u32 size) { return size; }
};

class BsdNull : public BsdCharDev
{
public:
	int Read(macptr memBuffer, u32 size) { return 0; }
	bool CanRead() { return false; }
	int Write(macptr memBuffer, u32 size) { return size; }
};

class BsdStdIn : public BsdCharDev
{
public:
	int Read(macptr memBuffer, u32 size) { Warn("STDIN!"); return 0; }
};
class BsdStdOut : public BsdCharDev
{
public:
	int Write(macptr memBuffer, u32 size) { Warn("STDOUT: %s", RamString(memBuffer, size).c_str()); return size; }
};
class BsdStdErr : public BsdCharDev
{
public:
	int Write(macptr memBuffer, u32 size) { Warn("STDERR: %s", RamString(memBuffer, size).c_str()); return size; }
};