#pragma once
#include <string>
#include <vector>

namespace MachO { class Reader; }

class App;
class IpcName;
extern App* g_app;

class App
{
public:
	void Init();

	void App::InitTask(std::string file);
	void App::NextTask(IpcName* bootPort);

	MachO::Reader* m_machO;	

	static std::string FsStr() { return g_app->m_dir; }
	static bool Launch() { return g_app->m_launch; }

	std::vector<std::string> m_apps;
	int m_appIndex;
	std::string m_dir;
	bool m_launch;
};
