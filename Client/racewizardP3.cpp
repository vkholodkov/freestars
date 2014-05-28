/////////////////////////////////////////////////////////////////////////////
// Name:        racewizard.cpp
// Purpose:     Freestars client
// Author:      Vambola Kotkas
// Created:     21/08/2006 03:43:03
/////////////////////////////////////////////////////////////////////////////

#include "FSClient.h"



#include "racewizard.h"

#include "left.xpm"
#include "right.xpm"
#include "shrink.xpm"
#include "expand.xpm"

#include "wx\msw\dcclient.h"


/*!
 * P3 type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RaceWizard::P3, wxWizardPageSimple )


BEGIN_EVENT_TABLE( RaceWizard::P3, wxWizardPageSimple )
	EVT_WIZARD_PAGE_CHANGED( -1, RaceWizard::P3::OnPageChanged )
    EVT_CHECKBOX( -1, RaceWizard::P3::OnImmuneClick ) 
    EVT_BUTTON( -1, RaceWizard::P3::OnButtonClick )
	EVT_SPIN(ID_GROWTH_SPIN, RaceWizard::P3::OnGrowthSpin )
END_EVENT_TABLE()

/*!
 * P3 constructors
 */

RaceWizard::P3::P3( )
	:m_pParent(NULL)
	,m_pRace(NULL) 
{
}

RaceWizard::P3::P3( RaceWizard* parent )
	:m_pParent(parent)
	,m_pRace(parent->m_pRace) 
{
    Create();
}

/*!
 * P3 creator
 */

bool RaceWizard::P3::Create()
{
    wxWizardPageSimple::Create( m_pParent, NULL, NULL, wxBitmap(wxNullBitmap) );

    CreateControls();
    if (GetSizer())
        GetSizer()->Fit(this);
    return true;
}

/*!
 * Control creation for P3
 */


