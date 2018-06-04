#include "BsdFile.h"
#include "Memory.h"
#include "MMFlag.h"
#include "VMProtect.h"
#include "VMFlags.h"
#include "PhysicalMem.h"
#include "DebugSymbols.h"
#include "FdSpace.h"

macptr MemMap(macptr address, u32 size, VMProtect protect, MMFlag flags, int fd, s64 offset)
{
	//totally inefficient for later
	try
	{
		if (flags.IsAnonymous()) // do not map from file
		{
			//MakeSure((fd & 1) && !flags.IsFixed());
			//.... fd contains flags!!!!
			address = g_task->vMem->Allocate(address, size, VMFlags::Anywhere);
		}
		else if (flags.IsFixed()) 
		{
			address = g_task->vMem->Allocate(address, size, VMFlags::Overwrite);
			g_task->fdSpace->GetFd(fd)->ReadP(address, size, offset);

			MachO::DebugSymbols::AddSymbolsFrom(g_task->fdSpace->GetFd(fd)->Path(), address); //attempt
		}
		else
		{
			address = g_task->vMem->Allocate(address, size, VMFlags::Anywhere);
			g_task->fdSpace->GetFd(fd)->ReadP(address, size, offset);

			MachO::DebugSymbols::AddSymbolsFrom(g_task->fdSpace->GetFd(fd)->Path(), address); //attempt
		}

		g_task->vMem->Protect(address, size, true, protect);
		g_task->vMem->Protect(address, size, false, protect);
	}
	catch (MachErr::Type& machErr)
	{
		Fail(0); //lazy
	}

	return address;
}

int MemUnmap(macptr address, u32 size)
{
	try
	{
		//SUPER-TEMP
		g_task->vMem->Deallocate(address, size);

		MachO::DebugSymbols::RemoveSymbolsFrom(address, size); //?
	}
	catch (MachErr::Type& machErr)
	{
		Fail(0); //lazy
	}

	return 0; //ONLY ON SUCCESS AND THIS IS SIMPLE TO DO BUT...
}

int MemMapShared(int fd, u32 count, macptr areas) 
{
	macptr ptr = areas;
	u32 i;
	for (i = 0; i < count; i++)
	{
		u32 vaddr = Ram<u32>(ptr);
		u32 vsize = Ram<u32>(ptr + 4);
		u32 offset = Ram<u32>(ptr + 8);
		u32 maxProt = Ram<u32>(ptr + 12);
		u32 prot = Ram<u32>(ptr + 16);
		MakeSure((maxProt & 0x1f) == maxProt);

		try
		{
			g_task->vMem->Allocate(vaddr, vsize, VMFlags::Fixed);

			if (!(maxProt & 0x10)) //not zero-fill
			{
				g_task->fdSpace->GetFd(fd)->ReadP(vaddr, vsize, offset); //fail?
				MachO::DebugSymbols::AddSymbolsFrom(g_task->fdSpace->GetFd(fd)->Path(), vaddr); //attempt
			}
			// COW not used yet

			g_task->vMem->Protect(vaddr, vsize, true, maxProt & 0x7);
			g_task->vMem->Protect(vaddr, vsize, false, prot & 0x7);
		}
		catch (MachErr::Type& machErr)
		{
			Fail(0); //lazy
		}

		ptr += 20;
	}

	return 0;
}