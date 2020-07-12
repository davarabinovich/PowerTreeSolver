
#include <string>

#include "commands/commands.h"
#include "config.h"




void printInitialMessage ();
void handleCommandError (string errorMessage);




int main (int argc, char * argv [], char * envp [])
{
	// Переменное количество аргументов команд
	// API в отдельный файл
	// Валидацию данных делать в процессоре команд и вынести в отдельный файл
	printInitialMessage();

	string newCommand_str;
	do
	{
		getline(cin, newCommand_str);

		try { executeCommand(newCommand_str); }
		catch (exception& err) { handleCommandError(err.what()); }
	} 
	while ( !isCommandToQuit(newCommand_str) );

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
