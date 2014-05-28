#include "wx/wx.h"
#include "wx/splitter.h"
#include "wx/sizer.h"
#include "freestars.h"
//#include "../common/Game.h"
//#include "../common/Galaxy.h"
#include "../common/FSServer.h"


BEGIN_EVENT_TABLE(MainWindow, wxFrame)
  EVT_MENU(ID_Exit,       MainWindow::OnExit)
  EVT_MENU(ID_AboutStars, MainWindow::OnAbout)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(UniversePane, wxScrolledWindow)
  EVT_PAINT(UniversePane::OnPaint)
END_EVENT_TABLE()

IMPLEMENT_APP(FreeStars)


bool FreeStars::OnInit()
{
  MainWindow *frame = new MainWindow( "FreeStars", wxPoint(-1,-1), wxSize(800,600) );
  frame->Centre( );
  frame->Show(true);
  SetTopWindow(frame);
  return true;
}


MainWindow::MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size): wxFrame((wxFrame *)NULL, -1, title, pos, size)
{

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
  menuFile->Append( ID_Exit,                "E&xit" );

  menuView->Append( ID_Toolbar,             "&Toolbar" );
  menuView->AppendSeparator();
  menuView->Append( ID_Find,                "&Find...\tCtrl+F" );
  menuView->Append( ID_Zoom,                "&Zoom", menuViewZoom );
    menuViewZoom->AppendCheckItem(ID_Zoom25, "25%");
    menuViewZoom->AppendCheckItem(ID_Zoom38, "38%");
    menuViewZoom->AppendCheckItem(ID_Zoom50, "50%");
    menuViewZoom->AppendCheckItem(ID_Zoom75, "75%");
    menuViewZoom->AppendCheckItem(ID_Zoom100, "100%");
    menuViewZoom->AppendCheckItem(ID_Zoom125, "125%");
    menuViewZoom->AppendCheckItem(ID_Zoom150, "150%");
    menuViewZoom->AppendCheckItem(ID_Zoom200, "200%");
    menuViewZoom->AppendCheckItem(ID_Zoom400, "400%");
    menuViewZoom->Check( ID_Zoom100, true );
  menuView->Append( ID_WindowLayout,        "&Window Layout", menuViewLayout );
    menuViewLayout->AppendCheckItem(ID_LayoutLarge,  "Large Screen");
    menuViewLayout->AppendCheckItem(ID_LayoutMedium, "Medium Screen");
    menuViewLayout->AppendCheckItem(ID_LayoutSmall,  "Small Screen");
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
  menuHelp->Append(ID_AboutStars,           "&About Stars!...");

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

  map = new UniversePane( this );
//  map = new UniversePane( right );
//  map->SetMinSize( wxSize( 600, 400 ) );
//  map->SetSize( 754, 571 );
  map->SetScrollbars( 10, 10, 250, 250 );
  map->Show( true );
  map->Refresh( true, NULL );

  TheGame = new Game( "../Data/TestGame.hst" );
  TheGalaxy = new Galaxy( );
  TheGame->LoadDefFile(  "../Data/Testgame.def" );
  TheGame->LoadHostFile( "../Data/TestGame.hst" );

}


UniversePane::UniversePane( wxWindow *parent ) : wxScrolledWindow( (MainWindow *) parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL )
{
}


