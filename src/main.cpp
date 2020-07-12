
#include <string>

#include "commands/commands.h"
#include "config.h"




void printInitialMessage ();
void handleCommandError (string errorMessage);




int main (int argc, char * argv [], char * envp [])
{
	// Внятное представление команд с полиморфной функцией execute и удобным добавлением новых команд
	// Переменное количество аргументов команд
	// API в отдельный файл
	// Валидацию данных делать в процессоре команд и вынести в отдельный файл
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
