#include "DgramSocket.h"
#include "FdSpace.h"

bool DgramSocket::CanRead()
{
	return !m_dgramQue.empty();
}

int DgramSocket::Connect(macptr addr, u32 addrLen)
{
	Ref<SockAddr> sockAddr = SockAddr::NewAddr(m_domain, addr, addrLen);

	if (sockAddr == NULL) //remove connection
	{
		m_target = NULL;
		throw ErrNo::AFUnsupport;
	}

	Ref<DgramSocket> targetSocket = sockAddr->GetSocket().Cast<DgramSocket>();
	if (targetSocket.IsNull())
		throw ErrNo::NoSuchFileDir;

	m_target = targetSocket;
	return 0;
}

void DgramSocket::ConnectDirect(Ref<BsdSocket> socket)
{
	Ref<DgramSocket> dSocket = socket.Cast<DgramSocket>();
	Assert(dSocket.IsValid());

	m_target = dSocket;
	dSocket->m_target = this;
}

int DgramSocket::Bind(macptr addr, u32 addrLen)
{
	m_myAddr->UnsetSocket();
	m_myAddr = SockAddr::NewAddr(m_domain, addr, addrLen);
	m_myAddr->SetSocket(this);

	return 0;
}

int DgramSocket::DoSend(macptr buffer, u32 length, int flags, Ref<DgramSocket> target)
{	
	Assert(flags == 0);

	Dgram dgram(length);
	dgram.addr = m_myAddr;
	for (u32 i = 0; i < length; i++)
		dgram.data[i] = Ram<u8>(buffer + i);

	Assert(target.IsValid());
	Assert(target->m_target.IsNull() || target->m_target == this);
	target->m_dgramQue.push_back(dgram);

	return length;
}

int DgramSocket::Send(macptr buffer, u32 length, int flags)
{
	Assert(m_target.IsValid());
	return DoSend(buffer, length, flags, m_target);
}
int DgramSocket::SendTo(macptr buffer, u32 length, int flags, macptr addr, u32 addrLen)
{
	Ref<DgramSocket> target = SockAddr::NewAddr(m_domain, addr, addrLen)->GetSocket().Cast<DgramSocket>();
	return DoSend(buffer, length, flags, target);
}

int DgramSocket::DoReceive(macptr buffer, u32 length, int flags, macptr addr, macptr lenPtr)
{
	Assert(flags == 0);

	if (m_dgramQue.empty())
	{
		if (m_flag.MustNotBlock())
			throw ErrNo::TryAgain;
		else
			Wait();
	}

	Dgram dgram = m_dgramQue.front();
	m_dgramQue.pop_front();

	if (addr)
		dgram.addr->StoreAddr(addr, lenPtr);

	int actSize = min(length, dgram.data.Size());
	Assert(actSize == length && actSize == dgram.data.Size()); //..
	for (int i = 0; i < actSize; i++)
		Ram<u8>(buffer + i) = dgram.data[i];

	return actSize;
}
 
int DgramSocket::Recv(macptr buffer, u32 length, int flags)
{
	return DoReceive(buffer, length, flags, NULL, NULL);
}
int DgramSocket::RecvFrom(macptr buffer, u32 length, int flags, macptr addr, macptr lenPtr)
{
	return DoReceive(buffer, length, flags, addr, lenPtr);
}


