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

#include <wx/minifram.h>

// wxDockIt stuff
#include <wx/pane.h>
#include <wx/dockwindow.h>
#include <wx/dockhost.h>
#include <wx/dockpanel.h>

#include "UniversePane.h"
#include "DetailsPlanet.h"
#include "DetailsFleet.h"
#include "OrdersPlanet.h"
#include "OrdersFleet.h"

#include "Battle.h"
#include "RaceBuild.h"
#include "racewizard.h"

#include "shipdesigner.h"


using namespace FreeStars;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool MainWindow::mPaintOK = false;

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
  EVT_MENU(wxID_EXIT,		MainWindow::OnExit)
  EVT_MENU(wxID_ABOUT,		MainWindow::OnAbout)
  EVT_MENU(ID_Open,			MainWindow::OnOpen)
  EVT_MENU(ID_CustomRaceWizard,MainWindow::OnCustomRaceWizard)
  EVT_MENU(ID_ShipDesign,MainWindow::OnShipDesign)
END_EVENT_TABLE()

MainWindow::MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size): wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	TheGame = new CGame();
	TheGalaxy = new CGalaxy( );
	TheGame->LoadPlayerFile("Data/TestGame.m1");

  wxMenu *menuFile       = new wxMenu;
  wxMenu *menuView       = new wxMenu;
  wxMenu *menuViewZoom   = new wxMenu;
  wxMenu *menuViewLayout = new wxMenu;
  wxMenu *menuTurn       = new wxMenu;
  wxMenu *menuCommands   = new wxMenu;
  wxMenu *menuReport     = new wxMenu;
  wxMenu *menuReportDump = new wxMenu;
  wxMenu *menuHelp       = new wxMenu;

  menuFile->Append( ID_New,                 "&New...\tCtrl+N" );
  menuFile->Append( ID_CustomRaceWizard,    "Custom &Race Wizard..." );
  menuFile->Append( ID_Open,                "&Open...\tCtrl+O" );
  menuFile->Append( ID_Close,               "&Close" );
  menuFile->Append( ID_Save,                "&Save\tCtrl+S" );
  menuFile->Append( ID_SaveAndSubmit,       "Save &And Submit\tCtrl+A" );
  menuFile->AppendSeparator();
  menuFile->Append( ID_PrintMap,            "&Print Map" );
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT,				"E&xit");

  menuView->Append( ID_Toolbar,             "&Toolbar" );
  menuView->AppendSeparator();
  menuView->Append( ID_Find,                "&Find...\tCtrl+F" );
  menuView->Append( ID_Zoom,                "&Zoom", menuViewZoom );
	menuViewZoom->AppendRadioItem(ID_Zoom25, "25%");
	menuViewZoom->AppendRadioItem(ID_Zoom38, "38%");
	menuViewZoom->AppendRadioItem(ID_Zoom50, "50%");
	menuViewZoom->AppendRadioItem(ID_Zoom75, "75%");
	menuViewZoom->AppendRadioItem(ID_Zoom100, "100%");
	menuViewZoom->AppendRadioItem(ID_Zoom125, "125%");
	menuViewZoom->AppendRadioItem(ID_Zoom150, "150%");
	menuViewZoom->AppendRadioItem(ID_Zoom200, "200%");
	menuViewZoom->AppendRadioItem(ID_Zoom400, "400%");
