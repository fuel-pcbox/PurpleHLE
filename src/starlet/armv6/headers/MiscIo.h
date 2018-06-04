#pragma once
#include "Task.h"
#include "IoServicePort.h"
#include "Surface.h"
#include "Memory.h"

class AesPort : public IoOpenPort
{
public:
	virtual u32 IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
						 macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
						 u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol)
	{
		switch (id)
		{
		default:
			Warn("AES unk method %d", id);
			return 0;
		}
	}
};

class PMRootPort : public IoOpenPort
{
public:
	virtual u32 IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
						 macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
						 u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol)
	{
		switch (id)
		{
		default:
			Warn("PMROOT unk method %d", id);
			return 0;
		}
	}
};

class PowerSrcPort : public IoOpenPort
{
public:
	virtual u32 IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
						 macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
						 u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol)
	{
		switch (id)
		{
		default:
			Warn("POWER unk method %d", id);
			return 0;
		}
	}
};

class BackLightPort : public IoOpenPort
{
public:
	virtual u32 IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
						 macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
						 u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol)
	{
		switch (id)
		{
		default:
			Warn("LIGHT unk method %d", id);
			return 0;
		}
	}
};

class MultiTouchPort : public IoOpenPort
{
public:
	virtual u32 IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
						 macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
						 u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol)
	{
		switch (id)
		{
		case 2:
			Warn("MT unk method %d", id);
			for (int i = 0; i < inSize8; i++)
				output8[i] = input8[i];
			return 0;

		default:
			Warn("MT unk method %d", id);
			return 0;
		}
	}
};