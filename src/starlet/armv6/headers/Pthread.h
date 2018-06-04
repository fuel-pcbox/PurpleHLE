#pragma once
#include "Memory.h"

int SemWaitSignal(u32 waitPort, u32 signalPort, int timeout, int relative, u32 timeSec, u32 timeNsec);

int SetCancel(u32 type);

s64 GetTimeOfDay(macptr time, macptr zone);