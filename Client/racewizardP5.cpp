/////////////////////////////////////////////////////////////////////////////
// Name:        racewizard.cpp
// Purpose:     Freestars client
// Author:      Vambola Kotkas
// Created:     21/08/2006 03:43:03
/////////////////////////////////////////////////////////////////////////////

#include "FSClient.h"



#include "racewizard.h"




/*!
 * P5 type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RaceWizard::P5, wxWizardPageSimple )


BEGIN_EVENT_TABLE( RaceWizard::P5, wxWizardPageSimple )
	EVT_WIZARD_PAGE_CHANGED( -1, RaceWizard::P5::OnPageChanged )
    EVT_CHOICE( -1, RaceWizard::P5::OnTechCostSelected )
    EVT_CHECKBOX( ID_START_HIGHER, RaceWizard::P5::OnStartAt )
END_EVENT_TABLE()

/*!
 * P5 constructors
 */

RaceWizard::P5::P5( )
	:m_pParent(NULL)
	,m_pRace(NULL) 
{
}

RaceWizard::P5::P5( RaceWizard* parent )
	:m_pParent(parent)
	,m_pRace(parent->m_pRace) 
{
    Create();
}

/*!
 * P5 creator
 */

bool RaceWizard::P5::Create()
{
    wxWizardPageSimple::Create( m_pParent, NULL, NULL, wxBitmap(wxNullBitmap) );

    CreateControls();
    if (GetSizer())
        GetSizer()->Fit(this);
    return true;
}

/*!
 * Control creation for P5
 */


void RaceWizard::P5::CreateControls()
{    

    P5* p5 = this;
	wxString str;

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    p5->SetSizer(sizer);

    wxFlexGridSizer* sizer_1 = new wxFlexGridSizer(2, 2, 0, 0);
    sizer_1->AddGrowableCol(1);
    sizer->Add(sizer_1, 0, wxGROW|wxALL, 5);

	sizer_1->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
   
    wxStaticBoxSizer* sizer_1_1 = new wxStaticBoxSizer(new wxStaticBox(p5, -1, _T("")), wxHORIZONTAL);
    sizer_1->Add(sizer_1_1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    sizer_1_1->Add(new wxStaticText( p5, -1, _("Advantage\nPoints Left")), 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	//str << m_pRace->GetAdvantagePoints();
    wxStaticText* leftover_points = new wxStaticText( p5, ID_LEFTOVER_POINTS, str);
    leftover_points->SetFont(wxFont(16, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));
    sizer_1_1->Add(leftover_points, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxFlexGridSizer* sizer_2 = new wxFlexGridSizer(2, 2, 0, 0);
    sizer->Add(sizer_2, 0, wxGROW|wxALL, 5);
	for (FreeStars::TechType tt = 0; tt < FreeStars::Rules::MaxTechType; ++tt) 
	{
		int ID = ID_TECH_COST_FIRST+tt;
		str = FreeStars::Rules::GetTechName(tt).data();
		str << "  research";
		if (ID > ID_TECH_COST_LAST)
		{
			wxMessageBox("I dont have room for so lot of different Technologies!","Sorry!");
			break;
		}

		sizer_2->Add(new wxStaticText( p5, -1, str), 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
		wxChoice* choice = new wxChoice( p5, ID, wxDefaultPosition, wxDefaultSize, m_pRace->GetTechCostStringsCount(), m_pRace->GetTechCostStrings(), 0 );
		choice->SetSelection(m_pRace->GetIntTechCost(tt));
		sizer_2->Add(choice, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

	}

	str = "All 'Costs 75% extra' fields start at Tech  ";
	str << m_pRace->StartAtBonus();

    wxCheckBox* start_higher = new wxCheckBox( p5, ID_START_HIGHER, str, wxDefaultPosition, wxDefaultSize, 0 );
    start_higher->SetValue(m_pRace->StartAt());
    sizer->Add(start_higher, 0, wxALIGN_LEFT|wxALL, 5);
    
}
void RaceWizard::P5::OnPageChanged( wxWizardEvent& WXUNUSED(event) ) 
{
	wxCheckBox* start_higher = (wxCheckBox*)FindWindow(ID_LEFTOVER_POINTS);
	wxString str = "All 'Costs 75% extra' fields start at Tech  ";
	str << m_pRace->StartAtBonus();
	start_higher->SetLabel(str);

	ShowPoints();
}

void RaceWizard::P5::ShowPoints()
{
	wxStaticText* leftover_points = (wxStaticText*)FindWindow(ID_LEFTOVER_POINTS);
	int points = m_pRace->GetAdvantagePoints()/3;
	wxString str;
	str << points;
	leftover_points->SetLabel(str);

	wxWindow *finish_button = FindWindowById(wxID_FORWARD,NULL);
	if (finish_button != NULL)
	{
		finish_button->Enable(points >= 0);
	}

	Layout();
}

void RaceWizard::P5::OnTechCostSelected( wxCommandEvent& event )
{
int tt = event.GetId() - ID_TECH_COST_FIRST;
	m_pRace->SetIntTechCost(tt, event.GetSelection());
	ShowPoints();
}

void RaceWizard::P5::OnStartAt( wxCommandEvent& event )
{
	long val = event.GetInt();
	m_pRace->SetStartAt(val);
	ShowPoints();
}