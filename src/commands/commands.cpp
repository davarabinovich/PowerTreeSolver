
#include <sstream>
#include <cctype>
#include <cmath>

#include <string>
#include <vector>
#include <deque>
#include <set>
#include <variant>


#include "tree/tree_api.h"

#include "commands.h"

#include "config.h"



#include "test_runner/test_runner.h"




#pragma todo carry over to library
void shiftSpaces (unsigned spaces_qty)
{
	for (; spaces_qty != 0; spaces_qty--)
		cout << " ";
}

void scrollInteratorToNewWord_unsafe(string::const_iterator & char_it)
{
	while (*char_it == ' ') char_it++;
}

double strToDouble (string str)
{
	double number = atof(str.c_str());
	if ( (number == 0.0) && (str != "0.0") )
		throw exception("There is no floating-point number");
	return number;
}

bool isYes (string str)
{
	if ( (str == "y") || (str == "Y") || (str == "yes") || (str == "Yes") )
		return true;
	return false;
}

bool isNo (string str)
{
	if ((str == "n") || (str == "N") || (str == "no") || (str == "No"))
		return true;
	return false;
}

bool isBool_str (string str)
{
	if (isYes(str) || isNo(str))
		return true;
	return false;
}








using TokensDeque = deque<string>;







class Command
{
	public:

		virtual void execute (TokensDeque & tokens) const = 0;




	protected:

		Command() {}


		
		static bool isFloatNumberString (const string & str)
		{
			auto letter_it = str.cbegin();

			for ( ; letter_it != str.cend(); letter_it++)
			{
				if (*letter_it == '.' || *letter_it == ',') break;
				if (!isdigit(*letter_it))
					return false;
			}

			if (letter_it == str.cend())    return true;

			letter_it++;
			for ( ; letter_it != str.cend(); letter_it++)
			{
				if (!isdigit(*letter_it))
					return false;
			}

			return true;
		}

		static bool isCvTypeString (const string & str)
		{
			if (str[0] != 'c' && str[0] != 'C' && str[0] != 'v' && str[0] != 'V') return false;
			if (str != "cur" && str != "Cur" && str != "current" && str != "Current")
				if (str != "vol" && str != "Vol" && str != "voltage" && str != "Voltage") return false;
			return true;
		}

		static CvType parseCvType (const string & str)
		{
			if (!isCvTypeString(str))
#pragma todo write exceptions message
				throw exception();

			if (str == "cur" || str == "Cur" || str == "current" || str == "Current") return CvType::CURRENT;
			return CvType::VOLTAGE;
		}
		
		static bool isConverterTypeString (const string & str)
		{
			if (str[0] != 'l' && str[0] != 'L' && str[0] != 'p' && str[0] != 'P') return false;
			if (str != "lin" && str != "Lin" && str != "linear" && str != "Linear")
				if (str != "pul" && str != "Pul" && str != "pulse" && str != "Pulse") return false;
			return true;
		}

		static ConverterType parseConverterType (const string & str)
		{
			if (!isConverterTypeString(str))
#pragma todo write exceptions message
				throw exception();

			if (str == "lin" || str == "Lin" || str == "linear" || str == "Linear") return ConverterType::LINEAR;
			return ConverterType::PULSE;
		}

		static bool isLoadTypeString (const string & str)
		{
			if (str[0] != 'r' && str[0] != 'R' && str[0] != 'c' && str[0] != 'C' && str[0] != 'p' && str[0] != 'P')     return false;
			if (str != "res" && str != "Res" && str != "resistive" && str != "Resistive")
				if (str != "cur" && str != "Cur" && str != "current" && str != "Current")
					if (str != "pow" && str != "Pow" && str != "power" && str != "Power")    return false;
			return true;
		}

		static LoadType parseLoadType (const string & str)
		{
			if (!isLoadTypeString(str))
				throw exception("Invalid type of load");

			if (str == "res" || str == "Res" || str == "resistive" || str == "Resistive") return LoadType::RESISTIVE;
			if (str == "cur" || str == "Cur" || str == "current" || str == "Current") return LoadType::CONSTANT_CURRENT;
			return LoadType::ENERGY;
		}

		static bool isDeletingModeString (const string & str)
		{
			if (str != "h" && str != "d" && str != "r")    return false;
			return true;
		}
		
		static DeletingMode parseDeletingMode (const string & str)
		{
			if (!isDeletingModeString(str))
				throw exception(  string("\"" + str + "\" is not a mode of deleting").c_str()  );

			if (str == "d")
				return DeletingMode::WITH_DESCES;
			if (str == "h")
				return DeletingMode::HANG_DESCES;
			return DeletingMode::RECONNECT_DESCES;
		}
};





class CommandCreate : public Command
{

	public:

		virtual void execute (TokensDeque & tokens) const 
		{
			Arguments args;			
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }

			if (args.name == "")
				args.name = suggestEnterNameAndGet();

			createTreeByArgs(args);
			createInputByArgs(args);

			reportExecution(args);
		}

		friend void TestCreateArgumentsParsing();




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
		
		
		
		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;

			if (tokens.empty()) return args;

			string handeledArg = tokens.front(); tokens.pop_front();
			if (!isCvTypeString(handeledArg))
				if (!isFloatNumberString(handeledArg))
					args.name = handeledArg;
				else
				{
					args.inputCvValue = strToDouble(handeledArg);
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
					if (!isFloatNumberString(handeledArg))
						#pragma todo write exceptions message
						throw exception();
					else
					{
						args.inputCvValue = strToDouble(handeledArg);
						if (tokens.size() != 0)
							#pragma todo write exceptions message
							throw exception();
					}
				}

				return args;
			}
			

			if (tokens.size() == 0)    return args;
		
			handeledArg = tokens.front(); tokens.pop_front();
			if (!isCvTypeString(handeledArg))
				if (!isFloatNumberString(handeledArg))
					args.inputName = handeledArg;
				else
				{
					args.inputCvValue = strToDouble(handeledArg);
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
					if (!isFloatNumberString(handeledArg))
#pragma todo write exceptions message
						throw exception();
					else
					{
						args.inputCvValue = strToDouble(handeledArg);
						if (tokens.size() != 0)
#pragma todo write exceptions message
							throw exception();
					}
				}

				return args;
			}



			if (tokens.size() == 0)    return args;
				
			handeledArg = tokens.front(); tokens.pop_front();
			if (!isCvTypeString(handeledArg))
				if (!isFloatNumberString(handeledArg))
