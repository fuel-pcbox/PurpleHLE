#include "BsdBasics.h"
#include "Devices.h"

void Devices::AddDevice(std::string path, Ref<BsdFile> device)
{
	m_deviceMap[path] = device;
}

void Devices::RemoveDevice(std::string path)
{
	m_deviceMap.erase(path);
}

Ref<BsdFile> Devices::GetDevice(std::string path)
{
	if (m_deviceMap.find(path) != m_deviceMap.end())
		return m_deviceMap[path];
	else
		return NULL;
}

Devices::Devices()
{
	AddDevice("/dev/urandom", new BsdRandom());
	AddDevice("/dev/random", new BsdRandom());
	AddDevice("/dev/zero", new BsdZero());
	AddDevice("/dev/null", new BsdNull());
}

Devices::~Devices()
{
	//?
}

Devices* g_devices = new Devices();
/*
void OpenFiles::AddOpen(std::string path, Ref<BsdFile> device)
{
	m_openMap[path].insert(device);
}
void OpenFiles::RemoveOpen(std::string path, Ref<BsdFile> device)
{
	m_openMap[path].remove(device);
}
bool OpenFiles::IsLocked(std::string path, bool exclusive, s64 start, s64 size)
{
	const OpenSet& set = m_openMap[path];
	for (OpenSetIter it = set.begin(); it != set.end(); it++)
		if ((*it)->IsLocked(exclusive, start, end))
			return true;
	return false;
}

OpenFiles* g_openFiles = new OpenFiles();*/