#pragma once
#include "types.h"
#include <queue>
#include <list>


struct MachMessageHeader
{
	u32 m_bits;
	u32 m_size;
	u32 m_remote_right;
	u32 m_local_right;
	u32 m_sequence; //?
	u32 m_id;
};

struct Ndr
{
	Ndr(u8 migVer, u8 ifVer, u8 r1, u8 migEnc, u8 intRep, u8 charRep, u8 floatRep, u8 r2)
		: migVer(migVer), ifVer(ifVer), reserved1(r1), migEnc(migEnc), intRep(intRep), charRep(charRep), floatRep(floatRep), reserved2(r2)
	{}

	static Ndr g_typical;
	void ForceTypical() 
	{ 
		if (memcmp(this, &g_typical, sizeof(Ndr)) != 0)
			Fail(0); //or do something about it....
	}

	u8 migVer;
	u8 ifVer;
	u8 reserved1;
	u8 migEnc;
	u8 intRep;
	u8 charRep;
	u8 floatRep;
	u8 reserved2;
};

struct Descriptor
{
	u32 data;
	u32 count;
	u8 dealloc;
	u8 copyType;
	u8 portType;
	u8 descType;
protected:
	Descriptor(u32 data, u32 count, u8 dealloc, u8 copy, u8 port, u8 desc)
		: data(data), count(count), dealloc(dealloc), copyType(copy), portType(port), descType(desc)
	{}
};

struct PortDescriptor : public Descriptor
{
	PortDescriptor(u32 portName, u8 portType) 
		: Descriptor(portName, 0, 0, 0, portType, 0) {}
};
struct OutlineDescriptor : public Descriptor
{
	OutlineDescriptor(u32 address, u32 size, u8 dealloc, u8 copyType) 
		: Descriptor(address, size, dealloc, copyType, 0, 1) {}
};
struct OutlinePortDescriptor : public Descriptor
{
	OutlinePortDescriptor(u32 address, u32 count, u8 dealloc, u8 copyType, u8 portType) 
		: Descriptor(address, count, dealloc, copyType, portType, 2) {}
};

/*
struct BodyType -- what is this?
{
	BodyType(u32 type, u32 size, u32 length, u32 doInline, u32 longForm, u32 dealloc, u32 unused)
		: type(type), size(size), length(length), doInline(doInline), longForm(longForm), dealloc(dealloc), unused(unused)
	{}

	static BodyType g_sendPort;

	u32 type : 8;
	u32 size : 8;
	u32 length : 12;
	u32 doInline :1;
	u32 longForm :1;
	u32 dealloc :1;
	u32 unused :1;
};*/

class MachMessage
{
public:
	MachMessage(u32 addr, u32 size);
	MachMessage();

	~MachMessage() { delete[] m_data; }

	static MachMessage* Reply(MachMessage* origMsg, int dataSize, bool complex = false);

	void CopyTo(u32 addr, u32 limit);

	template <typename T>
	T& Data(u32 dataAddr) 
	{ 
		assert(m_dataSize >= dataAddr + sizeof(T));
		return *(T *)(m_data + dataAddr);
	}

	MachMessageHeader m_header;
	u8* m_data;
	u32 m_dataSize;
};

class MachPortRight;

class MachPort
{
public:
	MachPort(int initRef) : m_refs(initRef) 
	{}

	void IncRef() { m_refs++; }
	void DecRef() { m_refs--; if (!m_refs) delete this; }

	virtual void PutMessage(MachMessage* message) 
	{
		m_messages.push(message); 
	}
	MachMessage* TakeMessage();
	bool CanTakeMessage() { return !m_messages.empty(); }

	MachPortRight* GetSendRight();
	MachPortRight* GetRecvRight();

private:
	std::queue<MachMessage*> m_messages;
	int m_refs;
};

class KernelPort : public MachPort
{
public:
	KernelPort() : MachPort(1)
	{}

	virtual void PutMessage(MachMessage* message)
	{
		ProcessMessage(message);
	}
	virtual void ProcessMessage(MachMessage* message) = 0;
};

class MachPortRightOrSet
{
public:
	virtual MachMessage* Receive() = 0;
};

class MachPortRight : public MachPortRightOrSet
{
public:
	MachPortRight(MachPort* port)
		: m_port(port)
	{
		port->IncRef();
	}
	~MachPortRight() { m_port->DecRef(); }

	void Send(MachMessage* message)
	{
		//if (!m_send) Fail(0);
		m_port->PutMessage(message);
	}
	MachMessage* Receive()
	{
		//if (!m_receive) Fail(0);
		if (!m_port->CanTakeMessage())
			Fail(0);
		MachMessage* mess =  m_port->TakeMessage();
		u32 t = mess->m_header.m_local_right;
		mess->m_header.m_local_right = mess->m_header.m_remote_right;
		mess->m_header.m_remote_right = t;
		return mess;
	}

//protected: //debug
	MachPort* m_port;
};

class MachPortSet : public MachPortRightOrSet
{
public:
	MachPortSet() {}

	void AddMember(MachPortRight* right) { m_rights.push_back(right); }

	MachMessage* Receive()
	{
		for (std::list<MachPortRight*>::iterator i = m_rights.begin(); i != m_rights.end(); i++)
		{
			if ((*i)->m_port->CanTakeMessage())
				return (*i)->Receive();
		}
		return NULL;
	}

	std::list<MachPortRight*> m_rights;
};