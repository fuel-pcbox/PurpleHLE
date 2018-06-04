#pragma once
#include "BsdSocket.h"
#include "Devices.h"
#include "FdSpace.h"
#include "Memory.h"
#include <sstream>

#include "StreamSocket.h"
#include "DgramSocket.h"

Ref<SockAddr> SockAddr::NewAddr(u32 domain, macptr addr, u32 addrLen)
{
	if (!addr || addrLen < 2 || Ram<u16>(addr) == 0)
		return NULL;

	//MakeSure(Ram<u16>(addr) == domain);
	//if (domain == 1)
		return new UnixSockAddr(domain, RamString(addr + 0x2, addrLen - 0x2));
	//else
	//	Fail(-1);
}

Ref<SockAddr> SockAddr::NewAddr(u32 domain)
{
	std::stringstream uniqueName;
	uniqueName << "/dev/unnamedSocketNumber" << (g_uniqueAddr++);

	//if (domain == 1)
		return new UnixSockAddr(domain, uniqueName.str().c_str());
	//else
	//	Fail(-1);
}

Ref<BsdSocket> UnixSockAddr::GetSocket()
{
	return g_devices->GetDevice(m_name).Cast<BsdSocket>();
}
	
void UnixSockAddr::SetSocket(Ref<BsdSocket> socket)
{
	g_devices->AddDevice(m_name, socket.Cast<BsdFile>());
}
	
void UnixSockAddr::UnsetSocket()
{
	g_devices->RemoveDevice(m_name);
}

void UnixSockAddr::StoreAddr(macptr addr, macptr lenPtr)
{
	u32 curLen = Ram<u32>(lenPtr);
	Assert(curLen > 2);
	Ram<u16>(addr) = m_domain;

	int strLen = min(m_name.size(), curLen - 2);
	for (int i = 0; i < strLen; i++)
		Ram<u8>(addr + i + 2) = m_name[i];

	Ram<u32>(lenPtr) = 2 + strLen;
}

int SockAddr::g_uniqueAddr = 0;

Ref<BsdFile> BsdSocket::NewSocket(u32 domain, u32 type, u32 protocol)
{
	if (type == 1)
		return new StreamSocket(domain, type, protocol);
	else if (type == 2)
		return new DgramSocket(domain, type, protocol);
	else 
		return new StreamSocket(domain, type, protocol); //hack
}

int BsdSocket::NewSocketPair(u32 domain, u32 type, u32 protocol, macptr pair)
{
	int fd_1 = g_task->fdSpace->NewFd(NewSocket(domain, type, protocol));
	int fd_2 = g_task->fdSpace->NewFd(NewSocket(domain, type, protocol));

	Ref<BsdSocket> sock_1 = BsdSocket::GetFd(fd_1);
	Ref<BsdSocket> sock_2 = BsdSocket::GetFd(fd_2);

	sock_1->ConnectDirect(sock_2);
	Ram<u32>(pair) = fd_1;
	Ram<u32>(pair + 0x4) = fd_2;
	return 0;
}

Ref<BsdSocket> BsdSocket::GetFd(int fd)
{
	Ref<BsdSocket> socket = g_task->fdSpace->GetFd(fd).Cast<BsdSocket>();
	if (socket.IsNull())
		throw ErrNo::NotSocket;
	return socket;
}

BsdSocket::BsdSocket(u32 domain, u32 type, u32 protocol) 
	: m_domain(domain), m_type(type), m_protocol(protocol)
{
	m_myAddr = SockAddr::NewAddr(m_domain);
	m_myAddr->SetSocket(this);
}