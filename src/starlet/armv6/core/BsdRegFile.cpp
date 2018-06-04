#include "BsdRegFile.h"
#include "StringUtils.h"
#include "Memory.h"

using namespace std;

int BsdRegFile::ReadP(macptr memBuffer, u32 size, s64 offset)
{
	u8* buffer = new u8[size];
	m_file.SetPtr(offset);
	int actualRead = m_file.Read(buffer, size);

	ToRamCpy(memBuffer, buffer, actualRead);
	delete[] buffer;
	return actualRead;
}

int BsdRegFile::WriteP(macptr memBuffer, u32 size, s64 offset)
{
	u8* buffer = new u8[size];
	m_file.SetPtr(offset);
	FromRamCpy(buffer, memBuffer, size);

	int actualWrite = m_file.Write(buffer, size);
	delete[] buffer;
	return actualWrite;
}

BsdRegFile::~BsdRegFile()
{
	m_file.Close();
}

void BsdRegFile::Open(FSOflag flags, FSMode fmode)
{
	bool success = m_file.Open(AbsToNativePath(m_absname), flags.GetAccessMode(), flags.GetOpenMode());
	if (!success)
		throw ErrNo::NoSuchFileDir;
}

void BsdDir::Open(FSOflag flags, FSMode fmode)
{
	bool success = m_files.Open(BsdRegFile::AbsToNativePath(m_absname) + "/*");
	if (!success)
		throw ErrNo::NoSuchFileDir;
}

int BsdDir::GetDirEntries(macptr memBuffer, u32 size, macptr newPos)
{
	int actualSize = 0;
	while (m_offset < m_files.GetNumFiles())
	{
		std::string fileName = m_files.GetFileName((u32)m_offset);
		u32 direntSize = 8 + ROUNDUP(fileName.size() + 1, 4);
		if (direntSize > size)
			break;

		Ref<BsdFile> file = BsdFile::OpenFile(m_fname + "/" + fileName);

		Ram<u32>(memBuffer) = (u32)file->GetInode();
		Ram<u16>(memBuffer + 4) = direntSize;
		Ram<u8>(memBuffer + 6) = file->GetType().AsDType(); 
		Ram<u8>(memBuffer + 7) = fileName.size();
		ToRamCpy(memBuffer + 8, fileName.c_str(), fileName.size() + 1);

		m_offset++;

		memBuffer += direntSize;
		actualSize += direntSize;
		size -= direntSize;
	}
	Ram<u32>(newPos) = (u32)m_offset; //not u64, yes?
	return actualSize;
}

int BsdDir::GetDirEntries64(macptr memBuffer, u32 size, macptr newPos)
{
	int actualSize = 0;
	while (m_offset < m_files.GetNumFiles())
	{
		std::string fileName = m_files.GetFileName((u32)m_offset);
		u32 direntSize = ROUNDUP(21 + fileName.size() + 1, 4);
		if (direntSize > size)
			break;

		Ref<BsdFile> file = BsdFile::OpenFile(m_fname + "/" + fileName);

		Ram<u64>(memBuffer) = file->GetInode();
		Ram<u64>(memBuffer + 8) = m_offset;
		Ram<u16>(memBuffer + 16) = direntSize; 
		Ram<u16>(memBuffer + 18) = fileName.size();
		Ram<u8>(memBuffer + 20) = file->GetType().AsDType();
		ToRamCpy(memBuffer + 21, fileName.c_str(), fileName.size() + 1);

		m_offset++;

		memBuffer += direntSize;
		actualSize += direntSize;
		size -= direntSize;
	}
	Ram<u64>(newPos) = m_offset; //u64?
	return actualSize;
}

string BsdRegFile::AbsToNativePath(const string& absname)
{
	return App::FsStr() + absname;
}
string BsdRegFile::ProcessToNativePath(const string& fname)
{
	return App::FsStr() + BsdFile::ProcessName(fname);
}

int MakeDir(std::string name, FSMode fmode)
{
	std::string natname = BsdRegFile::AbsToNativePath(BsdFile::ProcessName(name));
	if (File::Exists(natname))
		throw ErrNo::FileExists;
	if (!FileSys::CreateDir(natname))
		throw ErrNo::IOError;
	return 0;
}

int Rename(std::string oldName, std::string newName)
{
	std::string oldNat = BsdRegFile::AbsToNativePath(BsdFile::ProcessName(oldName, false, true));
	std::string newNat = BsdRegFile::AbsToNativePath(BsdFile::ProcessName(newName));

	if (File::IsDirectory(oldNat))
	{
		if (File::Exists(newNat))
		{
			if (!File::IsDirectory(newNat))
				throw ErrNo::NotDirectory;
			if (!FileSys::RemoveEmptyDir(newNat))
				throw ErrNo::DirectoryNotEmpty;
		}
	}
	else 
	{
		if (EndsWith(oldNat, BsdFile::SymlinkExt))
			newNat += BsdFile::SymlinkExt;
		if (File::Exists(newNat))
		{
			if (!FileSys::RemoveFile(newNat))
				throw ErrNo::IOError;
		}
	}
	if (!FileSys::Move(oldNat, newNat))
		throw ErrNo::IOError;

	return 0;
}

u32 ReadLink(std::string lname, macptr addr, u32 size)
{
	Ref<BsdFile> link = BsdFile::OpenFile(lname, FSOflag::SymLink);
	return link->Read(addr, size);
}