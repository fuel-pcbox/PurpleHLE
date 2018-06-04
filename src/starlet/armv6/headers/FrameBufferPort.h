#pragma once
#include "Task.h"
#include "IoServicePort.h"
#include "Surface.h"
#include "Memory.h"
#include "Timer.h"

class FrameBufferPort : public IoOpenPort
{
public:

	virtual u32 IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
						 macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
						 u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol);

	virtual u64 GetId() = 0; //?

	FrameBufferPort();
	u64 GetSurfaceId() { return m_surfaceId; } 

	u64 GetSurfaceWidth() { return GetSurface()->Width(); }
	u64 GetSurfaceHeight() { return GetSurface()->Height(); }

	Surface* GetSurface()
	{
		if (!SurfaceRootPort::GetSurface(m_surfaceId))
		{
			Surface* surface = NewSurface();
			m_surfaceId = SurfaceRootPort::AddSurface(surface);
		}
		return SurfaceRootPort::GetSurface(m_surfaceId);
	}

	static void Vsync(void* data)
	{
		FrameBufferPort* port = static_cast<FrameBufferPort*>(data);
		port->Vsync();
	}
	void Vsync();
	s64 NextVsyncTime();

private:
	virtual Surface* NewSurface() = 0;
	u32 m_surfaceId;
	Timer* m_vsync;
	u64 m_vsync_func, m_vsync_ref;
};

class LcdPort : public FrameBufferPort
{
public:
	u64 GetId() { return 0x4141414; } //?
	Surface* NewSurface() { return new LcdSurface(); }
	LcdPort() { GetSurface(); }
};
class TvOutPort : public FrameBufferPort
{
public:
	u64 GetId() { return 0x4545454; } //?
	Surface* NewSurface() { return new TvOutSurface(); }
	TvOutPort() { GetSurface(); }
};
class IpuPort : public FrameBufferPort
{
public:
	u64 GetId() { return 0x1a1a1a1; } //?
	Surface* NewSurface() { return new IpuSurface(); }
	IpuPort() { GetSurface(); }
};
