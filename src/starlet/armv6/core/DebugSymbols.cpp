#include "DebugSymbols.h"
#include "Memory.h"
#include "BsdRegFile.h"
#include "MachOHLE.h"

namespace MachO
{

std::map<u32, Symbol*> DebugSymbols::m_symbolAddrMap; 
std::map<std::string, Symbol*> DebugSymbols::m_symbolStrMap;
std::set<std::string> DebugSymbols::m_fileSet; 

std::string DebugSymbols::GetSymbolAt(u32 addr)
{
	SymbolMapIter iter = m_symbolAddrMap.find(addr);
	if (iter == m_symbolAddrMap.end())
		return "";
	else
		return iter->second->name + " (" + iter->second->filename + ")";
}

u32 DebugSymbols::GetAddrOf(std::string name)
{
	StrSymMapIter iter = m_symbolStrMap.find(name);
	if (iter == m_symbolStrMap.end())
		return 0;
	else
		return iter->second->position;
}

std::vector<std::string> DebugSymbols::GetFileList()
{
	std::vector<std::string> vec;
	for (FileSetIter i = m_fileSet.begin(); i != m_fileSet.end(); i++)
		vec.push_back(*i);
	return vec;
}

std::vector<std::string> DebugSymbols::GetSymbolList(std::string file)
{
	std::vector<std::string> vec;
	for (SymbolMapIter i = m_symbolAddrMap.begin(); i != m_symbolAddrMap.end(); i++)
		if (i->second->filename == file)
			vec.push_back(i->second->name);
	return vec;
}

void DebugSymbols::AddSymbolsFrom(std::string fileName, u32 addr)
{
	u32 magic = Ram<u32>(addr);

	if (magic == 0xfeedface) //MachO file
		AddSymbolsFromMachO(fileName, addr, fileName);

	else if (magic == 0x646c7964) //"dyld", dyld cache
	{
		u32 offset = Ram<u32>(addr + 0x18);
		u32 amount = Ram<u32>(addr + 0x1c);
		for (u32 i = 0; i < amount; i++)
		{
			u32 dylibaddr = Ram<u32>(addr + offset);
			u32 stroff = Ram<u32>(addr + offset + 0x18);

			AddSymbolsFromMachO(RamString(addr + stroff), dylibaddr, fileName);

			offset += 0x20;
		}
	}
}

void DebugSymbols::AddSymbolsFromMachO(std::string fileName, u32 addr, std::string cacheName)
{
	u32 cmdNum = Ram<u32>(addr + 0x10);
	u32 cmdAddr = addr + 0x1c;
	
	bool foundBase = false;
	u32 base;
	u32 symTabCmdAddr;

	for (u32 i = 0; i < cmdNum; i++)
	{
		u32 cmd = Ram<u32>(cmdAddr);
		u32 size = Ram<u32>(cmdAddr + 0x4);

		if (cmd == 0x1 && !foundBase) //segment
		{
			u32 prot = Ram<u32>(cmdAddr + 0x28);
			if (prot) //if not zero-page
			{				
				base = Ram<u32>(cmdAddr + 0x18);
				foundBase = true;
			}
		}

		if (cmd == 0x2) //symtab
			symTabCmdAddr = cmdAddr + 0x8;

		cmdAddr += size;
	}

	ReadSymbols(fileName, cacheName, symTabCmdAddr, addr - base);
}

void DebugSymbols::RemoveSymbolsFrom(u32 addr, u32 size)
{
	SymbolMapIter start, end;
	do
	{
		start = m_symbolAddrMap.lower_bound(addr);
		end = m_symbolAddrMap.lower_bound(addr + size);
		if (start == end)
			break;
		m_symbolAddrMap.erase(start);
	}
	while (true);
} 

void DebugSymbols::ReadSymbols(std::string fileName, std::string cacheName, u32 cmdAddr, u32 reloc)
{
	u32 symTabOff = Ram<u32>(cmdAddr);
	u32 symTabNum = Ram<u32>(cmdAddr + 0x4);
	u32 strTabOff = Ram<u32>(cmdAddr + 0x8);
	u32 strTabSize = Ram<u32>(cmdAddr + 0xc);

	SymDesc* symTab = new SymDesc[symTabNum];
	char* strTab = new char[strTabSize];

	fileName = BsdRegFile::ProcessToNativePath(fileName);
	cacheName = BsdRegFile::ProcessToNativePath(cacheName);

	File file;
	file.Open(cacheName, File::ReadOnly, File::OpenExisting);

	file.SetPtr(symTabOff);
	file.Read(symTab, symTabNum * sizeof(SymDesc));

	file.SetPtr(strTabOff);
	file.Read(strTab, strTabSize);

	InsertSymbols(fileName, reloc, symTab, symTabNum, strTab);

	delete[] symTab;
	delete[] strTab;
}


void DebugSymbols::InsertSymbols(std::string fileName, u32 reloc, SymDesc* symTab, u32 symTabNum, char* strTab)
{
	for (u32 i = 0; i < symTabNum; i++)
	{
		Symbol* symbol = new Symbol();
		symbol->name = strTab + symTab[i].strIndex;
		symbol->filename = fileName.substr(fileName.find_last_of('/') + 1);
		symbol->type = symTab[i].type;
		symbol->section = symTab[i].section;
		symbol->desc = symTab[i].desc;
		symbol->position = symTab[i].value + reloc;

		HLE::SetHLE(symbol);

		if (symbol->IsDefined())
		{
			m_symbolAddrMap.insert(SymbolPair(symbol->position, symbol));
			m_symbolStrMap.insert(StrSymPair(symbol->name, symbol));
			m_fileSet.insert(symbol->filename);
		}
	}
}

}