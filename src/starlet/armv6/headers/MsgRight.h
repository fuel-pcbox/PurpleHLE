#pragma once
#include "types.h"

class IpcName;
class IpcNameSpace;

class MsgRight
{
public:
	MsgRight(u8 value) : m_value(value) {  }
	MsgRight() : m_value(None) {}
	operator u8() { return m_value; }

	static const int None = 0x0;
	static const int MoveReceive = 0x10;
	static const int MoveSend = 0x11;
	static const int MoveSendOnce = 0x12;
	static const int CopySend = 0x13;
	static const int MakeSend = 0x14;
	static const int MakeSendOnce = 0x15;
	
	static const int PortReceive = 0x10;
	static const int PortSend = 0x11;
	static const int PortSendOnce = 0x12;

	IpcName* TransferTo(IpcName* ipcName, IpcNameSpace* space, u32 name = 0);
	IpcName* TransferToKernel(IpcName* ipcName);

	MsgRight GetTransferredType();

private:
	u8 m_value;
};

class RightType
{
public:
	RightType(u32 value) : m_value(value) { }
	RightType() : m_value(0) {}
	operator u32() { return m_value; }

	static const int Send = 0;
	static const int Receive = 1;
	static const int SendOnce = 2;
	static const int PortSet = 3;
	static const int DeadName = 4;

private:
	u32 m_value;
};
