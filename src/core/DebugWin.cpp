#include "DebugWin.h"
#include "GLWin.h"
#include "dis.h" 

#include "..\Memory.h"
#include "..\CpuState.h"
#include "..\Application.h"
#include "..\DebugSymbols.h"
#include "..\Thread.h"
#include "..\ErrNo.h"

DebugWin::DebugWin( wxWindow* parent )
:
DebugBase( parent )
{
	wxFrame *frame = new wxFrame(this, -1, _T("win"), wxPoint(1, 1));
	frame->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
    glWin = new GLWin(frame);
	sizer->Add(glWin, 1, wxEXPAND, 0);

	frame->SetSizer(sizer);
	frame->Layout();
	sizer->Fit(frame);

	frame->Show();

	InitDebug();
}

wxString DebugWin::HexUX8(u32 hex)
{
	return _T("0x") + HexU8(hex);
}
wxString DebugWin::HexU8(u32 hex)
{
	return wxString::Format(_T("%08x"), hex);
}
wxString DebugWin::HexU4(u32 hex)
{
	return wxString::Format(_T("%04x"), hex);
}
wxString DebugWin::HexU2(u32 hex)
{
	return wxString::Format(_T("%02x"), hex);
}
wxString DebugWin::HexC(u32 hex)
{
	return wxString::Format(_T("%c"), hex);
}

void DebugWin::InitDebug()
{
	g_app->Init();
	
	scrollCode->SetScrollbar(0, CODE_LINES - 2, VirtualMem::VramSize / 8, CODE_LINES - 2, true);
	scrollData->SetScrollbar(0, DATA_LINES - 2, VirtualMem::VramSize / 8, DATA_LINES - 2, true);

    onCodeGoPC( wxCommandEvent() );
    Update(true, true);
}
	
void DebugWin::onClose( wxCloseEvent& event )
{
	Destroy();
}

void DebugWin::onCodeScroll( wxScrollEvent& event )
{
	Update(true, false);
}

void DebugWin::onThumbCheck( wxCommandEvent& event )
{
	if (checkThumb->GetValue())
	{
		scrollCode->SetRange(scrollCode->GetRange() * 2);
		scrollCode->SetThumbPosition(scrollCode->GetThumbPosition() * 2);
	}
	else
	{
		scrollCode->SetRange(scrollCode->GetRange() / 2);
		scrollCode->SetThumbPosition(scrollCode->GetThumbPosition() / 2);
	}
	Update(true, false);
}

void DebugWin::onCodeGo( wxCommandEvent& event )
{
	long dest;
	if (textCodeGo->GetValue().ToLong(&dest, 16))
	{
		if (checkThumb->GetValue())
			scrollCode->SetThumbPosition(dest / 2);
		else
			scrollCode->SetThumbPosition(dest / 4);
		Update(true, false);
	}
}

void DebugWin::onCodeGoPC( wxCommandEvent& event )
{
	long dest = Interpreter::cpu.nia;
	if ((u8)checkThumb->GetValue() != Interpreter::cpu.thumb)
	{
		checkThumb->SetValue(Interpreter::cpu.thumb & 1);
		onThumbCheck(event);
	}

	if (checkThumb->GetValue())
		scrollCode->SetThumbPosition(dest / 2);
	else
		scrollCode->SetThumbPosition(dest / 4);
	Update(true, false);
}

void DebugWin::onDataScroll( wxScrollEvent& event )
{
	Update(false, true);
}

void DebugWin::onDataGo( wxCommandEvent& event )
{
	long dest;
	if (textDataGo->GetValue().ToLong(&dest, 16))
	{
		scrollData->SetThumbPosition(dest / 8);
		Update(false, true);
	}
}

void DebugWin::onDataFormat( wxCommandEvent& event )
{
	Update(false, true);
}

void DebugWin::onGoTo( wxCommandEvent& event )
{
	long until;
	if (textGoTo->GetValue().ToLong(&until, 16))
		LoopMain(until);
	onCodeGoPC(event);
	Update(true, true);
}

void DebugWin::onStep( wxCommandEvent& event )
{
	Interpreter::ExecuteOne();
	onCodeGoPC(event);
	Update(true, true);
}

void DebugWin::onRun( wxCommandEvent& event )
{
	LoopMain(0);
	onCodeGoPC(event);
	Update(true, true);
}

void DebugWin::onStop( wxCommandEvent& event )
{
	// TODO: Implement onStop
}

void DebugWin::onRefreshSym( wxCommandEvent& event )
{
	std::vector<std::string> files = MachO::DebugSymbols::GetFileList();

	wxArrayString wxfiles;
	for (u32 i = 0; i < files.size(); i++)
		wxfiles.Add(wxString::FromAscii(files[i].c_str()));

	choiceFile->Clear();
	choiceFile->Append(wxfiles);
}

void DebugWin::onChoiceFile( wxCommandEvent& event )
{
	std::string file = choiceFile->GetStringSelection().mb_str();	
	std::vector<std::string> syms = MachO::DebugSymbols::GetSymbolList(file);

	wxArrayString wxsyms;
	for (u32 i = 0; i < syms.size(); i++)
		wxsyms.Add(wxString::FromAscii(syms[i].c_str()));

	choiceSym->Clear();
	choiceSym->Append(wxsyms);
}

