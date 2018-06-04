#include "PlistBReader.h"

namespace Plist
{

BReader::BReader(const std::vector<u8>& data)
: m_data(data)
{
	ReadHeader();
	ReadFooter();
	ReadOffTable();
	m_topNode = ReadNode(m_offTable[m_objTop]);
}

void BReader::ReadHeader()
{
	if (Read8(0) != 'b' || Read8(1) != 'p' ||
		Read8(2) != 'l' || Read8(3) != 'i' ||
		Read8(4) != 's' || Read8(5) != 't' ||
		Read8(6) != '0' || Read8(7) != '0')
		Fail(0);
}

void BReader::ReadFooter()
{
	int footAddr = m_data.size() - 32;

	m_offSize = Read8(footAddr + 6);
	if (m_offSize >= 3) Fail(m_offSize);
	m_refSize = Read8(footAddr + 7);
	if (m_refSize >= 3) Fail(m_refSize);
	m_objNum = Read32(footAddr + 12);
	m_objTop = Read32(footAddr + 20);
	m_offTableAddr = Read32(footAddr + 28);
}

void BReader::ReadOffTable()
{
	int offAddr = m_offTableAddr;
	for (int i = 0; i < m_objNum; i++)
	{
		u32 offset = (u32)Read(offAddr, m_offSize);
		m_offTable.push_back(offset);
		offAddr += m_offSize;
	}
}

Node* BReader::ReadNode(int nodeAddr) //stack unsafe...
{
	u8 type = Read8(nodeAddr);
	u8 len = type & 0xf;
	u8 typehigh = type >> 4;
	switch (typehigh)
	{
	case 0: //null, bool
		if (len == 0)
			return new NullNode();
		else if (len == 8)
			return new BoolNode(false);
		else if (len == 9)
			return new BoolNode(true);
		else
			Fail(type);

	case 1: //int
		return new IntNode(Read(nodeAddr + 1, (1 << len)));
		
	case 2: //real
		if (len == 2) //float
			return new RealNode(As<float,u32>(Read32(nodeAddr + 1)));
		else if (len == 3) //double
			return new RealNode(As<double,u64>(Read64(nodeAddr + 1)));
		else
			Fail(type);
		
	case 3: //date
		if (len == 3)
			return new DateNode(As<double,u64>(Read64(nodeAddr + 1)));
		else
			Fail(type);

	case 4: //data
		{
			u32 size = (u32)ReadInt(len, nodeAddr + 1);
			int dataAddr = GetDataAddr(len, nodeAddr + 1);

			ptr data = new u8[size];
			if (!data) Fail(0);
			for (u32 i = 0; i < size; i++)
				data[i] = Read8(dataAddr + i);

			return new DataNode(data, size);
		}

	case 5: //ascii string
		{
			u32 size = (u32)ReadInt(len, nodeAddr + 1);
			int strAddr = GetDataAddr(len, nodeAddr + 1);

			std::wstring wstr(size, '\0');
			for (u32 i = 0; i < size; i++)
				wstr[i] = Read8(strAddr + i);

			return new StringNode(wstr);
		}
		
	case 6: //wide string
		{
			u32 size = (u32)ReadInt(len, nodeAddr + 1) / 2;
			int strAddr = GetDataAddr(len, nodeAddr + 1);

			std::wstring wstr(size, '\0');
			for (u32 i = 0; i < size; i++)
				wstr[i] = Read8(strAddr + i*2);

			return new StringNode(wstr);
		}

	case 8: //uid
		return new UidNode(Read(nodeAddr + 1, len + 1));

	case 10: //array
	case 12: //set
		{
			u32 size = (u32)ReadInt(len, nodeAddr + 1);
			int vecAddr = GetDataAddr(len, nodeAddr + 1);

			std::vector<Node*> *vec = new std::vector<Node*>(size);
			if (!vec) Fail(0);
			for (u32 i = 0; i < size; i++)
			{
				u32 ref = (u32)Read(vecAddr + i*m_refSize, m_refSize);
				(*vec)[i] = ReadNode(m_offTable[ref]);
			}

			if (typehigh == 10) //array
				return new ArrayNode(vec);
			else //set
				return new SetNode(vec);
		}
		
	case 13: //dict
		{
			u32 size = (u32)ReadInt(len, nodeAddr + 1);
			int mapAddr = GetDataAddr(len, nodeAddr + 1);

			std::vector<Node*> *vec2 = new std::vector<Node*>(size*2);
			if (!vec2) Fail(0);
			for (u32 i = 0; i < size; i++)
			{
				u32 keyRef = (u32)Read(mapAddr + i*m_refSize, m_refSize);
				u32 objRef = (u32)Read(mapAddr + (i + size)*m_refSize, m_refSize);
				(*vec2)[i*2] = ReadNode(m_offTable[keyRef]);
				(*vec2)[i*2 + 1] = ReadNode(m_offTable[objRef]);
			}

			return new DictNode(vec2);
		}
		
	default:
		Fail(type);
	}
}

u64 BReader::ReadInt(u8 len, int nodeAddr)
{
	if (len != 15)
		return len;
	else
	{
		IntNode* node = dynamic_cast<IntNode*>(ReadNode(nodeAddr));
		if (!node) Fail(0);
		u64 value = node->Value();
		delete node;
		return value;
	}
}
	
int BReader::GetDataAddr(u8 len, int nodeAddr)
{
	if (len != 15)
		return nodeAddr;
	else
	{
		u8 type = Read8(nodeAddr);
		if ((type >> 4) != 1)
			Fail(type);
		return nodeAddr + 1 + (1 << (type & 0xf));
	}
}

u64 BReader::Read(int addr, int size)
{
	if (size == 1)
		return Read8(addr);
	else if (size == 2)
		return Read16(addr);
	else if (size == 4)
		return Read32(addr);
	else if (size == 8)
		return Read64(addr);
	else
		Fail(size);
}

}