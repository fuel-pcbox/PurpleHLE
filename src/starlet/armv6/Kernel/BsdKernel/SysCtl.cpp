#include "SysCtl.h"
#include "MachoReader.h"
#include "VirtualMem.h"

int MakeInt(macptr buf, macptr bufSize, u32 value)
{
	if (buf)
	{
		Assert(Ram<u32>(bufSize) >= 4);
		Ram<u32>(buf) = value;
	}
	Ram<u32>(bufSize) = 4;
	return 0;
}

int MakeInt2(macptr buf, macptr bufSize, u32 value, u32 value2)
{
	if (buf)
	{
		Assert(Ram<u32>(bufSize) >= 8);
		Ram<u32>(buf) = value;
		Ram<u32>(buf + 4) = value2;
	}
	Ram<u32>(bufSize) = 8;
	return 0;
}

int MakeStr(macptr buf, macptr bufSize, const char* value)
{
	u32 len = strlen(value) + 1;
	if (buf)
	{
		Assert(Ram<u32>(bufSize) >= len);
		ToRamCpy(buf, value, len);
	}
	Ram<u32>(bufSize) = len;
	return 0;
}

int FillZero(macptr buf, u32 size)
{
	for (u32 i = 0; i < size; i++)
		Ram<u8>(buf + i) = 0;
	return 0;
}

enum { SYSCTL_KERN = 1, SYSCTL_HW = 6
		};

enum { KERN_HOSTNAME = 10, KERN_USRSTACK32 = 35, KERN_NETBOOT = 40, KERN_SHREG_PRIVATIZABLE = 54, KERN_SAFEBOOT = 66
		};

enum { HW_NCPU = 3, HW_PAGESIZE = 7, HW_MEMSIZE = 24
		};


int SysCtlName(macptr buf, macptr sizeBuf, std::string name)
{
	if (name == "vm.shared_region_version")
		return MakeInt2(buf, sizeBuf, 0xaaaaaaaa, 0);
	else if (name == "hw.memsize")
		return MakeInt2(buf, sizeBuf, SYSCTL_HW, HW_MEMSIZE);
	else
		Fail(-1);
}

int SysCtl(macptr nameBuf, u32 nameSize, macptr oldBuf, macptr oldSizeBuf, macptr newBuf, u32 newSize)
{
	MakeSure(nameSize >= 1);
	u32 oldSize = Ram<u32>(oldSizeBuf);

	u32 name = Ram<u32>(nameBuf);
	switch (name)
	{
	case 0xaaaaaaaa: //unknown stuff
		{
			MakeSure(nameSize >= 2);
			u32 name = Ram<u32>(nameBuf + 4);
			switch (name)
			{
			case 0: //vm.shared_region_version
				return MakeInt(oldBuf, oldSizeBuf, 3);

			default:
				Fail(name); //unlikely
			}

		}
	case 0: //misc
		{
			MakeSure(nameSize >= 2);
			u32 name = Ram<u32>(nameBuf + 4);
			switch (name)
			{
			case 3: //get by name
				return SysCtlName(oldBuf, oldSizeBuf, RamString(newBuf, newSize));

			default:
				Fail(name);
			}
		}
	case SYSCTL_KERN: //kernel
		{
			MakeSure(nameSize >= 2);
			u32 name = Ram<u32>(nameBuf + 4);
			switch (name)
			{
			case KERN_HOSTNAME:
				return MakeStr(oldBuf, oldSizeBuf, "iPhone");

			case KERN_USRSTACK32:
				return MakeInt(oldBuf, oldSizeBuf, MachO::Reader::StackTop/* - MachO::Reader::StackSize*/);

			case KERN_NETBOOT:
				return MakeInt(oldBuf, oldSizeBuf, 0);

			case KERN_SHREG_PRIVATIZABLE:
				return MakeInt(oldBuf, oldSizeBuf, 1);
				
			case KERN_SAFEBOOT:
				return MakeInt(oldBuf, oldSizeBuf, 0);

			default:
				Fail(name);
			}
		}
		break;

	case SYSCTL_HW: //hw
		{
			MakeSure(nameSize >= 2);
			u32 name = Ram<u32>(nameBuf + 4);
			switch (name)
			{
			case HW_NCPU:
				return MakeInt(oldBuf, oldSizeBuf, 1);

			case HW_PAGESIZE:
				return MakeInt(oldBuf, oldSizeBuf, VirtualMem::PageSize); //is this what it is?

			case HW_MEMSIZE:
				return MakeInt(oldBuf, oldSizeBuf, VirtualMem::PhyMemSize); 

			default:
				Fail(name);
			}
		}
		break;

	default:
		Fail(name);
	}
	return 0;
}

int SigMask(u32 how, macptr newBuf, macptr oldBuf)
{
	if (oldBuf)
		Ram<u32>(oldBuf) = 0; //nol blocked?
	if (newBuf) //replace in-place!
		Fail(0);
	return 0;
}

int SigAction(int signal, macptr newSig, macptr oldSig)
{
	if (oldSig)
	{
		Ram<u32>(oldSig) = 0; //DFL / handler
		Ram<u32>(oldSig + 4) = 0; //mask
		Ram<u32>(oldSig + 8) = 0; //flags
	}
	return 0;
}