#pragma once
#include "FrameBufferPort.h"

class MbxPort : public IoOpenPort
{
public:
	static const int MbxBufferSize = 0x4000;
	MbxPort() 
	{ 
		for (int i = 0; i < 2; i++)
			m_buffer[i] = new PhysicalMem(MbxBufferSize, VMProtect::All);
		m_curBufMem = 0;
		m_bufInd = 0;
	}

	virtual u32 IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
						 macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
						 u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol);

private:
	macptr MakeBuffer(bool next);
	u32 MakeSurface(macptr data, u32 size);

	Ref<PhysicalMem> m_buffer[2];
	int m_bufInd;
	macptr m_curBufMem;
};