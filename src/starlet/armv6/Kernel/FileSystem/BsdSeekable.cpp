#include "BsdSeekable.h"


s64 BsdSeekable::Seek(s64 offset, u32 whence)
{
	switch (whence)
	{
	case 0: //SEEK_SET
		m_offset = offset;
		break;
	case 1: //SEEK_CUR
		m_offset += offset;
		break;
	case 2: //SEEK_END
		m_offset = GetSize() + offset;
		break;
	default:
		throw ErrNo::InvalidArg;
	}
	return m_offset;
}

int BsdSeekable::Read(macptr memBuffer, u32 size)
{
	int advance = ReadP(memBuffer, size, m_offset);
	m_offset += advance;
	return advance;
}
int BsdSeekable::Write(macptr memBuffer, u32 size)
{
	int advance = WriteP(memBuffer, size, m_offset);
	m_offset += advance;
	return advance;
}