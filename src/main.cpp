
#include <string>

#include "commands/commands.h"
#include "config.h"




void printInitialMessage ();
void handleCommandError (string errorMessage);




int main (int argc, char * argv [], char * envp [])
{
	printInitialMessage();

	string whatToDo;
	while ( !isCommandToQuit(whatToDo) )
	{
		cin >> whatToDo;
		try { executeCommand(whatToDo); }
		catch (exception & err) { handleCommandError(err.what()); }
	}

	return 0;
}




void printInitialMessage ()
{
	cout << endl << endl << "The Power trees' solver is here." << endl << "Version: " << version_str << endl << endl;
	cout << "Please, enter you commands" << endl << endl;
	#pragma todo options of commands
}


void handleCommandError (string errorMessage)
{
	cout << errorMessage << endl;
}
