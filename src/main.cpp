
#include <string>
#include <iostream>

#include "config.h"




using namespace std;





namespace commands
{
	const string quit_large_str = "quit";
	const string quit_small_str = "qt";



	const string create_large_str = "create";
	const string create_small_str = "c";

	const string load_large_str = "load";
	const string load_small_str = "ld";

	const string save_large_str = "save";
	const string save_small_str = "sv";



	const string solve_large_str = "solve";
	const string solve_small_str = "slv";

	const string show_results_large_str = "show_results";
	const string show_results_small_str = "sr";

	const string show_tree_large_str = "show_tree";
	const string show_tree_small_str = "st";



	const string create_input_large_str = "create_input";
	const string create_input_small_str = "ci";

	const string delete_input_large_str = "delete_input";
	const string delete_input_small_str = "di";

	const string set_input_params_large_str = "set_input_params";
	const string set_input_params_small_str = "si";

	const string create_converter_large_str = "create_converter";
	const string create_converter_small_str = "cc";

	const string delete_converter_large_str = "delete_converter";
	const string delete_converter_small_str = "dc";

	const string set_converter_params_large_str = "set_converter_params";
	const string set_converter_params_small_str = "sc";

	const string create_load_large_str = "create_load";
	const string create_load_small_str = "cl";

	const string delete_load_large_str = "delete_load";
	const string delete_load_small_str = "dl";

	const string set_load_params_large_str = "set_load_params";
	const string set_load_params_small_str = "sl";
};
	



void printInitialMessage ();
bool isCommandToQuit (string );
void executeCommand (string );
void handleIncorrectCommand ();




int main (int argc, char * argv [], char * envp [])
{
	// Что вы хотите сделать? Создать новое дерево или загрузить имеющееся? Или выйти?
	printInitialMessage();

	string whatToDo;
	while ( !isCommandToQuit(whatToDo) )
	{
		cin >> whatToDo;
		try { executeCommand(whatToDo); }
		catch (exception) { handleIncorrectCommand(); }
	}

	return 0;

	/*Задать название и начать редактирование
	В любой момент обрабатывать команды:
		- создать вход (с параметрами или без)
		- удалить вход (переназначить потомков, удалить потомков, создать новый вход для потомков)
		- задать входу параметры
		
		- создать преобразователь (с параметрами или без)
		- удалить преобразователь (переназначить потомков, удалить потомков, создать новый вход для потомков)
		- задать преобразователю параметры

		- создать нагрузку (с параметрами или без)
		- удалить нагрузку
		- задать нагрузке параметры

		- переподчинить преобразователь или нагрузку входу или преобразователю (поднять потомков на уровень по иерархии, перетащить вслед за родителем)

		- показать структуру
		
		- рассчитать
		- показать результаты
		- рассчитать и показать результаты
	*/

	return 0;
}




void printInitialMessage ()
{
	cout << "The Power trees' solver is here." << endl << "Version: " << version_str << endl << endl;
	cout << "Please, enter you commands" << endl << endl;
	#pragma todo options of commands
}

bool isCommandToQuit (string command)
{
	if (command == commands::quit_large_str || command == commands::quit_small_str)    return true;
	return false;
}

void executeCommand (string command)
{
	using namespace commands;


	if (command == quit_large_str || command == quit_small_str)    return;


	if (command == load_large_str || command == load_small_str)
	{
		// Что делать с текущим?
		return;
	}
	if (command == save_large_str || command == save_small_str)
	{

		return;
	}

	if (command == solve_large_str || command == solve_small_str)
	{
		#pragma todo show results?
		return;
	}
	if (command == show_results_large_str || command == show_results_small_str)
	{
		return;
	}

	if (command == show_tree_large_str || command == show_tree_small_str)
	{
		return;
	}

	if (command == create_large_str || command == create_small_str)
	{
		#pragma todo what to do whit the current?
		return;
	}

	if (command == create_input_large_str || command == create_input_small_str)
	{

		return;
	}

	if (command == delete_input_large_str || command == delete_input_small_str)
	{

		return;
	}

	if (command == set_input_params_large_str || command == set_input_params_small_str)
	{

		return;
	}

	if (command == create_converter_large_str || command == create_converter_small_str)
	{

		return;
	}

	if (command == delete_converter_large_str || command == delete_converter_small_str)
	{

		return;
	}

	if (command == set_converter_params_large_str || command == set_converter_params_small_str)
	{

		return;
	}
	
	if (command == create_load_large_str || command == create_load_small_str)
	{

		return;
	}

	if (command == delete_load_large_str || command == delete_load_small_str)
	{

		return;
	}

	if (command == set_load_params_large_str || command == set_load_params_small_str)
	{

		return;
	}


	throw exception();
}

void handleIncorrectCommand ()
{
	cout << "Invalid command" << endl;
}
