/////////////////////////////////////////////////////////////////////////////
// Name:        shipdesigner.h
// Purpose:     
// Author:      Vambola Kotkas
// Modified by: 
// Created:     02/09/2006 22:51:39
// RCS-ID:      
// Copyright:   Copyright 2006 Vambola Kotkas
// Licence:     
/////////////////////////////////////////////////////////////////////////////
#ifndef _SHIPDESIGNER_H_
#define _SHIPDESIGNER_H_

#include "wx/spinbutt.h"


#define ID_SHIPDESIGNER 10001

#define ID_DESIGNBOX 10014
#define ID_VIEWBOX 10015
#define ID_EDIT 10016
#define ID_VIEWCHOICE 10017
#define ID_DESIGNPANEL 10018
#define ID_THUMB 10019
#define ID_SPINTHUMB 10020


/*!
 * ShipDesigner class declaration
 */

class ShipDesigner: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( ShipDesigner )
    DECLARE_EVENT_TABLE()

	enum TODESIGN 
	{
		TD_SHIPS,
		TD_STARBASES
	} m_ToDesign;

	enum TOVIEW
	{
		TV_EXISTING,
		TV_POSSIBLE,
		TV_ENEMIES,
		TV_COMPONENTS
	} m_ToView;

	int m_ChoosenView;



public:
    /// Constructors
    ShipDesigner( );
    ShipDesigner( wxWindow* parent);


    /// Creates the controls and sizers
    void CreateControls();
	void PopulateViewChoice( wxChoice* view_choice = NULL );

////@begin ShipDesigner event handler declarations
	void OnEdit( wxCommandEvent& event );
	void OnDelete( wxCommandEvent& event );
	void OnDesignboxSelected( wxCommandEvent& event );
	void OnViewboxSelected( wxCommandEvent& event );
////@end ShipDesigner event handler declarations

////@begin ShipDesigner member function declarations


////@end ShipDesigner member function declarations


////@begin ShipDesigner member variables
////@end ShipDesigner member variables
};

#endif
    // _SHIPDESIGNER_H_
