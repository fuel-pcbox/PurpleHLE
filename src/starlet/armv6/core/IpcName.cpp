#include "IpcName.h"
#include "IpcNameSpace.h"
#include "IpcRight.h"
#include "Task.h"

IpcName::IpcName(IpcNameSpace* space, u32 name)
{
	m_name = name;
	m_nameSpace = space;
	m_sendRight = NULL;
	m_recvRight = NULL;
	m_sendOnceRight = NULL;
	m_portSet = NULL;
	m_deadRight = NULL;

	m_notifyDead = NULL;
}

void IpcName::DeleteAll()
{
	bool isSendOnce = m_sendOnceRight != NULL;
	if (m_sendRight) { m_sendRight->DeleteAll(); m_sendRight = NULL; }
	if (m_sendOnceRight) { m_sendOnceRight->DeleteAll(); m_sendOnceRight = NULL; }
	if (m_portSet) { m_portSet->DeleteAll(); m_portSet = NULL; }
	if (m_deadRight) { m_deadRight->DeleteAll(); m_deadRight = NULL; }
	if (m_recvRight) { m_recvRight->DeleteAll(); m_recvRight = NULL; }  //last one, to prevent pointless dead names
	DeleteIfNeeded(isSendOnce); //needed
}

void IpcName::Delete()
{
	bool isSendOnce = m_sendOnceRight != NULL;
	if (m_sendRight) { if (m_sendRight->Delete()) m_sendRight = NULL; }
	if (m_sendOnceRight) { if (m_sendOnceRight->Delete()) m_sendOnceRight = NULL; }
	if (m_deadRight) { if (m_deadRight->Delete()) m_deadRight = NULL; }
	DeleteIfNeeded(isSendOnce); 
}

void IpcName::DeleteIfNeeded(bool isSendOnce)
{
	if (m_sendRight == NULL && m_recvRight == NULL && m_sendOnceRight == NULL && 
		m_portSet == NULL && m_deadRight == NULL)
	{
		if (m_notifyDead) 
		{
			if (isSendOnce)
				IpcNotify::NotifySendOnce(m_notifyDead);
			else
				IpcNotify::NotifyDeleted(m_notifyDead, Name());
		}

		m_nameSpace->DeleteName(m_name); //will delete this (ugh. worst design ever)
	}
}

void IpcName::MakeDead()
{
	if (m_sendRight) { m_deadRight = m_sendRight; m_sendRight = NULL; }
	else if (m_sendOnceRight) { m_deadRight = m_sendOnceRight; m_sendOnceRight = NULL; }

	if (m_notifyDead)
		IpcNotify::NotifyDeadName(m_notifyDead, Name()); 
}

IpcName* IpcName::GetSendReceiveName(IpcRight* right, IpcNameSpace* space, u32 name)
{
	MakeSure(right != NULL);
	if (!space) space = g_task->nameSpace;
	if (!name) 
	{
		IpcName* alreadyName = right->Port()->GetSendReceiveNameAt(space);
		if (alreadyName)
			return alreadyName;
		else
			name = space->GetVacantName();
	}
	MakeSure(right->Port()->GetSendReceiveNameAt(space) == space->Name(name));

	return space->NewOrGetName(name);
}

IpcName* IpcName::GetSendOnceName(IpcRight* right, IpcNameSpace* space, u32 name)
{
	MakeSure(right != NULL);
	if (!space) space = g_task->nameSpace;
	if (!name) name = space->GetVacantName();
	MakeSure(space->Name(name) == NULL); 

	return space->NewName(name);
}

IpcName* IpcName::GetNewName(IpcNameSpace* space, u32 name)
{
	if (!space) space = g_task->nameSpace;
	if (!name) name = space->GetVacantName();
	MakeSure(space->Name(name) == NULL); 

	return space->NewName(name);
}

IpcName* IpcName::NewReceive(IpcPort* newPort, IpcNameSpace* space, u32 name)
{
	IpcName* ipcName = GetNewName(space, name);
	IpcReceiveRight::New(ipcName, newPort);
	return ipcName;
}
IpcName* IpcName::NewPortSet(IpcNameSpace* space, u32 name)
{
	IpcName* ipcName = GetNewName(space, name);
	IpcPortSet::New(ipcName);
	return ipcName;
}
IpcName* IpcName::NewDeadName(IpcNameSpace* space, u32 name)
{
	IpcName* ipcName = GetNewName(space, name);
	IpcRightCanSend::New(ipcName);
	return ipcName;
}

