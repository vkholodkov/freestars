/////////////////////////////////////////////////////////////////////////////
// Name:        racewizard.cpp
// Purpose:     Freestars client
// Author:      Vambola Kotkas
// Created:     21/08/2006 03:43:03
/////////////////////////////////////////////////////////////////////////////

#include "FSClient.h"



#include "racewizard.h"

#include "ufowizard.xpm"


IMPLEMENT_DYNAMIC_CLASS( RaceWizard, wxWizard )


BEGIN_EVENT_TABLE( RaceWizard, wxWizard )
	EVT_WIZARD_FINISHED(-1, RaceWizard::OnFinish) 
END_EVENT_TABLE()

/*!
 * RaceWizard constructors
 */

RaceWizard::RaceWizard( )
{
}

RaceWizard::RaceWizard(RaceBuild *race, wxWindow* parent, wxWindowID id, const wxPoint& pos )
{
	m_pRace = race;
    Create(parent, id, pos);
}

/*!
 * RaceWizard creator
 */

bool RaceWizard::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos )
{
    SetExtraStyle(GetExtraStyle()|wxWIZARD_EX_HELPBUTTON);
	
	wxWizard::Create( parent, id, _("Custom Race Wizard"), wxBitmap(ufowizard_xpm), pos, wxDEFAULT_DIALOG_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    CreateControls();

	return true;
}

/*!
 * Control creation for RaceWizard
 */

void RaceWizard::CreateControls()
{    

    P1* p1 = new P1( this );
    GetPageAreaSizer()->Add(p1);

    P2* p2 = new P2( this );
    GetPageAreaSizer()->Add(p2);

    P3* p3 = new P3( this );
    GetPageAreaSizer()->Add(p3);

    P4* p4 = new P4( this );
    GetPageAreaSizer()->Add(p4);

    P5* p5 = new P5( this );
    GetPageAreaSizer()->Add(p5);

	wxWizardPageSimple::Chain(p1, p2);
    wxWizardPageSimple::Chain(p2, p3);
    wxWizardPageSimple::Chain(p3, p4);
    wxWizardPageSimple::Chain(p4, p5);
}

/*!
 * Runs the wizard.
 */

bool RaceWizard::Run()
{
    wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
    while (node)
    {
        wxWizardPage* startPage = wxDynamicCast(node->GetData(), wxWizardPage);
        if (startPage) 
			return RunWizard(startPage);
        node = node->GetNext();
    }
    return false;
}


void RaceWizard::OnFinish(wxWizardEvent& WXUNUSED(event))
{
	/*if (m_pRace->GetAdvantagePoints()<0)
	{
		wxMessageBox("Your advantage points are in hole.\nI dont save race files with negative balance!","Sorry!");
	}*/
}
