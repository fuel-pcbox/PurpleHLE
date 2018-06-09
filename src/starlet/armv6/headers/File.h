#pragma once

#ifdef _WIN32
#include <windows.h>
#endif
#include <vector>

#include "types.h"

class File
{
public:
	bool Open(const std::string& fileName, int accessType, int openType);
	long long GetSize();
	int Read(void* buffer, unsigned size);
	int Write(void* buffer, unsigned size);
	void SetPtr(long long value);
	void Close();

	static std::string ReadStr(const std::string& fileName);

	static bool IsDirectory(const std::string& fileName);
	static bool IsFile(const std::string& fileName);
	static bool Exists(const std::string& fileName);

	File()
		: m_hFile(INVALID_HANDLE_VALUE)
	{}
	~File()	{ Close(); }

	static const int NoAccess = 0;
	static const int ReadOnly = GENERIC_READ;
	static const int WriteOnly = GENERIC_WRITE;
	static const int ReadWrite = GENERIC_READ | GENERIC_WRITE;

	static const int OpenAlways = OPEN_ALWAYS;
	static const int OpenExisting = OPEN_EXISTING;
	static const int CreateAlways = CREATE_ALWAYS;
	static const int TruncExisting = TRUNCATE_EXISTING;
	static const int CreateNew = CREATE_NEW;

protected:
	HANDLE m_hFile;
};

class FileFinder
{
public:
	bool Open(const std::string& fileName);

	bool IsDirectory(int index);
	const char* GetFileName(int index);
	unsigned GetNumFiles();

protected:
	std::vector<WIN32_FIND_DATAA> m_files;
};

class FileSys
{
public:
	static bool CreateDir(const std::string& dirName);
	static bool Move(const std::string& oldName, const std::string& newName);
	static bool RemoveFile(const std::string& name);
	static bool RemoveEmptyDir(const std::string& name);
};