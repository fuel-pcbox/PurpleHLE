#pragma once
#include "types.h"
#include <map>
#include "BsdFile.h"
#include "BsdBasics.h"
#include "Ref.h"

struct Fd
{
	Fd() : file(NULL), closeOnExec(false) {}
	Fd(Ref<BsdFile> file) : file(file), closeOnExec(false) {}

	Ref<BsdFile> file;
	bool closeOnExec;
};

class FdSpace
{
public:
	static const int FdLimit = 200; //raise if needed

	FdSpace()
	{
		NewFd(new BsdStdIn());
		NewFd(new BsdStdOut());
		NewFd(new BsdStdErr());
	}

	FdSpace(const FdSpace& parent)
	{
		for (int fd = 0; fd < FdLimit; fd++)
			if (parent.m_fdArr[fd].file->Inherit())
				m_fdArr[fd] = parent.m_fdArr[fd];
	}

	void Execve()
	{
		for (int fd = 0; fd < FdLimit; fd++)
			if (GetCOE(fd))
				DelFd(fd);
	}

	Ref<BsdFile> GetFd(int fd) const { TestFdOpen(fd); return m_fdArr[fd].file; }
	bool IsFd(int fd) const { TestFd(fd); return m_fdArr[fd].file.IsValid(); }

	bool GetCOE(int fd) const { TestFdOpen(fd); return m_fdArr[fd].closeOnExec; }
	void SetCOE(int fd, bool COE) { TestFdOpen(fd); m_fdArr[fd].closeOnExec = COE; }

	int NewFd(Ref<BsdFile> file, int minFd = 0) 
	{ 
		int fd = FindFree(minFd);
		m_fdArr[fd].file = file; 
		m_fdArr[fd].closeOnExec = false;
		return fd;
	}
	int DelFd(int fd) 
	{ 
		TestFdOpen(fd); 
		m_fdArr[fd].file = NULL; 
		return 0;
	}

private:
	void TestFd(int fd) const { if (fd >= FdLimit || fd < 0) throw ErrNo::BadFd; }
	void TestFdOpen(int fd) const { TestFd(fd); if (m_fdArr[fd].file.IsNull()) throw ErrNo::BadFd; }

	int FindFree(int minFd = 0)
	{
		for (int i = minFd; i < FdLimit; i++)
			if (m_fdArr[i].file.IsNull())
				return i;
		throw ErrNo::TooManyFd;
	}

	Fd m_fdArr[FdLimit];
};