//    menuViewZoom->Check( ID_Zoom400, true );
  menuView->Append( ID_WindowLayout,        "&Window Layout", menuViewLayout );
	menuViewLayout->AppendRadioItem(ID_LayoutLarge,  "Large Screen");
	menuViewLayout->AppendRadioItem(ID_LayoutMedium, "Medium Screen");
	menuViewLayout->AppendRadioItem(ID_LayoutSmall,  "Small Screen");
	menuViewLayout->Check( ID_LayoutLarge, true );
  menuView->Append( ID_PlayerColors,        "Player &Color" );
  menuView->AppendSeparator();
  menuView->Append( ID_Race,                "&Race...\tF8" );
  menuView->Append( ID_GameParameters,      "&Game Parameters..." );

  menuTurn->Append(ID_WaitForNew,           "&Wait for New");
  menuTurn->Append(ID_Generate,             "&Generate\tF9");

  menuCommands->Append(ID_ShipDesign,       "&Ship Design...\tF4");
  menuCommands->Append(ID_Research,         "&Research...\tF5");
  menuCommands->Append(ID_BattlePlans,      "&Battle Plans...\tF6");
  menuCommands->Append(ID_PlayerRelations,  "&Player Relations...\tF7");
  menuCommands->AppendSeparator();
  menuCommands->Append(ID_ChangePassword,   "C&hange Password...");

  menuReport->Append(ID_Planets,            "&Planets...\tF3");
  menuReport->Append(ID_Fleets,             "&Fleets...\tF3");
  menuReport->Append(ID_OthersFleets,       "&Others' Fleets...\tF3");
  menuReport->AppendSeparator();
  menuReport->Append(ID_Battles,            "&Battles...\tF3");
  menuReport->AppendSeparator();
  menuReport->Append(ID_Score,              "&Score...\tF10");
  menuReport->AppendSeparator();
  menuReport->Append(ID_Dump,               "Dump to Text File", menuReportDump);
    menuReportDump->Append(ID_UniverseDefinition, "Universe Definition");
    menuReportDump->Append(ID_PlanetInformation,  "Planet Information");
    menuReportDump->Append(ID_FleetInformation,   "Fleet Information");

  menuHelp->Append(ID_Introduction,         "&Introduction");
  menuHelp->Append(ID_PlayersGuide,         "&Players Guide\tF1");
  menuHelp->AppendSeparator();
  menuHelp->Append(ID_TechnologyBrowser,    "&Technology Browser\tF2");
  menuHelp->Append(ID_Tutorial,             "T&utorial");
  menuHelp->AppendSeparator();
  menuHelp->Append(wxID_ABOUT,				"&About Stars!");

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append( menuFile,     "&File" );
  menuBar->Append( menuView,     "&View" );
  menuBar->Append( menuTurn,     "&Turn" );
  menuBar->Append( menuCommands, "&Commands" );
  menuBar->Append( menuReport,   "&Report" );
  menuBar->Append( menuHelp,     "&Help" );

  SetMenuBar( menuBar );

  this->SetMinSize( wxSize( 640, 400 ) );