void DebugWin::onChoiceSym( wxCommandEvent& event )
{
	std::string symbol = choiceSym->GetStringSelection().mb_str();
	u32 addr = MachO::DebugSymbols::GetAddrOf(symbol);
	textCodeGo->SetValue(HexU8(addr));
	onCodeGo(event);
	Update(true, false);
}

wxString DebugWin::DumpCodeLine(u32 offset, bool thumb)
{
	u32 code;
	if (thumb)
		code = ::SafeRamRead<u16>(offset) + (::SafeRamRead<u16>(offset + 2) << 16);
	else
		code = ::SafeRamRead<u32>(offset);
	wxString str = ArmDisasm(code, offset, thumb);
	wchar_t here = (Interpreter::cpu.nia == offset) ? '>' : ' ';
    wxString address = HexUX8(offset);
    return address + _T(" ") + here + _T(": ") + str;
}

void DebugWin::Update(bool updateCode, bool updateData)
{
	if (updateCode)
	{
		listCode->Clear();
		u32 code_i = scrollCode->GetThumbPosition();
		bool thumb = checkThumb->IsChecked();

		for (u32 i = 0; i < CODE_LINES; i++)
		{
			u32 offset = (code_i + i) * (thumb ? 2 : 4);
			wxString disasm = DumpCodeLine(offset, thumb);
			wxString symbol = wxString::FromAscii(MachO::DebugSymbols::GetSymbolAt(offset).c_str());

			listCode->Append(disasm + _T("  ") + symbol);
		}
	}

	if (updateData)
	{
		listData->Clear();
		u32 data_i = scrollData->GetThumbPosition();

		for (u32 i = 0; i < DATA_LINES; i++)
		{
			u32 offset = (data_i + i) * 8;

			wxString datastr = HexUX8(offset) + _T(" : ");

			if (choiceFormat->GetCurrentSelection() == 1)
			{
				for (u32 j = 0; j < 2; j++)
					datastr += _T("    ") + HexU8(::SafeRamRead<u32>(offset + j * 4));
			}
			else if (choiceFormat->GetCurrentSelection() == 0)
			{
				for (u32 j = 0; j < 8; j++)
					datastr += _T(" ") + HexU2(::SafeRamRead<u8>(offset + j));
			}
			else
			{
				for (u32 j = 0; j < 8; j++)
					datastr += _T(" ") + HexC(::SafeRamRead<u8>(offset + j));
			}

			listData->Append(datastr);
		}

		textReg0->SetValue(HexU8(Interpreter::cpu.gpr[0]));
		textReg1->SetValue(HexU8(Interpreter::cpu.gpr[1]));
		textReg2->SetValue(HexU8(Interpreter::cpu.gpr[2]));
		textReg3->SetValue(HexU8(Interpreter::cpu.gpr[3]));
		textReg4->SetValue(HexU8(Interpreter::cpu.gpr[4]));
		textReg5->SetValue(HexU8(Interpreter::cpu.gpr[5]));
		textReg6->SetValue(HexU8(Interpreter::cpu.gpr[6]));
		textReg7->SetValue(HexU8(Interpreter::cpu.gpr[7]));
		textReg8->SetValue(HexU8(Interpreter::cpu.gpr[8]));
		textReg9->SetValue(HexU8(Interpreter::cpu.gpr[9]));
		textReg10->SetValue(HexU8(Interpreter::cpu.gpr[10]));
		textReg11->SetValue(HexU8(Interpreter::cpu.gpr[11]));
		textReg12->SetValue(HexU8(Interpreter::cpu.gpr[12]));
		textReg13->SetValue(HexU8(Interpreter::cpu.gpr[13]));
		textReg14->SetValue(HexU8(Interpreter::cpu.gpr[14]));
		textReg15->SetValue(HexU8(Interpreter::cpu.nia));
		checkRegCF->SetValue(Interpreter::cpu.cf ? true : false);
		checkRegNF->SetValue(Interpreter::cpu.nf ? true : false);
		checkRegVF->SetValue(Interpreter::cpu.vf ? true : false);
		checkRegZF->SetValue(Interpreter::cpu.zf ? true : false);
		checkRegTF->SetValue(Interpreter::cpu.thumb ? true : false);
	}
}

void DebugWin::LoopMain(long until)
{
	bool okay = LoopSeh(until);
	if (!okay)
		wxMessageBox(_T("Memory Fault"));
}

bool DebugWin::LoopSeh(long until)
{
	__try
	{
		LoopInner(until);
	}
	__except(GetExceptionInformation(), EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
	return true;
}

void DebugWin::LoopInner(long until)
{
	stop = false;
	try
	{
		while (!stop && Interpreter::cpu.nia != until)
		{
			ActiveThreads::ChangeThread(); //hardly the place
			for (int i = 0; i < 1000000; i++)
			{
				Interpreter::ExecuteOne();
				if (stop || Interpreter::cpu.nia == until) break;
			}
			TimeLine::AdvanceLineBy(1000000);
		}
	}
	catch (MyError& err)
	{
		wxMessageBox(_T("Failure"));
	}
	catch (MachErr::Type& err)
	{
		wxMessageBox(_T("Unhandled MachErr"));
	}
	catch (ErrNo::Type& err)
	{
		wxMessageBox(_T("Unhandled ErrNo"));
	}
	catch (...)
	{
		wxMessageBox(_T("Unknown"));
	}
}