#pragma once
#include "types.h"
#include "BsdSocket.h"
#include "Memory.h"
#include <deque>

struct Dgram
{
	Dgram(u32 len) : data(len) {}
	Ref<SockAddr> addr;
	Array<u8> data;
};

class DgramSocket : public BsdSocket
{
public:
	bool CanRead();

	int Connect(macptr addr, u32 addrLen);
	int Bind(macptr addr, u32 addrLen);
	void ConnectDirect(Ref<BsdSocket> socket);

	int Send(macptr buffer, u32 length, int flags);
	//int SendMsg(macptr message, int flags);
	int SendTo(macptr buffer, u32 length, int flags, macptr addr, u32 addrLen);
	
	int Recv(macptr buffer, u32 length, int flags);
	//int RecvMsg(macptr message, int flags);
	int RecvFrom(macptr buffer, u32 length, int flags, macptr addr, macptr lenPtr);

	friend class BsdSocket;

protected:
	DgramSocket(u32 domain, u32 type, u32 protocol) 
		: BsdSocket(domain, type, protocol) {}
	
	int DoSend(macptr buffer, u32 length, int flags, Ref<DgramSocket> target);
	int DoReceive(macptr buffer, u32 length, int flags, macptr addr, macptr lenPtr);

	Ref<DgramSocket> m_target;
	std::deque<Dgram> m_dgramQue;
};