//  wxSplitterWindow *vertical = new wxSplitterWindow( this );
//  wxSplitterWindow *left     = new wxSplitterWindow( vertical );
//  wxSplitterWindow *right    = new wxSplitterWindow( vertical );
//  vertical->SplitVertically( left, right, 40 );
//  right->SetMinimumPaneSize( 40 );
//  left->SetMaxSize( wxSize( 40, 2000 ) );
	mStatusBar = CreateStatusBar();
	mToolBar = CreateToolBar();

	mLayoutManager = new wxLayoutManager( this );
    mLayoutManager->AddDefaultHosts();

    // add main client window
    wxPane * pPane = new wxPane(this, wxID_ANY, "");
    mLayoutManager->SetLayout(wxDWF_SPLITTER_BORDERS, pPane);
    pPane->ShowCloseButton(false);
    pPane->ShowHeader(false);

    // create dockwindows
    wxDockWindow * pDockWindow1;
	pDockWindow1 = new wxDockWindow(this, wxID_ANY, "Details - Planet", wxDefaultPosition, wxDefaultSize, "dp");//, wxDWC_NO_CONTROLS);
	mDetailsPlanet = new DetailsPlanet(this, pDockWindow1);
    pDockWindow1->SetClient(mDetailsPlanet);
    mLayoutManager->AddDockWindow(pDockWindow1);

    wxDockWindow * pDockWindow2;
	pDockWindow2 = new wxDockWindow(this, wxID_ANY, "Details - Fleet", wxDefaultPosition, wxDefaultSize, "df");//, wxDWC_NO_CONTROLS);
	mDetailsFleet = new DetailsFleet(this, pDockWindow2);
    pDockWindow2->SetClient(mDetailsFleet);
    mLayoutManager->AddDockWindow(pDockWindow2);

    wxDockWindow * pDockWindow3;
	pDockWindow3 = new wxDockWindow(this, wxID_ANY, "Orders - Planet", wxDefaultPosition, wxDefaultSize, "op");//, wxDWC_NO_CONTROLS);
	mOrdersPlanet = new OrdersPlanet(this, pDockWindow3);
    pDockWindow3->SetClient(mOrdersPlanet);
    mLayoutManager->AddDockWindow(pDockWindow3);

    wxDockWindow * pDockWindow4;
	pDockWindow4 = new wxDockWindow(this, wxID_ANY, "Orders - Fleet", wxDefaultPosition, wxDefaultSize, "of");//, wxDWC_NO_CONTROLS);
	mOrdersFleet = new OrdersFleet(this, pDockWindow4);
    pDockWindow4->SetClient(mOrdersFleet);
    mLayoutManager->AddDockWindow(pDockWindow4);

	// dock windows
    wxHostInfo hi;
    hi = mLayoutManager->GetDockHost(wxDEFAULT_BOTTOM_HOST);
	hi.SetPlacement ( wxHIP_FRONT);
	hi.GetHost()->SetAreaSize(mDetailsPlanet->GetMinSize().GetHeight());
	mLayoutManager->DockWindow(pDockWindow1, hi);

    hi = mLayoutManager->GetDockHost(wxDEFAULT_BOTTOM_HOST);
	hi.SetPlacement ( wxHIP_BACK);
	hi.SetPanel ( pDockWindow1->GetDockPanel());
	hi.GetHost()->SetAreaSize(mDetailsFleet->GetMinSize().GetHeight());
    mLayoutManager->DockWindow(pDockWindow2, hi);

    hi = mLayoutManager->GetDockHost(wxDEFAULT_LEFT_HOST);
	hi.GetHost()->SetAreaSize(128);
	hi.SetPlacement( wxHIP_FRONT);
	hi.GetHost()->SetAreaSize(mOrdersPlanet->GetMinSize().GetWidth());
    mLayoutManager->DockWindow(pDockWindow3, hi);

    hi = mLayoutManager->GetDockHost(wxDEFAULT_LEFT_HOST);
	hi.SetPlacement ( wxHIP_BACK);
	hi.GetHost()->SetAreaSize(mOrdersFleet->GetMinSize().GetWidth());
	hi.SetPanel ( pDockWindow3->GetDockPanel());
    mLayoutManager->DockWindow(pDockWindow4, hi);

	map = new UniversePane(this);
	map->Show( true );
	map->Refresh( true, NULL );
	pPane->SetClient(map);

	mPaintOK = true;
}

MainWindow::~MainWindow()
{
	delete mLayoutManager;
	Rules::Cleanup();
	Component::Cleanup();
	Battle::Cleanup();
	Ship::Cleanup();
	delete TheGalaxy;
	delete TheGame;
}

void MainWindow::OnExit(wxCommandEvent& WXUNUSED(event))
{
	mPaintOK = false;

	Close(true);
}

void MainWindow::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox( "FreeStars for all major operating systems\nThe Advanced Interstellar Strategy Game\nVersion 0.1\nCopyright (C) 2005 by Pieter Hulshoff",
                "About FreeStars", wxOK | wxICON_INFORMATION, this);
}