IpcName* IpcName::MoveReceive(IpcName* oldName, IpcNameSpace* space, u32 name)
{
	if (!oldName) return NULL;
	IpcName* ipcName = GetSendReceiveName(oldName->ReceiveRight(), space, name);
	IpcReceiveRight::Transfer(ipcName, oldName->ReceiveRight());
	return ipcName;
}
IpcName* IpcName::MakeSend(IpcName* oldName, IpcNameSpace* space, u32 name)
{
	if (!oldName) return NULL;
	IpcName* ipcName = GetSendReceiveName(oldName->ReceiveRight(), space, name);
	IpcSendRight::New(ipcName, oldName->ReceiveRight()->Port());
	oldName->ReceiveRight()->IncMakeSend();
	return ipcName;
}
IpcName* IpcName::MoveSend(IpcName* oldName, IpcNameSpace* space, u32 name)
{
	if (!oldName) return NULL;
	IpcName* ipcName = GetSendReceiveName(oldName->SendRight(), space, name);
	IpcSendRight::New(ipcName, oldName->SendRight()->Port());
	oldName->DeleteSendRight();
	return ipcName;
}
IpcName* IpcName::CopySend(IpcName* oldName, IpcNameSpace* space, u32 name)
{
	if (!oldName) return NULL;
	IpcName* ipcName = GetSendReceiveName(oldName->SendRight(), space, name);
	IpcSendRight::New(ipcName, oldName->SendRight()->Port());
	return ipcName;
}

IpcName* IpcName::MoveSendOnce(IpcName* oldName, IpcNameSpace* space, u32 name)
{
	if (!oldName) return NULL;
	IpcName* ipcName = GetSendOnceName(oldName->SendOnceRight(), space, name);
	IpcSendOnceRight::New(ipcName, oldName->SendOnceRight()->Port());
	oldName->DeleteSendOnceRight();
	return ipcName;
}
IpcName* IpcName::MakeSendOnce(IpcName* oldName, IpcNameSpace* space, u32 name)
{
	if (!oldName) return NULL;
	IpcName* ipcName = GetSendOnceName(oldName->ReceiveRight(), space, name);
	IpcSendOnceRight::New(ipcName, oldName->ReceiveRight()->Port());
	oldName->ReceiveRight()->IncMakeSend();
	return ipcName;
}

IpcName* IpcName::GetName(IpcPort* kernelPort) //if new, will create it in the kernel
{
	IpcNameSpace* kernel = IpcNameSpace::KernelIpcSpace();
	IpcName* ipcName;
	if (kernelPort->GetReceiveRight())
		ipcName = kernelPort->GetReceiveRight()->Name(); // Get the receive port
	else
		ipcName = IpcName::NewReceive(kernelPort, kernel, 0); // make receive port in kernel
	return ipcName;
}

IpcName* IpcName::GetSendName(IpcPort* kernelPort)  //in the kernel
{
	IpcNameSpace* kernel = IpcNameSpace::KernelIpcSpace();
	IpcName* ipcName = GetName(kernelPort);
	if (!ipcName->SendRight())
		ipcName = IpcName::MakeSend(ipcName, kernel, 0);
	return ipcName;
}

void IpcName::ModSendRight(int delta) { if (m_sendRight->Mod(delta)) m_sendRight = NULL; DeleteIfNeeded(false); }
void IpcName::ModSendOnceRight(int delta) { if (m_sendOnceRight->Mod(delta)) m_sendOnceRight = NULL; DeleteIfNeeded(true); }
void IpcName::ModReceiveRight(int delta) { if (m_recvRight->Mod(delta)) m_recvRight = NULL; DeleteIfNeeded(false); }
void IpcName::ModDeadRight(int delta) { if (m_deadRight->Mod(delta)) m_deadRight = NULL; DeleteIfNeeded(false); }
void IpcName::ModPortSet(int delta) { if (m_portSet->Mod(delta)) m_portSet = NULL; DeleteIfNeeded(false); }

u32 IpcName::GetTypeMask()
{
	u32 mask = 0;
	if (SendRight())     mask |= 1 << (16 + RightType::Send);
	if (ReceiveRight())  mask |= 1 << (16 + RightType::Receive);
	if (SendOnceRight()) mask |= 1 << (16 + RightType::SendOnce);
	if (PortSet())       mask |= 1 << (16 + RightType::PortSet);
	if (DeadRight())     mask |= 1 << (16 + RightType::DeadName);
	if (DeadRight())     mask |= 1 << (16 + RightType::DeadName);
	if (m_notifyDead)    mask |= 0x80000000;
	return mask;
}

//move this to IpcNameSpace.cpp
IpcNameSpace* IpcNameSpace::kernelIpcSpace = NULL;


void IpcNotify::NotifySendOnce(IpcSendOnceRight* notify)
{
	IpcKernMsg* reply = IpcKernMsg::MakeMsg(notify->Name(), MsgRight::MoveSendOnce, SEND_ONCE, true);
	reply->Send();
}

void IpcNotify::NotifyDeleted(IpcSendOnceRight* notify, u32 name)
{
	IpcKernMsg* reply = IpcKernMsg::MakeMsg(notify->Name(), MsgRight::MoveSendOnce, DELETED, true);
	OUTPUT(u32, name);
	reply->Send();
}

void IpcNotify::NotifyDeadName(IpcSendOnceRight* notify, u32 name)
{
	IpcKernMsg* reply = IpcKernMsg::MakeMsg(notify->Name(), MsgRight::MoveSendOnce, DEAD_NAME, true);
	OUTPUT(u32, name);
	reply->Send();
}