#pragma todo write exceptions message
					throw exception();
				else
				{
					args.inputCvValue = strToDouble(handeledArg);
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
					if (!isFloatNumberString(handeledArg))
#pragma todo write exceptions message
						throw exception();
					else
					{
						args.inputCvValue = strToDouble(handeledArg);
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
			cout << "Do you want to set a name for the new tree?" << endl;
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
			string name = args.inputName;
			if (name.empty())
				name = CreateInput();
			else
				CreateInput(name);

			SetCvTypeForInput(name, args.inputCvType);
			SetCvValueForInput(name, args.inputCvValue);
		}

		void reportExecution (const Arguments & args) const
		{
			string name = args.name;
			if (name == "")
				name = GetNameOfTree();
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

};




class CommandRename : public Command
{

	public:
	
		virtual void execute (TokensDeque& tokens) const
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
			reportExecution(args);
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

		void reportExecution (const Arguments & args) const
		{
			cout << "The power tree \"" << args.oldName << "\" is renamed " << args.newName 
				 << endl << endl;
		}

};





class CommandWithShowingResults : public Command
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const override
		{
			Arguments args = parseArguments(tokens);

			Results results = GetResults();

			try { showResults(results, args); }
			catch (exception & ex) { cout << ex.what(); }
		}
	
	
	
	
	protected:
	
		struct Arguments
		{
			bool needsShowPowers = false;
			bool needsShowSecondaryLoadParams = false;
		};



		CommandWithShowingResults () {}



		Arguments parseArguments (TokensDeque & tokens) const
		{
			if (tokens.size() > 2)	throw exception("Too many arguments for this command");

			Arguments args;
			if (tokens.size() == 0)    return args;

			unsigned unparsedArgs_cnt = tokens.size();
			for (const auto& token : tokens)
			{
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
			return args;
		}

		void showResults (Results results, Arguments args) const
		{
			cout << "Calcultion's results for the power tree \"" << GetNameOfTree() << "\":" << endl;
			for (auto& input : results.inputs)
				showSourceResults(input, args.needsShowPowers, args.needsShowSecondaryLoadParams);
			cout << endl;
		}




	private:

		void showSourceResults (const Results::Source & source, bool needsShowPower, bool needsShowSecondaryLoadParams,
			unsigned hierarchy_level = 1) const
		{
			shiftSpaces(4 * hierarchy_level);
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


			for (const auto & converterSink : source.converterSinks)
				showSourceResults(converterSink, needsShowPower, needsShowSecondaryLoadParams, hierarchy_level + 1);

			for (const auto & loadSink : source.loadSinks)
				showLoadResults(loadSink, needsShowPower, needsShowSecondaryLoadParams, hierarchy_level + 1);
		}

		void showLoadResults (const Results::Load & load, bool needsShowPower, bool needsShowSecondaryLoadParams,
			                    unsigned hierarchy_level) const
		{
			shiftSpaces(4 * (hierarchy_level + 1));

			cout << load.name << ": " << load.type << " load " << load.value << " ";

			if (load.type == LoadType::RESISTIVE)
				cout << "Ohm";
			else if (load.type == LoadType::CONSTANT_CURRENT)
				cout << "A";
			else
				cout << "W";

			if (needsShowPower)
				cout << ", " << load.power << " W";

			if (load.type == LoadType::ENERGY)
				if (needsShowSecondaryLoadParams)
					cout << load.secondaryParam << " V";

			cout << endl;
		}
	
		bool isPowerFlag (string token) const
		{
			if (token == "sp" || token == "Sp" || token == "showPower" || token == "ShowPower")
				return true;
			return false;
		}
	
		bool isSecondaryParamsFlag (string token) const
		{
			if (token == "ss" || token == "Ss" || token == "showSec" || token == "ShowSec")
				return true;
			return false;
		}

};





class CommandSolve : public CommandWithShowingResults
{

	public:
	
		virtual void execute (TokensDeque & tokens) const override
		{
			Arguments args = parseArguments(tokens);

			if (!args.needsToShowResults)
				args.needsToShowResults = suggestShowResultsAndGetAnswer();

			Solve();
			reportCalculationsFinishs();

			if (args.needsToShowResults)
			{
				Results results = GetResults();

				try { showResults(results, args.dispParams); }
				catch (exception & ex) { cout << ex.what(); }
			}
		}
	
	
	
	
	private:

		struct Arguments
		{
			bool needsToShowResults = false;
			CommandWithShowingResults::Arguments dispParams;
		};



		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;

			if (tokens.size() == 0)    return args;

			const auto & handeledArg = tokens.front();
			if (isBool_str(handeledArg))
			{
				args.needsToShowResults = isYes(handeledArg);

				tokens.pop_front();
				if (tokens.empty())    return args;
			}
			else
				throw exception(   string("Invalid argument \"" + handeledArg + "\"").c_str()   );

			if (args.needsToShowResults)
				args.dispParams = CommandWithShowingResults::parseArguments(tokens);
			
			throw exception("You can't specify structure displaying parameters if you don't wanna show results");
		}

		[[nodiscard]] bool suggestShowResultsAndGetAnswer () const
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

};





class CommandShowResults : public CommandWithShowingResults {};





