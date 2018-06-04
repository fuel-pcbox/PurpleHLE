#include "IpcMessage.h"
#include "IpcNameSpace.h"
#include "Memory.h"
#include "Task.h"
#include "VirtualMem.h"
#include "msgError.h"

// class IpcMessageHeader

void IpcMessageHeader::LoadFrom(macptr addr)
{
	this->remoteType = Ram<u8>(addr);
	this->localType = Ram<u8>(addr + 1);
	this->isComplex = (Ram<u16>(addr + 2) & 0x8000) ? true : false;
	this->destination = g_task->nameSpace->Name(Ram<u32>(addr + 8));
	this->localName = g_task->nameSpace->Name(Ram<u32>(addr + 12));
	this->id = Ram<u32>(addr + 20);
}

void IpcMessageHeader::PrepareSend()
{
	destination = remoteType.TransferToKernel(destination);
	localName = localType.TransferToKernel(localName);

	remoteType = remoteType.GetTransferredType();
	localType = localType.GetTransferredType();
}

void IpcMessageHeader::PrepareReceive(IpcNameSpace* dest, IpcName* recv)
{
	localName = localType.TransferTo(localName, dest);
	this->receiveName = recv;
}

void IpcMessageHeader::StoreTo(macptr addr)
{
	Ram<u8>(addr) = this->localType; //swapped
	Ram<u8>(addr + 1) = this->remoteType; //swapped
	Ram<u16>(addr + 2) = this->isComplex ? 0x8000 : 0;
	Ram<u32>(addr + 4) = this->size;
	Ram<u32>(addr + 8) = this->localName ? this->localName->Name() : 0; //swapped
	Ram<u32>(addr + 12) = this->receiveName->Name(); //swapped
	Ram<u32>(addr + 16) = 0; //what is this?
	Ram<u32>(addr + 20) = this->id;
}

// class IpcDescriptor

IpcDescriptor* IpcDescriptor::CreateFrom(macptr addr)
{
	u8 type = Ram<u8>(addr + 11);
	IpcDescriptor* desc;
	switch (type)
	{
	case 0:	desc = new IpcPortDescriptor(); break;
	case 1: desc = new IpcOolDescriptor(); break;
	case 2: desc = new IpcOolPortDescriptor(); break;
	}
	desc->LoadFrom(addr);
	return desc;
}

// class IpcPortDescriptor : public IpcDescriptor

void IpcPortDescriptor::LoadFrom(macptr addr)
{
	this->name = g_task->nameSpace->Name(Ram<u32>(addr));
	this->type = Ram<u8>(addr + 10);
}

void IpcPortDescriptor::PrepareSend()
{
	name = type.TransferToKernel(name);
	type = type.GetTransferredType();
}

void IpcPortDescriptor::PrepareReceive(IpcNameSpace* dest)
{
	name = type.TransferTo(name, dest);
}

void IpcPortDescriptor::StoreTo(macptr addr)
{
	Ram<u32>(addr) = this->name ? this->name->Name() : 0;
	Ram<u32>(addr + 4) = 0;
	Ram<u16>(addr + 8) = 0;
	Ram<u8>(addr + 10) = this->type;
	Ram<u8>(addr + 11) = 0;
}

// class IpcOolDescriptor : public IpcDescriptor

void IpcOolDescriptor::LoadFrom(macptr addr)
{
	// parameters
	this->alignment = Ram<u32>(addr + 0) & VirtualMem::PageMask;
	this->size = Ram<u32>(addr + 4);
	this->deallocate = Ram<u8>(addr + 8) ? true : false; //?
	this->copyMethod = Ram<u8>(addr + 9); //?

	// copy memory
	this->memory = new u8[size];
	u32 memAddr = Ram<u32>(addr);
	FromRamCpy(this->memory, memAddr, size);
	
	// optionally deallocate
	if (deallocate)
		g_task->vMem->Deallocate(memAddr, size);
}

void IpcOolDescriptor::StoreTo(macptr addr)
{
	u32 newAddr = 0;
	if (this->size > 0)
	{
		if (copyMethod == 0)
			newAddr = g_task->vMem->AnyAlloc(this->size);
		else if (copyMethod == 1)
			newAddr = g_task->vMem->AnyAlloc(this->size + this->alignment) + this->alignment;
		else
			Fail(-1);
	}
	ToRamCpy(newAddr, this->memory, this->size);

	Ram<u32>(addr + 0) = newAddr;
	Ram<u32>(addr + 4) = this->size;
	Ram<u8>(addr + 8) = this->deallocate; //?
	Ram<u8>(addr + 9) = this->copyMethod;
	Ram<u8>(addr + 10) = 0;
	Ram<u8>(addr + 11) = 1;
}

