/////////////////////////////////////////////////////////////////////////////
// Name:        racewizard.h
// Purpose:     Freestars client
// Author:      Vambola Kotkas
// Created:     21/08/2006 03:43:03
/////////////////////////////////////////////////////////////////////////////
#ifndef _RACEWIZARD_H_
#define _RACEWIZARD_H_

#include "wx/wizard.h"
#include "wx/spinbutt.h"
#include "RaceBuild.h"

/** \file racewizard.h
 * \brief Race Wizard
 * \ingroup Client
 */

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_RACE_WIZARD 10000

#define ID_EMBLEME 10013
#define ID_LEFTOVER_POINTS 10014
#define ID_SPINEMBLEME 10015
#define ID_RACE_NAME 10016
#define ID_PLURAL_NAME 10018
#define ID_PASSWORD 10019
#define ID_PRT 10020
#define ID_LEFTOVER 10021
#define ID_STEP2 10022
#define ID_LRT_FIRST 10023
#define ID_LRT_LAST 10039

enum HabCtrlID {
	HCI_TEXT = 0,		//text describing the hab like "Radiation: 5mR to 45mR" 
	HCI_LEFT,			//shift left button
	HCI_GRAPH,			//graph of hab
	HCI_RIGHT,			//shift right button 
	HCI_EXPAND,			//widen button
	HCI_IMMUNE,			//immune checkbox
	HCI_SHRINK,			//narroven button
	HCI_COUNT			//count of controls in block (7) ;)
};

#define ID_HAB_0 10040	// grav?
#define ID_HAB_1 10047	// temp?
#define ID_HAB_2 10054	// rad?
#define ID_HAB_3 10061	// reserved
#define MAX_RW_HABS 4

#define ID_GROWTH_RATE 10068
#define ID_GROWTH_SPIN 10069
#define ID_POP_EFF_1 10070
#define ID_POP_EFF_2 10071
#define ID_POP_EFF_SPIN 10072
#define ID_FAC_EFF_1 10073
#define ID_FAC_EFF_SPIN 10074
#define ID_FAC_EFF_2 10075
#define ID_FAC_COST_1 10076
#define ID_FAC_COST_SPIN 10077
#define ID_FAC_COST_2 10078
#define ID_FAC_OPER_1 10079
#define ID_FAC_OPER_SPIN 10080
#define ID_FAC_OPER_2 10081
#define ID_LESS_GERM 10082
#define ID_MIN_EFF_1 10083
#define ID_MIN_EFF_SPIN 10084
#define ID_MIN_EFF_2 10085
#define ID_MIN_COST_1 10086
#define ID_MIN_COST_SPIN 10087
#define ID_MIN_COST_2 10088
#define ID_MIN_OPER_1 10089
#define ID_MIN_OPER_SPIN 10090
#define ID_MIN_OPER_2 10091
#define ID_TECH_COST_FIRST 10092
#define ID_TECH_COST_LAST 10102
#define ID_START_HIGHER 10103

////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

///******************************************************************
/// RACEWIZARD
///******************************************************************
class RaceWizard: public wxWizard
{    
    DECLARE_DYNAMIC_CLASS( RaceWizard )
    DECLARE_EVENT_TABLE()

protected:
	// PAGE 1
	class P1;
	// PAGE 2
	class P2;
	// PAGE 3
	class P3;
	// PAGE 4
	class P4;
	// PAGE 5
	class P5;

	RaceBuild* m_pRace;
public:
    /// Constructors
    RaceWizard( );
	RaceWizard( RaceBuild* race, wxWindow* parent, 
		wxWindowID id = ID_RACE_WIZARD, const wxPoint& pos = wxDefaultPosition );

    /// Creation
    bool Create( wxWindow* parent, 
		wxWindowID id = ID_RACE_WIZARD, const wxPoint& pos = wxDefaultPosition );

    /// Creates the controls and sizers
    void CreateControls();

    /// Runs the wizard.
    bool Run();

	void OnFinish(wxWizardEvent& WXUNUSED(event));

};


///******************************************************************
/// PAGE 1 OF RACEWIZARD 
///******************************************************************
class RaceWizard::P1: public wxWizardPageSimple
{    
	DECLARE_DYNAMIC_CLASS( P1 )
	DECLARE_EVENT_TABLE()

	RaceWizard* m_pParent;
	int m_PrtNames;
	wxString* m_pPrtName;
	RaceBuild* m_pRace;

public:
	/// Constructors
	P1( );
	P1( RaceWizard* parent );
	virtual ~P1(void);


	/// Creation
	bool Create();

	/// Creates the controls and sizers
	void CreateControls();

	void OnPrtSelected( wxCommandEvent& event );
	void OnPageChanged( wxWizardEvent& WXUNUSED(event) ) {ShowPoints();}
	void OnRaceNameUpdated( wxCommandEvent& event );
	void OnPluralNameUpdated( wxCommandEvent& event );
	void OnSpinEmbleme ( wxSpinEvent& event );
	
protected:
	void ShowPoints();


};


