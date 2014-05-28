#ifndef _FREESTARS_H
#define _FREESTARS_H


class FreeStars: public wxApp
{
  virtual bool OnInit();
};


class UniversePane: public wxScrolledWindow
{
  public:
    UniversePane( wxWindow *parent );
    void OnPaint( wxPaintEvent &WXUNUSED( event ) );
    DECLARE_EVENT_TABLE()
};


class MainWindow: public wxFrame
{
  public:
    MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);
    void OnExit( wxCommandEvent& event );
    void OnAbout( wxCommandEvent& event );
    UniversePane *map;
    DECLARE_EVENT_TABLE()
};


enum
{
  ID_New = 1,
  ID_CustomRaceWizard,
  ID_Open,
  ID_Close,
  ID_Save,
  ID_SaveAndSubmit,
  ID_PrintMap,
  ID_Exit,
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
  ID_AboutStars
};


#endif // _FREESTARS_H
