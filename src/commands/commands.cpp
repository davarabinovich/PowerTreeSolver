
#include <sstream>
#include <cctype>
#include <cmath>

#include <string>
#include <vector>
#include <deque>

#pragma todo
#include <cassert>


#include "commands.h"

#include "config.h"
	







#pragma todo carry over to library
void shiftSpaces(unsigned spaces_qty)
{
	for (; spaces_qty != 0; spaces_qty--)
		cout << " ";
}

void scrollInteratorToNewWord_unsafe(string::const_iterator& char_it)
{
	while (*char_it == ' ') char_it++;
}









#pragma todo move to other file
enum class CvType { VOLTAGE, CURRENT };

ostream & operator << (ostream & os, const CvType & type)
{
	switch (type)
	{
		case CvType::VOLTAGE:
			return os << "voltage";
		case CvType::CURRENT:
			return os << "current";

		default:
			throw exception("Invalid type of controlled variable");
	}
}


enum class LoadType { RESISTIVE, CURRENT, POWER };

ostream & operator << (ostream & os, const LoadType & type)
{
	switch (type)
	{
		case LoadType::RESISTIVE:
			return os << "resistive";
		case LoadType::CURRENT:
			return os << "current";
		case LoadType::POWER:
			return os << "power";

		default:
			throw exception("Invalid type of load");
	}
}


struct Results
{
	struct Load
	{
		string name;
		LoadType type;
		double value;
		double power;
		double secondaryParam;
	};

	struct Source
	{
		string name;
		CvType cvType;
		double cvValue;
		double avValue;
		double power;

		vector<Source> converterSinks;
		vector<Load>   loadSinks;
	};
	
	string name;
	vector<Source> inputs;
};



bool IsThereSomeTree () { return true; };

void CreateTree () {}
void CreateTree (string name) {}

void RenameTree (string name) {}

void Solve () {}
Results GetResults () { return Results(); }