class CommandShowStructure : public Command
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			TreeStructure treeStructure = GetTreeStructure();
			displayTreeStructure(treeStructure);
		}
	
	
	
	
	private:
	
		void displayTreeStructure (const TreeStructure & trStr) const
		{
			cout << "Structure of the power tree \"" << GetNameOfTree() << "\":" << endl;

			for (const auto & input : trStr.inputs)
				displayInputWithDescendantes(input);
			
			for (const auto & converter : trStr.flyingConverters)
				displayConverterWithDescendantes(converter, 10);
			for (const auto & load : trStr.flyingLoads)
				displayLoad(load, 10);

			cout << endl;
		}

		void displayInputWithDescendantes (const TreeStructure::Input & inp) const
		{
			string thisSourceInfo = "\"" + inp.name + "\": " + inp.cvType + " input" + to_string(inp.cvValue) + " ";
			if (inp.cvType == CvType::VOLTAGE)
				thisSourceInfo += "V";
			else
				thisSourceInfo += "A";
			cout << thisSourceInfo;

			for (const auto & converter : inp.converterSinks)
				displayConverterWithDescendantes(converter, thisSourceInfo.size());
			for (const auto & load : inp.loadSinks)
				displayLoad(load, thisSourceInfo.size());

			if (inp.converterSinks.size() == 0 && inp.loadSinks.size() == 0)
				cout << endl;
		}

		void displayConverterWithDescendantes (const TreeStructure::Converter & cnvr, unsigned shiftLength = 0) const
		{
			shiftSpaces(shiftLength);

			string thisSourceInfo = "- \"" + cnvr.name + "\": " + cnvr.cvType + " source" + to_string(cnvr.cvValue) + " ";
			if (cnvr.cvType == CvType::VOLTAGE)
				thisSourceInfo += "V";
			else
				thisSourceInfo += "A";
			cout << thisSourceInfo;

			for (const auto & converter : cnvr.converterSinks)
				displayConverterWithDescendantes(converter, thisSourceInfo.size());
			for (const auto & load : cnvr.loadSinks)
				displayLoad(load, thisSourceInfo.size());

			if (cnvr.converterSinks.size() == 0 && cnvr.loadSinks.size() == 0)
				cout << endl;
		}

		void displayLoad (const TreeStructure::Load & ld, unsigned shiftLength = 0) const
		{
			shiftSpaces(shiftLength);

			string thisLoadInfo = "- \"" + ld.name + "\": " + ld.type + " load" + to_string(ld.value);
		}
	
};





class CommandCreateInput : public Command
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception & ex) { throw exception(ex.what()); }
	
			if (args.name == "")
				args.name = suggestEnterNameAndGet();
			while (IsInputExsist(args.name))
				args.name = requestUniqueName();

			if (isnan(args.cvValue))
				args.cvValue = requestCvValue(args.cvType);
	
			createInputByArgs(args);
	
			reportExecution(args);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string name = "";
			CvType cvType = CvType::VOLTAGE;
			double cvValue = NAN;
		};
	
	
	
		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;

			if (tokens.empty())    return args;


			auto handeledArg = tokens.front();
			
			if ( !isCvTypeString(handeledArg) && !isFloatNumberString(handeledArg) )
			{
				args.name = handeledArg;

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}

			if (isCvTypeString(handeledArg))
			{
				args.cvType = parseCvType(handeledArg);

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}
			
			if (isFloatNumberString(handeledArg))
			{
				args.cvValue = strToDouble(handeledArg);

				tokens.pop_front();
				if (tokens.empty())    return args;
			}

			throw exception("There is at least one invalid argument");
		}

		string suggestEnterNameAndGet () const
		{
			string name = "";
			cout << "Do you want to set a name for the new input?" << endl;
			string answer; getline(cin, answer);

			if (answer == "yes" || answer == "Yes" || answer == "y" || answer == "Y")
				getline(cin, name);
			else if (answer != "no" && answer != "No" && answer != "n" && answer != "N")
				throw exception("Invalid answer");

			return name;
		}
	
		string requestUniqueName () const
		{
			cout << "This name is already taken. Please enter other name" << endl;
			string newName; getline(cin, newName);
			return newName;
		}

		double requestCvValue (const CvType type) const
		{
			cout << "Plase enter a value of " << type << endl;
			string enteredValue; getline(cin, enteredValue);
			auto value = strToDouble(enteredValue);
			return value;
		}

		void createInputByArgs (Arguments & args) const
		{
			string & name = args.name;
			if (name.empty())
				name = CreateInput();
			else
				CreateInput(name);

			SetCvTypeForInput(name, args.cvType);
			SetCvValueForInput(name, args.cvValue);
		}
	
		void reportExecution (const Arguments & args) const
		{
			string name = "\"" + args.name + "\" ";

			string cvType = "voltage";
			if (args.cvType == CvType::CURRENT)
				cvType = "current";

			bool isCvValuePresent = false;
			string cvUnit = "V";
			if (!isnan(args.cvValue))
			{
				isCvValuePresent = true;
				if (args.cvType == CvType::CURRENT)
					cvUnit = "A";
			}


			cout << "A new " << cvType << " input " << name;
			if (isCvValuePresent)	
				cout << args.cvValue << " " << cvUnit;
			cout << " is created" << endl << endl;
		}
	
};





