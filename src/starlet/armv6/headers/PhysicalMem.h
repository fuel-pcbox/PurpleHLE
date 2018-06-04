#pragma once
#include "types.h"
#include "Ref.h"
#include "VMProtect.h"

/*class MemSource
{
public:
	virtual void FlushToSource(s64 offset, u32 size, u8* data) = 0;
};*/

class PhysicalMem : public Reffable
{
public:
	PhysicalMem(u32 size, VMProtect protect)
		: m_protect(protect), m_mem(size, 0)
	{}

	Ref<PhysicalMem> Copy()
	{
		Ref<PhysicalMem> copy = new PhysicalMem(m_protect, GetMemory(0), GetSize());
		return copy;
	}

	u32 GetSize() const { return m_mem.Size(); }
	bool IsValidFor(u32 size) const { return (m_mem.Size() >= size); }
	bool IsValidFor(VMProtect protect) const { return protect.RestrictedBy(m_protect); }
		
	u8* GetMemory(u32 offset) { return m_mem.RawPtr(offset); }

protected:
	PhysicalMem(PhysicalMem& phyMem);
	PhysicalMem(VMProtect protect, u8* memory, u32 size)
		: m_protect(protect), m_mem(memory, size)
	{}

	Array<u8> m_mem;
	VMProtect m_protect;
};

/*class BackedMem : public PhysicalMem
{
public:
	BackedMem(u32 size, VMProtect protect, Ref<MemSource> source) 
		: PhysicalMem(size, protect), m_source(source) {}

	void FlushToSource(s64 offset, u32 size)
	{
		m_source->FlushToSource(offset, size, GetMemory(offset));
	}

protected:
	BackedMem(BackedMem& phyMem);

	Ref<MemSource> m_source;
};*/