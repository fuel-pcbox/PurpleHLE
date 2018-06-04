#include "TaskPort.h"
#include "Thread.h"
#include "ThreadPort.h"
#include "Semaphore.h"
#include "phyMemPort.h"
#include "MachErr.h"

bool TaskPort::Process(IpcKernMsg* message, IpcKernMsg* reply)
{
	switch (message->ID())
	{
	case PORT_TYPE:
		{
			INPUT(u32, name);
			IpcName* portName = m_task->nameSpace->Name(name);
			if (!portName)
				throw MachErr::InvalidName;

			OUTPUT(u32, 0); //retCode
			OUTPUT(u32, portName->GetTypeMask());
		}
		break;

	case PORT_ALLOCATE:
		{
			INPUT(RightType, right);

			IpcName* newName;
			if (right == RightType::Receive)
				newName = IpcName::NewReceive(new IpcUserPort(), m_task->nameSpace);

			else if (right == RightType::PortSet)
				newName = IpcName::NewPortSet(m_task->nameSpace);

			else if (right == RightType::DeadName)
				newName = IpcName::NewDeadName(m_task->nameSpace);

			else
				Fail(right);
			
			OUTPUT(u32, 0); //retCode
			OUTPUT(u32, newName->Name());
		}
		break;

	case PORT_DESTROY:
		{
			INPUT(u32, name);

			IpcName* portName = m_task->nameSpace->Name(name);

			if (!portName)
				throw MachErr::InvalidName;

			portName->DeleteAll();

			OUTPUT(u32, 0); //retCode
		}
		break;

	case PORT_DEALLOCATE:
		{
			INPUT(u32, name);

			IpcName* portName = m_task->nameSpace->Name(name);

			if (!portName)
				throw MachErr::NoError; //returns no error
			if (!portName->SendRight() && !portName->SendOnceRight() && !portName->DeadRight())  //TODO: unsure
				throw MachErr::InvalidRight;

			portName->Delete();

			OUTPUT(u32, 0); //retCode
		}
		break;

	case PORT_MOD_REFS:
		{
			INPUT(u32, name);
			INPUT(u32, right);
			INPUT(u32, delta);

			IpcName* portName = m_task->nameSpace->Name(name);

			if (!portName)
				throw MachErr::InvalidName; 
			
			switch (right)
			{
			case RightType::Send:
				if (!portName->SendRight())
					throw MachErr::InvalidRight;
				portName->ModSendRight(delta);
				break;
				
			case RightType::Receive:
				if (!portName->ReceiveRight())
					throw MachErr::InvalidRight;
				portName->ModReceiveRight(delta);
				break;
				
			case RightType::SendOnce:
				if (!portName->SendOnceRight())
					throw MachErr::InvalidRight;
				portName->ModSendOnceRight(delta);
				break;
				
			case RightType::PortSet:
				if (!portName->PortSet())
					throw MachErr::InvalidRight;
				portName->ModPortSet(delta);
				break;
				
			case RightType::DeadName:
				if (!portName->DeadRight())
					throw MachErr::InvalidRight;
				portName->ModDeadRight(delta);
				break;

			default:
				Fail(-1);
			}

			OUTPUT(u32, 0); //retCode
		}
		break;

	case PORT_GET_SET_STATUS:
		{
			INPUT(u32, setName);

			IpcName* set = m_task->nameSpace->Name(setName);
			if (!set)
				throw MachErr::InvalidName;
			if (!set->PortSet())
				throw MachErr::InvalidRight;
			
			int count = set->PortSet()->GetNamesCount();
			u32* members = new u32[count];
			set->PortSet()->GetNames(members);

			OUTPUT_OOL(members, count * sizeof(u32));
			OUTPUT(u32, count);
		}
		break;

	case PORT_MOVE_MEMBER:
		{
			INPUT(u32, memberName);
			INPUT(u32, setName);

			IpcName* member = m_task->nameSpace->Name(memberName);
			IpcName* set = m_task->nameSpace->Name(setName);

			if (setName == 0)
			{
				if (!member)
					throw MachErr::InvalidName;
				if (!member->ReceiveRight())
					throw MachErr::InvalidRight;
				if (!member->ReceiveRight()->CountPortSets())
					throw MachErr::NotInSet;

				member->ReceiveRight()->RemoveFromPortSets();
			}
			else
			{
				if (!member || !set)
					throw MachErr::InvalidName;
				if (!member->ReceiveRight() || !set->PortSet())
					throw MachErr::InvalidRight;

				member->ReceiveRight()->RemoveFromPortSets();
				set->PortSet()->AddPort(member->ReceiveRight());
			}

			OUTPUT(u32, 0); //retCode
		}
		break;

	case PORT_REQUEST_NOTIFICATION:
		{
			INPUT(u32, name);
			INPUT(u32, msgId);
			INPUT(u32, sync);
			INPUT_PORT(notify);

			IpcName* portName = m_task->nameSpace->Name(name);
			IpcSendOnceRight* oldNotify = NULL;

			if (!portName)
				throw MachErr::InvalidName;
			if (!notify.name || !notify.name->SendOnceRight())
				throw MachErr::InvalidCapability;

			if (msgId == 0x48) //dead name notify
			{
				oldNotify = portName->ReplaceNotifyDead(notify.name->SendOnceRight());
			}
			else
				Fail(0);

			OUTPUT_PORT(oldNotify ? oldNotify->Name() : 0, MsgRight::MoveSendOnce);
		}
		break;

	case PORT_INSERT_RIGHT:
		{
			INPUT(u32, name);
			INPUT_PORT(right);

			if (!right.name)
				throw MachErr::InvalidCapability;

			right.type.TransferTo(right.name, m_task->nameSpace, name); 
			//TODO: throws inside! (NameExists, RightExists) (but remember to update Task.cpp, then.)
			
			OUTPUT(u32, 0); //retCode
		}
		break;

	case PORT_GET_ATTRIBUTES:
		{
			INPUT(u32, name);
			INPUT(u32, flavor);
			INPUT(u32, size);

			IpcName* ipcName = m_task->nameSpace->Name(name);

			if (!ipcName)
				throw MachErr::InvalidName;
			if (!ipcName->ReceiveRight())
				throw MachErr::InvalidRight;

			if (flavor == 2 && size == 10)
			{
				PortStatus portStatus = ipcName->ReceiveRight()->GetPortStatus();

				OUTPUT(u32, 0); //retCode
				OUTPUT(u32, size);
				OUTPUT(PortStatus, portStatus);
			}
			else if (flavor == 1 && size == 1)
			{					
				u32 limit = ipcName->ReceiveRight()->MsgLimit();

				OUTPUT(u32, 0); //retCode
				OUTPUT(u32, size);
				OUTPUT(u32, limit);
			}
			else
				Fail(flavor);
		}
		break;

	case PORT_SET_ATTRIBUTES:
		{
			INPUT(u32, name);
			INPUT(u32, flavor);
			INPUT(u32, size);
			INPUT_ARRAY(u32, attr, size);

			IpcName* ipcName = m_task->nameSpace->Name(name);

			if (!ipcName)
				throw MachErr::InvalidName;
			if (!ipcName->ReceiveRight())
				throw MachErr::InvalidRight;

			if (flavor == 1 && size == 1)
			{					
				ipcName->ReceiveRight()->SetMsgLimit(attr[0]);
				OUTPUT(u32, 0); //retCode
			}
			else
				Fail(flavor);
		}
		break;

	case PORT_INSERT_MEMBER:
		{
			INPUT(u32, memberName);
			INPUT(u32, setName);
			
			IpcName* member = m_task->nameSpace->Name(memberName);
			IpcName* set = m_task->nameSpace->Name(setName);

			if (!member || !set)
				throw MachErr::InvalidName;
			if (!member->ReceiveRight() || !set->PortSet())
				throw MachErr::InvalidRight;

			set->PortSet()->AddPort(member->ReceiveRight());

			OUTPUT(u32, 0); //retCode
		}
		break;
		
	case PORT_EXTRACT_MEMBER:
		{
			INPUT(u32, memberName);
			INPUT(u32, setName);
			
			IpcName* member = m_task->nameSpace->Name(memberName);
			IpcName* set = m_task->nameSpace->Name(setName);

			if (!member || !set)
				throw MachErr::InvalidName;
			if (!member->ReceiveRight() || !set->PortSet())
				throw MachErr::InvalidRight;
			if (!set->PortSet()->HasPort(member->ReceiveRight()))
				throw MachErr::NotInSet;

			set->PortSet()->RemovePort(member->ReceiveRight());

			OUTPUT(u32, 0); //retCode
		}
		break;

	case TASK_THREADS:
		{
			std::list<Thread*> threads = m_task->GetThreads();
			IpcName** threadPorts = new IpcName*[threads.size()];

			int i = 0;
			for (std::list<Thread*>::iterator it = threads.begin(); it != threads.end(); it++, i++)
				threadPorts[i] = (*it)->GetThreadSelf();

			OUTPUT_OOLPORT(threadPorts, threads.size(), MsgRight::MakeSend);
			OUTPUT(u32, threads.size());
		}
		break;

	case PORTS_LOOKUP:
		{			
			IpcName** ports = new IpcName*[3]; //VERY temp! don't do too much
			for (u32 i = 0; i < 3; i++)
				ports[i] = IpcName::GetName(new UnknownPort());

			OUTPUT_OOLPORT(ports, 3, MsgRight::MakeSend);
			OUTPUT(u32, 3);
		}
		break;

	case TASK_GET_SPECIAL_PORT:
		{
			INPUT(u32, special);

			OUTPUT_PORT(m_task->GetSpecialPort(special), MsgRight::MakeSend);
		}
		break;
		
	case TASK_SET_SPECIAL_PORT:
		{
			INPUT(u32, special);
			INPUT_PORT(port);

			m_task->SetSpecialPort(special, port.name);
			OUTPUT(u32, 0); //retCode
		}
		break;

	case THREAD_CREATE:
		{
			Thread* thread = m_task->NewThread();
			OUTPUT_PORT(thread->GetThreadSelf(), MsgRight::MakeSend);
		}
		break;

	case THREAD_CREATE_RUNNING:
		{
			INPUT(u32, flavor); 
			INPUT(u32, contextSize); 
			INPUT_ARRAY(u32, context, contextSize);

			Thread* thread = m_task->NewThread();
			thread->SetContext(flavor, context, contextSize);
			thread->Resume();

			OUTPUT_PORT(thread->GetThreadSelf(), MsgRight::MakeSend);
		}
		break;

	case SEMAPHORE_CREATE:
		{
			INPUT(u32, policy);
			INPUT(u32, init);

			IpcName* semaphoreName = IpcName::GetName(new Semaphore(policy, init));

			OUTPUT_PORT(semaphoreName, MsgRight::MakeSend);
		}
		break;

	case VM_ALLOCATE:
	case MACH_VM_ALLOCATE:
		{
			INPUT_STRUCT(u32 address; u32 size; u32 flags, vm);

			u32 newAddr = m_task->vMem->Allocate(vm.address, vm.size, vm.flags);

			OUTPUT(u32, 0); //retCode
			OUTPUT(u32, newAddr);
		}
		break;

	case VM_DEALLOCATE:
	case MACH_VM_DEALLOCATE:
		{
			INPUT_STRUCT(u32 address; u32 size, vm);

			m_task->vMem->Deallocate(vm.address, vm.size);

			OUTPUT(u32, 0); //retCode
		}
		break;

	case VM_PROTECT:
	case MACH_VM_PROTECT:
		{
			INPUT_STRUCT(u32 address; u32 size; u32 which; u32 protect, vm);

			m_task->vMem->Protect(vm.address, vm.size, vm.which != 0, vm.protect);

			OUTPUT(u32, 0); //retCode
		}
		break;

	case VM_MAP:
		{
			INPUT_STRUCT(u32 address; u32 size; u32 mask; u32 flags; u32 offset;
			                  u32 copy; u32 protect; u32 maxProtect; u32 inherit, vm);
			INPUT_PORT(port);

			Ref<PhysicalMem> memobj = NULL;
		    if (port.name != 0)
			{
				Assert(port.name->SendRight());
				PhyMemPort* memPort = dynamic_cast<PhyMemPort*>(port.name->SendRight()->Port());
				Assert(memPort != 0);
				memobj = memPort->PhyMem();
			}

			VMRegion entry(memobj, vm.offset, vm.protect, vm.maxProtect, vm.inherit);
			u32 newAddr = m_task->vMem->Map(vm.address, vm.size, vm.mask, vm.flags, vm.copy != 0, entry);

			OUTPUT(u32, 0); //retCode
			OUTPUT(u32, newAddr);
		}
		break;

	case MACH_VM_MAP:
		{
			INPUT_STRUCT(u32 address; u32 size; u32 mask; u32 flags; s64 offset;
			                  u32 copy; u32 protect; u32 maxProtect; u32 inherit, vm);
			INPUT_PORT(port);

			//why is offset 64-bit anyway? file-offset? needs changing, then

			Ref<PhysicalMem> memobj = NULL;
		    if (port.name != 0)
			{
				Assert(port.name->SendRight());
				PhyMemPort* memPort = dynamic_cast<PhyMemPort*>(port.name->SendRight()->Port());
				Assert(memPort != 0);
				memobj = memPort->PhyMem();
			}

			VMRegion entry(memobj, vm.offset, vm.protect, vm.maxProtect, vm.inherit);
			u32 newAddr = m_task->vMem->Map(vm.address, vm.size, vm.mask, vm.flags, vm.copy != 0, entry);

			OUTPUT(u32, 0); //retCode
			OUTPUT(u32, newAddr);
		}
		break;

	case MACH_MAKE_MEMORY_ENTRY_64:
		{
			INPUT(u64, size);
			INPUT(u64, address); 
			INPUT(u32, protect);
			INPUT_PORT(parent); 
			MakeSure(parent.name == 0); //?

			u32 size32 = (u32)size;
			Ref<PhysicalMem> memobj = m_task->vMem->GetObject((u32)address, &size32, protect, NULL);

			IpcName* memobjPort = IpcName::GetName(new PhyMemPort(memobj)); //temp. (need to link it back?)

			OUTPUT(u64, (u64)size32);
			OUTPUT_PORT(memobjPort, MsgRight::MakeSend);
		}
		break;

	case MACH_VM_REGION:
		{
			INPUT(u32, address); 
			INPUT(u32, flavor);
			INPUT(u32, count);
			MakeSure(flavor == 9 && count == 9);

			u32 size;
			VMRegion region = m_task->vMem->GetRegion(address, &size);

			IpcName* memobjPort = IpcName::GetName(new PhyMemPort(region.phyMem)); //temp. (need to link it back?)

			OUTPUT(u32, address);
			OUTPUT(u32, size);
			OUTPUT(u32, count);

			OUTPUT_STRUCT(u32 prot; u32 maxProt; u32 inherit; u32 shared; 
						  u32 reserved; s64 offset; u32 behaviour; u32 wired;,
						  region.protect, region.maxProtect, region.inherit,
						  true, true, //no idea how to tell
						  region.phyOffset, 0, 0);

			OUTPUT_PORT(memobjPort, MsgRight::MakeSend);
		}
		break;

	case VM_PURGABLE_CONTROL:
		{
			INPUT(u32, address);
			INPUT(u32, ctrl);
			INPUT(u32, state); //unimpl.

			OUTPUT(u32, 0); //retCode
			OUTPUT(u32, state);
		}
		break;

	default:
		Fail(message->ID());
	}
	return true;
}
