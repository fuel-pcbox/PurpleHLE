#pragma once
#include "types.h"
#include "File.h"
#include "BsdFile.h"
#include "Memory.h"

class BsdKqueue : public BsdFile
{
public:
	BsdKqueue() {}
	bool Inherit() { return false; }
	bool CanRead() { return false; }
	FSMode GetType() { return FSMode::CharDevice; } //?

	static int Kevent(int fd, macptr changeList, int changes, macptr evList, int events, macptr timeOut)
	{
		if (events > 0)
			g_task->fdSpace->GetFd(fd)->Wait();
		return 0;
	}
};
