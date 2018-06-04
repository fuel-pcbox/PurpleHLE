#include "BsdFile.h"
#include "StringUtils.h"

#include "BsdBasics.h"
#include "BsdRegFile.h"
#include "Devices.h"
#include "FdSpace.h"
#include "Thread.h"

using namespace std;

STRICT_ALIGN
struct StatStruct
{
	u32 device, inode;
	u16 mode, hardLinks;
	u32 uid, gid, specialDev;
	u32 aSec, aNsec, mSec, mNsec, cSec, cNsec;
	u64 size, blocks;
	u32 optBlock, flags, gen;
};
struct StatStruct64
{
	u32 device;
	u16 mode, hardLinks;
	u64 inode;
	u32 uid, gid, specialDev;
	u32 aSec, aNsec, mSec, mNsec, cSec, cNsec;
	u32 btSec, btNsec;
	u64 size, blocks;
	u32 optBlock, flags, gen;
};

struct StatFSStruct //probably too many or incorrect sizes ( u32 -> u64? )
{
	u16 typeOrReserved, typeOrReserved2; //huh?
	u32 blockSize, optBlockSize, blocks, freeBlocks, availBlocks, files, freeFiles;
	u32 fsid[2], owner;
	u16 reserved1, typeOrReserved3;
	u32 typeOrReserved4, reserved2[2];
	char name[15];
	char dirMount[90], mountFS[90];
};
struct StatFSStruct64 
{
	u32 blockSize, optBlockSize;
	u64 blocks, freeBlocks, availBlocks, files, freeFiles;
	u32 fsid[2], owner;
	u32 type, flags, subtype;
	char name[16];
	char dirMount[1024], mountFS[1024];
};
POP_ALIGN

string BsdFile::ProcessName(string fname, bool followLink, bool openLink)
{
	string path = fname;
	string absPath /*= currentDir*/;
	if (StartsWith(fname, "/"))
		absPath = "";
	do
	{
		string dir = BeforeFirst(path, '/');
		path = AfterFirst(path, '/');

		if (dir == string(".") || dir.empty())
			; //do nothing
		else if (dir == string(".."))
			absPath = BeforeLast(absPath, '/');
		else
		{
			absPath += string("/") + dir;
			if (File::IsDirectory(App::FsStr() + absPath))
				; //got directory, always okay
			else if (File::IsFile(App::FsStr() + absPath))
			{
				if (!path.empty()) //wanted directory, got file
					throw ErrNo::NotDirectory;
				//else, okay
			}
			else if (g_devices->GetDevice(absPath).IsValid())
			{
				if (!path.empty()) //wanted directory, got device
					throw ErrNo::NotDirectory; //???
				//else, okay
			}
			else if (File::IsFile(App::FsStr() + absPath + SymlinkExt))
			{
				if (path.empty()) //wanted file, got symlink
				{
					if (openLink)
					{
						absPath += SymlinkExt;
					}
					else if (followLink)
					{
						path = File::ReadStr(App::FsStr() + absPath + SymlinkExt);
						absPath = BeforeLast(absPath, '/');
						if (StartsWith(path, "/"))
							absPath = "";
					}
					else
						throw ErrNo::SymlinkLoop;
				}
				else //wanted directory, got symlink
				{
					path = File::ReadStr(App::FsStr() + absPath + SymlinkExt) + "/" + path;
					absPath = BeforeLast(absPath, '/');
					if (StartsWith(path, "/"))
						absPath = "";
				}
			}
			else if (!path.empty()) //wanted directory, got nothing
				throw ErrNo::NoSuchFileDir;
			//else, okay
		}
	}
	while (!path.empty());

	return absPath;
}

Ref<BsdFile> BsdFile::OpenFile(string fileName, FSOflag flags, FSMode fmode)
{
	string qName = ProcessName(fileName, !flags.DontFollowLinks(), flags.OpenLinks());

	Ref<BsdFile> file = g_devices->GetDevice(qName);
	if (file.IsNull())
	{
		if (flags.WantDirectory() || File::IsDirectory(App::FsStr() + qName)) //don't like this
			file = new BsdDir();
		else
			file = new BsdRegFile();
	}
	//else
	//	file = file->Copy();  //!!!?!?!??!?!
	file->m_fname = fileName;
	file->m_absname = qName;
	file->m_flag = flags;
	file->Open(flags, fmode);

	//g_openFiles->AddOpen(file->m_absname, file);

	return file;
}
	
BsdFile::~BsdFile()
{
	//g_openFiles->RemoveOpen(m_absname, this);
}

int BsdFile::ReadVec(macptr vecBuffer, int count)
{
	int total = 0;
	for (int i = 0; i < count; i++)
	{
		macptr base = Ram<u32>(vecBuffer + i*8);
		u32 size = Ram<u32>(vecBuffer + i*8 + 4);
		total += Read(base, size);
		if (!CanRead())
			break;
	}
	return total;
}

int BsdFile::WriteVec(macptr vecBuffer, int count)
{
	int total = 0;
	for (int i = 0; i < count; i++)
	{
		macptr base = Ram<u32>(vecBuffer + i*8);
		u32 size = Ram<u32>(vecBuffer + i*8 + 4);
		total += Write(base, size);
		if (!CanWrite())
			break;
	}
	return total;
}

