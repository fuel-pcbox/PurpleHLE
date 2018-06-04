#pragma once
#include "VMProtect.h"
#include "MMFlag.h"
#include "Memory.h"

macptr MemMap(macptr address, u32 size, VMProtect protect, MMFlag flags, int fd, s64 offset); //temp

int MemUnmap(macptr address, u32 size);

int MemMapShared(int fd, u32 count, macptr areas);