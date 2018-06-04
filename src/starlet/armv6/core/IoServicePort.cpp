#include "IoServicePort.h"
#include "TaskPort.h"
#include "StringUtils.h"

IpcName* IoServicePort::GetMasterPort()
{
	if (!g_ioMaster) 
		g_ioMaster = BuildAll();
	return IpcName::GetName(g_ioMaster);
}

IoServicePort* IoServicePort::GetIoSubPortRec(const std::string& path)
{
	if (path.empty())
		return this;
	std::string pathComponent = BeforeFirst(path, '/');

	PortIter it = m_subPorts.find(pathComponent);
	if (it == m_subPorts.end())
		return NULL;
	else
		return it->second->GetIoSubPortRec(AfterFirst(path, '/'));
}

void IoServicePort::GetIoIteratorRec(const Ref<PlistDict>& match, std::vector<IoServicePort*> *results)
{
	//do for children
	for (PortIter it = m_subPorts.begin(); it != m_subPorts.end(); it++)
		it->second->GetIoIteratorRec(match, results);

	bool success = true;
	for (PlistDict::Iter it = match->BeginKeys(); it != match->EndKeys(); it++)
	{
		if (*it == "IOProviderClass" || *it == "IONameMatch") //hack
		{
			Ref<PlistStr> want = match->GetValue("IOProviderClass").Cast<PlistStr>();
			Ref<PlistStr> get = m_props->GetValue("IOProviderClass").Cast<PlistStr>();

			if (want.IsNull() || get.IsNull() || want->GetStr() != get->GetStr())
				success = false;
		}
		else if (*it == "IOPropertyMatch")
		{
			Ref<PlistDict> want = match->GetValue("IOPropertyMatch").Cast<PlistDict>();

			if (want.IsNull() || !want->IsSubsetOf(m_props))
				success = false;
		}
		else
			Fail(-1);
	}

	if (success)
		results->push_back(this);
}

