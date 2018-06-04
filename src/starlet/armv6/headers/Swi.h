#pragma once
#include "CpuState.h"

void Swi(u32 swi);

u64& Long(u32& low, u32& high);

void SwiResume(bool success, u32 userData);