///******************************************************************
/// PAGE 2 OF RACEWIZARD 
///******************************************************************
class RaceWizard::P2: public wxWizardPageSimple
{    
	DECLARE_DYNAMIC_CLASS( P2 )
	DECLARE_EVENT_TABLE()

	RaceWizard* m_pParent;
	int m_LrtNames;
	wxString* m_pLrtName;
	RaceBuild* m_pRace;

public:
	/// Constructors
	P2( );
	P2( RaceWizard* parent );
	virtual ~P2(void);

	/// Creation
	bool Create();

	/// Creates the controls and sizers
	void CreateControls();

	void OnPageChanged( wxWizardEvent& WXUNUSED(event) ) {ShowPoints();}
	void OnLrtClick( wxCommandEvent& event );

protected:
	void ShowPoints();
};

///******************************************************************
/// PAGE 3 OF RACEWIZARD 
///******************************************************************
class RaceWizard::P3: public wxWizardPageSimple
{    
	DECLARE_DYNAMIC_CLASS( P3 )
	DECLARE_EVENT_TABLE()

	RaceWizard* m_pParent;
	RaceBuild* m_pRace;

	class HabPanel;
public:
	/// Constructors
	P3( );
	P3( RaceWizard* parent );

	/// Creation
	bool Create();

	/// Creates the controls and sizers
	void CreateControls();

	void OnPageChanged( wxWizardEvent& WXUNUSED(event) ) {ShowPoints();}
	void OnImmuneClick( wxCommandEvent& event );
	void OnButtonClick( wxCommandEvent& event );
	void OnGrowthSpin( wxSpinEvent& event );
protected:
	void ShowPoints();

};


///******************************************************************
/// PAGE 4 OF RACEWIZARD 
///******************************************************************
class RaceWizard::P4: public wxWizardPageSimple
{    
	DECLARE_DYNAMIC_CLASS( P4 )
	DECLARE_EVENT_TABLE()

	RaceWizard* m_pParent;
	RaceBuild* m_pRace;
public:
	/// Constructors
	P4( );
	P4( RaceWizard* parent );

	/// Creation
	bool Create();

	/// Creates the controls and sizers
	void CreateControls();

	void OnPageChanged( wxWizardEvent& WXUNUSED(event) ) {
			FindWindowById(wxID_FORWARD,NULL)->Enable();
			ShowPoints();}
	void OnLessGerm( wxCommandEvent& event );
	void OnSpin( wxSpinEvent& event );
protected:
	void ShowPoints();
};

///******************************************************************
/// PAGE 5 OF RACEWIZARD
///******************************************************************
class RaceWizard::P5: public wxWizardPageSimple
{    
	DECLARE_DYNAMIC_CLASS( P5 )
	DECLARE_EVENT_TABLE()

	RaceWizard* m_pParent;
	RaceBuild* m_pRace;
public:
	/// Constructors
	P5( );
	P5( RaceWizard* parent );

	/// Creation
	bool Create();

	/// Creates the controls and sizers
	void CreateControls();

	void OnPageChanged( wxWizardEvent& WXUNUSED(event) );
	void OnTechCostSelected( wxCommandEvent& event );
	void OnStartAt( wxCommandEvent& event );

protected:

	void ShowPoints();
};


///******************************************************************
/// HABPANEL OF PAGE 3 OF RACEWIZARD 
///******************************************************************
class RaceWizard::P3::HabPanel : public wxPanel
{    
	DECLARE_DYNAMIC_CLASS( HabPanel )
	DECLARE_EVENT_TABLE()

	long m_center;
	long m_width;
	long m_hab;
	bool m_mouse_captured;
	P3 *m_pParent;
public:
	/// Constructors
	HabPanel( ) {}
	HabPanel( wxWindow* parent, long id, long center, long width, long hab)
		:wxPanel(parent, id)
		,m_pParent((P3*)parent)
		,m_center(center)
		,m_width(width) 
		,m_hab(hab) 
		,m_mouse_captured(false) {}

	long Center() {return m_center;}
	long Width() {return m_width;}

	void SetImmune(bool yess);
	void Left();
	void Right();
	void Expand();
	void Shrink();

	/// wxEVT_SIZE event handler
	void OnSize( wxSizeEvent& WXUNUSED(event));

	/// wxEVT_PAINT event handler
	void OnPaint( wxPaintEvent& WXUNUSED(event) );

	/// wxEVT_LEFT_DOWN event handler
	void OnLeftDown( wxMouseEvent& event );

	/// wxEVT_LEFT_UP event handler
	void OnLeftUp( wxMouseEvent& event );

	/// wxEVT_MOTION event handler
	void OnMotion( wxMouseEvent& event );

};


#endif
    // _RACEWIZARD_H_
