#pragma once
#include "types.h"
#include "FSMode.h"
#include "FSOflag.h"
#include "ErrNo.h"
#include "Ref.h"
#include "Application.h"
#include "Waitable.h"

class BsdFile : public Reffable, public Waitable
{
public:
	static Ref<BsdFile> OpenFile(std::string fname, FSOflag flags = 0, FSMode fmode = 0);
	virtual ~BsdFile();

	virtual bool Inherit() { return true; }

	virtual int ReadP(macptr memBuffer, u32 size, s64 offset) { throw ErrNo::DeviceNotConfig; }
	virtual int Read(macptr memBuffer, u32 size) { throw ErrNo::DeviceNotConfig; }
	int ReadVec(macptr vecBuffer, int count);
	virtual bool CanRead() { return true; }

	virtual int WriteP(macptr memBuffer, u32 size, s64 offset) { throw ErrNo::DeviceNotConfig; }
	virtual int Write(macptr memBuffer, u32 size) { throw ErrNo::DeviceNotConfig; }
	int WriteVec(macptr vecBuffer, int count);
	virtual bool CanWrite() { return true; }

	static int Access(std::string fname, FSMode mode) { OpenFile(fname); return 0; }

	static int Stat(std::string fname, macptr statStruct) { return OpenFile(fname)->Stat(statStruct); }
	int Stat(macptr statStruct);
	
	static int LinkStat(std::string fname, macptr statStruct) 
		{ return OpenFile(fname, FSOflag::SymLink)->Stat(statStruct); }

	static int StatFS(std::string fname, macptr statFSStruct) { return OpenFile(fname)->StatFS(statFSStruct); }
	int StatFS(macptr statFSStruct);
	
	static int Stat64(std::string fname, macptr statStruct) { return OpenFile(fname)->Stat64(statStruct); }
	int Stat64(macptr statStruct);

	static int PathConf(std::string fname, u32 type) { return OpenFile(fname)->PathConf(type); }
	int PathConf(u32 type);

	static int Select(int numFd, macptr readFd, macptr writeFd, macptr errorFd, macptr timeOut);
	
	static int LinkStat64(std::string fname, macptr statStruct) 
		{ return OpenFile(fname, FSOflag::SymLink)->Stat64(statStruct); }

	static int StatFS64(std::string fname, macptr statFSStruct) { return OpenFile(fname)->StatFS64(statFSStruct); }
	int StatFS64(macptr statFSStruct);

	virtual s64 Seek(s64 offset, u32 whence) { return 0; }

	virtual int GetDirEntries(macptr memBuffer, u32 size, macptr newPos) { return 0; }
	virtual int GetDirEntries64(macptr memBuffer, u32 size, macptr newPos) { return 0; }

	std::string Path() { return m_fname; } 
	int Control(int fd, u32 command, u32 arg);
	int IoControl(int fd, u32 command, macptr args);

	virtual u64 GetInode() { return rand() + 1; } //bad!
	virtual u64 GetSize() { return 0; }
	virtual FSMode GetType() = 0;

	static std::string ProcessName(std::string fname, bool followLink = true, bool openLink = false);

	static const std::string SymlinkExt;

protected:
	BsdFile() : m_flag(0) {}

	virtual void Open(FSOflag flags, FSMode fmode) {} // called by GetFile

	std::string m_fname;
	std::string m_absname;
	FSOflag m_flag;
};