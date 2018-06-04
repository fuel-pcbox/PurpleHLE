#pragma once
#include "types.h"
#include "File.h"
#include "BsdSeekable.h"

class BsdRegFile : public BsdSeekable
{
public:
	void Open(FSOflag flags, FSMode fmode);
	virtual ~BsdRegFile();

	int ReadP(macptr memBuffer, u32 size, s64 offset);
	int WriteP(macptr memBuffer, u32 size, s64 offset);
	bool CanRead() { return GetSize() != m_offset; }

	u64 GetSize() { return m_file.GetSize(); }
	FSMode GetType() { return FSMode::Regular; }

	static std::string ProcessToNativePath(const std::string& fname);
	static std::string AbsToNativePath(const std::string& absname);

protected:
	File m_file;
};

class BsdDir : public BsdSeekable
{
public:
	u64 GetSize() { return 0; } //!?!??!?!?
	FSMode GetType() { return FSMode::Directory; }

	void Open(FSOflag flags, FSMode fmode);
	int GetDirEntries(macptr memBuffer, u32 size, macptr newPos);
	int GetDirEntries64(macptr memBuffer, u32 size, macptr newPos);

protected:
	FileFinder m_files;
};


int MakeDir(std::string name, FSMode fmode);
int Rename(std::string oldName, std::string newName);
u32 ReadLink(std::string lname, macptr addr, u32 size);