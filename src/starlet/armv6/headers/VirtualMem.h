#pragma once
#include "MachErr.h"
#include "VMRegion.h"
#include "VMFlags.h"

class VirtualMem
{
public:
	VirtualMem();

	Ref<PhysicalMem> GetObject(u32 address, u32* size, VMProtect protect, Ref<PhysicalMem> parent);
	VMRegion GetRegion(u32 address, u32* size);

	u32 Map(u32 address, u32 size, u32 mask, VMFlags flags, bool copy, VMRegion entry);

	u32 Allocate(u32 address, u32 size, VMFlags flags)
	{
		return Map(address, size, 0, flags, false, VMRegion());
	}

	u32 AnyAlloc(u32 size) //helper
	{
		return Allocate(0, size, VMFlags::Anywhere);
	}

	void Deallocate(u32 start, u32 size);

	void Protect(u32 start, u32 size, bool setMax, VMProtect newProtect);

	void* Access(u32 addr) //Remember : has to be aligned! (if unaligned access is supported, can't do this?)
	{
		return (u8 *)m_pageMap[addr / PageSize] + (addr & PageMask);
	}
	
	static const int PageSize = 0x1000;
	static const int PageMask = 0xfff;
	//static const int AvailVramSize = 0x40000000;
	static const s64 VramSize = 0x100000000;
	static const int PagesTotal = (int)(VramSize / PageSize);
	static const u32 PhyMemSize = 128 * 1024 * 1024;

private:

	void DoMap(u32 startPage, u32 countPages, VMRegion entry);

	void DoUnMap(u32 startPage, u32 countPages) { DoMap(startPage, countPages, VMRegion()); } //MEMLEAK

	void DoSetProtect(u32 startPage, u32 countPages, bool setMax, VMProtect newProt);

	bool TestRegion(u32 startPage, u32 countPages, bool shouldBeMapped);
	bool IsRegionMapped(u32 startPage, u32 countPages) { return TestRegion(startPage, countPages, true); }
	bool IsRegionFree(u32 startPage, u32 countPages) { return TestRegion(startPage, countPages, false); }

	bool TestProtect(u32 startPage, u32 countPages, VMProtect newProt);

	bool FindFree(u32* startPage, u32 countPages, u32 pageMask);

	void* m_pageMap[PagesTotal];
	VMRegion m_pageEntry[PagesTotal];
};