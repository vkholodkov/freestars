/////////////////////////////////////////////////////////////////////////////
// Name:        racewizard.cpp
// Purpose:     Freestars client
// Author:      Vambola Kotkas
// Created:     21/08/2006 03:43:03
/////////////////////////////////////////////////////////////////////////////

#include "FSClient.h"
#include "racewizard.h"
#include "graphics.h"


/*!
 * P1 type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RaceWizard::P1, wxWizardPageSimple )

/*!
 * P1 event table definition
 */

BEGIN_EVENT_TABLE( RaceWizard::P1, wxWizardPageSimple )
    EVT_CHOICE( ID_PRT, RaceWizard::P1::OnPrtSelected )
	EVT_WIZARD_PAGE_CHANGED( -1, RaceWizard::P1::OnPageChanged )
    EVT_TEXT( ID_RACE_NAME, RaceWizard::P1::OnRaceNameUpdated )
    EVT_TEXT( ID_PLURAL_NAME, RaceWizard::P1::OnPluralNameUpdated )
	EVT_SPIN(ID_SPINEMBLEME, RaceWizard::P1::OnSpinEmbleme ) 
END_EVENT_TABLE()

/*!
 * P1 constructors
 */

RaceWizard::P1::P1( )
	:m_pParent(NULL)
	,m_PrtNames(0)
	,m_pPrtName(NULL)
	,m_pRace(NULL) 
{
}

RaceWizard::P1::P1( RaceWizard* parent )
	:m_pParent(parent)
	,m_PrtNames(0)
	,m_pPrtName(NULL)
	,m_pRace(parent->m_pRace) 
{
	TheGraphics.EnsureLoaded();
    Create();
}

RaceWizard::P1::~P1( )
{
	if (m_PrtNames)
		delete [] m_pPrtName; 
}


/*!
 * P1 creator
 */

bool RaceWizard::P1::Create()
{
    wxWizardPageSimple::Create( m_pParent );
	m_PrtNames = FreeStars::RacialTrait::PrimaryTraitCount();
	m_pPrtName = new wxString[m_PrtNames];
	for (int i=0; i<m_PrtNames; i++)
	{
		m_pPrtName[i]=FreeStars::RacialTrait::GetPrimaryTrait(i)->Name().data();
	}

    CreateControls();

	if (GetSizer())
        GetSizer()->Fit(this);

	return true;
}

/*!
 * Control creation for P1
 */