// class IpcOolPortDescriptor : public IpcDescriptor

void IpcOolPortDescriptor::LoadFrom(macptr addr)
{
	// load parameters
	this->alignment = Ram<u32>(addr + 0) & VirtualMem::PageMask;
	this->amount = Ram<u32>(addr + 4);
	this->deallocate = Ram<u8>(addr + 8) ? true : false; //?
	this->copyMethod = Ram<u8>(addr + 9); //?
	this->type = Ram<u8>(addr + 10);
	
	// load ports
	this->ports = new IpcName*[amount];
	u32 portsAddr = Ram<u32>(addr);
	for (u32 i = 0; i < amount; i++)
	{
		u32 name = Ram<u32>(portsAddr + i * sizeof(IpcName*));
		this->ports[i] = g_task->nameSpace->Name(name);
	}

	// optionally deallocate
	if (deallocate)
		g_task->vMem->Deallocate(portsAddr, amount * sizeof(IpcName*));
}

void IpcOolPortDescriptor::PrepareSend()
{
	// transfer ports to kernel
	for (u32 i = 0; i < amount; i++)
		ports[i] = type.TransferToKernel(ports[i]);
	type = type.GetTransferredType();	
}

void IpcOolPortDescriptor::PrepareReceive(IpcNameSpace* dest)
{
	// transfer ports to dest
	for (u32 i = 0; i < amount; i++)
		ports[i] = type.TransferTo(ports[i], dest);
}

void IpcOolPortDescriptor::StoreTo(macptr addr)
{
	u32 newAddr = 0;
	if (this->amount > 0)
	{
		if (copyMethod == 0)
			newAddr = g_task->vMem->AnyAlloc(this->amount * 4);
		else if (copyMethod == 1)
			newAddr = g_task->vMem->AnyAlloc(this->amount * 4 + this->alignment) + this->alignment;
		else
			Fail(-1);
	}

	for (u32 i = 0; i < amount; i++)
		Ram<u32>(newAddr + i*4) = this->ports[i]->Name();

	Ram<u32>(addr + 0) = newAddr;
	Ram<u32>(addr + 4) = this->amount;
	Ram<u8>(addr + 8) = this->deallocate; //?
	Ram<u8>(addr + 9) = this->copyMethod;
	Ram<u8>(addr + 10) = this->type;
	Ram<u8>(addr + 11) = 2;
}

// class IpcData

void IpcData::LoadFrom(macptr addr, u32 size)
{
	if (size > 0)
	{
		this->buf.resize(size);
		FromRamCpy(&buf[0], addr, size);
	}
}

void IpcData::StoreTo(macptr addr)
{
	if (buf.size() > 0)
		ToRamCpy(addr, &buf[0], buf.size());
}

// class IpcTrailer

u32 IpcTrailer::Size(int num)
{
	switch (num)
	{
	case 0: return 0x8;
	case 1: return 0x8 + 0x4; //seqno
	case 2: return 0x8 + 0x4 + 0x8; //security token
	case 3: return 0x8 + 0x4 + 0x8 + 0x20; //audit token
	default: Fail(num);
	}
}

void IpcTrailer::StoreTo(macptr addr, u32 num)
{
	Ram<u32>(addr) = 0; //the only trailer type
	Ram<u32>(addr + 4) = Size(num);
	switch (num) //with fallthroughs
	{
	case 3:
		for (int i = 0; i < 8; i++)
			Ram<u32>(addr + 0x14 + 0x4*i) = 0x22222222; //audit token
	case 2:
		Ram<u32>(addr + 0xc) = Ram<u32>(addr + 0x10) = 0x11111111; //security token
	case 1: 
		Ram<u32>(addr + 0x8) = sequence; //seqno
	case 0:
		return;
	default:
		Fail(num);
	}
}

void IpcTrailer::PrepareReceive(u32 sequence)
{
	this->sequence = sequence;
}

// class IpcMessage

void IpcMessage::LoadFrom(macptr addr, u32 size)
{
	header.LoadFrom(addr);
	addr += IpcMessageHeader::Size;
	u32 needSize = IpcMessageHeader::Size;

	if (header.isComplex)
	{
		u32 descAmt = Ram<u32>(addr);
		addr += 4;
		needSize += 4;

		for (u32 i = 0; i < descAmt; i++)
		{
			descs.push_back(IpcDescriptor::CreateFrom(addr));
			addr += IpcDescriptor::Size;
			needSize += IpcDescriptor::Size;
		}
	}

	MakeSure(size >= needSize);
	data.LoadFrom(addr, size - needSize);

	header.SetSize(size);
}

