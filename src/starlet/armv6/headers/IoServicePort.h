#pragma once
#include <map>
#include "Task.h"
#include "Plist.h"
#include "IoOpenPort.h"

class IoServicePort : public IpcKernelPort
{
public:
	IoServicePort(const std::string& dictStr, IoOpenPort* openPort) : m_openPort(openPort) 
	{
		openPort->SetParent(this);
		m_props = Plist::Load(_S("<dict>") + dictStr + _S("</dict>")).Cast<PlistDict>();
	}
	IoServicePort(const std::string& dictStr) : m_openPort(NULL) 
	{
		m_props = Plist::Load(_S("<dict>") + dictStr + _S("</dict>")).Cast<PlistDict>();
	}
	IoServicePort() : m_openPort(NULL) 
	{
		m_props = new PlistDict();
	}

	static IpcName* GetMasterPort();

	IpcName* GetIoSubPort(const std::string& path) 
	{ 
		return IpcName::GetName(GetIoSubPortRec(path)); 
	}
	IpcName* GetIoIterator(const std::string& matchStr) 
	{ 
		Ref<PlistDict> match = Plist::Load(matchStr).Cast<PlistDict>();
		std::vector<IoServicePort*> results;

		GetIoIteratorRec(match, &results);
		return IpcName::GetName(new IoIterator(results)); 
	}
	IpcName* GetOpenPort()
	{
		return IpcName::GetName(m_openPort);
	}

	void AddSubPort(const std::string& name, IoServicePort* port)
	{
		m_subPorts[name] = port;
		port->AddParentPort(this);
	}

	bool Process(IpcKernMsg* message, IpcKernMsg* reply);

protected:
	IoServicePort* GetIoSubPortRec(const std::string& path);
	void GetIoIteratorRec(const Ref<PlistDict>& match, std::vector<IoServicePort*> *results);
	
	void AddParentPort(IoServicePort* port) { m_parents.insert(port); }
	static IoServicePort* BuildAll();

	static IoServicePort* g_ioMaster;

	typedef std::map<std::string, IoServicePort*> PortMap;
	typedef std::map<std::string, IoServicePort*>::iterator PortIter;

	typedef std::set<IoServicePort*> PortSet;

	PortMap m_subPorts;
	PortSet m_parents;
	IoOpenPort* m_openPort;
	Ref<PlistDict> m_props;
};