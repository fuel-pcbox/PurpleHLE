#include "MiscFunc.h"
#include "CpuState.h"
#include "MemBlock.h"
#include "Application.h"
#include "PlistBReader.h"
#include "NibReader.h"
#include "File.h"

using namespace Interpreter;

void UIApplicationMain()
{
	Plist::Node* infoTop = app->m_infoPlist->GetTop();
	Plist::StringNode nibNameKey(L"NSMainNibFile");
	Plist::Node* nibName = infoTop->GetByDict(nibNameKey);
	std::wstring nibStr = nibName->GetString() + L".nib";

	File file;
	file.OpenRead(app->m_dir + nibStr);
	int size = file.GetSize();
	
	std::vector<u8> dataVec(size);
	file.Read(&dataVec.front(), size);
	file.Close();
	
	Nib::Reader* nibReader = new Nib::Reader(dataVec);
	Plist::Node* nibTop = nibReader->GetTop();	

	Fail(0);
}
