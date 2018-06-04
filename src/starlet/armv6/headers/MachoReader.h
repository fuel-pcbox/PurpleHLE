#pragma once
#include "types.h"
#include "CpuState.h"
#include "MemBlock.h"

#include <vector>
#include <string>

namespace MachO
{

struct Segment
{
	std::string m_name;
	u32 m_vaddr;
	u32 m_vsize;
	u32 m_fileOff;
	u32 m_fileSize;
	u32 m_protectMax;
	u32 m_protect;
	u32 m_sectNum;
	u32 m_flags;
};

class Reader
{
public:
	Reader(std::string fname, const std::vector<u8>& data);
	static MachO::Reader* MakeReader(std::string fname);

	void CopyState();
	void CreateStack(std::string& execPath, std::vector<std::string>& args, std::vector<std::string>& envps);
	
	static const int StackSize = 512 * 1024; //what size? 
	static const int StackTop = 0x30000000;

protected:
	void AllocateMem();

	void ReadHeader();
	void ReadCommands();
	void ReadSegment(int cmdAddr);
	void ReadThread(int cmdAddr);
	void ReadDylinker(int cmdAddr);
	
	u32 PushStringToStack(u32 sp, std::string& str);
	u32 PushU32VectorToStack(u32 sp, std::vector<u32>& vec);

	//Little-Endian
	u8 Read8(int addr) { return m_data[addr]; }
	u16 Read16(int addr) { return Read8(addr) | (Read8(addr + 1) << 8); }
	u32 Read32(int addr) { return Read16(addr) | (Read16(addr + 2) << 16); }
	u64 Read64(int addr) { return (u64)Read32(addr) | ((u64)Read32(addr + 4) << 32); }
	char* Read8Str(int addr, int amount);
	char* Read8Str(int addr);

	static const int m_headAddr = 0;
	static const int m_cmdsAddr = 0x1c;

	u32 m_cpuType;
	u32 m_cpuSubType;
	u32 m_fileType;
	u32 m_cmdNum;
	u32 m_cmdTotalSize;
	u32 m_flags;
	bool m_stack;

	u32 m_symTabOff;
	u32 m_symTabNum;
	u32 m_strTabOff;
	u32 m_strTabSize;

	u32 m_endHeader; //???
	u32 m_stackBase;

	typedef struct{ u32 address, index; u8 type; } RelInfo;
	typedef struct{ u32 index; u8 type; } IndInfo;

	std::vector<Segment*> m_segmentVec; //zero-based unlike the spec
	Segment* m_textSegment;

	static const int RegCount = 17;
	u32 m_regs[RegCount];

	const std::vector<u8>& m_data; ///CHAAAAAAAAAANGE (must not be accessed after constructor!!!!)

	MachO::Reader* m_dylinker;
};

}