void UniversePane::OnPaint( wxPaintEvent &WXUNUSED( event ) )
{
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

  Planet *curPlanet = TheGalaxy->GetFirstPlanet( );

  // Draw scan
  for (curPlanet = TheGalaxy->GetFirstPlanet( );curPlanet != TheGalaxy->GetLastPlanet( );curPlanet = TheGalaxy->GetNextPlanet( curPlanet ))
  {
    if (curPlanet)
    {
      string printstring = (*curPlanet).GetName( );
      dc.SetBrush( wxBrush( wxColour( 120, 0, 0 ), 1 ) );
      dc.SetPen( wxPen( wxColour( 120, 0, 0 ), 3, wxSOLID ) );
      dc.DrawCircle( 50+xOffset+(((*curPlanet).GetPosX( ))-1000)*8, 50+yOffset+(1200-((*curPlanet).GetPosY( )))*8, (*curPlanet).GetScanSpace( ) );
    }
  }

  // Draw penscan
  for (curPlanet = TheGalaxy->GetFirstPlanet( );curPlanet != TheGalaxy->GetLastPlanet( );curPlanet = TheGalaxy->GetNextPlanet( curPlanet ))
  {
    if (curPlanet)
    {
      string printstring = (*curPlanet).GetName( );
      dc.SetBrush( wxBrush( wxColour( 0, 120, 0 ), 1 ) );
      dc.SetPen( wxPen( wxColour( 0, 120, 0 ), 3, wxSOLID ) );
      dc.DrawCircle( 50+xOffset+(((*curPlanet).GetPosX( ))-1000)*8, 50+yOffset+(1200-((*curPlanet).GetPosY( )))*8, (*curPlanet).GetScanPen( ) );
    }
  }

  // Draw planets
  for (curPlanet = TheGalaxy->GetFirstPlanet( );curPlanet != TheGalaxy->GetLastPlanet( );curPlanet = TheGalaxy->GetNextPlanet( curPlanet ))
  {
    if (curPlanet)
    {
      string printstring = (*curPlanet).GetName( );
      dc.SetBrush( wxBrush( wxColour( 0, 255, 0 ), 1 ) );
      dc.SetPen( wxPen( wxColour( 0, 100, 0 ), 3, wxSOLID ) );
      dc.DrawCircle( 50+xOffset+(((*curPlanet).GetPosX( ))-1000)*8, 50+yOffset+(1200-((*curPlanet).GetPosY( )))*8, 10 );
//      cout << (*curPlanet).GetName( ) << ", " << xOffset << ", " << yOffset << ", " << (*curPlanet).GetPosX( ) << ", " << (*curPlanet).GetPosY( ) << endl;
    }
  }

  // Write planet names
  for (curPlanet = TheGalaxy->GetFirstPlanet( );curPlanet != TheGalaxy->GetLastPlanet( );curPlanet = TheGalaxy->GetNextPlanet( curPlanet ))
  {
    if (curPlanet)
    {
      wxString *name = new wxString( (*curPlanet).GetName( ) );
      dc.SetTextForeground( wxColour( 255, 255, 255 ) );
      dc.DrawText( *name, 60+xOffset+(((*curPlanet).GetPosX( ))-1000)*8, 60+yOffset+(1200-((*curPlanet).GetPosY( )))*8 );
    }
  }

//  dc.SetBrush( wxBrush( wxColour( 120, 0, 0 ), 1 ) );
//  dc.SetPen( wxPen( wxColour( 120, 0, 0 ), 0, wxSOLID ) );
//  dc.DrawCircle( xOffset+150, yOffset+150, 100 );

//  dc.SetBrush( wxBrush( wxColour( 0, 120, 0 ), 1 ) );
//  dc.SetPen( wxPen( wxColour( 0, 120, 0 ), 0, wxSOLID ) );
//  dc.DrawCircle( xOffset+150, yOffset+150, 40 );


//  dc.SetBrush( wxBrush( wxColour( 200, 0, 0 ), 1 ) );
//  dc.SetPen( wxPen( wxColour( 80, 0, 0 ), 3, wxSOLID ) );
//  dc.DrawCircle( xOffset+50, yOffset+50, 10 );
}


void MainWindow::OnExit(wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}


void MainWindow::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox( "FreeStars for all major operating systems\nThe Advanced Interstellar Strategy Game\nVersion 0.1\nCopyright (C) 2005 by Pieter Hulshoff",
                "About FreeStars", wxOK | wxICON_INFORMATION, this);
}
