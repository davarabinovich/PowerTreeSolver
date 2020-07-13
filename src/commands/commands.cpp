
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
void CreateTree () {}
void CreateTree (string name) {}

void CreateInput (CvType cvType, double cvValue) {}
void CreateInput (string name, CvType cvType, double cvValue) {}





using TokensList = deque<string>;




class Command
{
	public:

		virtual void execute (TokensList & tokens) const 
		{ throw exception("The \"excecute\" method of parent class Command was used."); }

	protected:
		
		static bool isName (const string & str)
		{
#pragma todo in my library
			for (const auto & letter : str)
				if (!isalpha(letter))
					return false;
			return true;
		};

		static bool isFloatNumber (const string & str)
		{
			auto letter_it = str.cbegin();

			for ( ; letter_it != str.cend(); letter_it++)
			{
				if (*letter_it == '.' || *letter_it == ',') break;
				if (isdigit(*letter_it))
					return false;
			}

			for ( ; letter_it != str.cend(); letter_it++)
			{
				if (isdigit(*letter_it))
					return false;
			}

			return true;
		}

		static bool isCvType (const string & str)
		{
			if (str[0] != 'c' && str[0] != 'C' && str[0] != 'v' && str[0] != 'V') return false;
			if (str != "cur" && str != "Cur" && str != "current" && str != "Current")
				if (str != "vol" && str != "Vol" && str != "voltage" && str != "Voltage") return false;
			return true;
		}

		static CvType parseCvType(const string & str)
		{
			if (!isCvType(str))
#pragma todo write exceptions message
				throw exception();

			if (str == "cur" || str == "Cur" || str == "current" || str == "Current") return CvType::CURRENT;
			return CvType::VOLTAGE;
		}
};

class CommandCreate : public Command
{
	public:

		virtual void execute (TokensList & tokens) const override
		{
			Arguments args = parseArguments(tokens);
			
			try { validateArguments(args); }
			catch (exception& ex) { throw exception(ex.what()); }

			if (args.name == "")
				args.name = suggestEnterNameAndGet();

			createTreeByArgs(args);
			createInputByArgs(args);	
		}




	private:

		const string suggestion_enter_name_message = "Do you want to set a name for the new tree?";



		struct Arguments
		{
			string name = "";
			string inputName = "";
			CvType inputCvType = CvType::VOLTAGE;
			double inputCvValue = NAN;
		}; 
		
		
		
		Arguments parseArguments (TokensList & tokens) const
		{
			Arguments args;
			if (tokens.empty()) return args;

			string handeledArg = tokens.front(); tokens.pop_front();
			
			string name = "";
			if (!isCvType(handeledArg))
			{
				args.name = handeledArg;
				if (tokens.empty()) return args;

				handeledArg = tokens.front(); tokens.pop_front();
				string inputName = "";
				if (!isCvType(handeledArg))
				{
					args.inputName = handeledArg;
					if (tokens.empty()) return args;
				}

				handeledArg = tokens.front(); tokens.pop_front();
			}
				args.inputCvType = parseCvType(handeledArg);
				if (tokens.empty()) return args;

				handeledArg = tokens.front(); tokens.pop_front();
				if (tokens.empty())
					if (isFloatNumber(handeledArg))
					{
						args.inputCvValue = atof(handeledArg.c_str());
						return args;
					}
					else
#pragma todo write exceptions message
						throw exception();
				else
#pragma todo write exceptions message
					throw exception();
		}

		void validateArguments (const Arguments & args) const
		{

		}

		string suggestEnterNameAndGet () const
		{
			string name = "";
			cout << suggestion_enter_name_message << endl;
			string answer; cin >> answer;

			if (answer == "yes" || answer == "Yes" || answer == "y" || answer == "Y")
				cin >> name;
			else if (answer != "no" && answer != "No" && answer != "n" && answer != "N")
				throw exception("Invalid answer");

			return name;
		}

		void createTreeByArgs (const Arguments & args) const
		{
			if (args.name.empty())
				CreateTree();
			else
				CreateTree(args.name);
		}

		void createInputByArgs (const Arguments & args) const
		{
			if (args.inputName.empty())
				CreateInput(args.inputCvType, args.inputCvValue);
			else
				CreateInput(args. name, args.inputCvType, args.inputCvValue);
		}
};




const map<string, Command> commandDictionary = { {"cr", CommandCreate()} };









TokensList tokenize(string& tokens_str)
{
	return TokensList();
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
