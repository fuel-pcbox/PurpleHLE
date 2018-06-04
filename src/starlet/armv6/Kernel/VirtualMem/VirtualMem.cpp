#include "VirtualMem.h"

VirtualMem::VirtualMem()
{
	for (int i = 0; i < PagesTotal; i++)
		m_pageMap[i] = NULL;

	//Allocate(0x40000000, PageSize, VMFlags::Fixed); //commPage
	//for (int i = 0; i < PageSize; i++)
	//	*(u8 *)Access(0x40000000 + i) = 0xff;
}

u32 VirtualMem::Map(u32 address, u32 size, u32 mask, VMFlags flags, bool copy, VMRegion entry)
{
	u32 start = address;
	u32 startPage = start / PageSize;
	u32 countPages = (size + PageMask) / PageSize; // QQQ - doesn't check (start + size) ("feature"?)
	u32 pageMask = mask / PageSize;

	startPage &= ~pageMask;

	if (size == 0)
		return 0;

	bool free = IsRegionFree(startPage, countPages);
	if (startPage == 0 && flags.IsAnywhere()) //make sure not to allocate the first page
		free = false;

	if (!free && flags.IsAnywhere()) //can search other places
		free = FindFree(&startPage, countPages, pageMask);

	if (!free && flags.IsOverwrite()) //can overwrite
	{
		DoUnMap( startPage, countPages );
		free = true;
	}

	if (!free) //couldn't find a page or couldn't search everywhere
		throw MachErr::NoSpace; //?

	if (entry.phyMem.IsNull()) // if NULL, must allocate physical memory ourselves
	{
		//MakeSure( !copy ); // QQQ
		entry.phyMem = new PhysicalMem( countPages * PageSize, entry.maxProtect );
	}

	if (!entry.IsValidFor( countPages * PageSize ))
		throw MachErr::InvalidObject;
	
	if (!entry.IsProtectValid())
		throw MachErr::ProtectionFailure;
	
	MakeSure( entry.protect <= entry.maxProtect ); // QQQ

	if (copy)
		entry.phyMem = entry.phyMem->Copy();

	DoMap( startPage, countPages, entry );

	return startPage * PageSize;
}

void VirtualMem::Deallocate(u32 start, u32 size)
{
	u32 startPage = start / PageSize;
	u32 endPage = (start + size + PageMask) / PageSize; //QQQ - (start + size) only in dealloc?
	u32 countPages = endPage - startPage;

	bool mapped = IsRegionMapped( startPage, countPages );
	if (!mapped)
		throw MachErr::InvalidAddress;

	DoUnMap( startPage, countPages );
}

void VirtualMem::Protect(u32 start, u32 size, bool setMax, VMProtect newProtect)
{
	u32 startPage = start / PageSize;
	u32 endPage = (start + size + PageMask) / PageSize; //QQQ - (start + size) only in dealloc?
	u32 countPages = endPage - startPage;

	bool mapped = IsRegionMapped( startPage, countPages );
	if (!mapped)
		throw MachErr::InvalidAddress;

	bool protect = TestProtect( startPage, countPages, newProtect );
	if (!protect)
		throw MachErr::ProtectionFailure;
	
	DoSetProtect( startPage, countPages, setMax, newProtect );
}

void VirtualMem::DoMap(u32 startPage, u32 countPages, VMRegion entry)
{
	MakeSure(startPage < PagesTotal && startPage + countPages < PagesTotal);
	for (u32 page = startPage; page < startPage + countPages; page++)
	{
		m_pageEntry[page] = entry;

		m_pageMap[page] = entry.IsAccessible() ? entry.GetMemory() : NULL;

		entry.phyOffset += PageSize;
	}
}

void VirtualMem::DoSetProtect(u32 startPage, u32 countPages, bool setMax, VMProtect newProt)
{
	MakeSure(startPage < PagesTotal && startPage + countPages < PagesTotal);
	for (u32 page = startPage; page < startPage + countPages; page++)
	{
		VMRegion entry = m_pageEntry[page];
		if (setMax)
		{
			entry.maxProtect = newProt;
			if (!entry.protect.RestrictedBy( entry.maxProtect ))
				entry.protect = entry.maxProtect; //QQQ - is this it?
		}
		else
			entry.protect = newProt;
		
		m_pageMap[page] = entry.IsAccessible() ? entry.GetMemory() : NULL;
		m_pageEntry[page] = entry;
	}
}

bool VirtualMem::TestRegion(u32 startPage, u32 countPages, bool shouldBeMapped)
{
	MakeSure(startPage < PagesTotal && startPage + countPages < PagesTotal);
	for (u32 page = startPage; page < startPage + countPages; page++)
		if (m_pageEntry[page].IsValid() != shouldBeMapped)
			return false;
	return true;
}

bool VirtualMem::TestProtect(u32 startPage, u32 countPages, VMProtect newProt)
{
	MakeSure(startPage < PagesTotal && startPage + countPages < PagesTotal);
	for (u32 page = startPage; page < startPage + countPages; page++)
		if (!newProt.RestrictedBy( m_pageEntry[page].maxProtect ))
			return false;
	return true;
}

bool VirtualMem::FindFree(u32* startPage, u32 countPages, u32 pageMask) //slow
{
	u32 found = 0;
	u32 foundPage;
	for (u32 page = 1; page < PagesTotal; page++)
	{
		if (m_pageEntry[page].IsValid())
			found = 0;
		else // if page is free
		{
			if (found == 0) // first page of a free region
			{
				if ((page & pageMask) != 0)
					continue; 
				foundPage = page;
			}
			found++;
			if (found == countPages)
			{
				*startPage = foundPage;
				return true;
			}
		}
	}
	return false;
}

Ref<PhysicalMem> VirtualMem::GetObject(u32 address, u32* size, VMProtect protect, Ref<PhysicalMem> parent)
{
	u32 startPage = address / PageSize;
	u32 countPages = (*size + PageMask) / PageSize;

	Ref<PhysicalMem> newMem = m_pageEntry[startPage].phyMem;
	Assert(m_pageEntry[startPage].phyOffset == 0); //NO

	for (u32 page = startPage + 1; page < startPage + countPages; page++)
		Assert(m_pageEntry[page].phyMem == newMem);

	//change *size?. protect? very wrong
	return newMem;
}

VMRegion VirtualMem::GetRegion(u32 address, u32* size)
{
	u32 startPage = address / PageSize;

	VMRegion region = m_pageEntry[startPage];
	Assert(region.phyOffset == 0); //NO

	u32 countPages = 0;
	for (u32 page = startPage; ; page++, countPages++)
		if (m_pageEntry[page].phyMem != region.phyMem)
			break;
	*size = countPages * PageSize;

	return region;
}