string GetNameOfTree () { return string(); }


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
				if (!isdigit(*letter_it))
					return false;
			}

			for ( ; letter_it != str.cend(); letter_it++)
			{
				if (!isdigit(*letter_it))
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

		virtual void execute (TokensList & tokens) const 
		{
			Arguments args;			
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }

			if (args.name == "")
				args.name = suggestEnterNameAndGet();

			createTreeByArgs(args);
			createInputByArgs(args);

			reportExcecution(args);
		}

	


	private:

		struct Arguments
		{
			string name = "";
			string inputName = "";
			CvType inputCvType = CvType::VOLTAGE;
			double inputCvValue = NAN;

			bool operator == (const Arguments & partner)
			{
				if (name         != partner.name)         return false;
				if (inputName    != partner.inputName)    return false;
				if (inputCvType  != partner.inputCvType)  return false;
				
				if (isnan(inputCvValue))
					if (isnan(partner.inputCvValue))
						return true;
					else
						return false;
				if (inputCvValue != partner.inputCvValue) return false;
				
				return true;
			}

			bool operator != (const Arguments & partner)
			{
				bool result = !(*this == partner);
				return result;
			}
		}; 
		
		
		
		Arguments parseArguments (TokensList & tokens) const
		{
			Arguments args;

			if (tokens.empty()) return args;

			string handeledArg = tokens.front(); tokens.pop_front();
			if (!isCvType(handeledArg))
				if (!isFloatNumber(handeledArg))
					args.name = handeledArg;
				else
				{
					args.inputCvValue = atof(handeledArg.c_str());
					if (tokens.size() != 0)
						#pragma todo write exceptions message
						throw exception();
					return args;
				}
			else
			{
				args.inputCvType = parseCvType(handeledArg);

				if (tokens.size() != 0)
				{
					handeledArg = tokens.front(); tokens.pop_front();
					if (!isFloatNumber(handeledArg))
						#pragma todo write exceptions message
						throw exception();
					else
					{
						args.inputCvValue = atof(handeledArg.c_str());
						if (tokens.size() != 0)
							#pragma todo write exceptions message
							throw exception();
					}
				}

				return args;
			}
			

			if (tokens.size() == 0)    return args;
		
			handeledArg = tokens.front(); tokens.pop_front();
			if (!isCvType(handeledArg))
				if (!isFloatNumber(handeledArg))
					args.inputName = handeledArg;
				else
				{
					args.inputCvValue = atof(handeledArg.c_str());
					if (tokens.size() != 0)
#pragma todo write exceptions message
						throw exception();
					return args;
				}
			else
			{
				args.inputCvType = parseCvType(handeledArg);

				if (tokens.size() != 0)
				{
					handeledArg = tokens.front(); tokens.pop_front();
					if (!isFloatNumber(handeledArg))
#pragma todo write exceptions message
						throw exception();
					else
					{
						args.inputCvValue = atof(handeledArg.c_str());
						if (tokens.size() != 0)
#pragma todo write exceptions message
							throw exception();
					}
				}

				return args;
			}



			if (tokens.size() == 0)    return args;
				
			handeledArg = tokens.front(); tokens.pop_front();
			if (!isCvType(handeledArg))
				if (!isFloatNumber(handeledArg))
#pragma todo write exceptions message
					throw exception();
				else
				{
					args.inputCvValue = atof(handeledArg.c_str());
					if (tokens.size() != 0)
#pragma todo write exceptions message
						throw exception();
					return args;
				}
			else
			{
				args.inputCvType = parseCvType(handeledArg);

				if (tokens.size() != 0)
				{
					handeledArg = tokens.front(); tokens.pop_front();
					if (!isFloatNumber(handeledArg))
#pragma todo write exceptions message
						throw exception();
					else
					{
						args.inputCvValue = atof(handeledArg.c_str());
						if (tokens.size() != 0)
#pragma todo write exceptions message
							throw exception();
					}
				}

				return args;
			}
		}	


		string suggestEnterNameAndGet () const
		{
			string name = "";
			cout << "Do you want to set a name for the new tree ?" << endl;
			string answer; getline(cin, answer);

			if (answer == "yes" || answer == "Yes" || answer == "y" || answer == "Y")
				getline(cin, name);
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

		void reportExcecution (const Arguments & args) const
		{
			string name = args.name;
			if (name != "")
				name = "\"" + name + "\" ";
			
			string cvType = "voltage";
			if (args.inputCvType == CvType::CURRENT)
				cvType = "current"; 

			bool isCvValuePresent = false;
			string cvUnit = "V";
			if (!isnan(args.inputCvValue))
			{
				isCvValuePresent = true;
				if (args.inputCvType == CvType::CURRENT)
					cvUnit = "A";
			}


			cout << "A new power three " << name << "with a " << cvType << " input";
			if (isCvValuePresent)	cout << " " << args.inputCvValue << " " << cvUnit;
			cout << " is created" << endl << endl;
		}




	public:

		void test_parseArguments()
			{

				TokensList emptyTokens;

				Arguments emptyArgs;

				auto emptyOut = parseArguments(emptyTokens);
				assert(emptyArgs == emptyOut);



				TokensList onlyNameTokens = { "name" };

				Arguments onlyNameArgs;
				onlyNameArgs.name = "name";

				auto onlyNameOut = parseArguments(onlyNameTokens);
				assert(onlyNameArgs == onlyNameOut);



				TokensList onlyTypeTokens = { "cur" };

				Arguments onlyTypeArgs;
				onlyTypeArgs.inputCvType = CvType::CURRENT;

				auto onlyTypeOut = parseArguments(onlyTypeTokens);
				assert(onlyTypeArgs == onlyTypeOut);



				TokensList onlyValueTokens = { "24" };

				Arguments onlyValueArgs;
				onlyValueArgs.inputCvValue = 24;

				auto onlyValueOut = parseArguments(onlyValueTokens);
				assert(onlyValueArgs == onlyValueOut);
			}

};




class CommandRename : public Command
{

	public:
	
		virtual void execute (TokensList& tokens) const
		{
			bool isThereSomeTree = IsThereSomeTree();
			if (!isThereSomeTree)
			{
				requestToGetTree();
				return;
			}

			if (tokens.size() > 1)    throw exception("Too many arguments for this command");

			string newName;
			if (tokens.size() == 0)
				newName = requestAndGetNewName();
			else
				newName = tokens.front();

			string oldName = GetNameOfTree();
			RenameTree(newName);

			Arguments args = { newName, oldName };
			reportExcecution(args);
		}




	private:
	
		struct Arguments
		{
			string newName = "";
			string oldName = "";
	
			bool operator == (const Arguments & partner)
			{
				if (newName != partner.newName)    return false;
				if (newName != partner.oldName)    return false;
				return true;
			}
	
			bool operator != (const Arguments & partner)
			{
				bool result = !(*this == partner);
				return result;
			}
		};