void RaceWizard::P1::CreateControls()
{    
    P1* p1 = this;

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    p1->SetSizer(sizer);

    wxFlexGridSizer* sizer_1 = new wxFlexGridSizer(2, 2, 0, 0);
    sizer_1->AddGrowableCol(1);
    sizer->Add(sizer_1, 0, wxGROW|wxALL, 5);

	//The EMBLEME 
	wxBitmap bmp(TheGraphics.GetBigEmblemeName(m_pRace->GetRaceEmblem()), wxBITMAP_TYPE_JPEG);
    wxStaticBitmap* embleme = new wxStaticBitmap( p1, ID_EMBLEME, bmp, wxDefaultPosition, wxDefaultSize, 0 );
    sizer_1->Add(embleme, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);


	//RW points
	wxStaticBoxSizer* sizer_1_1 = new wxStaticBoxSizer(new wxStaticBox(p1, -1, _T("")), wxHORIZONTAL);
    sizer_1->Add(sizer_1_1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    sizer_1_1->Add(new wxStaticText( p1, -1, _("Advantage\nPoints Left")), 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	wxString str;
	//str << m_pRace->GetAdvantagePoints();
    wxStaticText* leftover_points = new wxStaticText( p1, ID_LEFTOVER_POINTS, str);
    leftover_points->SetFont(wxFont(16, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));
    sizer_1_1->Add(leftover_points, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	//The embleme Spinner Got to add it here to make it sit at good place
    wxSpinButton* spin_embleme = new wxSpinButton( p1, ID_SPINEMBLEME, wxDefaultPosition, wxDefaultSize, wxSP_HORIZONTAL | wxSP_WRAP );
	spin_embleme->SetRange(0, 31); //magic max embleme value
	spin_embleme->SetValue(m_pRace->GetRaceEmblem());
	sizer_1->Add(spin_embleme, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);


    wxFlexGridSizer* sizer_2 = new wxFlexGridSizer(2, 2, 0, 0);
    sizer_2->AddGrowableCol(1);
    sizer->Add(sizer_2, 0, wxGROW|wxALL, 5);

    sizer_2->Add(new wxStaticText( p1, -1, _("Race Name") ), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
	
	wxTextCtrl* race_name = new wxTextCtrl( p1, ID_RACE_NAME, m_pRace->GetSingleName().data());
    sizer_2->Add(race_name, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    sizer_2->Add(new wxStaticText( p1, -1, _("Plural Race Name")), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	wxTextCtrl* plural_name = new wxTextCtrl( p1, ID_PLURAL_NAME, m_pRace->GetPluralName().data());
    sizer_2->Add(plural_name, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxStaticText *pText = new wxStaticText( p1, -1, _("Password"));
	pText->SetForegroundColour(wxColour(128, 0, 0));
    pText->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, _T("Tahoma")));
    sizer_2->Add(pText, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    

    wxTextCtrl* password = new wxTextCtrl( p1, ID_PASSWORD, _T(""), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
    sizer_2->Add(password, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    sizer_2->Add(new wxStaticText( p1, -1, _("Primary Racial Trait")), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
	//wxChoice* choice = new wxChoice( p5, ID, wxDefaultPosition, wxDefaultSize, 3, tech_costStrings, 0 );
	//choice->SetSelection(m_pRace->GetIntTechCost(tt));
	wxChoice* primary_trait = new wxChoice( p1, ID_PRT, wxDefaultPosition, wxDefaultSize, m_PrtNames, m_pPrtName, 0 );
	primary_trait->SetStringSelection(m_pRace->GetPRT()->Name().data());
    sizer_2->Add(primary_trait, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    sizer_2->Add(new wxStaticText( p1, -1, _("Spend leftover advantage points on")), 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	long buys = m_pRace->GetLeftoverBuys();
	long buycount = m_pRace->GetLeftoverStringCount();
	const wxString *buystrings = m_pRace->GetLeftoverStrings();
    wxComboBox* leftover_to = new wxComboBox( p1, ID_LEFTOVER, buystrings[buys], wxDefaultPosition, wxDefaultSize, buycount, buystrings, wxCB_READONLY );
    sizer_2->Add(leftover_to, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);


}

void RaceWizard::P1::OnPrtSelected( wxCommandEvent& event )
{
	m_pRace->SetPRT(FreeStars::RacialTrait::GetPrimaryTrait(event.GetSelection()));
	m_pRace->ResetDefaults();
	ShowPoints();
}


void RaceWizard::P1::ShowPoints()
{
	wxStaticText* leftover_points = (wxStaticText*)FindWindow(ID_LEFTOVER_POINTS);
	wxString str;
	str << m_pRace->GetAdvantagePoints()/3;
	leftover_points->SetLabel(str);
	Layout();
}

void RaceWizard::P1::OnRaceNameUpdated( wxCommandEvent& event )
{
	wxString name = event.GetString();
	m_pRace->SetSingleName(name);
}

void RaceWizard::P1::OnPluralNameUpdated( wxCommandEvent& event )
{
	wxString name = event.GetString();
	m_pRace->SetPluralName(name);
}


void RaceWizard::P1::OnSpinEmbleme ( wxSpinEvent& event )
{
	int pos = event.GetPosition();
	m_pRace->SetRaceEmblem(pos);
	wxBitmap bmp(TheGraphics.GetBigEmblemeName(pos), wxBITMAP_TYPE_JPEG);
	wxStaticBitmap* embleme = (wxStaticBitmap*)FindWindow( ID_EMBLEME );
	embleme->SetBitmap(bmp);
}
