#include "IpcPort.h"
#include "IpcName.h"
#include "IpcNameSpace.h"
#include "MachErr.h"
#include "MsgError.h"

// class IpcPort

IpcName* IpcPort::GetSendReceiveNameAt(IpcNameSpace* space)
{ 
	if (m_sendRights[space])
		return m_sendRights[space]->Name();
	else if (m_receiveRight->Name()->Space() == space)
		return m_receiveRight->Name();
	else
		return NULL;
}

IpcPort::~IpcPort()
{
	while (!m_sendRights.empty())
	{
		IpcSendRight* right = m_sendRights.begin()->second;
		if (right)
			right->MakeDead();
		m_sendRights.erase(m_sendRights.begin());
	}

	while (!m_onceRights.empty())
		(*m_onceRights.begin())->MakeDead();

	RemoveAllPortSets();
}

void IpcPort::SetReceiveRight(IpcReceiveRight* right)
{
	m_receiveRight = right;	
}
void IpcPort::AddSendRight(IpcSendRight* right)
{
	m_sendRights[right->Name()->Space()] = right;
}
void IpcPort::AddSendOnceRight(IpcSendOnceRight* right)
{
	m_onceRights.insert(right);
}
void IpcPort::AddPortSet(IpcPortSet* set)
{
	m_portSets.insert(set);
}
void IpcPort::RemoveSendRight(IpcSendRight* right)
{
	m_sendRights[right->Name()->Space()] = NULL;
}
void IpcPort::RemoveSendOnceRight(IpcSendOnceRight* right)
{
	m_onceRights.erase(right);
}
void IpcPort::RemovePortSet(IpcPortSet* set)
{
	m_portSets.erase(set);
}

void IpcPort::RemoveAllPortSets()
{
	while (!m_portSets.empty())
		(*m_portSets.begin())->RemovePort(m_receiveRight);
}

// class IpcUserPort : public IpcPort

void IpcUserPort::ResumeAllReceives()
{
	m_receiveRight->ResumeAll();
	for (std::set<IpcPortSet*>::iterator it = m_portSets.begin(); it != m_portSets.end(); it++)
		(*it)->ResumeAll();
}

// class IpcKernelPort : public IpcPort

void IpcKernelPort::Send(IpcMessage* rawMsg)
{
	try
	{
		IpcKernMsg* message = new IpcKernMsg(rawMsg, UsesNdr());
		IpcKernMsg* reply = IpcKernMsg::MakeReply(rawMsg, UsesNdr());

		bool doReply = true; // throw MachErr::whatever indicates that you want a reply

		try
		{
			doReply = Process(message, reply);
		}
		catch (MachErr::Type& retCode)
		{
			Warn("Warn IPC : id = %d, err = %d", message->ID(), retCode);
			OUTPUT(u32, retCode);
		}
		catch (StopThread&)
		{
			reply->Send(); //still want to send
			throw;
		}

		if (doReply)
			reply->Send();
		else
			delete reply;
	}
	catch (MsgError::Type& err)
	{
		Fail(err);
	}
}