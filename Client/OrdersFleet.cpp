/*
Copyright 2005 Elliott Kleinrock

This file is part of FreeStars, a free clone of the Stars! game.

FreeStars is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

FreeStars is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FreeStars; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

The full GPL Copyright notice should be in the file COPYING.txt

Contact:
Email Elliott at 9jm0tjj02@sneakemail.com
*/

#include "FSClient.h"
#include <wx/sashwin.h>

#include "OrdersFleet.h"

using namespace FreeStars;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_EVENT_TABLE(OrdersFleet, wxPanel)
	EVT_BUTTON(ID_Rename, OrdersFleet::Rename)
	EVT_BUTTON(ID_Goto, OrdersFleet::Goto)
END_EVENT_TABLE()

OrdersFleet::OrdersFleet(MainWindow * parent, wxWindow * frame)
	: wxPanel(frame)
{
	mMainWindow = parent;
	mFleet = NULL;

	wxBoxSizer * top = new wxBoxSizer(wxVERTICAL);
	mFleetStaticBox = new wxStaticBoxSizer(wxVERTICAL, this, "");

	mFleetStaticBox->Add(new wxButton(this, ID_Previous, "Previous"),
		0,		// vertically stretchable
		wxALL,	// has border all around it
		5);	// border size is

	mFleetStaticBox->Add(new wxButton(this, ID_Next, "Next"),
		0,		// vertically stretchable
		wxALL,	// has border all around it
		5);	// border size is

	mFleetStaticBox->Add(new wxButton(this, ID_Rename, "Rename"),
		0,		// vertically stretchable
		wxALL,	// has border all around it
		5);	// border size is

	top->Add(mFleetStaticBox, 0, wxALIGN_LEFT);

	mLocationStaticBox = new wxStaticBoxSizer(wxHORIZONTAL, this, "");

	mLocationStaticBox->Add(new wxButton(this, ID_Goto, "Goto"),
		0,		// vertically stretchable
		wxALL,	// has border all around it
		5);	// border size is

	mLocationStaticBox->Add(new wxButton(this, ID_Transfer, "Transfer"),
		0,		// vertically stretchable
		wxALL,	// has border all around it
		5);	// border size is

	top->Add(mLocationStaticBox, 0, wxALIGN_LEFT);

//	wxStaticBoxSizer * sbs = new wxStaticBoxSizer(wxVERTICAL, this, "");
	SetSizer(top);      // use the sizer for layout

	top->SetSizeHints(this);   // set size hints to honour minimum size

/*	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

	// create text ctrl with minimal size 100x60
	topsizer->Add(
		new wxTextCtrl( this, -1, "My text.", wxDefaultPosition, wxSize(100,60), wxTE_MULTILINE),
		1,            // make vertically stretchable
		wxEXPAND |    // make horizontally stretchable
		wxALL,        //   and make border all around
		10 );         // set border width to 10


	wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );
	button_sizer->Add(
		new wxButton( this, wxID_OK, "OK" ),
		0,           // make horizontally unstretchable
		wxALL,       // make border all around (implicit top alignment)
		10 );        // set border width to 10
	button_sizer->Add(
		new wxButton( this, wxID_CANCEL, "Cancel" ),
		0,           // make horizontally unstretchable
		wxALL,       // make border all around (implicit top alignment)
		10 );        // set border width to 10

	topsizer->Add(
		button_sizer,
		0,                // make vertically unstretchable
		wxALIGN_CENTER ); // no border and centre horizontally

	SetSizer( topsizer );      // use the sizer for layout

	topsizer->SetSizeHints( this );   // set size hints to honour minimum size
*/

//	wxSashWindow * wd = new wxSashWindow(this, -1, wxPoint(10,10), wxSize(150,50), wxCLIP_CHILDREN | wxRAISED_BORDER);
//	wd->AddItem(
//	wd->Show(true);
}

OrdersFleet::~OrdersFleet()
{
}

void OrdersFleet::SetLocation(Fleet * fleet)
{
	mFleet = fleet;
	string t;
	t = fleet->GetName(TheGame->GetCurrentPlayer());
	mFleetStaticBox->GetStaticBox()->SetLabel(t.c_str());

	Planet * p = dynamic_cast<Planet *>(fleet->GetAlsoHere()->at(0));
	if (p == NULL)
		mLocationStaticBox->GetStaticBox()->SetLabel("In Deep Space");
	else {
		t = "Orbiting ";
		t += p->GetName();
		mLocationStaticBox->GetStaticBox()->SetLabel(t.c_str());
	}
}

void OrdersFleet::Rename(wxCommandEvent &)
{
	if (mFleet != NULL) {
		wxDialog dialog;
		dialog.Create(this, -1, "Rename Fleet", wxDefaultPosition, wxSize(300, 100));
		wxBoxSizer * top = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer * size1 = new wxBoxSizer(wxHORIZONTAL);
		size1->Add(new wxStaticText(&dialog, -1, "New name:"), 0, wxALL, 5);
		wxTextCtrl * tb = new wxTextCtrl(&dialog, -1, mFleet->GetName(TheGame->GetCurrentPlayer()).c_str(), wxDefaultPosition, wxSize(200, -1));
		size1->Add(tb, 1, wxALL, 5);
		top->Add(size1, 0, wxEXPAND | wxALIGN_LEFT);
		top->Add(dialog.CreateStdDialogButtonSizer(wxOK | wxCANCEL), 0, wxALIGN_CENTER);
		dialog.SetSizer(top);

		if (dialog.ShowModal() == wxID_OK) {
			string s = tb->GetValue();
			if (s != mFleet->GetName(TheGame->GetCurrentPlayer())) {
				mFleet->SetName(s.c_str());
				mMainWindow->SetLocation(mFleet);
			}
		}
	}
}

void OrdersFleet::Goto(wxCommandEvent &)
{
	if (mFleet == NULL)
		return;

	Planet * p = dynamic_cast<Planet *>(mFleet->GetAlsoHere()->at(0));
	if (p != NULL)
		mMainWindow->SetLocation(p);
}
