#include "Port.h"
#include "Memory.h"

Ndr Ndr::g_typical(0, 0, 0, 0, 1, 0, 0, 0);

#define HEADER_SIZE 0x18

MachMessage::MachMessage()
{
	m_data = NULL;
	memset(&m_header, 0, sizeof(m_header));
}

MachMessage::MachMessage(u32 addr, u32 size)
{
	assert((addr & 3) == 0); //aligned

	m_header.m_bits = Ram<u32>(addr + 0x0);
	m_header.m_size = Ram<u32>(addr + 0x4);
	m_header.m_remote_right = Ram<u32>(addr + 0x8);
	m_header.m_local_right = Ram<u32>(addr + 0xc);
	m_header.m_sequence = Ram<u32>(addr + 0x10);
	m_header.m_id = Ram<u32>(addr + 0x14);

	m_dataSize = size - HEADER_SIZE;
	m_data = new u8[m_dataSize];

	for (u32 i = 0; i < m_dataSize; i++)
		m_data[i] = Ram<u8>(addr + HEADER_SIZE + i);
}

MachMessage* MachMessage::Reply(MachMessage* origMsg, int dataSize, bool complex)
{
	MachMessage* reply = new MachMessage();
	memcpy(&reply->m_header, &origMsg->m_header, sizeof(MachMessageHeader));

	reply->m_header.m_id += 100; //usually

	u32 temp_right = reply->m_header.m_remote_right;
	reply->m_header.m_remote_right = reply->m_header.m_local_right;
	reply->m_header.m_local_right = temp_right;

	reply->m_header.m_size = HEADER_SIZE + dataSize;

	if (complex)
		reply->m_header.m_bits |= 0x80000000;

	reply->m_dataSize = dataSize;
	reply->m_data = new u8[dataSize];
	return reply;
}

void MachMessage::CopyTo(u32 addr, u32 limit)
{
	if (m_dataSize + HEADER_SIZE > limit)
		Fail(0); //fail usually? but not always?

	Ram<u32>(addr + 0x0) = m_header.m_bits;
	Ram<u32>(addr + 0x4) = m_header.m_size;
	Ram<u32>(addr + 0x8) = m_header.m_remote_right;
	Ram<u32>(addr + 0xc) = m_header.m_local_right;
	Ram<u32>(addr + 0x10) = m_header.m_sequence;
	Ram<u32>(addr + 0x14) = m_header.m_id;

	for (u32 i = 0; i < m_dataSize; i++)
		Ram<u8>(addr + HEADER_SIZE + i) = m_data[i];
}

MachMessage* MachPort::TakeMessage()
{ 
	assert(!m_messages.empty()); //else switch thread and wait
	MachMessage* msg = m_messages.front();
	m_messages.pop();
	return msg;
}

MachPortRight* MachPort::GetSendRight()
{
	return new MachPortRight(this);
}
MachPortRight* MachPort::GetRecvRight()
{
	return new MachPortRight(this); //expand MachPortRight
}