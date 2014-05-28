/////////////////////////////////////////////////////////////////////////////
// Name:        racewizard.cpp
// Purpose:     Freestars client
// Author:      Vambola Kotkas
// Created:     21/08/2006 03:43:03
/////////////////////////////////////////////////////////////////////////////

#include "FSClient.h"



#include "racewizard.h"


/*!
 * P4 type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RaceWizard::P4, wxWizardPageSimple )


BEGIN_EVENT_TABLE( RaceWizard::P4, wxWizardPageSimple )
	EVT_WIZARD_PAGE_CHANGED( -1, RaceWizard::P4::OnPageChanged )
	EVT_SPIN(-1, RaceWizard::P4::OnSpin )
	EVT_CHECKBOX( ID_LESS_GERM, RaceWizard::P4::OnLessGerm )
END_EVENT_TABLE()

/*!
 * P4 constructors
 */

RaceWizard::P4::P4( )
	:m_pParent(NULL)
	,m_pRace(NULL) 
{
}

RaceWizard::P4::P4( RaceWizard* parent )
	:m_pParent(parent)
	,m_pRace(parent->m_pRace) 
{
    Create();
}

/*!
 * P4 creator
 */

bool RaceWizard::P4::Create()
{
    wxWizardPageSimple::Create( m_pParent, NULL, NULL, wxBitmap(wxNullBitmap) );

    CreateControls();
    if (GetSizer())
        GetSizer()->Fit(this);
    return true;
}

/*!
 * Control creation for P4
 */

