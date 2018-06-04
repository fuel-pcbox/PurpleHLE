#pragma once
#include "types.h"
#include "BsdSocket.h"
#include "Memory.h"
#include <deque>

class StreamSocket : public BsdSocket
{
public:
	int Read(macptr memBuffer, u32 size);
	int Write(macptr memBuffer, u32 size);
	bool CanRead();

	int Connect(macptr addr, u32 addrLen);
	int Bind(macptr addr, u32 addrLen);
	int Listen(int backLog);
	int Accept(macptr addr, macptr lenPtr);
	
	void ConnectDirect(Ref<BsdSocket> socket);
	
	int Send(macptr buffer, u32 length, int flags) 
	{ 
		Assert(!flags); 
		return Write(buffer, length); 
	}
	
	int Recv(macptr buffer, u32 length, int flags) 
	{ 
		Assert(!flags); 
		return Read(buffer, length); 
	}

	friend class BsdSocket;

protected:
	StreamSocket(u32 domain, u32 type, u32 protocol) 
		: BsdSocket(domain, type, protocol), m_listen(false), m_direct(false) {}

	typedef struct { Ref<SockAddr> addr; Ref<StreamSocket> socket; } ConnectData;
	
	void AddConnection(ConnectData connect);
	int AddData(macptr memBuffer, u32 size);

	bool m_listen;
	bool m_direct;
	Ref<StreamSocket> m_target;
	std::deque<u8> m_dataQue;
	std::deque<ConnectData> m_listenQue;
};