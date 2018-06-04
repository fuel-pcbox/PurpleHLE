#pragma once
#include "types.h"

class FSMode
{
public:
	FSMode(u32 value) : m_value(value) {  }
	operator u32() { return m_value; }

	static const int OtherExecute = 0x1;
	static const int OtherWrite = 0x2;
	static const int OtherRead = 0x4;
	static const int OtherAll = 0x7;
	
	static const int GroupExecute = 0x8;
	static const int GroupWrite = 0x10;
	static const int GroupRead = 0x20;
	static const int GroupAll = 0x38;

	static const int OwnerExecute = 0x40;
	static const int OwnerWrite = 0x80;
	static const int OwnerRead = 0x100;
	static const int OwnerAll = 0x1c0;

	static const int AllPermissions = 0x1ff;
	static const int SomePermissions = 0x1ff &~ (OtherWrite | GroupWrite);

	static const int Sticky = 0x200;
	static const int SetGid = 0x400;
	static const int SetUid = 0x800;

	static const int Fifo = 0x1000;
	static const int CharDevice = 0x2000;
	static const int Directory = 0x4000;
	static const int BlockDevice = 0x6000;
	static const int Regular = 0x8000;
	static const int SymbolicLink = 0xa000;
	static const int Socket = 0xc000;
	static const int WhiteOut = 0xe000;

	bool CanOtherRead() { return !!(m_value & OtherRead); }
	bool CanOtherWrite() { return !!(m_value & OtherWrite); }
	bool CanOtherExecute() { return !!(m_value & OtherExecute); }
	bool CanGroupRead() { return !!(m_value & GroupRead); }
	bool CanGroupWrite() { return !!(m_value & GroupWrite); }
	bool CanGroupExecute() { return !!(m_value & GroupExecute); }
	bool CanOwnerRead() { return !!(m_value & OwnerRead); }
	bool CanOwnerWrite() { return !!(m_value & OwnerWrite); }
	bool CanOwnerExecute() { return !!(m_value & OwnerExecute); }

	bool IsSticky() { return !!(m_value & Sticky); }
	bool IsSetGid() { return !!(m_value & SetGid); }
	bool IsSetUid() { return !!(m_value & SetUid); }

	bool IsFifo() { return (GetType() == Fifo); }
	bool IsCharDevice() { return (GetType() == CharDevice); }
	bool IsDirectory() { return (GetType() == Directory); }
	bool IsBlockDevice() { return (GetType() == BlockDevice); }
	bool IsRegular() { return (GetType() == Regular); }
	bool IsSymbolicLink() { return (GetType() == SymbolicLink); }
	bool IsSocket() { return (GetType() == Socket); }
	bool IsWhiteOut() { return (GetType() == WhiteOut); }

	u32 AsDType() { return (GetType() >> 12); }

private:
	FSMode GetType() { return (m_value & TypeMask); }

	static const int TypeMask = 0xf000;

	u32 m_value;
};
