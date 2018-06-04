#pragma once
#include "types.h"

namespace MachErr
{
	typedef enum {
		NoError = 0,

		InvalidAddress = 1,
		ProtectionFailure = 2,
		NoSpace = 3,
		InvalidArgument = 4,
		AlreadyInSet = 11,
		NotInSet = 12,
		NameExists = 13,
		InvalidName = 15,
		InvalidTask = 16,
		InvalidRight = 17,
		InvalidValue = 18,
		UrefOverflow = 19,
		InvalidCapability = 20,
		RightExists = 21,
		InvalidHost = 22,
		InvalidObject = 29,
		AlreadyWaiting = 30,
		NotDepressed = 36,
		Terminated = 37,
		LockSetDestroyed = 38,
		LockUnstable = 39,
		LockOwned = 40,
		LockOwnedSelf = 41,
		SemaphoreDestroyed = 42,
		NotWaiting = 48,
		TimedOut = 49,

		//bootstrap
		BootstrapUnprivileged = 1100,
		BootstrapNameInUse = 1101,
		BootstrapUnknownService = 1102,
		BootstrapServiceActive = 1103,
		BootstrapBadCount = 1104,
		BootstrapNoMemory = 1105,

		//IO
		IoNoDevice = 0xe00002c0

	} Type;

}