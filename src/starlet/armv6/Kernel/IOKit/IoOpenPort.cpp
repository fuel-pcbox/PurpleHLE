#include "IoOpenPort.h"
#include "IoServicePort.h"
#include "TaskPort.h"

bool IoOpenPort::Process(IpcKernMsg* message, IpcKernMsg* reply)
{
	const int IO_CONNECT_SET_NOTIFICATION_PORT = 2818;
	const int IO_CONNECT_METHOD = 2865;
	const int IO_SERVICE_CLOSE = 2816;
	const int IO_CONNECT_MAP_MEMORY = 2819;

	switch (message->ID())
	{
	case IO_CONNECT_SET_NOTIFICATION_PORT:
		{
			INPUT(u32, notifyType);
			INPUT(u32, reference);
			INPUT_PORT(notifyPort);

			m_notifyPort = notifyPort.name;
			OUTPUT(u32, 0); //retCode
		}
		break;

	case IO_CONNECT_METHOD:
		{
			INPUT(u32, func);
			INPUT(u32, inSize64);
			INPUT_ARRAY(u64, input64, inSize64);
			INPUT(u32, inSize8);
			INPUT_ARRAY(u8, input8, ROUNDUP(inSize8, 4));
			INPUT(u32, inSizeOol); INPUT(u32, inAddrOol);
			INPUT(u32, outSize64); INPUT(u32, outSize8);
			INPUT(u32, outSizeOol); INPUT(u32, outAddrOol);

			MakeSure(inSizeOol == 0 && inAddrOol == 0 && outSizeOol == 0 && outAddrOol == 0); //not sure about order

			u64* output64 = new u64[outSize64];
			u8* output8 = new u8[outSize8];
	
			u32 retCode = IoMethod(func, input64, inSize64, input8, inSize8, inAddrOol, inSizeOol,
				                         output64, &outSize64, output8, &outSize8, outAddrOol, &outSizeOol);

			MakeSure((outSize8 & 3) == 0); //or else force align?

			OUTPUT(u32, retCode);
			OUTPUT(u32, outSize64);
			OUTPUT_ARRAY(u64, output64, outSize64);
			OUTPUT(u32, outSize8);
			OUTPUT_ARRAY(u8, output8, ROUNDUP(outSize8, 4));
			OUTPUT(u32, outSizeOol);
		}
		break;

	case IO_CONNECT_MAP_MEMORY:
		{
			INPUT_PORT(taskPort);
			INPUT_STRUCT(u32 type; u32 address; u32 size; u32 flags, vm); 

			Assert(taskPort.name && taskPort.name->SendRight());
			Task* task = dynamic_cast<TaskPort*>(taskPort.name->SendRight()->Port())->GetTask();

			Warn("io connect map size:%x", vm.size);
			//vm.size = 0x1000; //HACK!
			//Warn("io connect map HACK size:%x", vm.size);
			vm.address = task->vMem->Allocate(vm.address, vm.size, vm.flags);

			OUTPUT(u32, 0); //retCode
			OUTPUT(u32, vm.address);
			OUTPUT(u32, vm.size);
		}
		break;

	case IO_SERVICE_CLOSE:
		{
			OUTPUT(u32, 0); //??
		}
		break;

	default:
		Fail(message->ID());
	}
	return true;
}

bool IoIterator::Process(IpcKernMsg* message, IpcKernMsg* reply)
{
	const int IO_ITERATOR_NEXT = 2802;

	switch (message->ID())
	{
	case IO_ITERATOR_NEXT: 
		{
			IpcName* ipcName = NULL; //or should i throw an exception?
			if (m_index < m_contents.size())
				ipcName = IpcName::GetName(m_contents[m_index++]);
			OUTPUT_PORT(ipcName, MsgRight::MakeSend);
		}
		break;

	default:
		Fail(message->ID());
	}
	return true;
}