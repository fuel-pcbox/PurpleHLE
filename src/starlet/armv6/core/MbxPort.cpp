#include "MbxPort.h"
#include "Surface.h"
#include "PhysicalMem.h"
#include "Memory.h"
#include "Mbx2D.h"

u32 MbxPort::IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
				      macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
			    	  u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol)
{
	switch (id)
	{
	case 0: //nop?
		return 0;

	case 2: //v2+
		SIZES_64(0, 1);
		output64[0] = 0xd1d1d1d1; //...memory. sigh.
		return 0;

	case 3: //set surface?
		SIZES_64(1, 1);
		output64[0] = input64[0]; //NO!?
		return 0;

	case 4: //release surface?
		SIZES_64(1, 0);
		return 0;

	case 6: //get first buffer (diff meaning in V3)
		SIZES_64(0, 1);
		output64[0] = MakeBuffer(false);
		return 0;

	case 7: //get next buffer ?
		SIZES_64(0, 1);
		output64[0] = MakeBuffer(true);
		return 0;

	case 10: //finish surface?
		SIZES_64(1, 0);
		return 0;

	case 21: //make surface? no...
		SIZES_64(2, 2);
		output64[0] = MakeSurface((u32)input64[0], (u32)input64[1]);
		output64[1] = 0xabababab; //what's this?
		return 0;

	default:
		Warn("MBX unk method %d", id);
		return 0;
	}
}

macptr MbxPort::MakeBuffer(bool next)
{
	u32 bufSize = MbxBufferSize;
	u32 bufStreamSize = bufSize - 0x10;

	//remove old buffer from memory and operate on its command stream
	if (next && m_curBufMem)
	{
		u32 streamSize = Ram<u32>(m_curBufMem + bufStreamSize + 0x4); //in bytes
		m_task->vMem->Deallocate(m_curBufMem, bufSize); 

		u32* memory = (u32 *)m_buffer[m_bufInd^1]->GetMemory(0);
		::MbxCommandStream(memory, memory + streamSize / 4);
	}

	// add new buffer to memory
	VMRegion entry(m_buffer[m_bufInd], 0, VMProtect::All, VMProtect::All, VMInherit::None);
	u32 bufStart = m_task->vMem->Map(0, bufSize, 0, VMFlags::Anywhere, false, entry); 

	// initialize new buffer
	u32 bufCtrlStart = bufStart + bufSize - 0x10;

	Ram<u32>(bufCtrlStart) = bufStreamSize; //CS size
	Ram<u32>(bufCtrlStart + 0x4) = 0; //CS offset
	Ram<u32>(bufCtrlStart + 0x8) = 0xbabababa; //?????
	Ram<u32>(bufCtrlStart + 0xc) = bufStart; //CS ptr

	m_curBufMem = bufStart;
	m_bufInd ^= 1;
	return bufCtrlStart;
}
	
u32 MbxPort::MakeSurface(macptr data, u32 size)
{
	MbxSurface* surface = new MbxSurface(data, size);
	return SurfaceRootPort::AddSurface(surface);
}