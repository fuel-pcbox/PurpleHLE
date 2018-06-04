#pragma once
#include "types.h"
#include "IoServicePort.h"
#include "Plist.h"
#include "PhysicalMem.h"

class Surface 
{
public:
	static const int MaxPlanes = 16;

	virtual u32 MemBase() { return 0; }
	virtual u32 AllocSize() { return 0; }
	virtual u32 Width() { return 0; }
	virtual u32 Height() { return 0; }
	virtual u32 BytesPerRow() { return 0; }
	virtual u32 MemOffset() { return 0; }
	virtual u32 PixelFormat() { return 0; }
	virtual u32 PlaneAmount() { return 0; }

	virtual u32 PlaneWidth(int i) { return 0; }
	virtual u32 PlaneHeight(int i) { return 0; }
	virtual u32 PlaneMemOffset(int i) { return 0; }
	virtual u32 PlaneBytesPerRow(int i) { return 0; }

	void FillData(u32* data, u32 size)
	{
		MakeSure(size == 106);
		data[0] = MemBase();
		data[1] = AllocSize();
		data[2] = Width();
		data[3] = Height();
		data[4] = BytesPerRow();
		data[5] = MemOffset();
		data[6] = PixelFormat();
		data[7] = 0x0; //?
		data[8] = PlaneAmount();
		data[9] = 0x0; //?
		for (int i = 0; i < MaxPlanes; i++)
		{
			data[10 + 6*i] = PlaneWidth(i);
			data[11 + 6*i] = PlaneHeight(i);
			data[12 + 6*i] = 0x0; //?
			data[13 + 6*i] = PlaneMemOffset(i);
			data[14 + 6*i] = PlaneBytesPerRow(i);
			data[15 + 6*i] = 0x0; //?
		}
	}
};

class MbxSurface : public Surface
{
public:
	MbxSurface(macptr data, u32 size) : m_size(size), m_data(data) {}

	u32 MemBase() { return m_data; }
	u32 AllocSize() { return m_size; }

private:
	macptr m_data;
	u32 m_size;
};

class CustomSurface : public Surface
{
public:
	CustomSurface() { memset(&params, 0, sizeof(params)); } 

	u32 MemBase() { return params.memBase; }
	u32 AllocSize() { return params.allocSize; }
	u32 Width() { return params.width; }
	u32 Height() { return params.height; }
	u32 BytesPerRow() { return params.bytesPerRow; }
	u32 MemOffset() { return params.memOffset; }
	u32 PixelFormat() { return params.pixelFormat; } 
	u32 PlaneAmount() { return params.planeAmount; } //??

	u32 PlaneWidth(int i) { return params.planeWidth[i]; }
	u32 PlaneHeight(int i) { return params.planeHeight[i]; }
	u32 PlaneMemOffset(int i) { return params.planeMemOffset[i]; }
	u32 PlaneBytesPerRow(int i) { return params.planeBytesPerRow[i]; }

	struct
	{
		u32 memBase, allocSize, width, height, bytesPerRow, memOffset, pixelFormat, planeAmount;
		u32 planeWidth[MaxPlanes], planeHeight[MaxPlanes], planeMemOffset[MaxPlanes], planeBytesPerRow[MaxPlanes];
	} params;
};

class TvOutSurface : public Surface
{
public:
	virtual u32 AllocSize() { return BytesPerRow() * Height(); }
	virtual u32 Width() { return 320; }
	virtual u32 Height() { return 480; }
	virtual u32 BytesPerRow() { return 320 * 4; }
	virtual u32 PixelFormat() { return *(u32 *)"ARGB"; } 
	virtual u32 PlaneAmount() { return 1; } //??

	virtual u32 PlaneWidth(int i) { return Width(); }
	virtual u32 PlaneHeight(int i) { return Height(); }
	virtual u32 PlaneMemOffset(int i) { return MemOffset(); }
	virtual u32 PlaneBytesPerRow(int i) { return BytesPerRow(); }
};

class IpuSurface : public TvOutSurface
{
};
class LcdSurface : public TvOutSurface
{
};

class SurfaceRootPort : public IoOpenPort
{
public:
	virtual u32 IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
	  			         macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
				         u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol);
	
	static Surface* GetSurface(u32 id) { return g_surfaces[id]; }
	static u32 AddSurface(Surface* surface) 
	{ 
		g_surfaces[g_maxId] = surface; 
		return g_maxId++;
	}

private:
	u32 NewSurface(const std::string& propStr);
	u32 NewSurface(u32* data);

	static std::map<u32, Surface*> g_surfaces;
	static int g_maxId;
};