bool IoServicePort::Process(IpcKernMsg* message, IpcKernMsg* reply)
{
	const int IO_OBJECT_CONFORMS_TO = 2801;
	const int IO_SERVICE_GET_MATCHING_SERVICES = 2804;
	const int IO_REGISTERY_ENTRY_GET_PROPERTY = 2805;
	const int IO_REGISTERY_ENTRY_FROM_PATH = 2809;
	const int IO_REGISTERY_ENTRY_GET_PROPERTIES = 2811;
	const int IO_REGISTERY_ENTRY_SET_PROPERTIES = 2828;
	const int IO_SERVICE_ADD_NOTIFICATION = 2849;
	const int IO_SERVICE_ADD_INTEREST_NOTIFICATION = 2850;
	const int IO_SERVICE_OPEN_EXTENDED = 2862; //not sure about the name

	switch (message->ID())
	{
	case IO_OBJECT_CONFORMS_TO:
		{
			INPUT(u32, offset); //unused?
			INPUT(u32, len);
			INPUT_STRING(classStr, ROUNDUP(len, 4));
			Warn("io: check conforms to %s", classStr.c_str());
			
			OUTPUT(u32, 0);
			OUTPUT(u32, 1); //bool
		}
		break;

	case IO_SERVICE_GET_MATCHING_SERVICES:
		{
			INPUT(u32, offset); //unused?
			INPUT(u32, len);
			INPUT_STRING(matchStr, ROUNDUP(len, 4));
			Warn("io: searching for %s", matchStr.c_str());
			
			OUTPUT_PORT(GetIoIterator(matchStr), MsgRight::MakeSend);
		}
		break;

	case IO_REGISTERY_ENTRY_FROM_PATH:
		{
			INPUT(u32, offset); //unused?
			INPUT(u32, len);
			INPUT_STRING(pathStr, ROUNDUP(len, 4));
			Warn("io: taking path %s", pathStr.c_str());

			OUTPUT_PORT(GetIoSubPort(pathStr), MsgRight::MakeSend);
		}
		break;

	case IO_SERVICE_ADD_NOTIFICATION:
		{
			INPUT(u32, notifyOffset); //unused?
			INPUT(u32, notifyLen);
			INPUT_STRING(notifyStr, ROUNDUP(notifyLen, 4));
			INPUT(u32, matchOffset); //unused?
			INPUT(u32, matchLen);
			INPUT_STRING(matchStr, ROUNDUP(matchLen, 4));
			INPUT(u32, refLen);
			INPUT_ARRAY(u32, refs, refLen);			
			INPUT_PORT(notifyPort); 
			Warn("io: searching for %s (w/ notify)", matchStr.c_str());

			OUTPUT_PORT(GetIoIterator(matchStr), MsgRight::MakeSend);
			//should send any new such services to notifyPort's way
		}
		break;

	case IO_SERVICE_OPEN_EXTENDED:
		{
			INPUT(u32, openType);
			INPUT(NDR, usedNDR);
			INPUT(u32, propCount);
			INPUT_PORT(task);
			INPUT_OOL(properties);

			MakeSure(propCount == 0);
			MakeSure(memcmp(&usedNDR, &TypicalNDR, sizeof(NDR)) == 0);

			TaskPort* taskPort = dynamic_cast<TaskPort*>(task.name->SendRight()->Port());
			Assert(taskPort && m_openPort);
			m_openPort->Open(taskPort->GetTask());

			OUTPUT(u32, 0); //retCode
			OUTPUT_PORT(GetOpenPort(), MsgRight::MakeSend);
		}
		break;

	case IO_SERVICE_ADD_INTEREST_NOTIFICATION:
		{
			INPUT(u32, typeOffset); //unused?
			INPUT(u32, typeLen);
			INPUT_STRING(typeStr, ROUNDUP(typeLen, 4));
			INPUT(u32, refLen);
			INPUT_ARRAY(u32, refs, refLen);			
			INPUT_PORT(notifyPort);

			OUTPUT_PORT(IpcName::GetName(new UnknownPort()), MsgRight::MakeSend);
		}
		break;

	case IO_REGISTERY_ENTRY_GET_PROPERTIES:
		{
			std::string propStr = m_props->StoreStr();
			char* bytes = new char[propStr.size()];
			memcpy(bytes, propStr.data(), propStr.size());

			OUTPUT(u32, 0); //retCode
			OUTPUT_OOL(bytes, propStr.size());
		}
		break; 
	case IO_REGISTERY_ENTRY_SET_PROPERTIES:
		{
			INPUT_OOL(propmem);
			INPUT(u32, count);

			std::string propStr((char *)propmem.memory, propmem.size);
			Ref<PlistDict> dict = Plist::Load(propStr)->GetDict();
			Assert(dict.IsValid());
			m_props->MergeDict(dict);

			OUTPUT(u32, 0); //retCode
			OUTPUT(u32, 0); //extra result?
		}
		break; 
		
	case IO_REGISTERY_ENTRY_GET_PROPERTY:
		{
			INPUT(u32, keyOffset); //unused?
			INPUT(u32, keyLen);
			INPUT_STRING(keyStr, ROUNDUP(keyLen, 4));
			Warn("io: get property %s", keyStr.c_str());

			Ref<Plist> value = m_props->GetValue(keyStr);
			if (value.IsNull())
			{
				OUTPUT(u32, 0); //num of properties?!
				OUTPUT_OOL(0, 0);
				break;
			}
			std::string valueStr = value->StoreStr();
			char* bytes = new char[valueStr.size()];
			memcpy(bytes, valueStr.data(), valueStr.size());

			OUTPUT(u32, 1); //num of properties?!
			OUTPUT_OOL(bytes, valueStr.size());
		}
		break; 

	default:
		Fail(message->ID());
	}
	return true;
}

IoServicePort* IoServicePort::g_ioMaster = NULL;