		void requestToGetTree () const
		{
			cout << "There are no power tree. Create or load a tree" << endl << endl;
		}

		string requestAndGetNewName () const
		{
			cout << "Enter a new name for this power tree: ";
			string newName;
			getline(cin, newName);
			return newName;
		}

		void reportExcecution (const Arguments & args) const
		{
			cout << "The power tree \"" << args.oldName << "\" is renamed " << args.newName 
				 << endl << endl;
		}

};





class CommandSolve : public Command
{

	public:
	
		virtual void execute (TokensList & tokens) const override
		{
			Arguments args = parseArguments(tokens);

			bool needsToDisplayResults = args.needsDisplayResults;
			if (!needsToDisplayResults)
			{
				needsToDisplayResults = suggestDisplayResultsAndGetAnswer();
			}

			Solve();
			reportCalculationsFinishs();

			if (needsToDisplayResults)
			{
				Results results = GetResults();

				try { displayResults(results, args.resultsView, args.needsShowPowers, args.needsShowSecondaryLoadParams); }
				catch (exception & ex) { cout << ex.what(); }
			}
		}
	
	
	
	
	private:
		
		enum class ResultsView { TABLE, TREE };

		struct Arguments
		{
			bool needsDisplayResults = false;
			ResultsView resultsView = ResultsView::TABLE;
			bool needsShowPowers = false;
			bool needsShowSecondaryLoadParams = false;
		};
	
	
	
		Arguments parseArguments (TokensList & tokens) const
		{
			if (tokens.size() > 3)	throw exception("Too many arguments for this command");

			Arguments args;
			if (tokens.size() == 0)    return args;

			args.needsDisplayResults = true;

			unsigned unparsedArgs_cnt = tokens.size();
			for (const auto & token : tokens)
			{
				if (isResultViewMode(token))
				{
					args.resultsView = parseResultViewMode(token);
					unparsedArgs_cnt--;
					continue;
				}
				if (isPowerFlag(token))
				{
					args.needsShowPowers = true;
					unparsedArgs_cnt--;
					continue;
				}
				if (isSecondaryParamsFlag(token))
				{
					args.needsShowSecondaryLoadParams = true;
					unparsedArgs_cnt--;
					continue;
				}
			}

			if (unparsedArgs_cnt != 0)    throw exception("Invalid argument");

			return args;
		}

		bool suggestDisplayResultsAndGetAnswer () const
		{
			#pragma todo these must be generalised in the same function with the all similar functions (e. g. CommandCreate::suggestEnterNameAndGet)
			cout << "Do you want to see results ?" << endl;
			string answer_str; getline(cin, answer_str);

			if (answer_str == "yes" || answer_str == "Yes" || answer_str == "y" || answer_str == "Y")
				return true;
			else if (answer_str != "no" && answer_str != "No" && answer_str != "n" && answer_str != "N")
				throw exception("Invalid answer");

			return false;
		}

		void reportCalculationsFinishs () const
		{
			cout << "Calculations has finished successfully." << endl;
		}

		void displayResults (Results results, ResultsView view, bool needsShowPower, bool needsShowSecondaryLoadParams) const
		{
			cout << "Calcultion's results for power tree \"" << results.name << "\"";

			switch (view)
			{
				case ResultsView::TABLE:
					displayResultsTable(results, needsShowPower, needsShowSecondaryLoadParams);
					break;

				case ResultsView::TREE:
					displayResultsTree(results, needsShowPower, needsShowSecondaryLoadParams);
					break;

				default:
					throw exception("Invalid results display mode");
			}
		}

		bool isResultViewMode (string token) const
		{
			if (token == "tb" || token == "Tb" || token == "tab" || token == "Tab" || 
				token == "table" || token == "Table")    return true;
			if (token == "tr" || token == "Tr" || token == "tree" || token == "Tree")    
				return true;
			return false;
		}

		bool isPowerFlag (string token) const
		{
			if (token == "sp" || token == "Sp" || token == "showPower" || token == "ShowPower")
				return true;
			return false;
		}

		bool isSecondaryParamsFlag(string token) const
		{
			if (token == "ss" || token == "Ss" || token == "showSec" || token == "ShowSec")
				return true;
			return false;
		}

		ResultsView parseResultViewMode (string rwMode) const
		{
			if (!isResultViewMode(rwMode))    throw exception("Invalid display results mode");

			if (rwMode == "tb" || rwMode == "Tb" || rwMode == "tab" || rwMode == "Tab" ||
				rwMode == "table" || rwMode == "Table")    return ResultsView::TABLE;
			return ResultsView::TREE;
		}

