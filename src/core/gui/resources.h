///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __resources__
#define __resources__

#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/scrolbar.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DebugBase
///////////////////////////////////////////////////////////////////////////////
class DebugBase : public wxDialog 
{
	private:
	
	protected:
		wxListBox* listCode;
		wxScrollBar* scrollCode;
		wxTextCtrl* textCodeGo;
		wxButton* m_button1;
		wxCheckBox* checkThumb;
		wxButton* m_button2;
		wxButton* m_button8;
		wxChoice* choiceFile;
		wxChoice* choiceSym;
		wxStaticText* m_staticText2;
		wxTextCtrl* textReg0;
		wxStaticText* m_staticText21;
		wxTextCtrl* textReg1;
		wxStaticText* m_staticText22;
		wxTextCtrl* textReg2;
		wxStaticText* m_staticText23;
		wxTextCtrl* textReg3;
		wxStaticText* m_staticText24;
		wxTextCtrl* textReg4;
		wxStaticText* m_staticText25;
		wxTextCtrl* textReg5;
		wxStaticText* m_staticText26;
		wxTextCtrl* textReg6;
		wxStaticText* m_staticText27;
		wxTextCtrl* textReg7;
		wxStaticText* m_staticText28;
		wxTextCtrl* textReg8;
		wxStaticText* m_staticText211;
		wxTextCtrl* textReg9;
		wxStaticText* m_staticText221;
		wxTextCtrl* textReg10;
		wxStaticText* m_staticText231;
		wxTextCtrl* textReg11;
		wxStaticText* m_staticText241;
		wxTextCtrl* textReg12;
		wxStaticText* m_staticText251;
		wxTextCtrl* textReg13;
		wxStaticText* m_staticText261;
		wxTextCtrl* textReg14;
		wxStaticText* m_staticText271;
		wxTextCtrl* textReg15;
		wxCheckBox* checkRegNF;
		wxCheckBox* checkRegVF;
		wxCheckBox* checkRegZF;
		wxCheckBox* checkRegCF;
		wxCheckBox* checkRegTF;
		wxListBox* listData;
		wxScrollBar* scrollData;
		wxTextCtrl* textDataGo;
		wxButton* m_button11;
		wxChoice* choiceFormat;
		wxTextCtrl* textGoTo;
		wxButton* m_button10;
		wxButton* m_button3;
		wxButton* m_button4;
		wxButton* m_button5;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void onCodeScroll( wxScrollEvent& event ){ event.Skip(); }
		virtual void onCodeGo( wxCommandEvent& event ){ event.Skip(); }
		virtual void onThumbCheck( wxCommandEvent& event ){ event.Skip(); }
		virtual void onCodeGoPC( wxCommandEvent& event ){ event.Skip(); }
		virtual void onRefreshSym( wxCommandEvent& event ){ event.Skip(); }
		virtual void onChoiceFile( wxCommandEvent& event ){ event.Skip(); }
		virtual void onChoiceSym( wxCommandEvent& event ){ event.Skip(); }
		virtual void onDataScroll( wxScrollEvent& event ){ event.Skip(); }
		virtual void onDataGo( wxCommandEvent& event ){ event.Skip(); }
		virtual void onDataFormat( wxCommandEvent& event ){ event.Skip(); }
		virtual void onGoTo( wxCommandEvent& event ){ event.Skip(); }
		virtual void onStep( wxCommandEvent& event ){ event.Skip(); }
		virtual void onRun( wxCommandEvent& event ){ event.Skip(); }
		virtual void onStop( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		DebugBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxPoint( 100,100 ), const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~DebugBase();
	
};

#endif //__resources__
