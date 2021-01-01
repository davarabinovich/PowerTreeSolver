
#include <string>
#include <iomanip>


#include "commands/commands.h"
#include "config.h"

#include "tests/tests.h"




const string command_quit_mnemonic	 = "qt";
const string command_quit_large_form = "quit";




void printInitialMessage ();
void handleCommandError (string errorMessage);




int main (int argc, char * argv [], char * envp [])
{
#ifdef DEBUG
	TestAll();
#endif


	using namespace commands;




	cout.setf(std::ios::fixed);
	cout << std::setprecision(3);



	printInitialMessage();

	while (true)
	{
		string newCommand_str;
		do
			getline(cin, newCommand_str); 
		while (newCommand_str.size() == 0);
		
		if (newCommand_str == command_quit_mnemonic || newCommand_str == command_quit_large_form) break;

		try { executeCommand(newCommand_str); }
		catch (exception & err) { handleCommandError(err.what()); }
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