void RaceWizard::P3::CreateControls()
{    

    P3* p3 = this;
	wxString str = "";

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    p3->SetSizer(sizer);

	//leftover advantage points box 
    wxFlexGridSizer* sizer_1 = new wxFlexGridSizer(2, 2, 0, 0);
    sizer_1->AddGrowableCol(1);
    sizer->Add(sizer_1, 0, wxGROW|wxALL, 5);

	sizer_1->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);
   
    wxStaticBoxSizer* sizer_1_1 = new wxStaticBoxSizer(new wxStaticBox(p3, -1, _T("")), wxHORIZONTAL);
    sizer_1->Add(sizer_1_1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    sizer_1_1->Add(new wxStaticText( p3, -1, _("Advantage\nPoints Left")), 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    //str << m_pRace->GetAdvantagePoints();
    wxStaticText* leftover_points = new wxStaticText( p3, ID_LEFTOVER_POINTS, str);
	leftover_points->SetFont(wxFont(16, wxSWISS, wxNORMAL, wxNORMAL, false, _T("Arial")));
    sizer_1_1->Add(leftover_points, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

	//ABOUT HABS:
	//	long Rules::MaxHabType
	//  string Rules::GetHabName(long)
	//	long Race::mHabCenter[long]
	//	long Race::mHabWidth[long] that is -1 if immune
	// 
	for (int hab = 0; hab < FreeStars::Rules::MaxHabType;hab++)
	{
		if (hab >= MAX_RW_HABS)
		{
			wxMessageBox("I dont have room for so lot of different habs!","Sorry!");
			break;
		}
		//the values for HAB
		bool immune = (m_pRace->HabWidth(hab) == -1); 
		long ID_Start = ID_HAB_0 + HCI_COUNT * hab;  

		//HAB CONTROLS
		str = m_pRace->GetFullHabString(hab); 
		wxStaticText* hab_text = new wxStaticText( p3, ID_Start+HCI_TEXT, str);
		wxBitmapButton* hab_left = new wxBitmapButton( p3, ID_Start+HCI_LEFT, wxBitmap(left_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxBU_EXACTFIT );
		hab_left->Enable(!immune);
		HabPanel* hab_graph = new HabPanel( p3, ID_Start+HCI_GRAPH, m_pRace->HabCenter(hab), m_pRace->HabWidth(hab), hab); 
		hab_graph->SetBackgroundColour(wxColour(0, 0, 0));
		hab_graph->SetForegroundColour(m_pRace->GetHabColour(hab));
		wxBitmapButton* hab_right = new wxBitmapButton( p3, ID_Start+HCI_RIGHT, wxBitmap(right_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxBU_EXACTFIT );
		hab_right->Enable(!immune);
		wxBitmapButton* hab_expand = new wxBitmapButton( p3, ID_Start+HCI_EXPAND, wxBitmap(expand_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxBU_EXACTFIT );
		hab_expand->Enable(!immune);
		str = "Immune to ";
		str << FreeStars::Rules::GetHabName(hab).data();
		wxCheckBox* hab_immune = new wxCheckBox( p3, ID_Start+HCI_IMMUNE, str);
		hab_immune->SetValue(immune);
		wxBitmapButton* hab_shrink = new wxBitmapButton( p3, ID_Start+HCI_SHRINK, wxBitmap(shrink_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|wxBU_EXACTFIT );
		hab_shrink->Enable(!immune);
		
		//POSITION HAB PANE STUFF
		sizer->Add(hab_text, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxADJUST_MINSIZE, 5);
		wxBoxSizer* sizer_hab_1 = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(sizer_hab_1, 0, wxGROW|wxLEFT|wxRIGHT, 5);
		wxBoxSizer* sizer_hab_2 = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(sizer_hab_2, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);
		sizer_hab_1->Add(hab_left, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);
		sizer_hab_1->Add(hab_graph, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);
		sizer_hab_1->Add(hab_right, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);
		sizer_hab_2->Add(hab_expand, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);
		sizer_hab_2->Add(hab_immune, 1, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxBOTTOM, 5);
		sizer_hab_2->Add(hab_shrink, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);
	}

	//GROWTH RATE
	str = "";
	str << "Maximum colonist growth rate per year "
		<< m_pRace->GetIntGrowthRate()	<<"%";

	wxStaticText* growth_rate = new wxStaticText( p3, ID_GROWTH_RATE, str);
    wxSpinButton* growth_spin = new wxSpinButton( p3, ID_GROWTH_SPIN, wxDefaultPosition, wxDefaultSize, wxSP_VERTICAL );
	growth_spin->SetRange(FreeStars::Rules::GetConstant("MinGrowthRate",1),FreeStars::Rules::GetConstant("MaxGrowthRate",20));
	growth_spin->SetValue(m_pRace->GetIntGrowthRate());


    wxBoxSizer* sizer_8 = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(sizer_8, 0, wxGROW|wxALL, 5);
    sizer_8->Add(growth_rate, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    sizer_8->Add(growth_spin, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

}

void RaceWizard::P3::ShowPoints()
{
	wxStaticText* leftover_points = (wxStaticText*)FindWindow(ID_LEFTOVER_POINTS);
	wxString str;
	for (int hab = 0; hab < FreeStars::Rules::MaxHabType;hab++)
	{
		FindWindowById(ID_HAB_0 + hab*HCI_COUNT + HCI_TEXT, this)->SetLabel(m_pRace->GetFullHabString(hab));
	}
	str << m_pRace->GetAdvantagePoints()/3;
	leftover_points->SetLabel(str);
	Layout();
}

void RaceWizard::P3::OnImmuneClick( wxCommandEvent& event )
{
	bool immune = (bool)event.GetInt();
	long id = event.GetId()-HCI_IMMUNE;
	long hab = (id-ID_HAB_0)/HCI_COUNT;
	HabPanel* panel = (HabPanel*)FindWindowById(id+HCI_GRAPH, this);
	panel->SetImmune(immune);
	m_pRace->SetCenter(hab,panel->Center());
	m_pRace->SetWidth(hab,panel->Width());
	FindWindowById(id+HCI_LEFT, this)->Enable(!immune);
	FindWindowById(id+HCI_RIGHT, this)->Enable(!immune);
	FindWindowById(id+HCI_EXPAND, this)->Enable(!immune);
	FindWindowById(id+HCI_SHRINK, this)->Enable(!immune);
	ShowPoints();
}


void RaceWizard::P3::OnButtonClick( wxCommandEvent& event )
{
	long id = event.GetId();
	long hab = (id-ID_HAB_0)/HCI_COUNT;
	long id_ctrl = (id-ID_HAB_0)%HCI_COUNT;
	id -= id_ctrl;
	HabPanel* panel = (HabPanel*)FindWindowById(id+HCI_GRAPH, this);
	//bool shift = event.ShiftDown();
	switch (id_ctrl)
	{
	case HCI_LEFT:			//shift left button
		panel->Left();
		break;
	case HCI_RIGHT:			//shift right button 
		panel->Right();
		break;
	case HCI_EXPAND:			//widen button
		panel->Expand();
		break;
	case HCI_SHRINK:			//narroven button
		panel->Shrink();
		break;
	default:
		wxMessageBox("Hey, what button you pressed and how?","Sorry!");
		break;
	}

	m_pRace->SetCenter(hab,panel->Center());
	m_pRace->SetWidth(hab,panel->Width());
	ShowPoints();
}

void RaceWizard::P3::OnGrowthSpin( wxSpinEvent& event )
{
	int growth = event.GetPosition();
	wxString str = "Maximum colonist growth rate per year ";
	str << growth <<"%";
	FindWindowById(ID_GROWTH_RATE, this)->SetLabel(str);
	m_pRace->SetGrowth(growth);
	ShowPoints();
}


/*!
 * P3::HabPanel type definition
 */

IMPLEMENT_DYNAMIC_CLASS( RaceWizard::P3::HabPanel, wxPanel )

/*!
 * HabPanel event table definition
 */

 BEGIN_EVENT_TABLE( RaceWizard::P3::HabPanel, wxPanel )
    EVT_PAINT( RaceWizard::P3::HabPanel::OnPaint )
    EVT_LEFT_DOWN( RaceWizard::P3::HabPanel::OnLeftDown )
    EVT_LEFT_UP( RaceWizard::P3::HabPanel::OnLeftUp )
    EVT_MOTION( RaceWizard::P3::HabPanel::OnMotion )
	EVT_SIZE(RaceWizard::P3::HabPanel::OnSize) 
END_EVENT_TABLE()

void RaceWizard::P3::HabPanel::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	wxPaintDC dc(this);
	dc.SetPen( wxPen( wxColour(GetBackgroundColour()) , 0, wxSOLID ) );
	dc.SetBrush( wxBrush( wxColour(GetBackgroundColour()) ) );
	const wxSize size = GetClientSize();
	bool immune = (m_width == -1);
	if (!immune)
	{
		long x0 = ((m_center-m_width)*size.x)/100;
		long dX = ((m_width+m_width)*size.x + 99)/100;
		dc.DrawRectangle(0,0,x0+2,size.y);
		dc.DrawRectangle(x0+dX-2,0,size.x,size.y);
		dc.SetBrush( wxBrush( wxColour(GetForegroundColour()) ) );
		dc.DrawRoundedRectangle(x0,0,dX,size.y,4);
	}
	else
	{
		dc.SetBrush( wxBrush( wxColour(GetBackgroundColour()) ) );
		dc.DrawRectangle(0,0,size.x,size.y);
	}


}

/// wxEVT_LEFT_UP event handler
void RaceWizard::P3::HabPanel::OnLeftUp( wxMouseEvent& event ) 
{
	if (!m_mouse_captured)
		return;
	ReleaseMouse();
	m_mouse_captured = false;
	OnMotion(event);
	m_pParent->m_pRace->SetCenter(m_hab,m_center);
	m_pParent->m_pRace->SetWidth(m_hab,m_width);

	m_pParent->ShowPoints();
}

/// wxEVT_LEFT_DOWN event handler
void RaceWizard::P3::HabPanel::OnLeftDown( wxMouseEvent& event ) 
{
	CaptureMouse();
	m_mouse_captured = true;
	OnMotion(event);
}


/// wxEVT_MOTION event handler
void RaceWizard::P3::HabPanel::OnMotion( wxMouseEvent& event ) 
{
	if (!m_mouse_captured)
		return;
	const wxSize size = GetClientSize();
	int x = event.GetX();
	int newcenter = x*100/size.x;
	if (newcenter < m_width)
		newcenter = m_width;
	if (newcenter + m_width > 100)
		newcenter = 100 - m_width;
	if (newcenter != m_center)
	{
		m_center = newcenter; 
		Refresh();
	}
}


void RaceWizard::P3::HabPanel::OnSize( wxSizeEvent& WXUNUSED(event))
{
	wxWindow::Refresh(false,NULL);
}

void RaceWizard::P3::HabPanel::SetImmune(bool yess)
{
	if (yess)
	{
		m_width = -1;
		m_center = -1;
	}
	else
	{
		m_center = 50;
		m_width = 30;
	}
	Refresh();
}

void RaceWizard::P3::HabPanel::Left()
{
	if (m_center-m_width > 0)
		m_center--;
	Refresh();
}

void RaceWizard::P3::HabPanel::Right()
{
	if (m_center+m_width < 100)
		m_center++;

	Refresh();
}

void RaceWizard::P3::HabPanel::Expand()
{
	if (m_width <50)
		m_width++;
	if (m_center+m_width > 100)
		m_center--;
	if (m_center-m_width < 0)
		m_center++;
	Refresh();
}

void RaceWizard::P3::HabPanel::Shrink()
{
	if (m_width >10)
		m_width--;
	Refresh();
}



