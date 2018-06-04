#include "Application.h"
#include "MachoReader.h"
#include "DebugSymbols.h"

#include "stdio.h"
#include "Windows.h"
#include "types.h"
#include "Memory.h"
#include "Instruction.h"
#include <sstream>

using namespace std;

#include "gl/glew.h"
#include "gl/wglew.h"

#include "gl/gl.h"
#include "gl/glut.h"

#include "File.h"
#include "Task.h"
#include "BsdFile.h"
#include "BsdBasics.h"
#include "Thread.h"

namespace Interpreter
{
	CpuState cpu; //evil temp thing!
}
App* g_app = new App();

bool stop;  //even more evil temp thing!!

Logger* g_log = new Logger(); 

void App::InitTask(std::string file)
{
	MachO::Reader* m_machO = MachO::Reader::MakeReader(file);

	std::vector<std::string> args;
	args.push_back(file);
	args.push_back("-RegisterForSystemEvents"); //helps stuff

	std::vector<std::string> envp;

	m_machO->CreateStack(file, args, envp);

	g_task->GetThreads().front()->SetContext(Interpreter::cpu);
}

void App::NextTask(IpcName* bootPort) //temp crap. saves and restores context hackishly
{
	Task* saved = g_task;
	Task* newTask = new Task();
	g_task = newTask;
	Interpreter::CpuState savedCpu = Interpreter::cpu;

	g_task->SetBootstrap(bootPort); //use current bootport

	InitTask(m_apps[m_appIndex++]);

	g_task = saved;
	Interpreter::cpu = savedCpu;
}

void App::Init()
{
	m_dir = "C:/iEmu/fs"; // HARDCODED PATH!
	m_launch = false;
	
	std::string prog = "/Applications/Calculator.app/Calculator";
	std::string prog2 = "/Applications/MobileCal.app/MobileCal";
	std::string prog3 = "/Applications/MobileNotes.app/MobileNotes";
	std::string prog4 = "/Applications/MobileTimer.app/MobileTimer";
	std::string prog5 = "/Applications/Stocks.app/Stocks";
	std::string board = "/System/Library/CoreServices/SpringBoard.app/SpringBoard";
	std::string launch = "/sbin/launchd";

	m_apps.push_back("/usr/sbin/notifyd");
	m_apps.push_back(board);
	//m_apps.push_back(prog);

	m_appIndex = 0;

	g_task = new Task(); //VERY TEMP

	InitTask(m_apps[m_appIndex++]);
}

using namespace Interpreter;
