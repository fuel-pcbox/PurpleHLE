#pragma once
#include "types.h"

class MsgFlags
{
public:
	MsgFlags(u32 value) : m_value(value) { MakeSure(value == (value & 0x0f000117)); } //
	operator u32() { return m_value; }

	static const int MessageSend = 0x1;
	static const int MessageReceive = 0x2;
	static const int ReceiveLarge = 0x4;
	
	static const int SendTimeout = 0x10;
	static const int SendCancel = 0x80;

	static const int ReceiveTimeout = 0x100;
	static const int ReceiveNotify = 0x200;
	static const int ReceiveOverwrite = 0x400;

	bool ShouldSend() { return !!(m_value & MessageSend); }
	bool ShouldReceive() { return !!(m_value & MessageReceive); }
	bool ShouldReceiveLarge() { return !!(m_value & ReceiveLarge); }

	bool TimeoutSend() { return !!(m_value & SendTimeout); }
	bool TimeoutReceive() { return !!(m_value & ReceiveTimeout); }

	u32 GetTrailerType() { return ((m_value >> 28) & 0xf); }
	u32 GetTrailerNum() { return ((m_value >> 24) & 0xf); }

private:
	u32 m_value;
};