		void displayResultsTable (Results results, bool needsShowPower, bool needsShowSecondaryLoadParams) const
		{
			for (auto & input : results.inputs)
				displaySourceResults (input, needsShowPower, needsShowSecondaryLoadParams);
		}

		void displayResultsTree (Results results, bool needsShowPower, bool needsShowSecondaryLoadParams) const
		{

		}

		void displaySourceResults (Results::Source & source, bool needsShowPower, bool needsShowSecondaryLoadParams,
			                       unsigned hierarchy_level = 1) const
		{
			shiftSpaces(4*hierarchy_level);
			if (hierarchy_level != 1)
				cout << "- ";

			cout << source.name << ": " << source.cvValue << " " << source.cvType << ", ";

			if (source.cvType == CvType::VOLTAGE)
				cout << "consumpted current: " << source.avValue << " " << "A";
			else
				cout << "output voltage: " << source.avValue << " " << "V";

			if (needsShowPower)
				cout << ", consumpted power:" << source.power << " W";

			cout << endl;


			for (auto& converterSink : source.converterSinks)
				displaySourceResults (converterSink, needsShowPower, needsShowSecondaryLoadParams, hierarchy_level+1);

			for (auto& loadSink : source.loadSinks)
				displayLoadResults(loadSink, needsShowPower, needsShowSecondaryLoadParams, hierarchy_level + 1);
		}

		void displayLoadResults(Results::Load & load, bool needsShowPower, bool needsShowSecondaryLoadParams, 
			                    unsigned hierarchy_level) const
		{
			shiftSpaces(4*(hierarchy_level + 1));

			cout << load.name << ": " << load.type << " load " << load.value << " ";

			if (load.type == LoadType::RESISTIVE)
				cout << "Ohm";
			else if (load.type == LoadType::CURRENT)
				cout << "A";
			else
				cout << "W";

			if (needsShowPower)
				cout << ", " << load.power << " W";

			if (load.type == LoadType::POWER)
				if (needsShowSecondaryLoadParams)
					cout << load.secondaryParam << " V";

			cout << endl;
		}

};





#pragma todo
void test_Commands()
{
	CommandCreate cr;
	cr.test_parseArguments();
}
class CommandT: public Command
{

public:

	virtual void execute(TokensList& tokens) const
	{
		
	}




private:

	struct Arguments
	{
		bool operator == (const Arguments& partner)
		{
			return true;
		}

		bool operator != (const Arguments& partner)
		{
			bool result = !(*this == partner);
			return result;
		}
	};



	void reportExcecution(const Arguments& args) const
	{

	}

};




static const CommandCreate cr;
static const CommandRename rn;
static const CommandSolve  sv;

static const map< string, const shared_ptr<Command> > commandDictionary = {  { "cr", make_shared<CommandCreate>(cr) },
																		     { "rn", make_shared<CommandRename>(rn) },
                                                                             { "sv", make_shared<CommandSolve>(sv)  }  };








TokensList tokenize(const string& command_string)
{
	if (command_string.size() == 0)    return TokensList();

	TokensList tokens;
	auto wordBegin_it = command_string.cbegin();
	scrollInteratorToNewWord_unsafe(wordBegin_it);
	auto wordEnd_it = wordBegin_it;

	while (wordBegin_it != command_string.cend())
	{
		while (true)
		{
			if (wordEnd_it == command_string.cend())
				break;
			if (*wordEnd_it == ' ')
				break;
			wordEnd_it++;
		}

		tokens.push_back(string(wordBegin_it, wordEnd_it));
		if (wordEnd_it != command_string.cend())
			scrollInteratorToNewWord_unsafe(wordEnd_it);
		wordBegin_it = wordEnd_it;
	}

	return tokens;
}

void executeCommand(string enteredCommand)
{
#pragma todo whether replace deque with a vector?
	auto tokens = tokenize(enteredCommand);
	string commandMnemonic = tokens.front();
	tokens.pop_front();

	shared_ptr<Command> currentCommand;
	try { currentCommand = commandDictionary.at(commandMnemonic); }
	catch (out_of_range) { throw exception("Unrecognized command"); }

	currentCommand->execute(tokens);
}

command_mnemonic extractCommandMnemonicFrom (string commandWithParameters)
{
	return command_mnemonic();
}
