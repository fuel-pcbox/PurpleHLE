#pragma once
#include "types.h"
#include "BsdFile.h"

class BsdSocket;

class SockAddr : public Reffable
{
public:
	static Ref<SockAddr> NewAddr(u32 domain, macptr addr, u32 addrLen);
	static Ref<SockAddr> NewAddr(u32 domain);

	virtual Ref<BsdSocket> GetSocket() = 0;
	virtual void SetSocket(Ref<BsdSocket> socket) = 0;
	virtual void UnsetSocket() = 0;

	virtual void StoreAddr(macptr addr, macptr lenPtr) = 0;

	static int g_uniqueAddr;
};

class UnixSockAddr : public SockAddr
{
public:
	UnixSockAddr(u16 domain, std::string name) : m_domain(domain), m_name(name) {}

	Ref<BsdSocket> GetSocket();
	void SetSocket(Ref<BsdSocket> socket);
	void UnsetSocket();

	void StoreAddr(macptr addr, macptr lenPtr);

	u16 m_domain;
	std::string m_name;
};

class BsdSocket : public BsdFile
{
public:
	static Ref<BsdFile> NewSocket(u32 domain, u32 type, u32 protocol);
	static int NewSocketPair(u32 domain, u32 type, u32 protocol, macptr pair);

	static Ref<BsdSocket> GetFd(int fd);
	
	virtual int Connect(macptr addr, u32 addrLen) { throw ErrNo::OperationUnsupport; }
	virtual int Bind(macptr addr, u32 addrLen) { throw ErrNo::OperationUnsupport; }
	virtual int Listen(int backLog) { throw ErrNo::OperationUnsupport; }
	virtual int Accept(macptr addr, macptr lenPtr) { throw ErrNo::OperationUnsupport; }

	virtual void ConnectDirect(Ref<BsdSocket> socket) { throw ErrNo::OperationUnsupport; }
	
	virtual int Send(macptr buffer, u32 length, int flags) { throw ErrNo::OperationUnsupport; }
	virtual int SendMsg(macptr message, int flags) { throw ErrNo::OperationUnsupport; }
	virtual int SendTo(macptr buffer, u32 length, int flags,
		               macptr addr, u32 addrLen) { throw ErrNo::OperationUnsupport; }
	
	virtual int Recv(macptr buffer, u32 length, int flags) { throw ErrNo::OperationUnsupport; }
	virtual int RecvMsg(macptr message, int flags) { throw ErrNo::OperationUnsupport; }
	virtual int RecvFrom(macptr buffer, u32 length, int flags,
		                 macptr addr, macptr lenPtr) { throw ErrNo::OperationUnsupport; }

	FSMode GetType() { return FSMode::Socket; }

protected:
	BsdSocket(u32 domain, u32 type, u32 protocol);

	u32 m_domain, m_type, m_protocol;
	Ref<SockAddr> m_myAddr;
};
