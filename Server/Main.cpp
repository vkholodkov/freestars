/*
Copyright 2003 - 2005 Elliott Kleinrock, Dan Neely, Kurt W. Over, Damon Domjan

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

#include <memory>

#include "FSServer.h"

#include "Battle.h"

using namespace FreeStars;

#include <stdlib.h>

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
class CheckMemLeaks {
public:
	CheckMemLeaks() {}
	~CheckMemLeaks() { _CrtDumpMemoryLeaks(); }
};

CheckMemLeaks cml;
#endif

int main (int argc, char * argv[])
{
	if (argc != 2)
	{
		cerr << "usage : fs_server [file]" << endl;
		return -1;
	}

	bool error = false;

	{
	string HostFile = argv[1];
	string FileType = HostFile.substr(HostFile.find_last_of('.'));

	std::auto_ptr<Game> game(new Game());

	if (stricmp(FileType.c_str(), ".def") == 0) {
		cout << "Loading Defination File" << endl;
		if (!game->LoadDefFile(argv[1]))
			error = true;

		game->InitialSeen();

		if (!error) {
			cout << "Writing XY File" << endl;
			game->WriteXYFile();
		}
	} else if (stricmp(FileType.c_str(), ".hst") == 0) {
		cout << "Loading Host File" << endl;
		if (!game->LoadHostFile(argv[1]))
			error = true;

		cout << "Loading Turns" << endl;
		if (!error && !game->LoadTurns())
			error = true;
		
		cout << "Processing Turns" << endl;
		if (!error && !game->ProcessTurn())
			error = true;
	} else if (FileType[1] == 'm') {
		cout << "Loading player file and orders (.x file)" << endl;
		if (!game->LoadPlayerFile(argv[1]))
			error = true;

		cout << "Writing player orders (.x file)" << endl;
		if (!error && !game->GetCurrentPlayer()->SaveXFile())
			error = true;

		if (!error)
			game->GetCurrentPlayer()->TestUndoRedo();

		error = true;	// always set to true to prevent host file being written when .m file is read
	} else if (FileType[1] == 'r') {
		cout << "Loading race file" << endl;
		game->LoadRules(argv[1], "RaceDefinition");
		Player * p = NULL;
//		game->SetFileLocation(argv[1]);
		p = game->ObjectFactory(p, 1);
		error = p->CreateFromFile(argv[1]) == -1;
		if (!error) {
			cout << "Race " << p->GetSingleName() << " has ";
			cout << Long2String(p->GetAdvantagePoints() / 3);
			cout << " points left over" << endl;
		}

		delete p;
		error = true;	// always set to true to prevent host file being written when .m file is read
	} else {
		cout << "Unknown file type " << FileType << endl;
		error = true;
	}

	if (!error) {
		cout << "Writing Host File" << endl;
		error = !game->WriteHostFile();
	}

	if (!error) {
		cout << "Writing Players Files" << endl;
		error = !game->WritePlayerFiles();
		error = true;
	}
	
	// For debugging
	for (unsigned int i = 0; i < game->GetMessages().size(); ++i)
	{
		string message = game->GetMessages()[i]->ToString();
		/*if(message.find("Error") != -1 || message.find("Warn") != -1)*/
			cout << message << endl;
	}

	for (unsigned int j = 1; j <= game->NumberPlayers(); ++j)
	{
		const Player* p = game->GetPlayer(j);
		if (p != NULL) {
			for (unsigned int i = 0; i < p->GetMessages().size(); ++i)
			{
				string message = p->GetMessages()[i]->ToString();
				/*if(message.find("Error") != -1 || message.find("Warn") != -1)*/
				{
					cout << "Player(" << Long2String(j) << ")" << endl;
					cout << message << endl;
				}
			}
		}
	}

	Rules::Cleanup();
	Component::Cleanup();
	Battle::Cleanup();
	Ship::Cleanup();
	}

	if(error)
		return -1;

	return 0;
}
