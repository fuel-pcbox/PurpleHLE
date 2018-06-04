#pragma once
#include "types.h"
#include "BsdFile.h"

class BsdSeekable : public BsdFile
{
public:
	BsdSeekable() : m_offset(0) {}

	s64 Seek(s64 offset, u32 whence);
	int Read(macptr memBuffer, u32 size);
	int Write(macptr memBuffer, u32 size);

protected:
	s64 m_offset;
};