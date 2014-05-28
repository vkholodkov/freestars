/////////////////////////////////////////////////////////////////////////////
// Name:        shipdesigner.cpp
// Purpose:     
// Author:      Vambola Kotkas
// Modified by: 
// Created:     02/09/2006 22:51:39
// RCS-ID:      
// Copyright:   Copyright 2006 Vambola Kotkas
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "FSClient.h"
#include "shipdesigner.h"
#include "graphics.h"
#include <wx/image.h>



/*!
 * ShipDesigner type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ShipDesigner, wxDialog )

/*!
 * ShipDesigner event table definition
 */

BEGIN_EVENT_TABLE( ShipDesigner, wxDialog )
	EVT_BUTTON( ID_EDIT, ShipDesigner::OnEdit )
	EVT_BUTTON( wxID_DELETE, ShipDesigner::OnDelete )
    EVT_RADIOBOX( ID_DESIGNBOX, ShipDesigner::OnDesignboxSelected )
    EVT_RADIOBOX( ID_VIEWBOX, ShipDesigner::OnViewboxSelected )
END_EVENT_TABLE()

/*!
 * ShipDesigner constructors
 */

ShipDesigner::ShipDesigner( )
		:m_ToDesign(TD_SHIPS)
		,m_ToView(TV_EXISTING)
		,m_ChoosenView(-1)
{
}