void RaceWizard::P4::CreateControls()
{    

    P4* p4 = this;
	wxString str = "";

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    p4->SetSizer(sizer);

    wxFlexGridSizer* sizer_1 = new wxFlexGridSizer(2, 2, 0, 0);
    sizer_1->AddGrowableCol(1);
    sizer->Add(sizer_1, 0, wxGROW|wxALL, 5);

	sizer_1->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
   
    wxStaticBoxSizer* sizer_1_1 = new wxStaticBoxSizer(new wxStaticBox(p4, -1, _T("")), wxHORIZONTAL);
    sizer_1->Add(sizer_1_1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    sizer_1_1->Add(new wxStaticText( p4, -1, _("Advantage\nPoints Left")), 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	//str << m_pRace->GetAdvantagePoints();
    wxStaticText* leftover_points = new wxStaticText( p4, ID_LEFTOVER_POINTS, str);
    leftover_points->SetFont(wxFont(16, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));
    sizer_1_1->Add(leftover_points, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	//pop eff
	str = "";
	str << "One resource is generated each year for every  "
		<< m_pRace->PopEfficiency();

    wxStaticText* popeff_1 = new wxStaticText( p4, ID_POP_EFF_1, str, wxDefaultPosition, wxDefaultSize, 0 );
    wxSpinButton* popeff_spin = new wxSpinButton( p4, ID_POP_EFF_SPIN, wxDefaultPosition, wxDefaultSize, wxSP_VERTICAL );
	popeff_spin->SetRange(FreeStars::Rules::GetConstant("MinPopEfficiency",7),FreeStars::Rules::GetConstant("MaxPopEfficiency",25));
	popeff_spin->SetValue(m_pRace->PopEfficiency()/100);
    wxStaticText* popeff_2 = new wxStaticText( p4, ID_POP_EFF_2, _("colonists."), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer* sizer_8 = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(sizer_8, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    sizer_8->Add(popeff_1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    sizer_8->Add(popeff_spin, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
    sizer_8->Add(popeff_2, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	//factory eff
	str = "";
	str << "Every 10 factories produce  "
		<< m_pRace->FactoryRate();

	wxStaticText* faceff_1 = new wxStaticText( p4, ID_FAC_EFF_1, str, wxDefaultPosition, wxDefaultSize, 0 );
    wxSpinButton* faceff_spin = new wxSpinButton( p4, ID_FAC_EFF_SPIN, wxDefaultPosition, wxDefaultSize, wxSP_VERTICAL );
	faceff_spin->SetRange(FreeStars::Rules::GetConstant("MinFactoryRate",5),FreeStars::Rules::GetConstant("MaxFactoryRate",15));
	faceff_spin->SetValue(m_pRace->FactoryRate());
    wxStaticText* faceff_2 = new wxStaticText( p4, ID_FAC_EFF_2, _("resources each year."), wxDefaultPosition, wxDefaultSize, 0 );

    wxBoxSizer* sizer_9 = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(sizer_9, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    sizer_9->Add(faceff_1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    sizer_9->Add(faceff_spin, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
    sizer_9->Add(faceff_2, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	//factory cost
	str = "";
	str << "Factories require  "
		<< m_pRace->FactoryCost()[FreeStars::RESOURCES];

    wxStaticText* faccost_1 = new wxStaticText( p4, ID_FAC_COST_1, str, wxDefaultPosition, wxDefaultSize, 0 );
    wxSpinButton* faccost_spin = new wxSpinButton( p4, ID_FAC_COST_SPIN, wxDefaultPosition, wxDefaultSize, wxSP_VERTICAL );
	faccost_spin->SetRange(FreeStars::Rules::GetConstant("MinFactoryCost",5),FreeStars::Rules::GetConstant("MaxFactoryCost",25));
	faccost_spin->SetValue(m_pRace->FactoryCost()[FreeStars::RESOURCES]);
    wxStaticText* faccost_2 = new wxStaticText( p4, ID_FAC_COST_2, _("resources to build."), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer* sizer_10 = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(sizer_10, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    sizer_10->Add(faccost_1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    sizer_10->Add(faccost_spin, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
    sizer_10->Add(faccost_2, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	//factories operated
	str = "";
	str << "Every 10,000 colonists may operate up to  "
		<< m_pRace->FactoriesRun();

    wxStaticText* facoper_1 = new wxStaticText( p4, ID_FAC_OPER_1, str, wxDefaultPosition, wxDefaultSize, 0 );
    wxSpinButton* facoper_spin = new wxSpinButton( p4, ID_FAC_OPER_SPIN, wxDefaultPosition, wxDefaultSize, wxSP_VERTICAL );
	facoper_spin->SetRange(FreeStars::Rules::GetConstant("MinFactoriesRun",5),FreeStars::Rules::GetConstant("MaxFactoriesRun",25));
	facoper_spin->SetValue(m_pRace->FactoriesRun());
    wxStaticText* facoper_2 = new wxStaticText( p4, ID_FAC_OPER_2, _("factories."), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer* sizer_11 = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(sizer_11, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    sizer_11->Add(facoper_1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    sizer_11->Add(facoper_spin, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
    sizer_11->Add(facoper_2, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	//less germ
	wxCheckBox * pBox = new wxCheckBox( p4, ID_LESS_GERM,  _("Factories cost 1kT less of Germanium to build."));
	sizer->Add(pBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	pBox->SetValue(m_pRace->FactoryCost()[2]==3); //magic number
	
	//mine eff
	str = "";
	str << "Every 10 mines produce up to  "
		<< m_pRace->MineRate()
		<< "kT";

    wxStaticText* mineff_1 = new wxStaticText( p4, ID_MIN_EFF_1, str, wxDefaultPosition, wxDefaultSize, 0 );
    wxSpinButton* mineff_spin = new wxSpinButton( p4, ID_MIN_EFF_SPIN, wxDefaultPosition, wxDefaultSize, wxSP_VERTICAL );
	mineff_spin->SetRange(FreeStars::Rules::GetConstant("MinMineRate",5),FreeStars::Rules::GetConstant("MaxMineRate",25));
	mineff_spin->SetValue(m_pRace->MineRate());
    wxStaticText* mineff_2 = new wxStaticText( p4, ID_MIN_EFF_2, _("of each mineral every year."), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer* sizer_12 = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(sizer_12, 0, wxGROW|wxALL, 5);
    sizer_12->Add(mineff_1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    sizer_12->Add(mineff_spin, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
    sizer_12->Add(mineff_2, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	//mine cost
	str = "";
	str << "Mines require  "
		<< m_pRace->MineCost()[FreeStars::RESOURCES];

    wxStaticText* mincost_1 = new wxStaticText( p4, ID_MIN_COST_1, str, wxDefaultPosition, wxDefaultSize, 0 );
    wxSpinButton* mincost_spin = new wxSpinButton( p4, ID_MIN_COST_SPIN, wxDefaultPosition, wxDefaultSize, wxSP_VERTICAL );
	mincost_spin->SetRange(FreeStars::Rules::GetConstant("MinMineCost",2),FreeStars::Rules::GetConstant("MaxMineCost",15));
	mincost_spin->SetValue(m_pRace->MineCost()[FreeStars::RESOURCES]);
    wxStaticText* mincost_2 = new wxStaticText( p4, ID_MIN_COST_2, _("resources to build."), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer* sizer_13 = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(sizer_13, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    sizer_13->Add(mincost_1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    sizer_13->Add(mincost_spin, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
    sizer_13->Add(mincost_2, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
	//mines operated
	str = "";
	str << "Every 10,000 colonists may operate up to  "
		<< m_pRace->MinesRun();
    wxStaticText* minoper_1 = new wxStaticText( p4, ID_MIN_OPER_1, str, wxDefaultPosition, wxDefaultSize, 0 );
    wxSpinButton* minoper_spin = new wxSpinButton( p4, ID_MIN_OPER_SPIN, wxDefaultPosition, wxDefaultSize, wxSP_VERTICAL );
	minoper_spin->SetRange(FreeStars::Rules::GetConstant("MinMinesRun",5),FreeStars::Rules::GetConstant("MaxMinesRun",25));
	minoper_spin->SetValue(m_pRace->MinesRun());
    wxStaticText* minoper_2 = new wxStaticText( p4, ID_MIN_OPER_2, _("mines."), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer* sizer_14 = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(sizer_14, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    sizer_14->Add(minoper_1, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    sizer_14->Add(minoper_spin, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);
    sizer_14->Add(minoper_2, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

}

void RaceWizard::P4::ShowPoints()
{
	wxStaticText* leftover_points = (wxStaticText*)FindWindow(ID_LEFTOVER_POINTS);
	wxString str;
	str << m_pRace->GetAdvantagePoints()/3;
	leftover_points->SetLabel(str);
	Layout();
}

void RaceWizard::P4::OnLessGerm( wxCommandEvent& event )
{
	long val = event.GetInt();

	m_pRace->SetFactoryCost(2, val ? 3 : 4); //magic number pile

	ShowPoints();
}


void RaceWizard::P4::OnSpin( wxSpinEvent& event )
{
	long id = event.GetId();
	int pos = event.GetPosition();
	wxString str;

	switch (id)
	{
	case ID_POP_EFF_SPIN:
		str << "One resource is generated each year for every  "
			<< pos*100;
		FindWindowById(ID_POP_EFF_1, this)->SetLabel(str);
		m_pRace->SetIntPopEfficiency(pos);
		break;
	case ID_FAC_EFF_SPIN:
		str << "Every 10 factories produce  "
			<< pos;
		FindWindowById(ID_FAC_EFF_1, this)->SetLabel(str);
		m_pRace->SetFactoryRate(pos);
		break;
	case ID_FAC_COST_SPIN:
		str << "Factories require  "
			<< pos;
		FindWindowById(ID_FAC_COST_1, this)->SetLabel(str);
		m_pRace->SetFactoryCost(FreeStars::RESOURCES,pos);
		break;
	case ID_FAC_OPER_SPIN:
		str << "Every 10,000 colonists may operate up to  "
			<< pos;
		FindWindowById(ID_FAC_OPER_1, this)->SetLabel(str);
		m_pRace->SetFactoriesRun(pos);
		break;
	case ID_MIN_EFF_SPIN:
		str << "Every 10 mines produce up to  "
			<< pos
			<< "kT";
		FindWindowById(ID_MIN_EFF_1, this)->SetLabel(str);
		m_pRace->SetMineRate(pos);
		break;
	case ID_MIN_COST_SPIN:
		str << "Mines require  "
			<< pos;
		FindWindowById(ID_MIN_COST_1, this)->SetLabel(str);
		m_pRace->SetMineCost(FreeStars::RESOURCES, pos);
		break;
	case ID_MIN_OPER_SPIN:
		str << "Every 10,000 colonists may operate up to  "
			<< pos;
		FindWindowById(ID_MIN_OPER_1, this)->SetLabel(str);
		m_pRace->SetMinesRun(pos);
		break;
	default:
		wxMessageBox("Hey, what you spinned and how?","Sorry!");
		break;
	}
	ShowPoints();
}
