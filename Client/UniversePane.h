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

/** \file UniversePane.h
 * \brief Universe window
 * \ingroup Client
 */

#if !defined(UniversePane_h)
#define UniversePane_h


class UniversePane: public wxScrolledWindow
{
public:
    UniversePane(MainWindow * mainw);
    void OnPaint( wxPaintEvent &WXUNUSED( event ) );
	void OnRightDown(wxMouseEvent &me);
	void OnLeftDown(wxMouseEvent &me);
	void OnSelectLocation(wxCommandEvent& event);
	void SelectLocation(FreeStars::SpaceObject * loc);

    DECLARE_EVENT_TABLE()

private:
	deque<FreeStars::SpaceObject *> * mLastSelected;
	int mpLastSelected;
	MainWindow * mParent;
	double mScale;
};

class wxLocation : public wxObject
{
public:
	wxLocation(FreeStars::SpaceObject * l) : mLocation(l) {}
	virtual ~wxLocation();
	FreeStars::SpaceObject * mLocation;
};

#endif // UniversePane_h
