#pragma once
#include "types.h"
#include <map>
#include <set>
#include "BsdFile.h"

class Devices
{
public:
	Devices();
	~Devices();
	void AddDevice(std::string path, Ref<BsdFile> device);
	void RemoveDevice(std::string path);
	Ref<BsdFile> GetDevice(std::string path);

private:
	std::map<std::string, Ref<BsdFile> > m_deviceMap;
};

extern Devices* g_devices;

/*class OpenFiles
{
public:
	void AddOpen(std::string path, Ref<BsdFile> device);
	void RemoveOpen(std::string path, Ref<BsdFile> device);
	bool IsLocked(std::string path, bool exclusive, s64 start, s64 size);

private:
	typedef std::map<std::string, std::set<Ref<BsdFile> > > OpenMap;
	typedef std::set<Ref<BsdFile> > OpenSet;
	typedef std::set<Ref<BsdFile> >::iterator OpenSetIter;

	OpenMap m_openMap;
};

extern OpenFiles* g_openFiles;*/