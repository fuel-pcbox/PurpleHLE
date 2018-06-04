#include "MachoReader.h"
#include "CpuState.h"
#include "Memory.h"
#include "File.h"
#include "Task.h"
#include "DebugSymbols.h"
#include "BsdFile.h"
#include "Application.h"

using namespace Interpreter;

namespace MachO
{

	
MachO::Reader* Reader::MakeReader(std::string fname)
{
	File file;
	int size;
	file.Open(App::FsStr() + fname, File::ReadOnly, File::OpenAlways);
	size = (int)file.GetSize();
	
	std::vector<u8> exeVec(size);
	file.Read(&exeVec.front(), size);
	file.Close();

	return new MachO::Reader(fname, exeVec);
}

Reader::Reader(std::string fname, const std::vector<u8>& data)
	: m_data(data)
{
	ReadHeader();
	ReadCommands();
	AllocateMem(); //has to be done here

	DebugSymbols::AddSymbolsFrom(fname, m_textSegment->m_vaddr);
}

void Reader::AllocateMem() 
{
	for (u32 segi = 0; segi < m_segmentVec.size(); segi++)
	{
		Segment* seg = m_segmentVec[segi];

		g_task->vMem->Allocate(seg->m_vaddr, seg->m_vsize, VMFlags::Fixed);
		g_task->vMem->Protect(seg->m_vaddr, seg->m_vsize, false, seg->m_protect);
		g_task->vMem->Protect(seg->m_vaddr, seg->m_vsize, true, seg->m_protectMax);

		u32 i;
		for (i = 0; i < seg->m_fileSize; i++)
			Ram<u8>(seg->m_vaddr + i) = m_data[seg->m_fileOff + i];
		//rest is already 0 (through allocation)
	}
}

void Reader::CopyState()
{
	for (int i = 0; i < 15; i++)
		cpu.gpr[i] = m_regs[i];
	cpu.nia = m_regs[15];
	cpu.nf = BITS(m_regs[16], 31, 1);
	cpu.zf = BITS(m_regs[16], 30, 1);
	cpu.cf = BITS(m_regs[16], 29, 1);
	cpu.vf = BITS(m_regs[16], 28, 1);
	cpu.qf = BITS(m_regs[16], 27, 1);
	cpu.ge = BITS(m_regs[16], 16, 4);
	cpu.thumb = BITS(m_regs[16], 5, 1);
}

u32 Reader::PushStringToStack(u32 sp, std::string& str)
{
	u32 size = ROUNDUP(str.size() + 1, 4);
	for (u32 i = 0; i < str.size(); i++)
	{
		Ram<u8>(sp + i - size) = str[i];
	}
	//rest is 0/NULL (promised)
	return sp - size;
}

u32 Reader::PushU32VectorToStack(u32 sp, std::vector<u32>& vec)
{
	u32 size = (vec.size() + 1) * 4;
	for (u32 i = 0; i < vec.size(); i++)
	{
		Ram<u32>(sp - size + 4*i) = vec[i];
	}
	//rest is 0/NULL (promised)
	return sp - size;
}

void Reader::CreateStack(std::string& execPath, std::vector<std::string>& arg, std::vector<std::string>& envp)
{
	if (!m_stack)
		return;

	g_task->vMem->Allocate(StackTop - StackSize, StackSize, VMFlags::Fixed);
	cpu.sp = StackTop;

	std::vector<u32> envp_ptr(envp.size()); //ptrs to envps
	for (int i = envp.size() - 1; i >= 0; i--) //for all envps backwards
	{
		envp_ptr[i] = cpu.sp = PushStringToStack(cpu.sp, envp[i]); //push string and store its address
	}
	
	std::vector<u32> arg_ptr(arg.size()); //ptrs to args
	for (int i = arg.size() - 1; i >= 0; i--) //for all args backwards
	{
		arg_ptr[i] = cpu.sp = PushStringToStack(cpu.sp, arg[i]); //push string and store its address
	}

	std::vector<u32> exec_ptr(1);
	exec_ptr[0] = cpu.sp = PushStringToStack(cpu.sp, execPath); 

	cpu.sp = PushU32VectorToStack(cpu.sp, exec_ptr);
	cpu.sp = PushU32VectorToStack(cpu.sp, envp_ptr);
	cpu.sp = PushU32VectorToStack(cpu.sp, arg_ptr);

	Ram<u32>(cpu.sp - 4) = arg.size();
	cpu.sp -= 4;

	//for dyld
	MakeSure(m_textSegment->m_fileOff == 0);
	Ram<u32>(cpu.sp - 4) = m_textSegment->m_vaddr;
	cpu.sp -= 4;
}

void Reader::ReadHeader()
{
	u32 magic = Read32(m_headAddr);
	if (magic != 0xfeedface) Fail(magic);
	m_cpuType = Read32(m_headAddr + 0x4);
	m_cpuSubType = Read32(m_headAddr + 0x8);
	m_fileType = Read32(m_headAddr + 0xc);
	m_cmdNum = Read32(m_headAddr + 0x10);
	m_cmdTotalSize = Read32(m_headAddr + 0x14);
	m_flags = Read32(m_headAddr + 0x18);
}

void Reader::ReadCommands()
{
	u32 cmdAddr = m_cmdsAddr;
	for (u32 i = 0; i < m_cmdNum; i++)
	{
		u32 command = Read32(cmdAddr);
		u32 size = Read32(cmdAddr + 0x4);
		switch (command)
		{
		case 0x1: //segment
			ReadSegment(cmdAddr + 0x8); 
			break;

		case 0x4: //thread
			m_stack = false;
			ReadThread(cmdAddr + 0x8); 
			break;

		case 0x5: //unix thread
			m_stack = true;
			ReadThread(cmdAddr + 0x8); 
			break;

		case 0xe: //load dylinker
			ReadDylinker(cmdAddr + 0x8);
			break;
		}
		cmdAddr += size;
	}

	m_endHeader = cmdAddr;
}

void Reader::ReadThread(int cmdAddr)
{
	int type = Read32(cmdAddr);
	if (type != 1) Fail(type);
	int size = Read32(cmdAddr + 0x4);
	if (size != RegCount) Fail(size);
	
	for (int i = 0; i < RegCount; i++)
		m_regs[i] = Read32(cmdAddr + 0x8 + i*4);
}
	
void Reader::ReadDylinker(int cmdAddr)
{
	int offset = Read32(cmdAddr);
	std::string dylinker = Read8Str(cmdAddr + offset - 8);

	m_dylinker = MakeReader(dylinker); 
	m_dylinker->CopyState(); //this begins the state!
}

void Reader::ReadSegment(int cmdAddr)
{
	Segment* segment = new Segment();
	segment->m_name = Read8Str(cmdAddr, 0x10);
	segment->m_vaddr = Read32(cmdAddr + 0x10);
	segment->m_vsize = Read32(cmdAddr + 0x14);
	segment->m_fileOff = Read32(cmdAddr + 0x18);
	segment->m_fileSize = Read32(cmdAddr + 0x1c);
	segment->m_protectMax = Read32(cmdAddr + 0x20);
	segment->m_protect = Read32(cmdAddr + 0x24);
	segment->m_sectNum = Read32(cmdAddr + 0x28);
	segment->m_flags = Read32(cmdAddr + 0x2c);

	if (segment->m_name == "__TEXT")
		m_textSegment = segment;

	m_segmentVec.push_back(segment);
}

char* Reader::Read8Str(int addr, int amount)
{
	char* mem = new char[amount + 1];
	if (!mem) Fail(amount);

	int i;
	for (i = 0; i < amount; i++)
		mem[i] = Read8(addr + i);
	mem[i] = '\0';

	return mem;
}

char* Reader::Read8Str(int addr) //unsafe (what isn't in this class, though...)
{
	int len = 0;
	while (Read8(addr + len) != 0) len++;

	return Read8Str(addr, len);
}

}