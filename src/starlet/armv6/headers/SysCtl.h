#pragma once
#include "Memory.h"


int SysCtl(u32 type, u32 subtype, macptr oldbuf, macptr oldsize, macptr newbuf, u32 newsize);

int SigMask(u32 how, macptr newBuf, macptr oldBuf);

int SigAction(int signal, macptr newsig, macptr oldsig);