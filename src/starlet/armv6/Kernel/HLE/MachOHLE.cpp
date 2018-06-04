#include "MachOHLE.h"
#include "CpuState.h"
#include "Memory.h"
#include "StringUtils.h"

namespace MachO
{
using namespace Interpreter;

void HLE::commpage_gettimeofday()
{
	cpu.a1 = 1; //couldn't get time through commpage
}

void HLE::get_cpu_capabilities()
{
	cpu.a1 = 0; //a bit vector. not sure about contents.
}

void HLE::getifaddrs()
{
	cpu.a1 = 0; //success
}

void HLE::nop()
{
	u32 addr = cpu.nia - (cpu.thumb ? 2 : 4);
	Warn("CALLED CALLED CALLED CALLED :       %s ", g_hleStrMap[addr].c_str()); 
}

void HLE::SetHLE(Symbol* symbol, void (*func)())
{
	u32 addr = symbol->position;
	bool thumb = symbol->IsThumb();
	u8 bkpt = BkptVal();

	g_hleMap[addr] = func;
	g_hleStrMap[addr] = symbol->name;

	if (thumb)
		Ram<u16>(addr) = 0xbe00 | bkpt;
	else
		Ram<u32>(addr) = 0xe1200070 | (bkpt & 0xf) | ((bkpt & 0xf0) << 4);
}

void HLE::Do(macptr addr)
{
	FuncHLE func = g_hleMap[addr];
	Assert(func);

	func();
	
	cpu.nia = cpu.lr & ~1;
	cpu.thumb = cpu.lr & 1;
}

void HLE::SetHLE(Symbol* symbol)
{
	if (symbol->IsDefined())
	{
		if (symbol->name == "___commpage_gettimeofday")
			SetHLE(symbol, commpage_gettimeofday);

		if (symbol->name == "__get_cpu_capabilities")
			SetHLE(symbol, get_cpu_capabilities);

		if (symbol->name == "_getifaddrs")
			SetHLE(symbol, getifaddrs);

		if (symbol->filename == "OpenGLES" && symbol->IsExported() && 
			(StartsWith(symbol->name, "_gl") || StartsWith(symbol->name, "_egl")))
			SetHLE(symbol, nop);
	}
}

std::map<macptr, HLE::FuncHLE> HLE::g_hleMap;
std::map<macptr, std::string> HLE::g_hleStrMap;

}