void MainWindow::OnOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog fd(this, "Open Freestars File", TheGame->GetFileLoc().c_str());
	fd.SetWildcard("Freestars Files|*.m*;*.r*;*.hst|Freestars Game|*.m*|Freestars Race|*.r*|Freestars Host|*.hst|All Files|*");

	if (fd.ShowModal() == wxID_OK) 
	{
		wxString path = fd.GetPath();
		wxString ext = path.AfterLast('.').MakeLower();
		if (ext[0]=='r')
		{
			//so its race file
			::wxBeginBusyCursor();
			Rules::Cleanup();
			Component::Cleanup();
			Battle::Cleanup();
			Ship::Cleanup();
			delete TheGalaxy;
			delete TheGame;
			TheGame = new CGame();
			TheGalaxy = new CGalaxy( );
			if (!TheGame->LoadRules(path, "RaceDefinition"))
			{
				::wxEndBusyCursor();
				wxMessageBox("No rules for race, loading default ones!","Sorry!");
				::wxBeginBusyCursor();
				TheGame->LoadRules("MyModRules.xml", NULL, 0.0, false);
			}

			RaceBuild * race = new RaceBuild();

			if (race->CreateFromFile(path) == -1)
			{
				::wxEndBusyCursor();
				wxMessageBox("Failed to load race file!","Sorry!");
			}
			else
			{
				::wxEndBusyCursor();
				bool ret;
				//race wizard
				RaceWizard rw(race, this);
				if (rw.Run())
				{
					//save the race
					wxFileDialog sfd(this, "Save Race File"
							,TheGame->GetFileLoc().c_str(),"stars.r1"
							,"Freestars Race|*.r*|All Files|*"
							,wxSAVE|wxOVERWRITE_PROMPT);
					
					if (sfd.ShowModal() == wxID_OK) 
					{
						wxString path = sfd.GetPath();
						race->WriteRaceFile(path);
					}
				}
				/*else
				{
				}*/
			}
			delete race;
		}
		else if (ext=="hst")
		{
			//host file
			wxMessageBox("Opening Host Files is not implemented!","Sorry!");
		}
		else if (ext[0]=='m')
		{
			//game file
			::wxBeginBusyCursor();
			Rules::Cleanup();
			Component::Cleanup();
			Battle::Cleanup();
			Ship::Cleanup();
			delete TheGalaxy;
			delete TheGame;
			TheGame = new CGame();
			TheGalaxy = new CGalaxy( );
			TheGame->LoadPlayerFile(path);
			map->Refresh(true, NULL);
			::wxEndBusyCursor();
		}
		else
		{
			//unknown file
			wxMessageBox("Unknown file type?!?","Error Error Error");
		}
	}
}


void MainWindow::OnShipDesign(wxCommandEvent& WXUNUSED(event))
{
	ShipDesigner designer(this);
	designer.ShowModal();

}

void MainWindow::OnCustomRaceWizard(wxCommandEvent& WXUNUSED(event))
{
	//not sure if its fair to trash everything in sight
	::wxBeginBusyCursor();
	Rules::Cleanup();
	Component::Cleanup();
	Battle::Cleanup();
	Ship::Cleanup();
	delete TheGalaxy;
	delete TheGame;
	TheGame = new CGame();
	TheGalaxy = new CGalaxy( );
	TheGame->LoadRules("MyModRules.xml", NULL, 0.0, false);

	RaceBuild * race = new RaceBuild();

	if (race->CreateFromFile("rules\\default.r0") == -1)
	{
		::wxEndBusyCursor();
		wxMessageBox("Failed to load default race!","Sorry!");
	}
	else
	{
		bool ret;
		//race wizard
		RaceWizard rw(race, this);
		::wxEndBusyCursor();
		if (rw.Run())
		{
			//save the race
			wxFileDialog sfd(this, "Save Race File"
					,TheGame->GetFileLoc().c_str(),"stars.r1"
					,"Freestars Race|*.r*|All Files|*"
					,wxSAVE|wxOVERWRITE_PROMPT);
			
			if (sfd.ShowModal() == wxID_OK) 
			{
				wxString path = sfd.GetPath();
				race->WriteRaceFile(path);
			}
		}
	}
	delete race;
}

void MainWindow::SetLocation(FreeStars::SpaceObject * loc)
{
	Fleet * f = dynamic_cast<Fleet *>(loc);
	if (f != NULL) {
		if (f->GetOwner() == TheGame->GetCurrentPlayer()) 
		{
			mOrdersFleet->SetLocation(f);
//			mOrdersFleet->SetFocus();
		} 
		else 
		{
			mDetailsFleet->SetLocation(f);
//			mDetailsFleet->SetFocus();
		}
//		SetFocus();
	}

	Planet * p = dynamic_cast<Planet *>(loc);
	if (p != NULL) {
		mDetailsPlanet->SetLocation(p);
//		mDetailsPlanet->SetFocus();
		if (p->GetOwner() == TheGame->GetCurrentPlayer()) {
			mOrdersPlanet->SetLocation(p);
//			mOrdersPlanet->SetFocus();
		}
//		SetFocus();
	}
}
