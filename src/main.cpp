
#include <string>
#include <iomanip>


#include "config.h"


#include "tests/tests.h"


#include "command_processor/command_processor.h"




using namespace command_processor;




const string command_quit_mnemonic	 = "qt";
const string command_quit_large_form = "quit";




void printInitialMessage ();
void handleCommandError (string errorMessage);




int main (int argc, char * argv [], char * envp [])
{
#ifdef DEBUG
	TestAll();
#endif




	cout.setf(std::ios::fixed);
	cout << std::setprecision(3);



	printInitialMessage();

	while (true)
	{
		string enteredCommandWithArgs_str;
		do
			getline(cin, enteredCommandWithArgs_str); 
		while (enteredCommandWithArgs_str.size() == 0);
		
		if (enteredCommandWithArgs_str == command_quit_mnemonic || enteredCommandWithArgs_str == command_quit_large_form) break;

		try { executeCommand(enteredCommandWithArgs_str); }
		catch (exception & err) { handleCommandError(err.what()); }
	} 

	return 0;
}




void printInitialMessage ()
{
	cout << endl << endl << "The Power trees' solver is here." << endl << "Version: " << version_str 
		 << endl << endl;
	cout << "Please, enter you commands" << endl;
	cout << "You can create a new power tree using a command \"cr\" or load an existing power tree using a command \"ld\"" << endl;
}


void handleCommandError (string errorMessage)
{
	cout << errorMessage << endl;
}
