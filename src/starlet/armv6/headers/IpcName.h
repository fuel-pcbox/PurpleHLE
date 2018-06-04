#pragma once
#include "types.h"

class IpcRight;
class IpcSendRight;
class IpcReceiveRight;
class IpcSendOnceRight;
class IpcPortSet;
class IpcNameSpace;
class IpcRightCanSend;

class IpcPort;

class IpcName
{
public:

	// special meaning : if name = 0, allocate name
	// special meaning : if space = 0, use current space
	static IpcName* NewReceive(IpcPort* newPort, IpcNameSpace* space = NULL, u32 name = 0); 
	static IpcName* NewPortSet(IpcNameSpace* space = NULL, u32 name = 0);
	static IpcName* NewDeadName(IpcNameSpace* space = NULL, u32 name = 0);
	static IpcName* MoveReceive(IpcName* oldName, IpcNameSpace* space = NULL, u32 name = 0);
	static IpcName* MakeSend(IpcName* oldName, IpcNameSpace* space = NULL, u32 name = 0);
	static IpcName* MoveSend(IpcName* oldName, IpcNameSpace* space = NULL, u32 name = 0);
	static IpcName* CopySend(IpcName* oldName, IpcNameSpace* space = NULL, u32 name = 0);
	static IpcName* MakeSendOnce(IpcName* oldName, IpcNameSpace* space = NULL, u32 name = 0);
	static IpcName* MoveSendOnce(IpcName* oldName, IpcNameSpace* space = NULL, u32 name = 0);

	//GetName creates new port in kernel if it doesn't exist.
	//GetName it returns a receive right (for make send)
	static IpcName* GetName(IpcPort* kernelPort); 
	static IpcName* GetSendName(IpcPort* kernelPort); 

	u32 Name() { return m_name; }
	IpcNameSpace* Space() { return m_nameSpace; }

	IpcSendRight* SendRight()         { return m_sendRight; }
	IpcReceiveRight* ReceiveRight()   { return m_recvRight; }
	IpcSendOnceRight* SendOnceRight() { return m_sendOnceRight; }
	IpcRightCanSend* DeadRight()         { return m_deadRight; }
	IpcPortSet* PortSet()             { return m_portSet; }
	
	//bad
	void SetSendRight(IpcSendRight* right)         { m_sendRight = right; }
	void SetReceiveRight(IpcReceiveRight* right)   { m_recvRight = right; }
	void SetSendOnceRight(IpcSendOnceRight* right) { m_sendOnceRight = right; }
	void SetDeadRight(IpcRightCanSend* right)   { m_deadRight = right; }
	void SetPortSet(IpcPortSet* set)               { m_portSet = set; }

	void ModSendRight(int delta);
	void ModSendOnceRight(int delta);
	void ModReceiveRight(int delta);
	void ModDeadRight(int delta);
	void ModPortSet(int delta);

	void DeleteSendRight() { ModSendRight(-1); }
	void DeleteSendOnceRight() { ModSendOnceRight(-1); }
	void DeleteReceiveRight() { ModReceiveRight(-1); }
	void DeleteDeadRight() { ModDeadRight(-1); }
	void DeletePortSet() { ModPortSet(-1); }
	void NullReceiveRight() { m_recvRight = NULL; }

	u32 GetTypeMask();

	IpcSendOnceRight* ReplaceNotifyDead(IpcSendOnceRight* right)
	{
		IpcSendOnceRight* retval = m_notifyDead;
		m_notifyDead = right;
		return retval;
	}

	void Delete();
	void DeleteAll();
	void MakeDead();

	friend class IpcNameSpace;

protected:
	static IpcName* GetNewName(IpcNameSpace* space, u32 name);
	static IpcName* GetSendOnceName(IpcRight* right, IpcNameSpace* space, u32 name);
	static IpcName* GetSendReceiveName(IpcRight* right, IpcNameSpace* space, u32 name);

	IpcName(IpcNameSpace* space, u32 name);
	void DeleteIfNeeded(bool isSendOnce);

	u32 m_name;
	IpcNameSpace* m_nameSpace;

	IpcReceiveRight* m_recvRight;
	IpcSendRight* m_sendRight;
	IpcSendOnceRight* m_sendOnceRight;
	IpcPortSet* m_portSet;
	IpcRightCanSend* m_deadRight;

	IpcSendOnceRight* m_notifyDead;
};

class IpcNotify
{
	static const int DELETED = 65;
	static const int DESTROYED = 69;
	static const int NO_SENDERS = 70;
	static const int SEND_ONCE = 71;
	static const int DEAD_NAME = 72;

public:
	static void NotifySendOnce(IpcSendOnceRight* notify);
	static void NotifyDeadName(IpcSendOnceRight* notify, u32 name);
	static void NotifyDeleted(IpcSendOnceRight* notify, u32 name);
};