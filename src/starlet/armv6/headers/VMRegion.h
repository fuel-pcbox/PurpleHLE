#pragma once
#include "PhysicalMem.h"
#include "VMProtect.h"
#include "VMInherit.h"

struct VMRegion
{
public:
	VMRegion(Ref<PhysicalMem> phyMem, u32 offset, VMProtect protect, VMProtect maxProtect, VMInherit inherit)
		: phyMem(phyMem), phyOffset(offset), protect(protect), maxProtect(maxProtect), inherit(inherit) 
	{}
	VMRegion() //default 
		: phyMem(NULL), phyOffset(0), protect(VMProtect::All), maxProtect(VMProtect::All), inherit(VMInherit::Copy)
	{}

	bool IsValid() const { return phyMem.IsValid(); }

	bool IsAccessible() const { return IsValid() && protect.CanAccess(); }

	bool IsValidFor(u32 size) const { return IsValid() && phyMem->IsValidFor( size + phyOffset ); }

	bool IsProtectValid() const { return IsValid() && phyMem->IsValidFor(protect) && phyMem->IsValidFor(maxProtect); }

	void* GetMemory() { return phyMem->GetMemory(phyOffset); } //pre: IsValid()

	Ref<PhysicalMem> phyMem;
	u32 phyOffset;
	VMProtect protect;
	VMProtect maxProtect;
	VMInherit inherit;
};