#pragma once
#include "types.h"
#include "IpcPort.h"
#include "IpcName.h"
#include <set>
#include "Waitable.h"

struct PortStatus
{
	u32 psetcount; u32 seqno; u32 mscount; u32 qlimit; u32 msgcount; 
	u32 soRights; u32 sRights; u32 pdReq; u32 nsReq; u32 flags;
};

class IpcRight
{
public:
	IpcPort* Port() { return m_port; }
	IpcName* Name() { return m_name; }

	friend class IpcName;

protected:
	IpcRight(IpcName* name, IpcPort* port) : m_name(name), m_port(port), m_refs(1) {}
	virtual ~IpcRight() {}

	bool Delete();
	void DeleteAll();

	bool Mod(int delta);
	int GetRef() { return m_refs; }

	void AddRef() { m_refs++; MakeSure(m_refs != 0); }

	IpcName* m_name;
	IpcPort* m_port;
	int m_refs;
};

class IpcRightCanReceive : public IpcRight, public Waitable
{
public:
	virtual IpcMessage* Receive() = 0;
	virtual bool CanReceive() = 0;

protected:
	IpcRightCanReceive(IpcName* name, IpcPort* port) : IpcRight(name, port) {}
};

class IpcReceiveRight : public IpcRightCanReceive
{
public:
	void IncMakeSend() { m_makeSend++; }
	void IncSequence() { m_seqNo++; }

	IpcMessage* Receive();
	bool CanReceive() { return m_port->MsgCount() > 0; }

	static IpcReceiveRight* New(IpcName* name, IpcPort* port);
	static IpcReceiveRight* Transfer(IpcName* name, IpcReceiveRight* oldRight);

	PortStatus GetPortStatus();
	int CountPortSets();
	void RemoveFromPortSets();
	u32 MsgLimit() { return m_port->MsgLimit(); }
	void SetMsgLimit(u32 val) { m_port->SetMsgLimit(val); }

protected:
	virtual ~IpcReceiveRight();
	IpcReceiveRight(IpcName* name, IpcPort* port);

	u32 m_makeSend;
	u32 m_seqNo;
};

class IpcPortSet : public IpcRightCanReceive
{
public:
	static IpcPortSet* New(IpcName* name);

	void AddPort(IpcReceiveRight* right);
	void RemovePort(IpcReceiveRight* right);
	bool HasPort(IpcReceiveRight* right);

	IpcMessage* Receive();
	bool CanReceive();

	int GetNamesCount() { return m_rights.size(); }
	void GetNames(u32* names);

protected:
	IpcPortSet(IpcName* name);
	virtual ~IpcPortSet();

	typedef std::set<IpcReceiveRight*>::iterator Iter;
	std::set<IpcReceiveRight*> m_rights;
};

class IpcRightCanSend : public IpcRight
{
public:
	void MakeDead() { m_port = NULL; m_name->MakeDead(); } //removed from m_port afterwards
	bool IsDead() { return m_port == NULL; }

	bool CanSend() { Assert(m_port); return m_port->MsgCount() < m_port->MsgLimit(); } 
	void Send(IpcMessage* message);

	static IpcRightCanSend* New(IpcName* name);

protected:
	IpcRightCanSend(IpcName* name, IpcPort* port) : IpcRight(name, port) {}
	IpcRightCanSend(IpcName* name);
};

class IpcSendRight : public IpcRightCanSend
{
public:
	static IpcSendRight* New(IpcName* name, IpcPort* port);

protected:
	virtual ~IpcSendRight();
	IpcSendRight(IpcName* name, IpcPort* port);
};

class IpcSendOnceRight : public IpcRightCanSend
{
public:
	static IpcSendOnceRight* New(IpcName* name, IpcPort* port);

protected:
	virtual ~IpcSendOnceRight();
	IpcSendOnceRight(IpcName* name, IpcPort* port);
};