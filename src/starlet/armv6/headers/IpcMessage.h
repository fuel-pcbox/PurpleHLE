#pragma once
#include "MsgRight.h"
#include <vector>

class IpcName;
class IpcRight;

struct IpcMessageHeader
{
	MsgRight remoteType;
	MsgRight localType;
	bool isComplex; 
	u32 size;
	IpcName* destination;
	IpcName* receiveName;
	IpcName* localName;
	u32 id;

	static const int Size = 6 * sizeof(u32);

	void LoadFrom(macptr addr); //doesn't set size yet
	void SetSize(u32 size) { this->size = size; }
	void PrepareSend();
	void PrepareReceive(IpcNameSpace* dest, IpcName* recv);
	void StoreTo(macptr addr);
};

struct IpcPortDescriptor;
struct IpcOolDescriptor;
struct IpcOolPortDescriptor;

struct IpcDescriptor
{
	static const int Size = 3 * sizeof(u32);

	static IpcDescriptor* CreateFrom(macptr addr);
	virtual void LoadFrom(macptr addr) = 0;
	virtual void PrepareSend() = 0;
	virtual void PrepareReceive(IpcNameSpace* dest) = 0;
	virtual void StoreTo(macptr addr) = 0;
};

struct IpcPortDescriptor : public IpcDescriptor
{
	IpcName* name;
	MsgRight type;

	void LoadFrom(macptr addr);
	void PrepareSend();
	void PrepareReceive(IpcNameSpace* dest);
	void StoreTo(macptr addr);
	
	IpcPortDescriptor(IpcName* name, MsgRight type) 
		: name(name), type(type) {}
	IpcPortDescriptor() {}
};
struct IpcOolDescriptor : public IpcDescriptor
{
	u8* memory;
	u32 size;
	bool deallocate; //??
	u32 copyMethod; //--
	u32 alignment;

	void LoadFrom(macptr addr);
	void PrepareSend() {}
	void PrepareReceive(IpcNameSpace* dest) {}
	void StoreTo(macptr addr);
	
	IpcOolDescriptor() : memory(NULL) {}
	IpcOolDescriptor(void* memory, u32 size) 
		: memory((u8 *)memory), size(size), deallocate(false), copyMethod(0) {}
	~IpcOolDescriptor() { delete[] memory; }
};
struct IpcOolPortDescriptor : public IpcDescriptor
{
	IpcName** ports;
	u32 amount;
	bool deallocate; //??
	u32 copyMethod;
	MsgRight type;
	u32 alignment;

	void LoadFrom(macptr addr);
	void PrepareSend();
	void PrepareReceive(IpcNameSpace* dest);
	void StoreTo(macptr addr);

	IpcOolPortDescriptor() : ports(NULL) {}
	IpcOolPortDescriptor(IpcName** ports, u32 amount, MsgRight type) 
		: ports(ports), amount(amount), type(type), deallocate(false), copyMethod(0) {}
	~IpcOolPortDescriptor() { delete[] ports; }
};

struct IpcData
{
	std::vector<u8> buf;

	void LoadFrom(macptr addr, u32 size);
	void StoreTo(macptr addr);
	u32 Size() { return (buf.size() + 3) &~ 3; } //round up

	IpcData() {}
};

struct IpcTrailer
{
	u32 sequence;

	static u32 Size(int num);

	void PrepareReceive(u32 sequence);
	void StoreTo(macptr addr, u32 num);
};

struct IpcMessage
{
private:
	IpcMessageHeader header;
	std::vector<IpcDescriptor*> descs;
	IpcData data;	
	IpcTrailer trailer;
	
	void PrepareSend(); //called by Send()
	void PrepareReceive(IpcName* recv, u32 sequence); //called by IpcRight::Receive

public:
	void LoadFrom(macptr addr, u32 size);
	void StoreTo(macptr addr, u32 limit, u32 trailerNum);

	void Send(bool doTimeOut, s64 timeOut, bool mustSend = false);
	static IpcMessage* Receive(IpcName* receive, bool doTimeOut, s64 timeOut);

	~IpcMessage();

	friend class IpcKernMsg;
	friend class IpcReceiveRight;
};

class IpcKernMsg //interface for simpler send/receive
{
public:
	template <typename T>
	T* _Get(u32 count);
	template <typename T>
	T* _GetDesc();
	
	template <typename T>
	void _Add(T* source, u32 count);
	template <typename T>
	void _AddDesc(T* source);

	void SetReplyRight(IpcName* dest, MsgRight right);