ShipDesigner::ShipDesigner( wxWindow* parent)
		:m_ToDesign(TD_SHIPS)
		,m_ToView(TV_EXISTING)
		,m_ChoosenView(-1)
{
    SetExtraStyle(GetExtraStyle()|wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, ID_SHIPDESIGNER, _("Ship & Starbase Designer")
				,wxDefaultPosition, wxSize(-1, -1)
				,wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

	CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
}


/*!
 * Control creation for ShipDesigner
 */

wxString design_radio_str[] = {
    _("&Ships"),
    _("&Starbases")
};

wxString view_radio_str[] = {
    _("Existing Designs"),
    _("Available Hull Types"),
    _("Enemy Hulls"),
    _("Components")
};

wxString ship_comp_type_str[] = {
    _("All"),			_("Armor"),
    _("Beam Weapons"),	_("Bombs"),
    _("Electrical"),    _("Engines"),
    _("Mechanical"),    _("Mine Layers"),
    _("Mining Robots"), _("Scanners"),
    _("Shields"),	    _("Torpedoes"),
    _("Weapons")
};

wxString base_comp_type_str[] = {
    _("All"),			_("Armor"),
    _("Beam Weapons"),	_("Electrical"),
	_("Orbital"),		_("Shields"),
	_("Torpedoes"),    _("Weapons")
};


#define COUNTOF(array) (sizeof(array)/sizeof(array[0]))


static void AddTwoTexts(wxBitmap &bmp, const wxString &txt1,const wxString &txt2)
{
	int W_bmp = bmp.GetWidth();
	int H_bmp = bmp.GetHeight();

	//create bmp for drawing 
	wxBitmap temp_bmp(W_bmp, H_bmp);
	wxMemoryDC temp_dc;
	temp_dc.SelectObject(temp_bmp);
	//use Almost Black background because of black bordered text
	wxBrush almost_black(wxColour(1,1,1));
	temp_dc.SetBackground(almost_black);
	temp_dc.Clear();
	//draw our text
	temp_dc.SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
	int W_txt1, H_txt1, W_txt2, H_txt2, X_txt1, Y_txt1, X_txt2, Y_txt2;
	temp_dc.GetTextExtent(txt1, &W_txt1, &H_txt1);
	temp_dc.GetTextExtent(txt2, &W_txt2, &H_txt2);
	X_txt1 = (W_bmp-W_txt1)/2;
	X_txt2 = (W_bmp-W_txt2)/2;
	Y_txt1 = (H_bmp-H_txt1-H_txt2-H_txt1/4)/2;
	Y_txt2 = Y_txt1+H_txt1+H_txt1/4;
	temp_dc.SetTextForeground(*wxBLACK);
	temp_dc.DrawText(txt1,X_txt1-1,Y_txt1-1);
	temp_dc.DrawText(txt1,X_txt1+1,Y_txt1-1);
	temp_dc.DrawText(txt1,X_txt1-1,Y_txt1+1);
	temp_dc.DrawText(txt1,X_txt1+1,Y_txt1+1);
	temp_dc.DrawText(txt2,X_txt2-1,Y_txt2-1);
	temp_dc.DrawText(txt2,X_txt2+1,Y_txt2-1);
	temp_dc.DrawText(txt2,X_txt2-1,Y_txt2+1);
	temp_dc.DrawText(txt2,X_txt2+1,Y_txt2+1);
	temp_dc.SetTextForeground(*wxWHITE);
	temp_dc.DrawText(txt1,X_txt1,Y_txt1);
	temp_dc.DrawText(txt2,X_txt2,Y_txt2);
	//convert to image that has background transparent
	wxImage temp_im = temp_bmp.ConvertToImage();
	temp_im.SetMaskColour(1,1,1); //same Almost Black
	temp_im.InitAlpha(); 
	//image goes to another bmp 
	wxBitmap temp_bmp2(temp_im);
	temp_dc.SelectObject(temp_bmp2);
	//and blit drawn text to our original bmp
	wxMemoryDC bmp_dc;
	bmp_dc.SelectObject(bmp);
	bmp_dc.Blit(0, 0, W_bmp, H_bmp, & temp_dc, 0, 0, wxCOPY, false);
	//done! probably destructors do these anyway?
	temp_dc.SelectObject(wxNullBitmap);
	bmp_dc.SelectObject(wxNullBitmap);
};

static void AddCenterText(wxBitmap &bmp, const wxString &txt)
{
	int W_bmp = bmp.GetWidth();
	int H_bmp = bmp.GetHeight();

	//create bmp for drawing 
	wxBitmap temp_bmp(W_bmp, H_bmp);
	wxMemoryDC temp_dc;
	temp_dc.SelectObject(temp_bmp);
	//use Almost White background because of white bordered text
	wxBrush almost_white(wxColour(254,254,254));
	temp_dc.SetBackground(almost_white);
	temp_dc.Clear();
	//draw our text
	temp_dc.SetFont(wxFont(16, wxSWISS, wxNORMAL, wxBOLD, false, _T("Lucida Console")));
	int W_txt, H_txt, X_txt, Y_txt;
	temp_dc.GetTextExtent(txt, &W_txt, &H_txt);
	X_txt = (W_bmp-W_txt)/2;
	Y_txt = (H_bmp-H_txt)/2;
	temp_dc.SetTextForeground(*wxWHITE);
	temp_dc.DrawText(txt,X_txt-1,Y_txt-1);
	temp_dc.DrawText(txt,X_txt+1,Y_txt-1);
	temp_dc.DrawText(txt,X_txt-1,Y_txt+1);
	temp_dc.DrawText(txt,X_txt+1,Y_txt+1);
	temp_dc.SetTextForeground(*wxBLACK);
	temp_dc.DrawText(txt,X_txt,Y_txt);
	//convert to image that has background transparent
	wxImage temp_im = temp_bmp.ConvertToImage();
	temp_im.SetMaskColour(254,254,254); //same Almost White
	temp_im.InitAlpha(); 
	//image goes to another bmp 
	wxBitmap temp_bmp2(temp_im);
	temp_dc.SelectObject(temp_bmp2);
	//and blit drawn text to our original bmp
	wxMemoryDC bmp_dc;
	bmp_dc.SelectObject(bmp);
	bmp_dc.Blit(0, 0, W_bmp, H_bmp, & temp_dc, 0, 0, wxCOPY, false);
	//done! probably destructors do these anyway?
	temp_dc.SelectObject(wxNullBitmap);
	bmp_dc.SelectObject(wxNullBitmap);
};


void ShipDesigner::CreateControls()
{
	TheGraphics.EnsureLoaded();

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(sizer);

    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    sizer->Add(sizer_1, 0, wxALIGN_TOP|wxALL, 5);

    wxRadioBox* design_radio = new wxRadioBox( this, ID_DESIGNBOX, _("Design"), wxDefaultPosition, wxDefaultSize, 2, design_radio_str, 2, wxRA_SPECIFY_ROWS );
    design_radio->SetSelection(m_ToDesign);
    sizer_1->Add(design_radio, 0, wxGROW|wxALL, 5);
	
    wxRadioBox* view_radio = new wxRadioBox( this, ID_VIEWBOX, _("View"), wxDefaultPosition, wxDefaultSize, 4, view_radio_str, 4, wxRA_SPECIFY_ROWS );
    view_radio->SetSelection(m_ToView);
    sizer_1->Add(view_radio, 0, wxGROW|wxALL, 5);

    wxButton* copy_button = new wxButton( this, wxID_COPY, _("&Copy Selected Design"), wxDefaultPosition, wxDefaultSize, 0 );
    sizer_1->Add(copy_button, 0, wxGROW|wxALL, 5);

    wxButton* delete_button = new wxButton( this, wxID_DELETE, _("&Delete Selected Design"), wxDefaultPosition, wxDefaultSize, 0 );
    sizer_1->Add(delete_button, 0, wxGROW|wxALL, 5);

    wxButton* edit_button = new wxButton( this, ID_EDIT, _("&Edit Selected Design"), wxDefaultPosition, wxDefaultSize, 0 );
    sizer_1->Add(edit_button, 0, wxGROW|wxALL, 5);

    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(sizer_2, 1, wxGROW|wxALL, 5);

    wxBoxSizer* sizer_2_1 = new wxBoxSizer(wxVERTICAL);
    sizer_2->Add(sizer_2_1, 1, wxGROW|wxALL, 5);

    wxString* view_choiceStrings = NULL;
    wxChoice* view_choice = new wxChoice( this, ID_VIEWCHOICE, wxDefaultPosition, wxDefaultSize, 0, view_choiceStrings, 0 );
	PopulateViewChoice( view_choice );
    sizer_2_1->Add(view_choice, 0, wxALIGN_RIGHT|wxALL, 5);

    //designing panel ? 
	//wxPanel* design_panel = new wxPanel( this, ID_DESIGNPANEL, wxDefaultPosition, wxSize(643, 515), wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    //sizer_2_1->Add(design_panel, 1, wxGROW|wxALL, 5);

	//list of components
	wxScrolledWindow* component_list = new wxScrolledWindow( this, -1 /*ID_SCROLLINGDIALOG_SCROLLEDWINDOW*/, wxDefaultPosition, wxSize(250, 240), wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
	sizer_2_1->Add(component_list, 1, wxGROW|wxALL, 5); 
    component_list->SetScrollbars(1, 1, 0, 0);

    wxBoxSizer* sizer_2_1_1 = new wxBoxSizer(wxVERTICAL);
    component_list->SetSizer(sizer_2_1_1);
	
	wxFlexGridSizer* sizer_2_1_1_1 = new wxFlexGridSizer(2, 2, 0, 0);
    sizer_2_1_1_1->AddGrowableCol(1);
    sizer_2_1_1->Add(sizer_2_1_1_1, 1, wxGROW|wxALL, 5);

	//TheGraphics
	//TheGraphics.ComponentPictures.m_vec
	const std::deque<FreeStars::Component *> &comp_que = TheGame->GetComponents();

	for (int i = 0; i < comp_que.size(); i++)
	{
		const char *bmp_name = TheGraphics.GetComponentPictureName(comp_que[i]);
		wxBitmap bmp(bmp_name != NULL?bmp_name:"graphics/Unknown.png", wxBITMAP_TYPE_PNG);
		//some components should have texts
		wxString textie, textie2;
		switch(comp_que[i]->GetType())
		{
		case FreeStars::CT_TERRAFORM:
			textie = "±";
			textie << comp_que[i]->GetTerraLimit();
			AddCenterText(bmp, textie);
			break;
		case FreeStars::CT_MINELAY:
			textie = "";
			textie << comp_que[i]->GetMineAmount();
			AddCenterText(bmp, textie);
			break;
		case FreeStars::CT_DRIVER:
			textie = " "; //driver icons are little off
			textie << comp_que[i]->GetDriverSpeed();
			AddCenterText(bmp, textie);
			break;
		case FreeStars::CT_GATE:
			textie = "";
			textie2 = "";
			if (comp_que[i]->GetGateMass() == -1)
			{
				textie << "OO"; //got lazy to draw 8 on side
			}
			else
			{
				textie << comp_que[i]->GetGateMass() << "kT" ;
			}
			if (comp_que[i]->GetGateRange() == -1)
			{
				textie2 << "OO"; //got lazy to draw 8 on side
			}
			else
			{
				textie2 << comp_que[i]->GetGateRange() << "ly" ;
			}
			AddTwoTexts(bmp, textie, textie2);
			break;
	
		default:
			//no texts
			break;
		}
	    wxStaticBitmap* wx_bmp = new wxStaticBitmap( component_list, -1/*ID_EMBLEME*/, /*temp_*/bmp, wxDefaultPosition, wxDefaultSize, 0/*wxRAISED_BORDER*/ );
		wxStaticText* wx_txt = new wxStaticText( component_list, -1, comp_que[i]->GetName().data(), wxDefaultPosition, wxDefaultSize, 0 );

		sizer_2_1_1_1->Add(wx_bmp, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);
		sizer_2_1_1_1->Add(wx_txt, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
	}

    /* kräpp
	wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel12->SetSizer(itemBoxSizer13);

    wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer13->Add(itemBoxSizer14, 0, wxALIGN_TOP|wxALL, 5);

    wxBitmap itemStaticBitmap15Bitmap(wxNullBitmap);
    wxStaticBitmap* itemStaticBitmap15 = new wxStaticBitmap( itemPanel12, ID_THUMB, itemStaticBitmap15Bitmap, wxDefaultPosition, wxSize(64, 64), 0 );
    itemBoxSizer14->Add(itemStaticBitmap15, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxSpinButton* itemSpinButton16 = new wxSpinButton( itemPanel12, ID_SPINTHUMB, wxDefaultPosition, wxDefaultSize, wxSP_HORIZONTAL );
    itemBoxSizer14->Add(itemSpinButton16, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
	*/

    wxStdDialogButtonSizer* sizer_buttons = new wxStdDialogButtonSizer;

    sizer_2_1->Add(sizer_buttons, 0, wxGROW|wxALL, 5);

	wxButton* button_cancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    sizer_buttons->AddButton(button_cancel);

    wxButton* button_help = new wxButton( this, wxID_HELP, _("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    sizer_buttons->AddButton(button_help);

    wxButton* button_save = new wxButton( this, wxID_SAVE, _("&Save"), wxDefaultPosition, wxDefaultSize, 0 );
    sizer_buttons->AddButton(button_save);

    sizer_buttons->Realize();

////@end ShipDesigner content construction
}

void ShipDesigner::PopulateViewChoice( wxChoice* view_choice )
{
	if (view_choice == NULL)
		view_choice = (wxChoice*)FindWindow(ID_VIEWCHOICE);

	view_choice->Clear();

	switch (m_ToDesign)
	{
	case TD_SHIPS:
		switch (m_ToView)
		{
		case TV_EXISTING: //existing ships
			{
				FreeStars::Player *ourPlayer = TheGame->GetCurrentPlayer();
				for (unsigned long s = 1;s <= FreeStars::Rules::GetConstant("MaxShipDesigns");s++)
				{
					const FreeStars::Ship *ship = ourPlayer->GetShipDesign(s);
					if (ship != NULL)
						view_choice->Append(ship->GetName().data());
				}
			}
			break;
		case TV_POSSIBLE:
			{
				FreeStars::Player *ourPlayer = TheGame->GetCurrentPlayer();
				const deque<FreeStars::Component *> *compQue = &TheGame->GetComponents();
				for (unsigned long c = 0;c < compQue->size();c++)
				{
					const FreeStars::Component *comp = (*compQue)[c];
					if (comp->IsType(FreeStars::CT_HULL) && comp->IsBuildable(ourPlayer))
					{
                        view_choice->Append(comp->GetName().data());
					}
				}
			}
			break;
		case TV_ENEMIES:
			{
				FreeStars::Player *ourPlayer = TheGame->GetCurrentPlayer();
				for (unsigned int p = 1; p <= TheGame->NumberPlayers(); p++)
				{
					FreeStars::Player *opponent = TheGame->NCGetPlayer(p);
					if (opponent == ourPlayer || opponent == NULL)
						continue;
					for (unsigned long s = 1;s<=FreeStars::Rules::GetConstant("MaxShipDesigns");s++)
					{
						const FreeStars::Ship *design = opponent->GetShipDesign(s);
						if (design != NULL)
							view_choice->Append((opponent->GetSingleName()+" "+design->GetName()).data());
					}
				}
			}
			break;
		case TV_COMPONENTS:
			{
				for (int ct = 0; ct < COUNTOF(ship_comp_type_str); ct++)
				{
					view_choice->Append(ship_comp_type_str[ct]);
				}
			}
			break;
		default:
			wxMessageBox("How to view such ship?","Sorry!");
			break;
		}
		break;
	case TD_STARBASES:
		switch (m_ToView)
		{
		case TV_EXISTING:
			{
				FreeStars::Player *ourPlayer = TheGame->GetCurrentPlayer();
				for (unsigned long b = 0; b <FreeStars::Rules::GetConstant("MaxBaseDesigns");b++)
				{
					const FreeStars::Ship *base = ourPlayer->GetBaseDesign(b);
					if (base != NULL)
						view_choice->Append(base->GetName().data());
				}
			}
			break;
		case TV_POSSIBLE:
			{
				FreeStars::Player *ourPlayer = TheGame->GetCurrentPlayer();
				const deque<FreeStars::Component *> *compQue = &TheGame->GetComponents();
				for (unsigned long c = 0;c < compQue->size();c++)
				{
					const FreeStars::Component *comp = (*compQue)[c];
					if (comp->IsType(FreeStars::CT_BASE) && comp->IsBuildable(ourPlayer))
					{
                        view_choice->Append(comp->GetName().data());
					}
				}
			}
			break;
		case TV_ENEMIES:
			{
				FreeStars::Player *ourPlayer = TheGame->GetCurrentPlayer();
				for (unsigned int p = 1; p <= TheGame->NumberPlayers(); p++)
				{
					FreeStars::Player *opponent = TheGame->NCGetPlayer(p);
					if (opponent == ourPlayer || opponent == NULL)
						continue;
					for (unsigned long b = 1;b<=FreeStars::Rules::GetConstant("MaxBaseDesigns");b++)
					{
						const FreeStars::Ship *design = opponent->GetBaseDesign(b);
						if (design != NULL)
							view_choice->Append((opponent->GetSingleName()+" "+design->GetName()).data());
					}
				}
			}
			break;
		case TV_COMPONENTS:
			{
				for (int ct = 0; ct < COUNTOF(base_comp_type_str); ct++)
				{
					view_choice->Append(base_comp_type_str[ct]);
				}
			}
			break;
			break;
		default:
			wxMessageBox("How to view such base?","Sorry!");
			break;
		}
		break;
	default:
		wxMessageBox("How to design such thing?","Sorry!");
		break;
	}
	if(view_choice->IsEmpty())
	{
		m_ChoosenView = -1;
	}
	else
	{
		m_ChoosenView = 0;
		view_choice->SetSelection(m_ChoosenView);
	}
}



void ShipDesigner::OnEdit( wxCommandEvent& event )
{
	FindWindow(ID_DESIGNBOX)->Show();
    event.Skip();
	Layout();
	GetSizer()->Fit(this);
}

void ShipDesigner::OnDelete( wxCommandEvent& event )
{
	FindWindow(ID_DESIGNBOX)->Show(false);
    event.Skip();
	Layout();
	GetSizer()->Fit(this);
}

void ShipDesigner::OnDesignboxSelected( wxCommandEvent& event )
{
	m_ToDesign = (TODESIGN)event.GetInt();
	PopulateViewChoice();
}

void ShipDesigner::OnViewboxSelected( wxCommandEvent& event )
{
	m_ToView = (TOVIEW)event.GetInt();
	PopulateViewChoice();
}