class CommandCreateConverter : public Command
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }
	
			if (args.name == "")
				args.name = suggestEnterNameAndGet();
			while (IsConverterExsist(args.name))
				args.name = requestUniqueName();

			if (isnan(args.cvValue))
				args.cvValue = requestCvValue(args.cvType);
			if (args.parentName == "")
				args.parentName = suggestSpecifieParentAndGet();
	
			createConverterByArgs(args);
	
			reportExecution(args);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string name = "";
			CvType cvType = CvType::VOLTAGE;
			double cvValue = NAN;

			ConverterType type = ConverterType::PULSE;
			double efficiency = NAN;

			string parentName = "";
		};
	
	
	
		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;

			if (tokens.empty())    return args;


			auto handeledArg = tokens.front();

			if (!isCvTypeString(handeledArg) && !isConverterTypeString(handeledArg) && !isFloatNumberString(handeledArg))
			{
				args.name = handeledArg;

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}

			if (isCvTypeString(handeledArg))
			{
				args.cvType = parseCvType(handeledArg);

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}

			if (isFloatNumberString(handeledArg))
			{
				args.cvValue = strToDouble(handeledArg);

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}

			if (isConverterTypeString(handeledArg))
			{
				args.type = parseConverterType(handeledArg);

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}

			if (isFloatNumberString(handeledArg))
			{
				args.efficiency = strToDouble(handeledArg);

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}

			if (!isCvTypeString(handeledArg) && !isConverterTypeString(handeledArg) && !isFloatNumberString(handeledArg))
			{
				args.parentName = handeledArg;

				tokens.pop_front();
				if (tokens.empty())    return args;
			}

			throw exception("There is at least one invalid argument");
		}

		string suggestEnterNameAndGet () const
		{
			string name = "";
			cout << "Do you want to set a name for the new converter?" << endl;
			string answer; getline(cin, answer);

			if (answer == "yes" || answer == "Yes" || answer == "y" || answer == "Y")
				getline(cin, name);
			else if (answer != "no" && answer != "No" && answer != "n" && answer != "N")
				throw exception("Invalid answer");

			return name;
		}

		string requestUniqueName () const
		{
			cout << "This name is already taken. Please enter other name" << endl;
			string newName; getline(cin, newName);
			return newName;
		}

		double requestCvValue (const CvType type) const
		{
			cout << "Plase enter a value of " << type << endl;
			string enteredValue; getline(cin, enteredValue);
			auto value = strToDouble(enteredValue);
			return value;
		}

		string suggestSpecifieParentAndGet () const
		{
			string parentName = "";
			cout << "Do you want to leave the new converter unconnected?" << endl;
			string answer; getline(cin, answer);

			if (answer == "n" || answer == "N" || answer == "no" || answer == "No")
			{
				cout << "Enter the name of parent source" << endl;
				getline(cin, parentName);
			}
			else if (answer != "y" && answer != "Y" && answer != "yes" && answer != "Yes")
				throw exception("Invalid answer");

			return parentName;
		}

		void createConverterByArgs (const Arguments & args) const
		{
			string name = args.name;
			if (name.empty())
				name = CreateConverter();
			else
				CreateConverter(name);

			if (!args.parentName.empty())
			{
				if (IsSourceExsist(args.parentName))
					ConnectSinkTo(name, args.parentName);
				else
					throw exception("Invalid parent source for the new converter is specified");
			}

			SetTypeForConverter(name, args.type);
			SetCvTypeForConverter(name, args.cvType);
			SetCvValueForConverter(name, args.cvValue);
			SetEfficiencyForConverter(name, args.efficiency);
		}

		void reportExecution (const Arguments & args) const
		{
			bool isFree = args.parentName.empty();


			string name = "\"" + args.name + "\" ";

			string cvUnit = "V";
			if (args.cvType == CvType::CURRENT)
				cvUnit = "A";


			cout << "A new ";
			if (isFree)
				cout << "free ";
			cout << args.type << " converter " << name << args.cvValue << " " << cvUnit;
			cout << " is created";
			if (!isFree)
				cout << " at the " << GetSourceType(args.parentName) << " \"" << args.parentName << "\"";
			cout << endl << endl;
		}
	
};





class CommandCreateLoad : public Command
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }

			if (args.name == "")
				args.name = suggestEnterNameAndGet();
			while (IsLoadExsist(args.name))
				args.name = requestUniqueName();

			if (isnan(args.value))
				args.value = requestValue(args.type);
			if (args.parentName == "")
				args.parentName = suggestSpecifieParentAndGet();

			createLoadByArgs(args);

			reportExecution(args);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string name = "";
			LoadType type = LoadType::RESISTIVE;
			double value = NAN;

			double nomVoltage = NAN;

			string parentName = "";
		};
	
	

		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;

			if (tokens.empty())    return args;


			auto handeledArg = tokens.front();

			if (!isLoadTypeString(handeledArg) && !isFloatNumberString(handeledArg))
			{
				args.name = handeledArg;

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}

			if (isLoadTypeString(handeledArg))
			{
				args.type = parseLoadType(handeledArg);

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}

			if (isFloatNumberString(handeledArg))
			{
				args.value = strToDouble(handeledArg);

				tokens.pop_front();
				if (args.type != LoadType::ENERGY) 
				{
					if (tokens.empty())    return args;
				}
				else
				{
					if (tokens.empty())    return args;
					handeledArg = tokens.front();

					if (isFloatNumberString(handeledArg))
					{
						args.value = strToDouble(handeledArg);

						tokens.pop_front();
						if (tokens.empty())    return args;
					}
				}
			}

			throw exception("There is at least one invalid argument");
		}
		
		string suggestEnterNameAndGet () const
		{
			string name = "";
			cout << "Do you want to set a name for the new load?" << endl;
			string answer; getline(cin, answer);

			if (answer == "yes" || answer == "Yes" || answer == "y" || answer == "Y")
				getline(cin, name);
			else if (answer != "no" && answer != "No" && answer != "n" && answer != "N")
				throw exception("Invalid answer");

			return name;
		}

		string requestUniqueName () const
		{
			cout << "This name is already taken. Please enter other name" << endl;
			string newName; getline(cin, newName);
			return newName;
		}

		double requestValue (const LoadType type) const
		{
			cout << "Plase enter a value of ";
			string mainParam; 
			switch (type)
			{
				case LoadType::RESISTIVE:
					mainParam = "resistance";
					break;

				case LoadType::CONSTANT_CURRENT:
					mainParam = "current";
					break;

				case LoadType::ENERGY:
						mainParam = "power";
						break;
			
				default:
						throw exception("Invalid type of load");
			}
			cout << mainParam << endl;
			
			string enteredValue; getline(cin, enteredValue);
			auto value = strToDouble(enteredValue);
			return value;
		}

		string suggestSpecifieParentAndGet () const
		{
			string parentName = "";
			cout << "Do you want to leave the new load unconnected?" << endl;
			string answer; getline(cin, answer);

			if (answer == "n" || answer == "N" || answer == "no" || answer == "No")
			{
				cout << "Enter the name of parent source" << endl;
				getline(cin, parentName);
			}
			else if (answer != "y" && answer != "Y" && answer != "yes" && answer != "Yes")
				throw exception("Invalid answer");

			return parentName;
		}

		void createLoadByArgs (const Arguments & args) const
		{
			string name = args.name;
			if (name.empty())
				name = CreateLoad();
			else
				CreateLoad(name);

			if (!args.parentName.empty())
			{
				if (IsSourceExsist(args.parentName))
					ConnectSinkTo(name, args.parentName);
				else
					throw exception("Invalid parent source for the new converter is specified");
			}

			SetTypeForLoad(name, args.type);
			SetValueForLoad(name, args.value);
			if (args.type == LoadType::ENERGY)
				SetNomVoltageForPowerLoad(name, args.nomVoltage);
		}

		void reportExecution (const Arguments & args) const
		{
			bool isFree = args.parentName.empty();


			string name = "\"" + args.name + "\" ";

			string valueUnit = "Ohm";
			if (args.type == LoadType::CONSTANT_CURRENT)
				valueUnit = "A";
			else if (args.type == LoadType::ENERGY)
				valueUnit = "W";


			cout << "A new ";
			if (isFree)
				cout << "free ";
			cout << args.type << " load " << name << args.value << " " << valueUnit;
			cout << " is created";
			if (!isFree)
				cout << " at the " << GetSourceType(args.parentName) << " \"" << args.parentName << "\"";
			cout << endl << endl;
		}
	
};





