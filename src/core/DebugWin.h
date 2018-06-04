#pragma once
#include "resources.h"
#include "GLWin.h"

#include "..\types.h"

class DebugWin : public DebugBase
{
protected:
	void onCodeScroll( wxScrollEvent& event );
	void onCodeGo( wxCommandEvent& event );
	void onCodeGoPC( wxCommandEvent& event );
	void onDataScroll( wxScrollEvent& event );
	void onDataGo( wxCommandEvent& event );
	void onDataFormat( wxCommandEvent& event );
	void onGoTo( wxCommandEvent& event );
	void onStep( wxCommandEvent& event );
	void onRun( wxCommandEvent& event );
	void onStop( wxCommandEvent& event );
	void onThumbCheck( wxCommandEvent& event );
	void onClose( wxCloseEvent& event );
	void onRefreshSym( wxCommandEvent& event );
	void onChoiceSym( wxCommandEvent& event );
	void onChoiceFile( wxCommandEvent& event );
	GLWin* glWin;

	wxString HexUX8(u32 hex);
	wxString HexU8(u32 hex);
	wxString HexU4(u32 hex);
	wxString HexU2(u32 hex);
	wxString HexC(u32 hex);
	void SetHex(wxString name, u32 hex);
	void CheckHex(wxString name, u32 check);

	void InitDebug();
	void LoopMain(long until);
	bool LoopSeh(long until);
	void LoopInner(long until);
	void Loop(long until);
	void Update(bool code, bool data);
	wxString DumpCodeLine(u32 offset, bool thumb);
	
	static const int CODE_LINES = 42;
	static const int DATA_LINES = 20;
	
public:
	DebugWin( wxWindow* parent );
};