	static IpcKernMsg* MakeReply(IpcMessage* message, bool ndr);
	static IpcKernMsg* MakeMsg(IpcName* dest, MsgRight right, u32 id, bool ndr);

	void Send();

	u32 ID() { return m_msg->header.id; }
	
	IpcKernMsg(IpcMessage* msg, bool ndr) : m_msg(msg), m_dataIndex(0), m_portIndex(0), m_ndr(ndr) {}

private:
	IpcMessage* m_msg;

	u32 m_dataIndex;
	u32 m_portIndex;

	bool m_ndr;
};


// some templates, etc.

STRICT_ALIGN struct NDR 
{
	u8 migVer, ifVer, reserved1, migEnc, intRep, charRep, floatRep, reserved2;
} POP_ALIGN;
extern NDR TypicalNDR;

template <typename T>
T* IpcKernMsg::_Get(u32 count)
{
	IpcData* data = &m_msg->data;

	u32 size = count * sizeof(T);
	if (!size) return NULL;
	if (m_dataIndex == 0 && m_ndr)
	{
		MakeSure(data->buf.size() >= sizeof(NDR));
		MakeSure(memcmp(&data->buf[0], &TypicalNDR, sizeof(NDR)) == 0);
		m_dataIndex += sizeof(NDR);		
	}
	MakeSure(data->buf.size() >= m_dataIndex + size);
	T* result = (T *)&data->buf[m_dataIndex];
	m_dataIndex += size;
	return result;
}

template <typename T>
void IpcKernMsg::_Add(T* source, u32 count)
{
	IpcData* data = &m_msg->data;

	u32 size = count * sizeof(T);
	if (!size) return;
	if (m_dataIndex == 0 && m_ndr)
	{
		data->buf.resize(sizeof(NDR));
		memcpy(&data->buf[0], &TypicalNDR, sizeof(NDR));
		m_dataIndex += sizeof(NDR);		
	}
	data->buf.resize(data->buf.size() + size);
	memcpy(&data->buf[m_dataIndex], source, size);
	m_dataIndex += size;
}

template <typename T>
T* IpcKernMsg::_GetDesc()
{
	MakeSure(m_portIndex < m_msg->descs.size() && m_msg->descs[m_portIndex]);
	T* desc = dynamic_cast<T*>(m_msg->descs[m_portIndex]);
	MakeSure(desc);
	m_portIndex++;
	return desc;
}

template <typename T>
void IpcKernMsg::_AddDesc(T* source)
{
	m_msg->descs.push_back(source);
	m_portIndex++;
}

template <typename T> class arr
{
public:
	arr(T* arr, u32 count) { m_arr = new T[count]; memcpy(m_arr, arr, sizeof(T) * count); }
	~arr() { delete[] m_arr; }
	operator T*() { return m_arr; }
private:
	T* m_arr;
};

#define OUTPUT(type, val) MACRO( \
	type result = val;\
	reply->_Add(&result, 1); )

#define OUTPUT_STRUCT(strct, ...) MACRO( \
	STRICT_ALIGN struct { strct; } results = { __VA_ARGS__ };\
	reply->_Add(&results, 1); POP_ALIGN )

#define OUTPUT_ARRAY(type, val, count) MACRO( \
	type* arr = val;  \
	reply->_Add(arr, count); )

#define OUTPUT_DESC(type, ...) \
	reply->_AddDesc(new Ipc ## type ## Descriptor( __VA_ARGS__ ))

#define OUTPUT_PORT(...) OUTPUT_DESC(Port, __VA_ARGS__)
#define OUTPUT_OOL(...) OUTPUT_DESC(Ool, __VA_ARGS__)
#define OUTPUT_OOLPORT(...) OUTPUT_DESC(OolPort, __VA_ARGS__)


#define INPUT(type, name) \
	type name = *message->_Get<type>(1)

#define INPUT_STRUCT(strct, name) \
	STRICT_ALIGN struct SS ## name { strct; } name = *message->_Get<struct SS ## name>(1) POP_ALIGN

// does not create new memory
#define INPUT_ARRAY(type, name, count) \
	arr<type> name(message->_Get<type>(count), count)

#define INPUT_STRING(name, count) \
	std::string name = MakeZStr(message->_Get<char>(count), count)

#define INPUT_DESC(type, name) \
	Ipc ## type ## Descriptor name = *message->_GetDesc<Ipc ## type ## Descriptor>()

#define INPUT_PORT(...) INPUT_DESC(Port, __VA_ARGS__)
#define INPUT_OOL(...) INPUT_DESC(Ool, __VA_ARGS__)
#define INPUT_OOLPORT(...) INPUT_DESC(OolPort, __VA_ARGS__)


