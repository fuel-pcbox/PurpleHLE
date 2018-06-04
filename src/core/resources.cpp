///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "resources.h"

///////////////////////////////////////////////////////////////////////////

DebugBase::DebugBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Code") ), wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	listCode = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 650,600 ), 0, NULL, wxLB_HSCROLL ); 
	listCode->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer5->Add( listCode, 1, wxBOTTOM|wxEXPAND|wxLEFT|wxTOP, 5 );
	
	scrollCode = new wxScrollBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL );
	bSizer5->Add( scrollCode, 0, wxBOTTOM|wxEXPAND|wxRIGHT|wxTOP, 5 );
	
	sbSizer1->Add( bSizer5, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	textCodeGo = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( textCodeGo, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Go"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer7->Add( m_button1, 0, wxALL, 5 );
	
	checkThumb = new wxCheckBox( this, wxID_ANY, wxT("Thumb"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer7->Add( checkThumb, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_ANY, wxT("Go to PC"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	bSizer7->Add( m_button2, 0, wxALL, 5 );
	
	bSizer6->Add( bSizer7, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button8 = new wxButton( this, wxID_ANY, wxT("Refresh"), wxDefaultPosition, wxSize( 70,-1 ), 0 );
	bSizer14->Add( m_button8, 0, wxALL, 5 );
	
	wxArrayString choiceFileChoices;
	choiceFile = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choiceFileChoices, 0 );
	choiceFile->SetSelection( 0 );
	bSizer14->Add( choiceFile, 0, wxALL, 5 );
	
	wxArrayString choiceSymChoices;
	choiceSym = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choiceSymChoices, 0 );
	choiceSym->SetSelection( 0 );
	bSizer14->Add( choiceSym, 0, wxALL, 5 );
	
	bSizer6->Add( bSizer14, 0, wxALIGN_RIGHT|wxEXPAND, 5 );
	
	sbSizer1->Add( bSizer6, 0, wxEXPAND, 5 );
	
	bSizer17->Add( sbSizer1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Registers") ), wxVERTICAL );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("A1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer2->Add( m_staticText2, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg0 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg0->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer2->Add( textReg0, 0, wxALL, 5 );
	
	m_staticText21 = new wxStaticText( this, wxID_ANY, wxT("A2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	fgSizer2->Add( m_staticText21, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg1 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg1->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer2->Add( textReg1, 0, wxALL, 5 );
	
	m_staticText22 = new wxStaticText( this, wxID_ANY, wxT("A3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	fgSizer2->Add( m_staticText22, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg2 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer2->Add( textReg2, 0, wxALL, 5 );
	
	m_staticText23 = new wxStaticText( this, wxID_ANY, wxT("A4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer2->Add( m_staticText23, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg3 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg3->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer2->Add( textReg3, 0, wxALL, 5 );
	
	m_staticText24 = new wxStaticText( this, wxID_ANY, wxT("V1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	fgSizer2->Add( m_staticText24, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg4 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg4->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer2->Add( textReg4, 0, wxALL, 5 );
	
	m_staticText25 = new wxStaticText( this, wxID_ANY, wxT("V2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer2->Add( m_staticText25, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg5 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg5->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer2->Add( textReg5, 0, wxALL, 5 );
	
	m_staticText26 = new wxStaticText( this, wxID_ANY, wxT("V3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	fgSizer2->Add( m_staticText26, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg6 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg6->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer2->Add( textReg6, 0, wxALL, 5 );
	
	m_staticText27 = new wxStaticText( this, wxID_ANY, wxT("V4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	fgSizer2->Add( m_staticText27, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg7 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg7->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer2->Add( textReg7, 0, wxALL, 5 );
	
	bSizer9->Add( fgSizer2, 1, 0, 5 );
	
	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer21->AddGrowableCol( 1 );
	fgSizer21->SetFlexibleDirection( wxBOTH );
	fgSizer21->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText28 = new wxStaticText( this, wxID_ANY, wxT("V5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText28->Wrap( -1 );
	fgSizer21->Add( m_staticText28, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg8 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg8->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer21->Add( textReg8, 0, wxALL, 5 );
	
	m_staticText211 = new wxStaticText( this, wxID_ANY, wxT("V6"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText211->Wrap( -1 );
	fgSizer21->Add( m_staticText211, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg9 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg9->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer21->Add( textReg9, 0, wxALL, 5 );
	
	m_staticText221 = new wxStaticText( this, wxID_ANY, wxT("V7"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText221->Wrap( -1 );
	fgSizer21->Add( m_staticText221, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg10 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg10->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer21->Add( textReg10, 0, wxALL, 5 );
	
	m_staticText231 = new wxStaticText( this, wxID_ANY, wxT("V8"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText231->Wrap( -1 );
	fgSizer21->Add( m_staticText231, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg11 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg11->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer21->Add( textReg11, 0, wxALL, 5 );
	
	m_staticText241 = new wxStaticText( this, wxID_ANY, wxT("IP"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText241->Wrap( -1 );
	fgSizer21->Add( m_staticText241, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg12 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg12->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer21->Add( textReg12, 0, wxALL, 5 );
	
	m_staticText251 = new wxStaticText( this, wxID_ANY, wxT("SP"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText251->Wrap( -1 );
	fgSizer21->Add( m_staticText251, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg13 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg13->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer21->Add( textReg13, 0, wxALL, 5 );
	
	m_staticText261 = new wxStaticText( this, wxID_ANY, wxT("LR"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText261->Wrap( -1 );
	fgSizer21->Add( m_staticText261, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg14 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg14->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer21->Add( textReg14, 0, wxALL, 5 );
	
	m_staticText271 = new wxStaticText( this, wxID_ANY, wxT("PC"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText271->Wrap( -1 );
	fgSizer21->Add( m_staticText271, 0, wxALIGN_CENTER|wxALL, 5 );
	
	textReg15 = new wxTextCtrl( this, wxID_ANY, wxT("12345678"), wxDefaultPosition, wxSize( 64,-1 ), wxTE_READONLY );
	textReg15->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer21->Add( textReg15, 0, wxALL, 5 );
	
	bSizer9->Add( fgSizer21, 1, 0, 5 );
	
	sbSizer3->Add( bSizer9, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	checkRegNF = new wxCheckBox( this, wxID_ANY, wxT("Negative"), wxDefaultPosition, wxDefaultSize, 0 );
	
	checkRegNF->Enable( false );
	
	bSizer10->Add( checkRegNF, 0, wxALL, 5 );
	
	checkRegVF = new wxCheckBox( this, wxID_ANY, wxT("Overflow"), wxDefaultPosition, wxDefaultSize, 0 );
	
	checkRegVF->Enable( false );
	
	bSizer10->Add( checkRegVF, 0, wxALL, 5 );
	
	checkRegZF = new wxCheckBox( this, wxID_ANY, wxT("Zero"), wxDefaultPosition, wxDefaultSize, 0 );
	
	checkRegZF->Enable( false );
	
	bSizer10->Add( checkRegZF, 0, wxALL, 5 );
	
	checkRegCF = new wxCheckBox( this, wxID_ANY, wxT("Carry"), wxDefaultPosition, wxDefaultSize, 0 );
	
	checkRegCF->Enable( false );
	
	bSizer10->Add( checkRegCF, 0, wxALL, 5 );
	
	checkRegTF = new wxCheckBox( this, wxID_ANY, wxT("thumb"), wxDefaultPosition, wxDefaultSize, 0 );
	
	checkRegTF->Enable( false );
	
	bSizer10->Add( checkRegTF, 0, wxALL, 5 );
	
	sbSizer3->Add( bSizer10, 0, wxEXPAND, 5 );
	
	bSizer2->Add( sbSizer3, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Data") ), wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	listData = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 340,270 ), 0, NULL, 0 ); 
	listData->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer11->Add( listData, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxTOP, 5 );
	
	scrollData = new wxScrollBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSB_VERTICAL );
	bSizer11->Add( scrollData, 0, wxBOTTOM|wxEXPAND|wxRIGHT|wxTOP, 5 );
	
	sbSizer4->Add( bSizer11, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );
	
	textDataGo = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( textDataGo, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_button11 = new wxButton( this, wxID_ANY, wxT("Go"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer15->Add( m_button11, 0, wxALL, 5 );
	
	bSizer71->Add( bSizer15, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString choiceFormatChoices[] = { wxT("bytes"), wxT("words"), wxT("chars") };
	int choiceFormatNChoices = sizeof( choiceFormatChoices ) / sizeof( wxString );
	choiceFormat = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 100,-1 ), choiceFormatNChoices, choiceFormatChoices, 0 );
	choiceFormat->SetSelection( 0 );
	bSizer16->Add( choiceFormat, 0, wxALL, 5 );
	
	bSizer71->Add( bSizer16, 0, wxALIGN_RIGHT, 5 );
	
	sbSizer4->Add( bSizer71, 0, wxEXPAND, 5 );
	
	bSizer2->Add( sbSizer4, 1, wxEXPAND, 5 );
	
	bSizer17->Add( bSizer2, 0, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer17, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	textGoTo = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( textGoTo, 0, wxALL, 5 );
	
	m_button10 = new wxButton( this, wxID_ANY, wxT("Go To"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button10, 0, wxALL, 5 );
	
	m_button3 = new wxButton( this, wxID_ANY, wxT("Step"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button3, 0, wxALL, 5 );
	
	m_button4 = new wxButton( this, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button4, 0, wxALL, 5 );
	
	m_button5 = new wxButton( this, wxID_ANY, wxT("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button5, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer8, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DebugBase::onClose ) );
	scrollCode->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	textCodeGo->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DebugBase::onCodeGo ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onCodeGo ), NULL, this );
	checkThumb->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DebugBase::onThumbCheck ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onCodeGoPC ), NULL, this );
	m_button8->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onRefreshSym ), NULL, this );
	choiceFile->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DebugBase::onChoiceFile ), NULL, this );
	choiceSym->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DebugBase::onChoiceSym ), NULL, this );
	scrollData->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	textDataGo->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DebugBase::onDataGo ), NULL, this );
	m_button11->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onDataGo ), NULL, this );
	choiceFormat->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DebugBase::onDataFormat ), NULL, this );
	m_button10->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onGoTo ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onStep ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onRun ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onStop ), NULL, this );
}

DebugBase::~DebugBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DebugBase::onClose ) );
	scrollCode->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	scrollCode->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( DebugBase::onCodeScroll ), NULL, this );
	textCodeGo->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DebugBase::onCodeGo ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onCodeGo ), NULL, this );
	checkThumb->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DebugBase::onThumbCheck ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onCodeGoPC ), NULL, this );
	m_button8->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onRefreshSym ), NULL, this );
	choiceFile->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DebugBase::onChoiceFile ), NULL, this );
	choiceSym->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DebugBase::onChoiceSym ), NULL, this );
	scrollData->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	scrollData->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( DebugBase::onDataScroll ), NULL, this );
	textDataGo->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DebugBase::onDataGo ), NULL, this );
	m_button11->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onDataGo ), NULL, this );
	choiceFormat->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DebugBase::onDataFormat ), NULL, this );
	m_button10->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onGoTo ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onStep ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onRun ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebugBase::onStop ), NULL, this );
}
