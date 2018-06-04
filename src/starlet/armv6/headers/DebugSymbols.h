#pragma once
#include "types.h"
#include <map>
#include <vector>
#include <set>

namespace MachO
{

struct Symbol
{
	u8 type;
	u8 section;
	u16 desc;

	//extra
	std::string name;
	std::string filename;
	u32 position;
	
	bool IsDefined() { return (type & 0xe) == 0xe; }
	bool IsExported() { return (type & 0xf) == 0xf; }
	bool IsThumb() { return (desc & 0x8) == 0x8; }
};

struct SymDesc
{
	u32 strIndex;
	u8 type;
	u8 section;
	u16 desc;
	u32 value;
};

class DebugSymbols
{
public:
	static std::string GetSymbolAt(u32 addr);
	static u32 GetAddrOf(std::string name);
	static std::vector<std::string> GetFileList();
	static std::vector<std::string> GetSymbolList(std::string file);
	static void AddSymbolsFrom(std::string fileName, u32 addr);
	static void RemoveSymbolsFrom(u32 addr, u32 size);

private:
	typedef std::map<u32, Symbol*> SymbolMap;
	typedef std::pair<u32, Symbol*> SymbolPair;
	typedef std::map<u32, Symbol*>::iterator SymbolMapIter;
	
	typedef std::map<std::string, Symbol*> StrSymMap;
	typedef std::pair<std::string, Symbol*> StrSymPair;
	typedef std::map<std::string, Symbol*>::iterator StrSymMapIter;
	
	typedef std::set<std::string> FileSet;
	typedef std::set<std::string>::iterator FileSetIter;

	static SymbolMap m_symbolAddrMap;
	static StrSymMap m_symbolStrMap; 
	static FileSet m_fileSet; 

	static void AddSymbolsFromMachO(std::string fileName, u32 addr, std::string cacheName);
	static void ReadSymbols(std::string fileName, std::string cacheName, u32 cmdAddr, u32 reloc);
	
	static void InsertSymbols(std::string fileName, u32 reloc, SymDesc* symTab, u32 symTabNum, char* strTab);
};

}