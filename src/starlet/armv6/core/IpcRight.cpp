#include "IpcRight.h"
#include "Task.h"
#include "MsgFlags.h"

// class IpcRight

bool IpcRight::Delete() 
{
	m_refs--; 
	if (m_refs == 0) 
	{
		delete this;
		return true;
	}
	return false;
}

void IpcRight::DeleteAll()
{
	m_refs = 0;
	delete this;
}

bool IpcRight::Mod(int delta) 
{
	if (m_refs + delta < 0)
		throw MachErr::InvalidValue; 
	m_refs += delta; 
	if (m_refs == 0) 
	{
		delete this;
		return true;
	}
	return false;
}

// class IpcReceiveRight : public IpcRightCanReceive

IpcMessage* IpcReceiveRight::Receive()
{ 
	Assert(CanReceive());
	IpcMessage* msg = m_port->Receive(); 
	msg->PrepareReceive(Name(), m_seqNo++);
	return msg;
}
	
IpcReceiveRight* IpcReceiveRight::New(IpcName* name, IpcPort* port)
{
	Assert(name->ReceiveRight() == NULL);
	return new IpcReceiveRight(name, port);
}

IpcReceiveRight* IpcReceiveRight::Transfer(IpcName* name, IpcReceiveRight* oldRight)
{
	if (name != oldRight->m_name)
	{
		name->SetReceiveRight(oldRight);

		oldRight->m_name->NullReceiveRight();
		oldRight->m_name = name;
	}
	oldRight->m_seqNo = oldRight->m_makeSend = 0;
	return oldRight;
}

IpcReceiveRight::~IpcReceiveRight()
{
	delete m_port;
}

IpcReceiveRight::IpcReceiveRight(IpcName* name, IpcPort* port) 
	: IpcRightCanReceive(name, port), m_makeSend(0), m_seqNo(0)
{
	name->SetReceiveRight(this);
	port->SetReceiveRight(this);
}

int IpcReceiveRight::CountPortSets()
{
	return m_port->m_portSets.size();
}
void IpcReceiveRight::RemoveFromPortSets()
{
	m_port->RemoveAllPortSets();
}

PortStatus IpcReceiveRight::GetPortStatus()
{
	PortStatus status;
	status.psetcount = m_port->m_portSets.size();
	status.seqno = m_seqNo;
	status.mscount = m_makeSend;
	status.qlimit = 1000; //whatever
	status.msgcount = m_port->MsgCount();
	status.soRights = m_port->m_onceRights.size();
	status.sRights = m_port->m_sendRights.size() ? 1 : 0; //??????? needs no-senders notification?
	status.pdReq = 0; //...
	status.nsReq = 0; //...
	status.flags = 0; //???
	return status;
}

// class IpcPortSet : public IpcRightCanReceive

IpcPortSet* IpcPortSet::New(IpcName* name) 
{ 
	Assert(name->PortSet() == NULL);
	return new IpcPortSet(name);
}

IpcPortSet::~IpcPortSet() {}

void IpcPortSet::AddPort(IpcReceiveRight* right) 
{ 
	m_rights.insert(right);
	right->Port()->AddPortSet(this);
	this->ResumeAll(); //the new port may contain messages to receive
}

void IpcPortSet::RemovePort(IpcReceiveRight* right) 
{ 
	m_rights.erase(right); 
	right->Port()->RemovePortSet(this);
}

bool IpcPortSet::HasPort(IpcReceiveRight* right)
{
	return m_rights.count(right) != 0;
}

void IpcPortSet::GetNames(u32* names)
{
	int i = 0;
	for (Iter it = m_rights.begin(); it != m_rights.end(); it++)
	{
		names[i++] = (*it)->Name()->Name();
	}
}

bool IpcPortSet::CanReceive()
{
	for (Iter i = m_rights.begin(); i != m_rights.end(); i++)
	{
		if ((*i)->CanReceive())
			return true;
	}
	return false;
}

IpcMessage* IpcPortSet::Receive()
{
	for (Iter i = m_rights.begin(); i != m_rights.end(); i++)
	{
		if ((*i)->CanReceive())
			return (*i)->Receive();
	}
	Assert(0);
}

IpcPortSet::IpcPortSet(IpcName* name) 
	: IpcRightCanReceive(name, NULL)
{
	name->SetPortSet(this);
}

// class IpcRightCanSend : public IpcRight

IpcRightCanSend* IpcRightCanSend::New(IpcName* name)
{
	if (name->DeadRight() != NULL)
	{
		name->DeadRight()->AddRef();
		return name->DeadRight();
	}
	else
		return new IpcRightCanSend(name);
}

IpcRightCanSend::IpcRightCanSend(IpcName* name)
	: IpcRight(name, NULL) 
{
	name->SetDeadRight(this);
}

void IpcRightCanSend::Send(IpcMessage* message) 
{	
	Assert(m_port);
	m_port->Send(message); 
	Name()->Delete();
}

// class IpcSendRight : public IpcRightCanSend

IpcSendRight* IpcSendRight::New(IpcName* name, IpcPort* port)
{
	if (name->SendRight() != NULL)
	{
		name->SendRight()->AddRef();
		return name->SendRight();
	}
	else
		return new IpcSendRight(name, port);
}

IpcSendRight::~IpcSendRight()
{
	if (m_port)
		m_port->RemoveSendRight(this);
}

IpcSendRight::IpcSendRight(IpcName* name, IpcPort* port)
	: IpcRightCanSend(name, port) 
{
	name->SetSendRight(this);
	if (port)
		port->AddSendRight(this);
}

// class IpcSendOnceRight : public IpcRightCanSend

IpcSendOnceRight* IpcSendOnceRight::New(IpcName* name, IpcPort* port)
{
	Assert(name->SendOnceRight() == NULL);
	return new IpcSendOnceRight(name, port);
}

IpcSendOnceRight::~IpcSendOnceRight()
{
	if (m_port)
		m_port->RemoveSendOnceRight(this);
}

IpcSendOnceRight::IpcSendOnceRight(IpcName* name, IpcPort* port) 
	: IpcRightCanSend(name, port) 
{
	name->SetSendOnceRight(this);
	if (port)
		port->AddSendOnceRight(this);
}