class CommandModifyInput : public Command
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }

			if (IsInputExsist(args.currentName))
			{
				modifyInputParams(args);
				reportExecution(args);
			}
			else
				reportNonexsistentInput(args.currentName);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string currentName;

			pair<bool, string> newName = { false, "" };
			pair<bool, CvType> cvType = { false, CvType::VOLTAGE };
			pair<bool, double> cvValue = { false, NAN };
		};
	
	
	
		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;
			if (tokens.empty())    return args;

			args.currentName = tokens.front();
			if (tokens.empty())    return args;

			tokens.pop_front();
			for (const auto & token : tokens)
			{
				if (isParamWithKey(token))
				{
					string key = extractKeyFromToken(token);
					if (key == "n")
					{
						if (args.newName.first == true)    continue;

						args.newName.first = true;
						args.newName.second = extractParamFromToken(token);
					}
					else if (key == "u")
					{
						if (args.cvType.first == true)    continue;

						args.cvType.first = true;
						args.cvType.second = parseCvType(extractParamFromToken(token));
					}
					else if (key == "v")
					{
						if (args.cvValue.first == true)    continue;

						args.cvValue.first = true;
						args.cvValue.second = strToDouble(extractParamFromToken(token));
					}
					else
						throw exception(  string("Unrecognized parameter \"" + key).c_str()  );
				}
				else
				{
					if (isCvTypeString(token))
					{
						if (args.cvType.first == false)
						{
							args.cvType.first = true;
							args.cvType.second = parseCvType(token);
							continue;
						}
					}

					if (isFloatNumberString(token))
					{
						if (args.cvValue.first == false)
						{
							args.cvValue.first = true;
							args.cvValue.second = strToDouble(token);
							continue;
						}
					}
					
					if (args.newName.first == true)    continue;

					args.newName.first = true;
					args.newName.second = token;
				}
			}

			return args;
		}

		bool isParamWithKey (const string & token) const
		{
			const auto charEqual_it = find(token.begin(), token.end(), '=');
			if (charEqual_it != token.end())    return true;
			return false;
		}

		string extractKeyFromToken (const string & token) const
		{
			if (!isParamWithKey(token))
				throw exception("This is not a parameter with a key");

			const auto charEqual_it = find(token.begin(), token.end(), '=');
			string result = string(token.begin(), charEqual_it);
			return result;
		}

		string extractParamFromToken (const string & token) const
		{
			if (!isParamWithKey(token))
				throw exception("This is not a parameter with a key");

			const auto charEqual_it = find(token.begin(), token.end(), '=');
			string result = string(charEqual_it+1, token.end());
			return result;
		}


		void modifyInputParams (const Arguments & args) const
		{
			if (args.newName.first == true)
				RenameInput(args.currentName, args.newName.second);
			if (args.cvType.first == true)
				SetCvTypeForInput(args.currentName, args.cvType.second);
			if (args.cvValue.first == true)
				SetCvValueForInput(args.currentName, args.cvValue.second);
		}

		void reportExecution (const Arguments & args) const
		{
			cout << "Parameters of input \"" << args.currentName << "\" is changed: ";
			
			if (args.newName.first == true)
				cout << endl << "    Name - \"" << args.newName.second << "\"";
			if (args.cvType.first == true)
				cout << endl << "    Type of controlled variable - " << args.cvType.second;
			if (args.cvValue.first == true)
			{
				cout << endl << "    Controlled variable - " << args.cvValue.second;

				string cvUnit = "V";
				if (args.cvType.second == CvType::CURRENT)
					cvUnit = "A";
				cout << " " << cvUnit;
			}

			cout << endl;
		}

		void reportNonexsistentInput (const string & name) const
		{
			cout << "An input \"" << name << "\" doesn't exsist." << endl;
		}
	
};