int BsdFile::Stat(macptr statStruct)
{
	StatStruct stat;
	memset(&stat, 0, sizeof(StatStruct));

	stat.device = 1; //??
	stat.inode = (u32)GetInode();
	stat.size = GetSize();
	stat.optBlock = 0x200; //??
	stat.blocks = stat.size / stat.optBlock;
	stat.mode = GetType() | FSMode::SomePermissions;
	stat.hardLinks = 1;

	ToRamCpy(statStruct, &stat, sizeof(StatStruct));
	return 0;
}

int BsdFile::Stat64(macptr statStruct)
{
	StatStruct64 stat;
	memset(&stat, 0, sizeof(StatStruct64));

	stat.device = 1; //??
	stat.inode = GetInode();
	stat.size = GetSize();
	stat.optBlock = 0x200; //??
	stat.blocks = stat.size / stat.optBlock;
	stat.mode = GetType() | FSMode::SomePermissions;
	stat.hardLinks = 1;

	ToRamCpy(statStruct, &stat, sizeof(StatStruct64));
	return 0;
}

int BsdFile::StatFS(macptr statFSStruct)
{
	StatFSStruct stat;
	memset(&stat, 0, sizeof(StatFSStruct));

	//strcpy(stat.name, "hfsx"); //???

	//ToRamCpy(statFSStruct, &stat, sizeof(StatFSStruct));
	return 0;
}

int BsdFile::StatFS64(macptr statFSStruct)
{
	StatFSStruct64 stat;
	memset(&stat, 0, sizeof(StatFSStruct64));

	//strcpy(stat.name, "hfsx"); //???

	//ToRamCpy(statFSStruct, &stat, sizeof(StatFSStruct));
	return 0;
}

int BsdFile::PathConf(u32 type)
{
	switch (type)
	{
	case 6: 
		return 0x100; //or anything, really

	default:
		Fail(-1);
	}
}

int BsdFile::Control(int fd, u32 command, u32 arg)
{
	const int GET_CLOSE_EXEC = 1;
	const int SET_CLOSE_EXEC = 2;
	const int GET_O_FLAG = 3;
	const int SET_O_FLAG = 4;
	const int GET_LOCK = 7; 
	const int SET_LOCK = 8; 

	switch (command)
	{
	case GET_CLOSE_EXEC:
		return g_task->fdSpace->GetCOE(fd);

	case SET_CLOSE_EXEC: 
		g_task->fdSpace->SetCOE(fd, (arg & 1)); 
		return 0;

	case GET_O_FLAG:
		return m_flag;

	case SET_O_FLAG:
		m_flag = arg;
		return 0;

	case GET_LOCK:
	case SET_LOCK:
		Warn("lock! %s", m_absname.c_str());
		return 0; //BAD!

	case 44: //F_RDADVISE
	case 59: //F_ADDSIGS
		return 0; //??

	case 61: //??
		return 0;

	default:
		Fail(command);
	}
	return 0;
}

int BsdFile::IoControl(int fd, u32 command, macptr args)
{
	switch (command)
	{
	case 0x4004667a: //get file type
		Ram<u32>(args) = GetType().AsDType();
		break;
		
	case 0x800c6502: //????
		break;

	default:
		Fail(command);
	}
	return 0;
}

int BsdFile::Select(int numFd, macptr readFd, macptr writeFd, macptr errorFd, macptr timeOut)
{
	int ready = 0;
	for (int i = 0; i < numFd; i++)
	{
		if (readFd && ((Ram<u8>(readFd + (i/8)) >> (i%8)) & 1)) //get bit
		{
			if (g_task->fdSpace->GetFd(i)->CanRead())
				ready++;
			else
				Ram<u8>(readFd + (i/8)) &= ~(1 << (i%8)); //clear bit
		}
		
		if (writeFd && ((Ram<u8>(writeFd + (i/8)) >> (i%8)) & 1)) //get bit
		{
			if (g_task->fdSpace->GetFd(i)->CanWrite())
				ready++;
			else
				Ram<u8>(writeFd + (i/8)) &= ~(1 << (i%8)); //clear bit
		}
		
		if (errorFd && ((Ram<u8>(errorFd + (i/8)) >> (i%8)) & 1)) //get bit
		{
			//if (FdSpace::GetFd(i)->HasError())
			Ram<u8>(errorFd + (i/8)) &= ~(1 << (i%8)); //clear bit
		}
	}

	if (ready == 0)
	{
		Warn("SELECT WITH NOTHING READY");
		ActiveThreads::Current()->Terminate();
	}

	return ready;
}

/*FLock BsdFile::GetLocalLock(FLock lock)
{
}

FLock BsdFile::GetLock(FLock lock)
{
}

bool BsdFile::SetLock(FLock lock)
{
	Assert(lock.start == 0 && lock.length == 1); //ugh.

	if (lock.lock)
	{
		FLock exists = GetLock(lock);
		if (exists.lock &&
	}
}*/

const std::string BsdFile::SymlinkExt = ".symlink";