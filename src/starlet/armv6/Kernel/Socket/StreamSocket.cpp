#include "StreamSocket.h"
#include "FdSpace.h"

int StreamSocket::Connect(macptr addr, u32 addrLen)
{
	Assert(m_target.IsNull());

	Ref<SockAddr> sockAddr = SockAddr::NewAddr(m_domain, addr, addrLen);

	m_target = sockAddr->GetSocket().Cast<StreamSocket>();
	if (m_target.IsNull())
		throw ErrNo::NoSuchFileDir;

	if (m_target->m_listen)
	{
		ConnectData newConnect;
		newConnect.addr = m_myAddr;
		newConnect.socket = this;

		m_target->AddConnection(newConnect);
		Wait();
	}
	else
		throw ErrNo::ConnectRefused;

	return 0;
}	

void StreamSocket::ConnectDirect(Ref<BsdSocket> socket)
{
	Ref<StreamSocket> strSocket = socket.Cast<StreamSocket>();
	Assert(strSocket.IsValid());

	m_target = strSocket;
	strSocket->m_target = this;
}

int StreamSocket::Bind(macptr addr, u32 addrLen)
{
	m_myAddr->UnsetSocket();
	m_myAddr = SockAddr::NewAddr(m_domain, addr, addrLen);
	m_myAddr->SetSocket(this);

	return 0;
}

int StreamSocket::Listen(int backLog)
{
	Assert(m_myAddr.IsValid());
	m_listen = true;
	return 0;
}

int StreamSocket::Accept(macptr addr, macptr lenPtr)
{
	Assert(m_listen);
	if (m_listenQue.empty())
	{
		if (m_flag.MustNotBlock())
			throw ErrNo::TryAgain;
		else
			Wait();
	}
	ConnectData connect = m_listenQue.back();
	m_listenQue.pop_back();

	connect.addr->StoreAddr(addr, lenPtr);
	
	Ref<StreamSocket> acceptSock = NewSocket(m_domain, m_type, m_protocol).Cast<StreamSocket>();
	int fd = g_task->fdSpace->NewFd(acceptSock.Cast<BsdFile>());

	Ref<StreamSocket> targetSock = connect.addr->GetSocket().Cast<StreamSocket>();
	Assert(targetSock.IsValid());

	acceptSock->m_target = targetSock;
	targetSock->m_target = acceptSock;

	targetSock->ResumeOne();
	return fd;
}


int StreamSocket::Read(macptr memBuffer, u32 size)
{
	Assert(m_target.IsValid());
	if (m_dataQue.empty())
	{
		if (m_flag.MustNotBlock())
			throw ErrNo::TryAgain;
		else
			Wait();
	}

	int actSize = min(size, m_dataQue.size());
	for (int i = 0; i < actSize; i++)
		Ram<u8>(memBuffer + i) = m_dataQue[i];
	m_dataQue.erase(m_dataQue.begin(), m_dataQue.begin() + actSize);

	return actSize;
}

int StreamSocket::Write(macptr memBuffer, u32 size)
{
	Warn("to socket: %s", RamString(memBuffer).c_str());
	return size;
	//
	Assert(m_target.IsValid());
	return m_target->AddData(memBuffer, size);
}

bool StreamSocket::CanRead()
{
	if (m_listen)
		return !m_listenQue.empty();
	else
		return !m_dataQue.empty();
}

void StreamSocket::AddConnection(ConnectData connect)
{
	Assert(m_listen);
	m_listenQue.push_back(connect);
	ResumeOne();
}

int StreamSocket::AddData(macptr memBuffer, u32 size)
{
	Assert(m_target.IsValid());
	
	for (u32 i = 0; i < size; i++)
		m_dataQue.push_back(Ram<u8>(memBuffer + i)); //bad?

	return size;
}