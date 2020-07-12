
#include <sstream>
#include <cctype>
#include <vector>
#include <deque>


#include "commands.h"





using namespace deleting_modes_keys;
using namespace inputs_parameters_keys;
using namespace converters_types_keys;
using namespace loads_types_keys;



#pragma todo move to other file
enum class CvType {VOLTAGE, CURRENT};
void CreateTree (string name);
void CreateInput (string name, CvType );





using TokensList = deque<string>;




class Command
{
	public:
		virtual void execute (TokensList & args) const { throw exception("The \"excecute\" method of parent class Command was used."); }

	protected:
		bool isName (const string & str) const
		{
#pragma todo in my library
			for (const auto & letter : str)
				if (!isalpha(letter))
					return false;
			return true;
		};
};

class CommandQuit : public Command 
{
	public:
		virtual void execute (TokensList & args) const override { return; }
};

class CommandCreate : public Command
{
	public:
		virtual void execute (TokensList & args) const override
		{
			string name = default_tree_name;
			string firstInputName = default_input_name;
			CvType firstInputCvType;

			if ( isName(args.front()) )
			{
				name = args.front();
				args.pop_front();
			}
			
		}
};




const map<string, Command> commandDictionary = { {"qt", CommandQuit()    },
												 {"cr", CommandCreate()} };









TokensList tokenize(string& tokens_str)
{
	return TokensList();
}




bool isCommandToQuit (string command)
{
	if (command == quit_mnem)    return true;
	return false;
}

void executeCommand (string enteredCommand)
{
#pragma todo Whether replace deque with a vector?
	auto tokens = tokenize(enteredCommand);
	string commandMnemonic = tokens.front();
	tokens.pop_front();
	
	Command currentCommand;
	try { currentCommand = commandDictionary.at(commandMnemonic); }
	catch (out_of_range()) { throw exception("Unrecognized command"); }

	currentCommand.execute(tokens);
}


command_mnemonic extractCommandMnemonicFrom(string commandWithParameters)
{
	return command_mnemonic();
}







#pragma todo move to other file
void CreateTree(string name)
{
	
}

void CreateInput(string name, CvType)
{
	
}

