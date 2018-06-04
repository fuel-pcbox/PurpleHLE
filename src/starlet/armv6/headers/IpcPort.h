#pragma once
#include "types.h"
#include <deque>
#include <set>
#include <map>
#include "IpcMessage.h"
#include "Waitable.h"

class IpcName;
class IpcNameSpace;

class IpcPort
{
public:
	friend class IpcRightCanReceive;
	friend class IpcRightCanSend;
	
	friend class IpcReceiveRight;
	friend class IpcPortSet;

	friend class IpcSendRight;
	friend class IpcSendOnceRight;

	IpcName* GetSendReceiveNameAt(IpcNameSpace* space);
	IpcReceiveRight* GetReceiveRight() { return m_receiveRight; }

	void RemoveAllPortSets();
	
	virtual ~IpcPort();

	virtual Waitable* GetSendWait() { return NULL; }

protected:
	IpcPort() : m_receiveRight(NULL), m_limit(UINT_MAX) {}

	virtual void Send(IpcMessage* message) = 0;
	virtual IpcMessage* Receive() = 0;
	virtual u32 MsgCount() = 0;
	
	void SetReceiveRight(IpcReceiveRight* right);
	void AddSendRight(IpcSendRight* right);
	void AddSendOnceRight(IpcSendOnceRight* right);
	void AddPortSet(IpcPortSet* set);
	void RemoveSendRight(IpcSendRight* right);
	void RemoveSendOnceRight(IpcSendOnceRight* right);
	void RemovePortSet(IpcPortSet* set);

	u32 MsgLimit() { return m_limit; }
	void SetMsgLimit(u32 val) { m_limit = val; }

	IpcReceiveRight* m_receiveRight;
	std::map<IpcNameSpace*, IpcSendRight*> m_sendRights; 
	std::set<IpcSendOnceRight*> m_onceRights;
	std::set<IpcPortSet*> m_portSets;
	u32 m_limit;
};

class IpcUserPort : public IpcPort
{
public:
	IpcUserPort() {}

	void Send(IpcMessage* message)
	{
		Assert(m_messages.size() < m_limit);
		m_messages.push_back(message);

		ResumeAllReceives();
	}
	IpcMessage* Receive()
	{
		Assert(!m_messages.empty());
		IpcMessage* msg = m_messages.front();
		m_messages.pop_front();

		m_sendWait.ResumeAll();
		return msg;
	}
	u32 MsgCount()
	{
		return m_messages.size();
	}

	Waitable* GetSendWait() { return &m_sendWait; }
	void ResumeAllReceives();

private:
	std::deque<IpcMessage*> m_messages;
	Waitable m_sendWait;
};

class IpcKernelPort : public IpcPort
{
public:
	void Send(IpcMessage* message);

	IpcMessage* Receive() { Fail(-1); }
	u32 MsgCount() { return 0; }

	virtual bool Process(IpcKernMsg* message, IpcKernMsg* reply) = 0;
	virtual bool UsesNdr() { return true; } 
};

//extra:

class UnknownPort : public IpcKernelPort
{
public:
	bool Process(IpcKernMsg* message, IpcKernMsg* reply) 
	{ 
		Fail(message->ID());
	}
};