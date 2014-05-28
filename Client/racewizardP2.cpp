/////////////////////////////////////////////////////////////////////////////
// Name:        racewizard.cpp
// Purpose:     Freestars client
// Author:      Vambola Kotkas
// Created:     21/08/2006 03:43:03
/////////////////////////////////////////////////////////////////////////////

#include "FSClient.h"



#include "racewizard.h"


/*!
 * P2 type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RaceWizard::P2, wxWizardPageSimple )



BEGIN_EVENT_TABLE( RaceWizard::P2, wxWizardPageSimple )
	EVT_WIZARD_PAGE_CHANGED( -1, RaceWizard::P2::OnPageChanged )
    EVT_CHECKBOX( -1, RaceWizard::P2::OnLrtClick ) //we got only LRT checkboxes so all goes
END_EVENT_TABLE()

/*!
 * P2 constructors
 */

RaceWizard::P2::P2( )
	:m_pParent(NULL)
	,m_LrtNames(0)
	,m_pLrtName(NULL)
	,m_pRace(NULL) 
{
}

RaceWizard::P2::P2( RaceWizard* parent )
	:m_pParent(parent)
	,m_LrtNames(0)
	,m_pLrtName(NULL)
	,m_pRace(parent->m_pRace) 
{
    Create();
}

RaceWizard::P2::~P2( )
{
	if (m_LrtNames)
		delete [] m_pLrtName; 
}

/*!
 * P2 creator
 */

bool RaceWizard::P2::Create()
{
    wxWizardPageSimple::Create( m_pParent, NULL, NULL, wxBitmap(wxNullBitmap) );
	m_LrtNames = FreeStars::RacialTrait::LesserTraitCount();
	m_pLrtName = new wxString[m_LrtNames];
	for (int i=0; i<m_LrtNames; i++)
	{
		m_pLrtName[i]=FreeStars::RacialTrait::GetLesserTrait(i)->Name().data();
	}

    CreateControls();
    if (GetSizer())
        GetSizer()->Fit(this);
    return true;
}

/*!
 * Control creation for P2
 */

void RaceWizard::P2::CreateControls()
{    

    P2* p2 = this;
	wxString str;

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    p2->SetSizer(sizer);

    wxFlexGridSizer* sizer_1 = new wxFlexGridSizer(2, 2, 0, 0);
    sizer_1->AddGrowableCol(1);
    sizer->Add(sizer_1, 0, wxGROW|wxALL, 5);

	sizer_1->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
   
    wxStaticBoxSizer* sizer_1_1 = new wxStaticBoxSizer(new wxStaticBox(p2, -1, _T("")), wxHORIZONTAL);
    sizer_1->Add(sizer_1_1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    sizer_1_1->Add(new wxStaticText( p2, -1, _("Advantage\nPoints Left")), 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    //str << m_pRace->GetAdvantagePoints();
    wxStaticText* leftover_points = new wxStaticText( p2, ID_LEFTOVER_POINTS, str);
    leftover_points->SetFont(wxFont(16, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));
    sizer_1_1->Add(leftover_points, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    sizer->Add(new wxStaticText( p2, -1, _("Lesser Racial Traits")), 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    wxFlexGridSizer* sizer_2 = new wxFlexGridSizer(2, 2, 0, 0);
    sizer_2->AddGrowableCol(1);
    sizer->Add(sizer_2, 0, wxGROW|wxALL, 5);

	for (int i=0; i<m_LrtNames;i++)
	{
		int ID_i = ID_LRT_FIRST+i;
		if (ID_i > ID_LRT_LAST)
		{
			wxMessageBox("I dont have room for so lot of Lesser Traits!","Sorry!");
			break;
		}
		wxCheckBox * pBox = new wxCheckBox( p2, ID_i, m_pLrtName[i]);
		int lrtcount = m_pRace->GetLRTCount();
		pBox->SetValue(false);
		for (int j=0; j<lrtcount;j++)
		{
			if( m_pLrtName[i] == m_pRace->GetLRT(j)->Name().data())
			{
				pBox->SetValue(true);
				break;
			}
		}
	    sizer_2->Add(pBox, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);
	}
    
}

void RaceWizard::P2::ShowPoints()
{
	wxStaticText* leftover_points = (wxStaticText*)FindWindow(ID_LEFTOVER_POINTS);
	wxString str;
	str << m_pRace->GetAdvantagePoints()/3;
	leftover_points->SetLabel(str);
	Layout();
}

void RaceWizard::P2::OnLrtClick( wxCommandEvent& event )
{
	long id = event.GetId() - ID_LRT_FIRST;
	long val = event.GetInt();

	if (val) //so we add LRT
	{
		m_pRace->AddLRT(FreeStars::RacialTrait::GetLesserTrait(id));
	}
	else	//we remove LRT
	{
		m_pRace->RemoveLRT(FreeStars::RacialTrait::GetLesserTrait(id));
	}
	m_pRace->ResetDefaults();
	ShowPoints();
}


