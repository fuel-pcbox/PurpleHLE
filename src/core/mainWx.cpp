#include "Main.h"
#include "Resources.h"
#include "DebugWin.h"

IMPLEMENT_APP(Main)

bool Main::OnInit()
{
	DebugWin* debug = new DebugWin(NULL);
	debug->Show();

	return true;
}