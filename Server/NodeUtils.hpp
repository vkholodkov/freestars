/*
Copyright 2006 Elliott Kleinrock, Brandon Bergren

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

/* Note: This file has templates used by other code, so the whole thing
 * needs to be included.
 */

namespace FreeStars {

template <class T>
bool ParseGroup(const TiXmlNode * node, deque<T*> & deck)
{
	const TiXmlNode * w;
	for(w = node->FirstChildElement(T::ELEMENT_NAME()); w; w = w->NextSiblingElement(T::ELEMENT_NAME())) {
		T* obj = NULL;
		obj = TheGame->ObjectFactory(obj);

		if(!obj->ParseNode(w)){
			delete obj;
			return false;
		}
		deck.push_back(obj);
	}
	return true;
}

template <class T>
bool WriteGroup(TiXmlNode * node, const Player * viewer, const deque<T*> & deck)
{
	int i;
	for (i = 0; i < deck.size(); ++i) {
		TiXmlElement * obj = new TiXmlElement(T::ELEMENT_NAME());
		deck[i]->WriteNode(obj, viewer);
		if (!obj->NoChildren())
			node->LinkEndChild(obj);
		else
			delete obj;
	}
	return true;
}

}