class CommandModifyConverter : public Command
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }
	
			if (IsConverterExsist(args.currentName))
			{
				modifyConverterParams(args);
				reportExecution(args);
			}
			else
				reportNonexsistentConverter(args.currentName);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string currentName;
	
			pair<bool, string> newName = { false, "" };
			pair<bool, CvType> cvType = { false, CvType::VOLTAGE };
			pair<bool, double> cvValue = { false, NAN };

			pair<bool, ConverterType> type = { false, ConverterType::PULSE };
			pair<bool, double> efficiency = { false, NAN };
		};
	
	
	
		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;
			if (tokens.empty())    return args;
	
			args.currentName = tokens.front();
			if (tokens.empty())    return args;
	
			tokens.pop_front();
			for (const auto& token : tokens)
			{
				if (isParamWithKey(token))
				{
					string key = extractKeyFromToken(token);
					if (key == "n")
					{
						if (args.newName.first == true)    continue;
	
						args.newName.first = true;
						args.newName.second = extractParamFromToken(token);
					}
					else if (key == "u")
					{
						if (args.cvType.first == true)    continue;
	
						args.cvType.first = true;
						args.cvType.second = parseCvType(extractParamFromToken(token));
					}
					else if (key == "v")
					{
						if (args.cvValue.first == true)    continue;
	
						args.cvValue.first = true;
						args.cvValue.second = strToDouble(extractParamFromToken(token));
					}
					else if (key == "t")
					{
						if (args.type.first == true)    continue;

						args.type.first = true;
						args.type.second = parseConverterType(extractParamFromToken(token));
					}
					else if (key == "e")
					{
						if (args.efficiency.first == true)    continue;

						args.efficiency.first = true;
						args.efficiency.second = strToDouble(extractParamFromToken(token));
					}
					else
						throw exception(string("Unrecognized parameter \"" + key).c_str());
				}
				else
				{
					if (isCvTypeString(token))
					{
						if (args.cvType.first == false)
						{
							args.cvType.first = true;
							args.cvType.second = parseCvType(token);
							continue;
						}
					}

					if (isConverterTypeString(token))
					{
						if (args.type.first == false)
						{
							args.type.first = true;
							args.type.second = parseConverterType(token);
							continue;
						}
					}
	
					if (isFloatNumberString(token))
					{
						if (args.cvValue.first == false)
						{
							args.cvValue.first = true;
							args.cvValue.second = strToDouble(token);
							continue;
						}
						if (args.efficiency.first == false)
						{
							args.efficiency.first = true;
							args.efficiency.second = strToDouble(token);
							continue;
						}
					}
	
					if (args.newName.first == true)    continue;
	
					args.newName.first = true;
					args.newName.second = token;
				}
			}
	
			return args;
		}
	
		bool isParamWithKey (const string & token) const
		{
			const auto charEqual_it = find(token.begin(), token.end(), '=');
			if (charEqual_it != token.end())    return true;
			return false;
		}
	
		string extractKeyFromToken (const string & token) const
		{
			if (!isParamWithKey(token))
				throw exception("This is not a parameter with a key");
	
			const auto charEqual_it = find(token.begin(), token.end(), '=');
			string result = string(token.begin(), charEqual_it);
			return result;
		}
	
		string extractParamFromToken (const string & token) const
		{
			if (!isParamWithKey(token))
				throw exception("This is not a parameter with a key");
	
			const auto charEqual_it = find(token.begin(), token.end(), '=');
			string result = string(charEqual_it + 1, token.end());
			return result;
		}
	
	
		void modifyConverterParams (const Arguments & args) const
		{
			if (args.newName.first == true)
				RenameConverter(args.currentName, args.newName.second);
			if (args.cvType.first == true)
				SetCvTypeForConverter(args.currentName, args.cvType.second);
			if (args.cvValue.first == true)
				SetCvValueForConverter(args.currentName, args.cvValue.second);

			if (args.type.first == true)
				SetTypeForConverter(args.currentName, args.type.second);
			if (args.efficiency.first == true)
				SetEfficiencyForConverter(args.currentName, args.efficiency.second);
		}
	
		void reportExecution (const Arguments & args) const
		{
			cout << "Parameters of converter \"" << args.currentName << "\" is changed: ";
	
			if (args.newName.first == true)
				cout << endl << "    Name - \"" << args.newName.second << "\"";
			if (args.type.first == true)
				cout << endl << "    Type - " << args.type.second;
			if (args.cvType.first == true)
				cout << endl << "    Type of controlled variable - " << args.cvType.second;
			if (args.cvValue.first == true)
			{
				cout << endl << "    Controlled variable - " << args.cvValue.second;
	
				string cvUnit = "V";
				if (args.cvType.second == CvType::CURRENT)
					cvUnit = "A";
				cout << " " << cvUnit;
			}
			if (args.efficiency.first == true)
			{
				cout << endl << "    Efficiency - " << args.efficiency.second << " %";
			}
	
			cout << endl;
		}
	
		void reportNonexsistentConverter (const string & name) const
		{
			cout << "An converter \"" << name << "\" doesn't exsist." << endl;
		}
	
};





