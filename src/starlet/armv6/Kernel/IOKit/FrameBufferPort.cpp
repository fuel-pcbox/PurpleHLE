#include "FrameBufferPort.h"

FrameBufferPort::FrameBufferPort()
{
	m_vsync = new Timer(NextVsyncTime(), Vsync, this);
	m_surfaceId = 0;
}

u32 FrameBufferPort::IoMethod(u32 id, u64* input64, u32 inSize64, u8* input8, u32 inSize8, 
									  macptr inAddrOol, u32 inSizeOol, u64* output64, u32* outSize64, 
									  u8* output8, u32* outSize8, macptr outAddrOol, u32* outSizeOol)
{
	switch (id)
	{
	case 3: //get default surface
		SIZES_64(0, 1);
		output64[0] = GetSurfaceId();
		return 0;

	case 4: //swap start
		SIZES_64(0, 1);
		output64[0] = 0xdddddddd;
		return 0;

	case 5: //swap end
		SIZES_8(128, 0); 
		return 0;

	case 6: //swap wait?
		SIZES_64(2, 0);
		return 0;

	case 7: //get id
		SIZES_64(0, 1);
		output64[0] = GetId();
		return 0;

	case 8: //get FB size
		SIZES_64(0, 2);
		output64[0] = GetSurfaceWidth(); //?
		output64[1] = GetSurfaceHeight(); //?
		return 0;

	case 9: //enable/disable vsync notifies
		SIZES_64(2, 0);
		m_vsync_func = input64[0];
		m_vsync_ref = input64[1];
		Warn("vsync e/d %x for %x", (u32)m_vsync_func, (u32)this);
		return 0;

	case 14: //enable/disable //diff meaning in v3
	case 12: //same?
		SIZES_64(1, 0);
		return 0;

	case 15: //set debug flags
		SIZES_64(2, 1);
		output64[0] = 0xacacacac; //?
		return 0;


	//case 10: //v3+
	//case 16: //v3+
	//case 19: //v3+
	default:
		Warn("FB unk method %d", id);
		return 0;
	}
}

s64 FrameBufferPort::NextVsyncTime()
{
	const s64 vsyncPeriod = 0x1000000000LL; //???
	s64 time = TimeLine::GetTime();
	return time - (time % vsyncPeriod) + vsyncPeriod;
}

void FrameBufferPort::Vsync()
{
	if (m_vsync_func && m_notifyPort)
	{
		IpcKernMsg* reply = IpcKernMsg::MakeMsg(m_notifyPort, MsgRight::CopySend, 53, false);
		IpcName* service_name = m_receiveRight->Name(); //NO?
		OUTPUT(u32, 8 + 32 + 8); //size
		OUTPUT(u32, 160); //type
		OUTPUT_STRUCT(u32 reserved; u32 func; u32 refcon; u32 service; u32 reserved2[4],
					  0, (u32)m_vsync_func, (u32)m_vsync_ref, service_name->Name(), {0, 0, 0, 0});
		OUTPUT(u32, (u32)m_vsync_ref); //???
		OUTPUT(u32, 0); //???
		OUTPUT_PORT(service_name, MsgRight::MakeSend);
		reply->Send();

		Warn("send vsync for %x", (u32)this);
	}

	delete m_vsync;
	m_vsync = new Timer(NextVsyncTime(), Vsync, this);
}