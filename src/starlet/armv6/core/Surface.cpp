#include "Surface.h"
#include "FrameBufferPort.h"

u32 SurfaceRootPort::IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
						              macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
						              u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol)
{
	switch (id)
	{
	case 6: //create (different in v+)
		SIZES_8(0x18, 4);
		((u32 *)output8)[0] = NewSurface((u32 *)input8);
		return 0;

	case 4: //look up 
		SIZES_64(1, 0); //what does it do?
		return 0;

	case 2: //lock
		SIZES_8(8, 0x1a8);
		MakeSure(((u32 *)input8)[1] == 1);
		GetSurface(((u32 *)input8)[0])->FillData((u32*)output8, *outSize8 / 4);
		return 0;

	case 3: //unlock
		SIZES_64(2, 0);
		MakeSure(input64[1] == 1);
		return 0;

	case 1: //release
		SIZES_64(1, 0);
		return 0;

	case 0: //create
		((u32 *)output8)[0] = NewSurface(MakeZStr((char *)input8, inSize8));
		return 0;

	//V3: case 6: //in64 - 1 (contains 1,2,3), out8 - 0x4b8
	//V3: case 16: //out8 = 0x14
	default:
		Warn("SURFACE unk method %d", id);
		return 0;
	}
}

u32 SurfaceRootPort::NewSurface(const std::string& propStr)
{
	Ref<PlistDict> dict = Plist::Load(propStr)->GetDict();
	Assert(dict.IsValid());

	CustomSurface* surface = new CustomSurface();
	
	surface->params.allocSize = (u32)dict->GetValue("IOCSBufferAllocSize")->GetInt();
	surface->params.width = (u32)dict->GetValue("IOCSBufferWidth")->GetInt();
	surface->params.height = (u32)dict->GetValue("IOCSBufferHeight")->GetInt();
	surface->params.bytesPerRow = (u32)dict->GetValue("IOCSBufferPitch")->GetInt();
	surface->params.pixelFormat = (u32)dict->GetValue("IOCSBufferPixelFormat")->GetInt();
	bool global = dict->GetValue("IOCSBufferGlobal")->GetBool(); //?
	std::string memory = dict->GetValue("IOCSBufferMemoryRegion")->GetStr(); //??

	surface->params.memBase = m_task->vMem->AnyAlloc(surface->AllocSize()); //temp

	return AddSurface(surface);
}

u32 SurfaceRootPort::NewSurface(u32* data)
{
	CustomSurface* surface = new CustomSurface();
	
	surface->params.width = data[0];
	surface->params.height = data[1];
	surface->params.pixelFormat = data[2];
	surface->params.bytesPerRow = data[3];
	surface->params.allocSize = data[4];
	surface->params.memBase = data[5];

	return AddSurface(surface);
}

std::map<u32, Surface*> SurfaceRootPort::g_surfaces;
int SurfaceRootPort::g_maxId = 1; //start from 1