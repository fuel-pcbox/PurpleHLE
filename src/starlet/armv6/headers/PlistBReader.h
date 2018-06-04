#pragma once
#include "PlistNode.h"

namespace Plist
{

class BReader
{
public:
	BReader(const std::vector<u8>& data);
	Node* GetTop() { return m_topNode; }

protected:
	void ReadHeader();
	void ReadFooter();
	void ReadOffTable();
	Node* ReadNode(int nodeAddr);
	u64 ReadInt(u8 len, int nodeAddr);
	int GetDataAddr(u8 len, int nodeAddr);

	//Big-Endian
	u8 Read8(int addr) { return m_data[addr]; }
	u16 Read16(int addr) { return (Read8(addr) << 8) | Read8(addr + 1); }
	u32 Read32(int addr) { return (Read16(addr) << 16) | Read16(addr + 2); }
	u64 Read64(int addr) { return ((u64)Read32(addr) << 32) | (u64)Read32(addr + 4); }
	u64 Read(int addr, int size);

	int m_offSize;
	int m_refSize;
	int m_objNum;
	int m_objTop;
	int m_offTableAddr;
	std::vector<u32> m_offTable;

	Node* m_topNode;
	const std::vector<u8>& m_data;
};

}