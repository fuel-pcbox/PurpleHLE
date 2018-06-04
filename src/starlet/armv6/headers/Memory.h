#pragma once
#include "types.h"
#include "VirtualMem.h"
#include "Task.h"

template <typename T>
void MakeSureUnaligned(u32 addr)
{
	if (((addr & VirtualMem::PageMask) + sizeof(T) - 1) < 0x1000)
		return;
	if ((u8 *)g_task->vMem->Access(addr &~ VirtualMem::PageMask) + VirtualMem::PageSize ==
		(u8 *)g_task->vMem->Access((addr &~ VirtualMem::PageMask) + VirtualMem::PageSize))
		return;
	Fail(addr);
}

template <typename T>
INLINE T& Ram(u32 addr) //aligned access only!
{
	if ((addr & (sizeof(T) - 1)) != 0)
		MakeSureUnaligned<T>(addr);

	return *(T *)g_task->vMem->Access(addr);
}

template <typename T>
INLINE T SafeRamRead(u32 addr) //aligned access only!
{
	Assert((addr & (sizeof(T) - 1)) == 0);

	T* memptr = (T *)g_task->vMem->Access(addr);
	if (memptr < (T *)VirtualMem::PageSize)
		return 0;
	return *memptr;
}

INLINE void ToRamCpy(u32 addr, const void* memory, u32 size)
{
	//VERY slow for now:
	u8* memBuf = (u8 *)memory;
	for (u32 i = 0; i < size; i++)
		Ram<u8>(i + addr) = memBuf[i];
}
INLINE void FromRamCpy(void* memory, u32 addr, u32 size)
{
	//VERY slow for now:
	u8* memBuf = (u8 *)memory;
	for (u32 i = 0; i < size; i++)
		memBuf[i] = Ram<u8>(i + addr);
}

INLINE std::string RamString(u32 addr)
{
	std::string str; //slow!
	while (Ram<u8>(addr) != 0)
	{
		str += Ram<u8>(addr);
		addr++;
	}
	return str;
}

INLINE std::string RamString(u32 addr, int size)
{
	std::string str; //slow!
	for (int i = 0; i < size && Ram<u8>(addr) != 0; i++)
	{
		str += Ram<u8>(addr);
		addr++;
	}
	return str;
}