class CommandModifyLoad : public Command
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }
	
			if (IsLoadExsist(args.currentName))
			{
				modifyLoadParams(args);
				reportExecution(args);
			}
			else
				reportNonexsistentLoad(args.currentName);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string currentName;
	
			pair<bool, string> newName = { false, "" };
			pair<bool, LoadType> type = { false, LoadType::RESISTIVE };
			pair<bool, double> value = { false, NAN };

			pair<bool, double> nomVoltage = { false, NAN };
		};
	
	
	
		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;
			if (tokens.empty())    return args;
	
			args.currentName = tokens.front();
			if (tokens.empty())    return args;
	
			tokens.pop_front();
			for (const auto& token : tokens)
			{
				if (isParamWithKey(token))
				{
					string key = extractKeyFromToken(token);
					if (key == "n")
					{
						if (args.newName.first == true)    continue;
	
						args.newName.first = true;
						args.newName.second = extractParamFromToken(token);
					}
					else if (key == "t")
					{
						if (args.type.first == true)    continue;
	
						args.type.first = true;
						args.type.second = parseLoadType(extractParamFromToken(token));
					}
					else if (key == "v")
					{
						if (args.value.first == true)    continue;
	
						args.value.first = true;
						args.value.second = strToDouble(extractParamFromToken(token));
					}
					else if (key == "n")
					{
						if (args.type.second != LoadType::ENERGY)    throw exception("Only loads of type \"power\" have a parameter \"nominal voltage\"");
						
						if (args.nomVoltage.first == true)    continue;
	
						args.nomVoltage.first = true;
						args.nomVoltage.second = strToDouble(extractParamFromToken(token));
					}
					else
						throw exception(string("Unrecognized parameter \"" + key).c_str());
				}
				else
				{
					if (isLoadTypeString(token))
					{
						if (args.type.first == false)
						{
							args.type.first = true;
							args.type.second = parseLoadType(token);
							continue;
						}
					}
	
					if (isFloatNumberString(token))
					{
						if (args.value.first == false)
						{
							args.value.first = true;
							args.value.second = strToDouble(token);
							continue;
						}

						if (args.type.second != LoadType::ENERGY)    throw exception("Only loads of type \"power\" have a parameter \"nominal voltage\"");

						if (args.nomVoltage.first == false)
						{
							args.nomVoltage.first = true;
							args.nomVoltage.second = strToDouble(token);
							continue;
						}
					}
	
					if (args.newName.first == true)    continue;
	
					args.newName.first = true;
					args.newName.second = token;
				}
			}
	
			return args;
		}
	
		bool isParamWithKey (const string & token) const
		{
			const auto charEqual_it = find(token.begin(), token.end(), '=');
			if (charEqual_it != token.end())    return true;
			return false;
		}
	
		string extractKeyFromToken (const string & token) const
		{
			if (!isParamWithKey(token))
				throw exception("This is not a parameter with a key");
	
			const auto charEqual_it = find(token.begin(), token.end(), '=');
			string result = string(token.begin(), charEqual_it);
			return result;
		}
	
		string extractParamFromToken (const string & token) const
		{
			if (!isParamWithKey(token))
				throw exception("This is not a parameter with a key");
	
			const auto charEqual_it = find(token.begin(), token.end(), '=');
			string result = string(charEqual_it + 1, token.end());
			return result;
		}
	
	
		void modifyLoadParams (const Arguments & args) const
		{
			if (args.newName.first == true)
				RenameLoad(args.currentName, args.newName.second);
			if (args.type.first == true)
				SetTypeForLoad(args.currentName, args.type.second);
			if (args.value.first == true)
				SetValueForLoad(args.currentName, args.value.second);
			if (args.nomVoltage.first == true)
				SetNomVoltageForPowerLoad(args.currentName, args.nomVoltage.second);
		}
	
		void reportExecution (const Arguments & args) const
		{
			cout << "Parameters of load \"" << args.currentName << "\" is changed: ";
	
			if (args.newName.first == true)
				cout << endl << "    Name - \"" << args.newName.second << "\"";
			if (args.type.first == true)
				cout << endl << "    Type - " << args.type.second;
			if (args.value.first == true)
			{
				cout << endl << "    Value - " << args.value.second;
	
				string valueUnit = "Ohm";
				if (args.type.second == LoadType::CONSTANT_CURRENT)
					valueUnit = "A";
				else if (args.type.second == LoadType::ENERGY)
					valueUnit = "W";
				cout << " " << valueUnit;
			}
			if (args.nomVoltage.first == true)
			{
				cout << endl << "    Nominal voltage - " << args.nomVoltage.second << " V";
			}
	
			cout << endl;
		}
	
		void reportNonexsistentLoad (const string & name) const
		{
			cout << "An load \"" << name << "\" doesn't exsist." << endl;
		}
	
};





class CommandDeleteNode : public Command
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }

			if (!IsNodeExsist(args.name))
				reportNonexsistentNode(args.name);

			if (IsLoadExsist(args.name))
			{
				if (args.mode != DeletingMode::NONE)
					throw exception("You can't specify a mode by deleting a load");
				if (args.newParentName != "")
					throw exception("You shouldn't specify a name of new parent by deleting a load");
			}
			else
			{
				if ( (args.mode == DeletingMode::WITH_DESCES) && (args.newParentName != "") )
					throw exception("You shouldn't specify a name of new parent by deleting a source with its descendants");
				if ((args.mode == DeletingMode::HANG_DESCES) && (args.newParentName != ""))
					throw exception("You shouldn't specify a name of new parent if you want to leave descentants of a deleting source unconnected");
				if ((args.mode == DeletingMode::RECONNECT_DESCES) && (args.newParentName == ""))
					args.newParentName = requestNewParentNameAndGet();
			}

			deleteNode(args);
			reportExecution(args);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string name = "";
			DeletingMode mode = DeletingMode::NONE;
			string newParentName = "";
		};
	
	
	
		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;

			if (tokens.empty())    return args;


			auto handeledArg = tokens.front();

			if (!isDeletingModeString(handeledArg))
			{
				args.name = handeledArg;

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}

			if (isDeletingModeString(handeledArg))
			{
				args.mode = parseDeletingMode(handeledArg);

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}

			if (!isDeletingModeString(handeledArg))
			{
				args.newParentName = handeledArg;

				tokens.pop_front();
				if (tokens.empty())    return args;
			}

			throw exception("Too many arguments for this command");
		}

		
	
		void reportNonexsistentNode (const string & name) const
		{
			cout << GetTypeOfNode_str(name) << " \"" << name << "\" doesn't exsist." << endl;
		}

		string requestNewParentNameAndGet() const
		{
			cout << "Please enter name of a desired new parent" << endl;
			string newName; getline(cin, newName);
			return newName;
		}

		void deleteNode (const Arguments & args) const
		{
			if (IsLoadExsist(args.name))
			{
				DeleteLoad(args.name);
			}
			else
			{
				switch (args.mode)
				{
					case DeletingMode::WITH_DESCES:
						DeleteSourceWithDescendants(args.name);
						break;

					case DeletingMode::HANG_DESCES:
						DeleteSourceAndMoveDescendants(args.name);
						break;

					case DeletingMode::RECONNECT_DESCES:
						DeleteSourceAndMoveDescendants(args.name, args.newParentName);
						break;


					default:
						throw exception("Invalid mode of deleting a source");
				}
			}
		}



		void reportExecution (const Arguments & args) const
		{
			cout << GetTypeOfNode_str(args.name) << " \"" << args.name << "\" is deleted successfully. ";
			
			if (IsSourceExsist(args.name))
			{
				cout << "Descendants";
				switch (args.mode)
				{
					case DeletingMode::WITH_DESCES:
						cout << " are also deleted";
						break;

					case DeletingMode::HANG_DESCES:
						cout << " are left unconnected";
						break;

					case DeletingMode::RECONNECT_DESCES:
						cout << " are connected to the " << GetTypeOfSource_str(args.newParentName) << " \"" << args.newParentName << "\"";
						break;


					default:
						throw exception("Invalid mode of deleting a source");
				}
			}
		}

};





