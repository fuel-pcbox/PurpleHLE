#pragma once
#include "types.h"

namespace MsgError
{
	typedef enum { 
		NoError = 0, 
		SendInvalidData = 0x10000002, 
		SendInvalidDest = 0x10000003, 
		SendTimedOut = 0x10000004,
		ReceiveInvalidName = 0x10004002,
		ReceiveTimedOut = 0x10004003,
		ReceiveTooLarge = 0x10004004
	} Type;
}
