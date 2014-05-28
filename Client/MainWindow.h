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

/** \file MainWindow.h
 * \brief Main Window
 * \ingroup Client
 */


#if !defined(MainWindow_h)
#define MainWindow_h

class UniversePane;
class DetailsFleet;
class DetailsPlanet;
class OrdersFleet;
class OrdersPlanet;

/**
 * Main Window.
 * All of the other windows fit into this window.
 */
class MainWindow: public wxFrame
{
public:
	static bool mPaintOK;
    MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);
	~MainWindow();
    void OnExit( wxCommandEvent& WXUNUSED(event) );
    void OnAbout( wxCommandEvent& WXUNUSED(event) );
	void OnOpen( wxCommandEvent& WXUNUSED(event) );
	void OnCustomRaceWizard(wxCommandEvent& WXUNUSED(event));
	void OnShipDesign(wxCommandEvent& WXUNUSED(event));
	void SetLocation(FreeStars::SpaceObject * loc);

private:
    UniversePane *map;
	wxStatusBar * mStatusBar;
	wxToolBar* mToolBar;
	DetailsFleet * mDetailsFleet;
	DetailsPlanet * mDetailsPlanet;
	OrdersFleet * mOrdersFleet;
	OrdersPlanet * mOrdersPlanet;
	wxLayoutManager * mLayoutManager;

public:
    DECLARE_EVENT_TABLE()
};

enum
{
  ID_New = wxID_HIGHEST+1,
  ID_CustomRaceWizard,
  ID_Open,
  ID_Close,
  ID_Save,
  ID_SaveAndSubmit,
  ID_PrintMap,
  ID_Toolbar,
  ID_Find,
  ID_Zoom,
  ID_Zoom25,
  ID_Zoom38,
  ID_Zoom50,
  ID_Zoom75,
  ID_Zoom100,
  ID_Zoom125,
  ID_Zoom150,
  ID_Zoom200,
  ID_Zoom400,
  ID_WindowLayout,
  ID_LayoutLarge,
  ID_LayoutMedium,
  ID_LayoutSmall,
  ID_PlayerColors,
  ID_Race,
  ID_GameParameters,
  ID_WaitForNew,
  ID_Generate,
  ID_ShipDesign,
  ID_Research,
  ID_BattlePlans,
  ID_PlayerRelations,
  ID_ChangePassword,
  ID_Planets,
  ID_Fleets,
  ID_OthersFleets,
  ID_Battles,
  ID_Score,
  ID_Dump,
  ID_UniverseDefinition,
  ID_PlanetInformation,
  ID_FleetInformation,
  ID_Introduction,
  ID_PlayersGuide,
  ID_TechnologyBrowser,
  ID_Tutorial,
  ID_SelLocation,
  ID_Previous,
  ID_Next,
  ID_Rename,
  ID_Goto,
  ID_Transfer,
  ID_MAX_MAIN,
};

#endif // MainWindow_h
