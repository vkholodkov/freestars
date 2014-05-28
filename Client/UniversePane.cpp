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

#include "UniversePane.h"

using namespace FreeStars;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_EVENT_TABLE(UniversePane, wxScrolledWindow)
  EVT_PAINT(UniversePane::OnPaint)
  EVT_RIGHT_DOWN(UniversePane::OnRightDown)
  EVT_LEFT_DOWN(UniversePane::OnLeftDown)
END_EVENT_TABLE()

wxLocation::~wxLocation()
{
}

UniversePane::UniversePane(MainWindow * mainw) : wxScrolledWindow(mainw, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL)
{
	mParent = mainw;
	mScale = 8.0;
	mLastSelected = NULL;
	SetVirtualSize(100 + 200*mScale, 100 + 200*mScale);
	SetScrollRate(1+2*mScale, 1+2*mScale);
}

void UniversePane::OnPaint( wxPaintEvent &WXUNUSED( event ) )
{
	if (!MainWindow::mPaintOK)
		return;

  wxPaintDC dc( this );
  dc.Clear( );
  int vx=0, vy=0, unitX=0, unitY=0;

  GetViewStart(&vx, &vy);
  GetScrollPixelsPerUnit( &unitX, &unitY );

  int xOffset = -vx*unitX;
  int yOffset = -vy*unitY;

  // Paint the screen black
  dc.SetBrush( *wxBLACK_BRUSH );
  dc.SetPen( wxPen( wxColour( 0, 0, 0 ), 0, wxSOLID ) );
  dc.DrawRectangle( xOffset, yOffset, 3000, 3000 );

  Planet *curPlanet;
  int i;

  // Draw scan
  dc.SetBrush( wxBrush( wxColour( 120, 0, 0 ), wxSOLID ) );
  dc.SetPen( wxPen( wxColour( 120, 0, 0 ), 3, wxSOLID ) );
  for (i = 1; i <= TheGalaxy->GetPlanetCount(); ++i) {
	curPlanet = TheGalaxy->GetPlanet(i);
    if (curPlanet && curPlanet->GetOwner() == TheGame->GetCurrentPlayer())
    {
      dc.DrawCircle( 50+xOffset+(((*curPlanet).GetPosX( ))-1000)*mScale, 50+yOffset+(1200-((*curPlanet).GetPosY( )))*mScale, (*curPlanet).GetScanSpace( )*mScale );
    }
  }

  const Fleet * curFleet;
  for (i = 1; i <= Rules::MaxFleets; ++i) {
	curFleet = TheGame->GetCurrentPlayer()->GetFleet(i);
    if (curFleet)
    {
      dc.DrawCircle( 50+xOffset+(curFleet->GetPosX() - 1000)*mScale, 50+yOffset+(1200 - curFleet->GetPosY())*mScale, curFleet->GetScanSpace()*mScale);
    }
  }

  // Draw penscan
  dc.SetBrush( wxBrush( wxColour( 0, 120, 0 ), wxSOLID ) );
  dc.SetPen( wxPen( wxColour( 0, 120, 0 ), 3, wxSOLID ) );
  for (i = 1; i <= TheGalaxy->GetPlanetCount(); ++i) {
	curPlanet = TheGalaxy->GetPlanet(i);
    if (curPlanet && curPlanet->GetOwner() == TheGame->GetCurrentPlayer())
    {
      dc.DrawCircle( 50+xOffset+(((*curPlanet).GetPosX( ))-1000)*mScale, 50+yOffset+(1200-((*curPlanet).GetPosY( )))*mScale, (*curPlanet).GetScanPenetrating( )*mScale );
    }
  }

  wxPoint TriPoints[] = {wxPoint(0,0), wxPoint(0,6), wxPoint(3,3)};

  for (i = 1; i <= Rules::MaxFleets; ++i) {
	curFleet = TheGame->GetCurrentPlayer()->GetFleet(i);
    if (curFleet)
    {
	  dc.SetBrush( wxBrush( wxColour( 0, 120, 0 ), wxSOLID ) );
	  dc.SetPen( wxPen( wxColour( 0, 120, 0 ), 3, wxSOLID ) );
      dc.DrawCircle( 50+xOffset+(curFleet->GetPosX() - 1000)*mScale, 50+yOffset+(1200 - curFleet->GetPosY())*mScale, curFleet->GetScanPenetrating()*mScale);
	  dc.SetBrush( wxBrush( wxColour( 0, 240, 0 ), wxSOLID ) );
	  dc.SetPen( wxPen( wxColour( 0, 240, 0 ), 3, wxSOLID ) );
	  dc.DrawPolygon(3, TriPoints,  50+xOffset+(curFleet->GetPosX() - 1000)*mScale, 50+yOffset+(1200 - curFleet->GetPosY())*mScale);
    }
  }

  // Draw planets and names
  dc.SetTextForeground( wxColour( 255, 255, 255 ) );
  for (i = 1; i <= TheGalaxy->GetPlanetCount(); ++i) {
	curPlanet = TheGalaxy->GetPlanet(i);
    if (curPlanet)
    {
      dc.SetBrush( wxBrush( wxColour( 0, 255, 0 ), wxSOLID ) );
      dc.SetPen( wxPen( wxColour( 0, 100, 0 ), 3, wxSOLID ) );
      dc.DrawCircle( 50+xOffset+(((*curPlanet).GetPosX( ))-1000)*mScale, 50+yOffset+(1200-((*curPlanet).GetPosY( )))*mScale, 10 );
      dc.DrawText(curPlanet->GetName().c_str(), 60+xOffset+(((*curPlanet).GetPosX( ))-1000)*mScale, 60+yOffset+(1200-((*curPlanet).GetPosY( )))*mScale );
    }
  }
}

void UniversePane::OnRightDown(wxMouseEvent &me)
{
	int x;
	int y;
	CalcUnscrolledPosition(me.m_x, me.m_y, &x, &y);
	x = (x-50)/mScale + 1000;
	y = 1200 - (y-50)/mScale;
	wxMenu menu;
	deque<SpaceObject *> * d;
	d = TheGame->GetClosestTop(x, y);
	if (d != NULL) {
		for (int i = 0; i < d->size(); ++i) {
			menu.Append(ID_SelLocation + i, d->at(i)->GetName(TheGame->GetCurrentPlayer()).c_str());
			Connect(ID_SelLocation + i, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(UniversePane::OnSelectLocation), new wxLocation(d->at(i)));
		}
	}

	PopupMenu(&menu);
}

void UniversePane::OnLeftDown(wxMouseEvent &me)
{
	int x;
	int y;
	CalcUnscrolledPosition(me.m_x, me.m_y, &x, &y);
	x = (x-50)/mScale + 1000;
	y = 1200 - (y-50)/mScale;

	deque<SpaceObject *> * d;
	d = TheGame->GetClosestTop(x, y);
	if (d != NULL) {
		if (d == mLastSelected) {
			mpLastSelected++;
			if (mpLastSelected >= d->size())
				mpLastSelected = 0;
		} else {
			mLastSelected = d;
			mpLastSelected = 0;
		}

		SelectLocation(d->at(mpLastSelected));
	}
}

void UniversePane::OnSelectLocation(wxCommandEvent& event)
{
	wxLocation * wl = dynamic_cast<wxLocation *>(event.m_callbackUserData);
	if (wl != NULL)
		SelectLocation(wl->mLocation);
}

void UniversePane::SelectLocation(SpaceObject * loc)
{
	if (loc == NULL)
		return;

	mParent->SetLocation(loc);
}
