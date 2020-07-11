
#include <string>
#include <iostream>




using namespace std;





const string command_create_large = "create";
const string command_create_small = "cr";
const string command_load_large   = "load";
const string command_load_small   = "ld";
const string command_quit_large   = "quit";
const string command_quit_small   = "qt";
	



void printInitialMessage ();
bool isCommandToQuit ();
void printAskWhatToDo ();
void executeCommand (string );
void handleIncorrectCommand ();




int main (int argc, char * argv [], char * envp [])
{
	// Что вы хотите сделать? Создать новое дерево или загрузить имеющееся? Или выйти?
	printInitialMessage();

	string whatToDo;
	while ( !isCommandToQuit() )
	{
		printAskWhatToDo();
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

		- завершить редактирование
		- возобновить редактирование
		
		- рассчитать
		- показать результаты
		- рассчитать и показать результаты
	*/

	return 0;
}




void printInitialMessage ()
{
#pragma todo
}

bool isCommandToQuit ()
{
#pragma todo
	return false;
}

void printAskWhatToDo ()
{
#pragma todo
}

void executeCommand (string)
{
#pragma todo
}

void handleIncorrectCommand ()
{
#pragma todo
}