class CommandMoveSink : public Command
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }
	
			if (args.name == "")
				args.name = requestNameAndGet();
			if (args.newParentName == "")
				args.newParentName = requestNewParentNameAndGet();
			
			if (!IsSinkExsist(args.name))    throw exception(("There is no sinks with the name \"" + args.name + "\"").c_str());
			if (!IsSourceExsist(args.newParentName))    throw exception(("There is no sources with the name \"" + 
				                                                          args.newParentName + "\"").c_str());
			if (AreParentAndDescendant(args.name, args.newParentName))    
				throw exception("The desired new parent is a descendant of the moving sink now");
	
			ConnectSinkTo(args.name, args.newParentName);
	
			reportExecution(args);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string name = "";
			string newParentName = "";
		};



		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;

			if (tokens.empty())    return args;


			auto handeledArg = tokens.front();
			args.name = handeledArg;

			tokens.pop_front();
			if (tokens.empty())    return args;


			handeledArg = tokens.front();
			args.newParentName = handeledArg;

			tokens.pop_front();
			if (tokens.empty())    return args;


			throw exception("Too many arguments for this command");
		}

		string requestNameAndGet () const
		{
			cout << "Please enter name of the moving sink" << endl;
			string newName; getline(cin, newName);
			return newName;
		}

		string requestNewParentNameAndGet () const
		{
			cout << "Please enter name of a desired new parent" << endl;
			string newName; getline(cin, newName);
			return newName;
		}

		void reportExecution (const Arguments & args) const
		{
			cout << GetTypeOfSink_str(args.name) << " \"" << args.name << "\" is connected to the " 
				 << GetTypeOfSource_str(args.newParentName) << " \"" << args.newParentName << "\" now" << endl;
		}
	
};





class CommandDisconnectSink : public Command
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			if (tokens.size() > 1)    throw exception("Too many argumnets for this command");

			string name;
			if (tokens.size() == 1)
				name = tokens.front();
			else
				name = requestNameAndGet();

			if (!IsSinkExsist(name))    throw exception(   ("There is no sinks with the name \"" + name + "\"").c_str()   );
	
			DisconnectSink(name);
	
			reportExecution(name);
		}
	
	
	
	
	private:
	
		string requestNameAndGet () const
		{
			cout << "Please enter name" << endl;
			string newName; getline(cin, newName);
			return newName;
		}

		void reportExecution (string name) const
		{
			cout << GetTypeOfSink_str(name) << " \"" << name << "\" is disconnected" << endl;
		}
	
};








#pragma todo
class CommandT: public Command
{

public:

	virtual void execute(TokensDeque& tokens) const
	{
		
	}




private:

	struct Arguments
	{

	};



	void reportExecution(const Arguments& args) const
	{

	}

};




static const CommandCreate cr;
static const CommandRename rn;
static const CommandSolve  sv;
static const CommandShowResults   sr;
static const CommandShowStructure ss;

static const CommandCreateInput     ci;
static const CommandCreateConverter cc;
static const CommandCreateLoad      cl;
static const CommandModifyInput     mi;
static const CommandModifyConverter mc;
static const CommandModifyLoad      ml;
static const CommandMoveSink        ms;
static const CommandDisconnectSink  ds;
static const CommandDeleteNode      dn;



static const map< string, const shared_ptr<Command> > commandDictionary = { { "cr", make_shared<CommandCreate>(cr)           },
																			{ "rn", make_shared<CommandRename>(rn)           },
																			{ "sv", make_shared<CommandSolve>(sv)            },
																			{ "sr", make_shared<CommandShowResults>(sr)      },
																			{ "ss", make_shared<CommandShowStructure>(ss)    },

																			{ "ci", make_shared<CommandCreateInput>(ci)      },
																			{ "cc", make_shared<CommandCreateConverter>(cc)  },
																			{ "cl", make_shared<CommandCreateLoad>(cl)       },
																			{ "mi", make_shared<CommandModifyInput>(mi)      },
																			{ "mc", make_shared<CommandModifyConverter>(mc)  },
																			{ "ml", make_shared<CommandModifyLoad>(ml)       },
																			{ "ms", make_shared<CommandMoveSink>(ms)         },
																			{ "ds", make_shared<CommandDisconnectSink>(ds)   },
																			{ "dn", make_shared<CommandDeleteNode>(dn)       }  };








TokensDeque tokenize (const string & command_string)
{
	if (command_string.size() == 0)    return TokensDeque();

	TokensDeque tokens;
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

void executeCommand (string enteredCommand)
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




void TestCreateArgumentsParsing()
{
	using Arguments = CommandCreate::Arguments;




	{
		TokensDeque emptyTokens;

		Arguments emptyArgs;

		auto emptyOut = cr.parseArguments(emptyTokens);

		Assert(emptyArgs == emptyOut, "");
	}




	{
		TokensDeque onlyNameTokens = { "name" };

		CommandCreate::Arguments onlyNameArgs;
		onlyNameArgs.name = "name";

		auto onlyNameOut = cr.parseArguments(onlyNameTokens);

		Assert(onlyNameArgs == onlyNameOut, "");
	}




	{
		TokensDeque onlyTypeTokens = { "cur" };

		Arguments onlyTypeArgs;
		onlyTypeArgs.inputCvType = CvType::CURRENT;

		auto onlyTypeOut = cr.parseArguments(onlyTypeTokens);

		Assert(onlyTypeArgs == onlyTypeOut, "");
	}




	{
		TokensDeque onlyValueTokens = { "24" };

		Arguments onlyValueArgs;
		onlyValueArgs.inputCvValue = 24;

		auto onlyValueOut = cr.parseArguments(onlyValueTokens);

		Assert(onlyValueArgs == onlyValueOut, "");
	}
}
