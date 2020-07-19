
#include <string>
#include <iomanip>

#include "commands/commands.h"
#include "config.h"




const string command_quit_mnemonic	 = "qt";
const string command_quit_large_form = "quit";




void printInitialMessage ();
void handleCommandError (string errorMessage);




int main (int argc, char * argv [], char * envp [])
{
	//test_Commands();
	// Переменное количество аргументов команд
	// API в отдельный файл
	// Валидацию данных делать в процессоре команд и вынести в отдельный файл
	cout.setf(ios::fixed);
	cout << setprecision(3);



	printInitialMessage();

	while (true)
	{
		string newCommand_str;
		getline(cin, newCommand_str); 
		
		if (newCommand_str == command_quit_mnemonic || newCommand_str == command_quit_large_form) break;

		try { executeCommand(newCommand_str); }
		catch (exception& err) { handleCommandError(err.what()); }
	} 

	return 0;
}




void printInitialMessage ()
{
	cout << endl << endl << "The Power trees' solver is here." << endl << "Version: " << version_str 
		 << endl << endl;
	cout << "Please, enter you commands" << endl;
	#pragma todo options of commands
}


void handleCommandError (string errorMessage)
{
	cout << errorMessage << endl;
}
