#include "Bootstrap.h"
#include "DistNotifyPort.h"
#include "NotifyCenterPort.h"

//TEMP
#include "Application.h"

Bootstrap::Bootstrap(Task* task) 
	: m_task(task)
{
	//m_portMap["com.apple.distributed_notifications.2"] = IpcName::GetSendName(new DistNotifyPort());
	//m_portMap["com.apple.system.notification_center"] = IpcName::GetSendName(new NotifyCenterPort());
	//m_portMap["PurpleSystemEventPort"] = IpcName::GetSendName(new PurplePort());
	//m_portMap["LayerKitServer"] = IpcName::GetSendName(new LKSPort());
}

bool Bootstrap::Process(IpcKernMsg* message, IpcKernMsg* reply) 
{ 
	switch (message->ID())
	{
	case 403:  // register port
		{
			INPUT_STRING(portStr, 128);
			INPUT_PORT(port);

			m_portMap[portStr] = port.name;
			Warn("bs: register %s", portStr.c_str());

			//if (portStr == "LayerKitServer" ||
			//	portStr == "com.apple.CARenderServer")
			//	g_app->NextTask(m_receiveRight->Name());

			OUTPUT(u32, 0); //when errors?
			break;
		}
	case 404:  // lookup port
		{
			INPUT_STRING(portStr, 128);

			PortIter iter = m_portMap.find(portStr);
			if (iter == m_portMap.end())
			{
				Warn("bs: failed getting %s", portStr.c_str());
				throw MachErr::BootstrapUnknownService;
			}

			OUTPUT_PORT(iter->second, MsgRight::CopySend);
			break;
		}
	case 402:  // check in
		{
			INPUT_STRING(portStr, 128);

			PortIter iter = m_portMap.find(portStr);
			if (iter == m_portMap.end())
				throw MachErr::BootstrapUnknownService;

			if (portStr == "com.apple.system.notification_center")
				g_app->NextTask(m_receiveRight->Name());
			else 
				throw MachErr::BootstrapUnknownService;

			OUTPUT_PORT(iter->second, MsgRight::MoveReceive);
			break;
		}
	case 400:  // create server
	case 401:  // unprivileged
		{
			OUTPUT_PORT(m_task->GetBootstrap(), MsgRight::MakeSend); //like i care
			break;
		}
	case 410:  // create service
		{
			INPUT_STRING(portStr, 128);

			IpcName* name = IpcName::GetSendName(new IpcUserPort()); //keep both receive and send? or only receive?
			m_portMap[portStr] = name;

			OUTPUT_PORT(name, MsgRight::CopySend);
			break;
		}

	default:
		Fail(message->ID());
		break;
	}
	return true;
}