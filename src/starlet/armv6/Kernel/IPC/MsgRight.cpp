#include "MsgRight.h"
#include "IpcName.h"
#include "IpcNameSpace.h"

IpcName* MsgRight::TransferTo(IpcName* ipcName, IpcNameSpace* space, u32 name)
{
	if (m_value == None || ipcName == NULL)
		return NULL;

	switch (m_value)
	{
	case MoveReceive:   return IpcName::MoveReceive(ipcName, space, name); 		
	case MoveSend:      return IpcName::MoveSend(ipcName, space, name); 
	case MoveSendOnce:  return IpcName::MoveSendOnce(ipcName, space, name); 
	case CopySend:   	return IpcName::CopySend(ipcName, space, name); 
	case MakeSend:		return IpcName::MakeSend(ipcName, space, name); 
	case MakeSendOnce:	return IpcName::MakeSendOnce(ipcName, space, name); 

	default:
		Fail(m_value);
	}
}

IpcName* MsgRight::TransferToKernel(IpcName* ipcName)
{
	return TransferTo(ipcName, IpcNameSpace::KernelIpcSpace());
}

MsgRight MsgRight::GetTransferredType()
{
	switch (m_value)
	{
	case None:
		return None;

	case MoveReceive:
		return PortReceive;

	case MoveSend:
	case CopySend:
	case MakeSend:
		return PortSend;

	case MoveSendOnce:
	case MakeSendOnce:
		return PortSendOnce;

	default:
		Fail(m_value);
	}
}