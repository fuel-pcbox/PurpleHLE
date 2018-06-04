#pragma once
#include "DebugSymbols.h"
#include <map>

namespace MachO
{

class HLE
{
public:
	static void SetHLE(Symbol* symbol);
	static void Do(macptr addr);

	static u8 BkptVal() { return 0x44; }

private:
	static void SetHLE(Symbol* symbol, void (*func)());

	static void commpage_gettimeofday();
	static void get_cpu_capabilities();
	static void getifaddrs();
	static void nop();

	typedef void (*FuncHLE)();
	static std::map<macptr, FuncHLE> g_hleMap;
	static std::map<macptr, std::string> g_hleStrMap;
};

}