void IpcMessage::PrepareSend()
{
	header.PrepareSend();
	
	for (u32 i = 0; i < descs.size(); i++)
		descs[i]->PrepareSend();
}

void IpcMessage::PrepareReceive(IpcName* recv, u32 sequence)
{
	header.PrepareReceive(recv->Space(), recv);
	
	for (u32 i = 0; i < descs.size(); i++)
		descs[i]->PrepareReceive(recv->Space());
	
	trailer.PrepareReceive(sequence);
}

void IpcMessage::StoreTo(macptr addr, u32 limit, u32 trailerNum)
{
	Assert(limit >= header.size + IpcTrailer::Size(trailerNum));

	header.StoreTo(addr);
	addr += IpcMessageHeader::Size;

	if (header.isComplex)
	{
		Ram<u32>(addr) = descs.size();
		addr += 4;
		
		for (u32 i = 0; i < descs.size(); i++)
		{
			descs[i]->StoreTo(addr);
			addr += IpcDescriptor::Size;
		}
	}

	data.StoreTo(addr);
	addr += data.Size();

	trailer.StoreTo(addr, trailerNum);
}

IpcMessage::~IpcMessage()
{
	for (u32 i = 0; i < descs.size(); i++)
		delete descs[i];
}

void IpcMessage::Send(bool doTimeOut, s64 timeOut, bool mustSend)
{
	PrepareSend();

	IpcName* send = header.destination;
	if (!send)
		throw MsgError::SendInvalidDest;

	IpcRightCanSend* right = (send->SendRight() ? 
							  (IpcRightCanSend*)send->SendRight() : 
						      send->SendOnceRight());
	if (!right)
		throw MsgError::SendInvalidDest;

	if (right->CanSend())
		right->Send(this);
	else 
	{
		Waitable* sendWait = right->Port()->GetSendWait();
		Assert(sendWait);
		Assert(!mustSend);
		if (doTimeOut)
			sendWait->WaitTime(timeOut, MsgFlags::MessageSend);
		else
			sendWait->Wait(MsgFlags::MessageSend);
	}
}

IpcMessage* IpcMessage::Receive(IpcName* receive, bool doTimeOut, s64 timeOut)
{
	if (!receive)
		throw MsgError::ReceiveInvalidName;

	IpcRightCanReceive* right = (receive->ReceiveRight() ? 
								 (IpcRightCanReceive*)receive->ReceiveRight() : 
								 receive->PortSet());
	if (!right)
		throw MsgError::ReceiveInvalidName;

	if (right->CanReceive())
		return right->Receive();
	else if (doTimeOut)
		right->WaitTime(timeOut, MsgFlags::MessageReceive);
	else
		right->Wait(MsgFlags::MessageReceive);

	Fail(-1);
}

IpcKernMsg* IpcKernMsg::MakeReply(IpcMessage* msg, bool ndr)
{
	IpcMessage* reply = new IpcMessage();
	reply->header.remoteType = msg->header.localType;
	reply->header.localType = 0;
	reply->header.destination = msg->header.localName;
	reply->header.localName = 0;
	reply->header.id = msg->header.id + 100; //usually?

	return new IpcKernMsg(reply, ndr);
}

IpcKernMsg* IpcKernMsg::MakeMsg(IpcName* dest, MsgRight right, u32 id, bool ndr)
{
	Assert(dest);
	IpcMessage* msg = new IpcMessage();
	msg->header.remoteType = right;
	msg->header.localType = 0;
	msg->header.destination = dest;
	msg->header.localName = 0;
	msg->header.id = id;
	
	return new IpcKernMsg(msg, ndr);
}

void IpcKernMsg::SetReplyRight(IpcName* dest, MsgRight right)
{
	m_msg->header.localType = right;
	m_msg->header.localName = dest;
}

void IpcKernMsg::Send()
{
	//compute message size
	m_msg->header.size = IpcMessageHeader::Size + m_msg->data.Size();
	m_msg->header.isComplex = m_msg->descs.size() ? true : false;
	if (m_msg->header.isComplex)
		m_msg->header.size += sizeof(u32) + IpcDescriptor::Size * m_msg->descs.size();

	m_msg->Send(true, 0, true); //must send?
}

NDR TypicalNDR = {0, 0, 0, 0, 1, 0, 0, 0};