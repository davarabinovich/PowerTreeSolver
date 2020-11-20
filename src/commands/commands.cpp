
#include <sstream>
#include <cctype>
#include <cmath>

#include <deque>
#include <set>
#include <map>
#include <memory>
#include <optional>


#include "lib/ciflib.h"


#include "config.h"


#include "electric_net/electric_net_if.h"
#include "electric_net/electric_net.h"


#include "commands.h"




using namespace std;
using namespace electric_net;




























struct TreeStructure
{
	struct Load
	{
		string name;
		LoadType type;
		double value;
	};

	struct Converter
	{
		string name;
		CvType cvType;
		double cvValue;
		ConverterType type;

		vector<Converter> converterSinks;
		vector<Load>      loadSinks;
	};

	struct Input
	{
		string name;
		CvType cvType;
		double cvValue;

		vector<Converter> converterSinks;
		vector<Load>      loadSinks;
	};




	vector<Input> inputs;
	vector<Converter> flyingConverters;
	vector<Load> flyingLoads;
};



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


	vector<Source> inputs;
};





void Solve() {}
Results GetResults() { return Results(); }

string GetNameOfTree() { return string(); }
TreeStructure GetTreeStructure() { return TreeStructure(); }
















// Dependencies injection is there
static shared_ptr<ElectricNet_If> activePowerTree;

bool isThereSomeTree ()
{ 
	bool result = !(activePowerTree == nullptr);
	return result;
}




namespace commands
{

	using TokensDeque = deque<string>;




	const string defaultTreeName = "New power tree";
	const string defaultInputName = "Input 1";
	
	











	

	class Command
	{
	
		public:
	
			virtual void execute (TokensDeque & tokens) const = 0;
	
	
	
	
		protected:
	
			virtual ~Command () {;}
	
	};
	
	
	
	
	class CommandCreate : public Command
	{

		public:

			virtual void execute (TokensDeque & tokens) const 
			{
				Arguments args;			
				try { args = parseArguments(tokens); }
				catch (exception & ex) { throw exception(ex.what()); }

				if (args.name == "")
					args.name = suggestEnterNameAndGet();
				if (args.inputName == "")
					args.inputName = "Input1";

				createTreeByArgs(args);
				createInputByArgs(args);

				reportExecution(args);
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
				string name = args.name;
				if (name == "")
					name = defaultTreeName;
				
				activePowerTree = make_shared<ElectricNet>(name);
			}

			void createInputByArgs (const Arguments & args) const
			{
				string name = args.inputName;
				if (name == "")
					name = defaultInputName;

				activePowerTree->addInput(name, args.inputCvType, args.inputCvValue);
			}

			void reportExecution (const Arguments & args) const
			{
				string name = args.name;
				if (name == "")
					name = defaultTreeName;
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


				cout << "A new power three " << name << "with a " << cvType << " input \"" << args.inputName << "\"";
				if (isCvValuePresent)	cout << " " << args.inputCvValue << " " << cvUnit;
				cout << " is created" << endl << endl;
			}

};
	
	
	
	
	class CommandWorkingWithExsistingTree : public Command
	{

		protected:

			void ensureIfThereAreSomeTree () const
			{
				if ( isThereSomeTree() )	return;
				throw exception("There is no power tree. Create or load a tree");
			}



			virtual ~CommandWorkingWithExsistingTree () {;}

	};





	class CommandRename : public CommandWorkingWithExsistingTree
{

	public:
	
		virtual void execute (TokensDeque& tokens) const
		{
			ensureIfThereAreSomeTree();

			if (tokens.size() > 1)    throw exception("Too many arguments for this command");

			string newName;
			if (tokens.size() == 0)
				newName = requestAndGetNewName();
			else
				newName = tokens.front();

			string oldName = activePowerTree->getTitle();
			activePowerTree->rename(newName);

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
	
	
	
	
	class CommandWithShowingResults : public CommandWorkingWithExsistingTree
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const override
		{
			ensureIfThereAreSomeTree();


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
			ensureIfThereAreSomeTree();


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
	
	
	
	
	class CommandShowStructure : public CommandWorkingWithExsistingTree
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			ensureIfThereAreSomeTree();

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





	class CommandCreateInput : public CommandWorkingWithExsistingTree
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			ensureIfThereAreSomeTree();


			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception & ex) { throw exception(ex.what()); }
	

#pragma todo not only NAN is invalid
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


	

		double requestCvValue (const CvType type) const
		{
			cout << "Plase enter a value of " << type << endl;
			string enteredValue; getline(cin, enteredValue);
			auto value = strToDouble(enteredValue);
			return value;
		}

		void createInputByArgs (Arguments & args) const
		{
			activePowerTree->addInput(args.name, args.cvType, args.cvValue);
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
	
	
	
	
	class CommandCreateConverter : public CommandWorkingWithExsistingTree
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			ensureIfThereAreSomeTree();


			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }

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
			if (args.parentName == "")
				activePowerTree->addConverter(args.name, args.type, args.cvType, args.cvValue, args.efficiency);
			else
				activePowerTree->addConverter(args.name, args.parentName, args.type, args.cvType, args.cvValue, args.efficiency);
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
				cout << " at the " << toStr( activePowerTree->getNodeType(args.parentName) ) << " \"" << args.parentName << "\"";
			cout << endl << endl;
		}
	
};
	
	
	
	
	class CommandCreateLoad : public CommandWorkingWithExsistingTree
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			ensureIfThereAreSomeTree();


			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }

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

			double addValue = NAN;

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
			switch (args.type)
			{
				case (LoadType::RESISTIVE): [[__fallthrough]]
				case (LoadType::CONSTANT_CURRENT):
				{
					if (args.parentName == "")
						activePowerTree->addLoad(args.name, args.type, args.value);
					else
						activePowerTree->addLoad(args.name, args.parentName, args.type, args.value);

					break;
				}

				case (LoadType::DIODE): [[__fallthrough]]
				case (LoadType::ENERGY):
				{
					if (args.parentName == "")
						activePowerTree->addLoad(args.name, args.type, args.value, args.addValue);
					else
						activePowerTree->addLoad(args.name, args.parentName, args.type, args.value, args.addValue);

					break;
				}
			}
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
				cout << " at the " << toStr( activePowerTree->getNodeType(args.parentName) ) << " \"" << args.parentName << "\"";
			cout << endl << endl;
		}
	
};
	
	
	
	
	class CommandModifyInput : public CommandWorkingWithExsistingTree
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			ensureIfThereAreSomeTree();


			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }

			modifyInputParams(args);
			reportExecution(args);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string currentName;

			optional<string> newName;
			optional<CvType> cvType;
			optional<double> cvValue;
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
						if (args.newName)    
							continue;

						args.newName = extractParamFromToken(token);
					}
					else if (key == "u")
					{
						if (args.cvType)    
							continue;

						args.cvType = parseCvType(extractParamFromToken(token));
					}
					else if (key == "v")
					{
						if (args.cvValue)    
							continue;

						args.cvValue = strToDouble(extractParamFromToken(token));
					}
					else
						throw exception(  string("Unrecognized parameter \"" + key).c_str()  );
				}
				else
				{
					if (isCvTypeString(token))
					{
						if (args.cvType)
						{
							args.cvType = parseCvType(token);
							continue;
						}
					}

					if (isFloatNumberString(token))
					{
						if (args.cvValue)
						{
							args.cvValue = strToDouble(token);
							continue;
						}
					}
					
					if (args.newName)    
						continue;

					args.newName = token;
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
			if (args.newName)
				activePowerTree->renameNode(args.currentName, *args.newName);
			if (args.cvType)
				activePowerTree->setSourceCvType(args.currentName, *args.cvType);
			if (args.cvValue)
				activePowerTree->setSourceCvValue(args.currentName, *args.cvValue);
		}

		void reportExecution (const Arguments & args) const
		{
			cout << "Parameters of input \"" << args.currentName << "\" is changed: ";
			
			if (args.newName)
				cout << endl << "    Name - \"" << *args.newName << "\"";
			if (args.cvType)
				cout << endl << "    Type of controlled variable - " << *args.cvType;
			if (args.cvValue)
			{
				cout << endl << "    Controlled variable - " << *args.cvValue;

				string cvUnit = "V";
				if (*args.cvType == CvType::CURRENT)
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
	
	
	
	
	class CommandModifyConverter : public CommandWorkingWithExsistingTree
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			ensureIfThereAreSomeTree();


			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }
	

			modifyConverterParams(args);
			
			reportExecution(args);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string currentName;
	
			optional<string> newName; 
			optional<CvType> cvType;
			optional<double> cvValue; 

			optional<ConverterType> type;
			optional<double> efficiency;
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
						if (args.newName)    continue;
	
						args.newName = extractParamFromToken(token);
					}
					else if (key == "u")
					{
						if (args.cvType)    continue;
	
						args.cvType = parseCvType(extractParamFromToken(token));
					}
					else if (key == "v")
					{
						if (args.cvValue)    continue;
	
						args.cvValue = strToDouble(extractParamFromToken(token));
					}
					else if (key == "t")
					{
						if (args.type)    continue;

						args.type = parseConverterType(extractParamFromToken(token));
					}
					else if (key == "e")
					{
						if (args.efficiency)    continue;

						args.efficiency = strToDouble(extractParamFromToken(token));
					}
					else
						throw exception(string("Unrecognized parameter \"" + key).c_str());
				}
				else
				{
					if (isCvTypeString(token))
					{
						if (args.cvType)
						{
							args.cvType = parseCvType(token);
							continue;
						}
					}

					if (isConverterTypeString(token))
					{
						if (args.type)
						{
							args.type = parseConverterType(token);
							continue;
						}
					}
	
					if (isFloatNumberString(token))
					{
						if (args.cvValue)
						{
							args.cvValue = strToDouble(token);
							continue;
						}
						if (args.efficiency)
						{
							args.efficiency = strToDouble(token);
							continue;
						}
					}
	
					if (args.newName)    continue;
	
					args.newName = token;
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
#pragma split structue to separate values;
			if (args.newName)
				activePowerTree->renameNode(args.currentName, *args.newName);
			if (args.cvType)
				activePowerTree->setSourceCvType(args.currentName, *args.cvType);
			if (args.cvValue)
				activePowerTree->setSourceCvValue(args.currentName, *args.cvValue);

			if (args.type)
				activePowerTree->setConverterType(args.currentName, *args.type);
			if (args.efficiency)
				activePowerTree->setConverterEfficiency(args.currentName, *args.efficiency);
		}
	
		void reportExecution (const Arguments & args) const
		{
			cout << "Parameters of converter \"" << args.currentName << "\" is changed: ";
	
			if (args.newName)
				cout << endl << "    Name - \"" << *args.newName << "\"";
			if (args.type)
				cout << endl << "    Type - " << *args.type;
			if (args.cvType)
				cout << endl << "    Type of controlled variable - " << *args.cvType;
			if (args.cvValue)
			{
				cout << endl << "    Controlled variable - " << *args.cvValue;
	
				string cvUnit = "V";
				if (*args.cvType == CvType::CURRENT)
					cvUnit = "A";
				cout << " " << cvUnit;
			}
			if (args.efficiency)
			{
				cout << endl << "    Efficiency - " << *args.efficiency << " %";
			}
	
			cout << endl;
		}
	
		void reportNonexsistentConverter (const string & name) const
		{
			cout << "An converter \"" << name << "\" doesn't exsist." << endl;
		}
	
};
	
	
	
	
	class CommandModifyLoad : public CommandWorkingWithExsistingTree
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			ensureIfThereAreSomeTree();


			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception& ex) { throw exception(ex.what()); }
	
			modifyLoadParams(args);
			reportExecution(args);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string currentName;
	
			optional<string>   newName;
			optional<LoadType> type;
			optional<double>   value;

			optional<double> addValue;
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
						if (args.newName)    continue;
	
						args.newName = extractParamFromToken(token);
					}
					else if (key == "t")
					{
						if (args.type)    continue;
	
						args.type = parseLoadType(extractParamFromToken(token));
					}
					else if (key == "v")
					{
						if (args.value)    continue;
	
						args.value = strToDouble(extractParamFromToken(token));
					}
					else if (key == "n")
					{
						if (*args.type != LoadType::ENERGY)    throw exception("Only loads of type \"power\" have a parameter \"nominal voltage\"");
						
						if (args.addValue)    continue;
	
						args.addValue = strToDouble(extractParamFromToken(token));
					}
					else
						throw exception(string("Unrecognized parameter \"" + key).c_str());
				}
				else
				{
					if (isLoadTypeString(token))
					{
						if (args.type)
						{
							args.type = parseLoadType(token);
							continue;
						}
					}
	
					if (isFloatNumberString(token))
					{
						if (args.value)
						{
							args.value = strToDouble(token);
							continue;
						}

						if (*args.type != LoadType::ENERGY)    throw exception("Only loads of type \"power\" have a parameter \"nominal voltage\"");

						if (args.addValue)
						{
							args.addValue = strToDouble(token);
							continue;
						}
					}
	
					if (args.newName)    continue;
	
					args.newName = token;
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
			string actualName = args.currentName;
			if (args.newName)
			{
				activePowerTree->renameNode(actualName, *args.newName);
				actualName = *args.newName;
			}

			if (args.type)
				activePowerTree->setLoadType(actualName, *args.type);

			LoadType actualType = activePowerTree->getLoadType(actualName);
			switch (actualType)
			{
				case LoadType::RESISTIVE:
				{
					if (args.value)
						activePowerTree->setLoadResistance(actualName, *args.value);
					return;
				}

				case LoadType::CONSTANT_CURRENT:
				{
					if (args.value)
						activePowerTree->setLoadCurrent(actualName, *args.value);
					return;
				}

				case LoadType::ENERGY:
				{
					if (args.value)
						activePowerTree->setLoadForawrdVoltage(actualName, *args.value);
					if (args.value)
						activePowerTree->setLoadForwardCurrent(actualName, *args.addValue);
					return;
				}

				case LoadType::DIODE:
				{
					if (args.value)
						activePowerTree->setLoadNomPower(actualName, *args.value);
					if (args.value)
						activePowerTree->setLoadNomVoltage(actualName, *args.addValue);
					return;
				}
			}
			

		}
	
		void reportExecution (const Arguments & args) const
		{
			cout << "Parameters of load \"" << args.currentName << "\" is changed: ";
	
			if (args.newName)
				cout << endl << "    Name - \"" << *args.newName << "\"";
			if (args.type)
				cout << endl << "    Type - " << *args.type;
			if (args.value)
			{
				cout << endl << "    Value - " << *args.value;
	
				string valueUnit = "Ohm";
				if (args.type == LoadType::CONSTANT_CURRENT)
					valueUnit = "A";
				else if (*args.type == LoadType::ENERGY)
					valueUnit = "W";
				cout << " " << valueUnit;
			}
			if (args.addValue)
			{
				cout << endl << "    Nominal voltage - " << *args.addValue << " V";
			}
	
			cout << endl;
		}
	
		void reportNonexsistentLoad (const string & name) const
		{
			cout << "An load \"" << name << "\" doesn't exsist." << endl;
		}
	
};
	
	
	
	
	class CommandDeleteNode : public CommandWorkingWithExsistingTree
	{
		
		public:
		
			virtual void execute (TokensDeque & tokens) const
			{
				ensureIfThereAreSomeTree();


				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception & ex) { throw exception(ex.what()); }
	
				deleteNode(args);
				reportExecution(args);
			}
		
		
		
		
		private:
		
			struct Arguments
			{
				string name = "";
				optional<MotionMode> mode;
				optional<string> newParentName;
			};
		
		
		
			Arguments parseArguments (TokensDeque & tokens) const
			{
				Arguments args;
	
				if (tokens.empty())    return args;
	
	
				auto handeledArg = tokens.front();
	
				if (!isMotionModeString(handeledArg))
				{
					args.name = handeledArg;

					tokens.pop_front();
					if (tokens.empty())    return args;
					handeledArg = tokens.front();
				}
	
				if (isMotionModeString(handeledArg))
				{
					args.mode = parseDeletingMode(handeledArg);
	
					tokens.pop_front();
					if (tokens.empty())    return args;
					handeledArg = tokens.front();
				}
	
				if (!isMotionModeString(handeledArg))
				{
					args.newParentName = handeledArg;
	
					tokens.pop_front();
					if (tokens.empty())    return args;
				}
	

				throw exception("Too many arguments for this command");
			}
	
			
		
			void reportNonexsistentNode (const string & name) const
			{
				string type = toStr( activePowerTree->getNodeType(name) );
				cout << capitalize(type) << " \"" << name << "\" doesn't exsist." << endl;
			}
	
			string requestNewParentNameAndGet() const
			{
				cout << "Please enter name of a desired new parent" << endl;
				string newName; getline(cin, newName);
				return newName;
			}
	
			void deleteNode (const Arguments & args) const
			{
				if ( activePowerTree->isLoadExsist(args.name) )
					activePowerTree->deleteLoad(args.name);
				else
				{
					switch (*args.mode)
					{
						case MotionMode::WITH_DESCES:
							activePowerTree->deleteSubnet(args.name);
							break;

						case MotionMode::FREE_DESCES:
							activePowerTree->deleteNode(args.name);
							break;

						case MotionMode::RECONNECT_DESCES:
							activePowerTree->deleteNode(args.name, *args.newParentName);
							break;


						default:
							;
					}
				}
			}
	

	
			void reportExecution (const Arguments & args) const
			{
				string type = toStr( activePowerTree->getNodeType(args.name) );
				cout << capitalize(type) << " \"" << args.name << "\" is deleted successfully. ";
				
				if (args.mode)
				{
					cout << "Descendants";
					switch (*args.mode)
					{
						case MotionMode::WITH_DESCES:
							cout << " are also deleted";
							break;
	
						case MotionMode::FREE_DESCES:
							cout << " are left unconnected";
							break;
	
						case MotionMode::RECONNECT_DESCES:
							cout << " are connected to the " << toStr( activePowerTree->getNodeType(*args.newParentName) )
								 << " \"" << *args.newParentName << "\"";
							break;
	
	
						default:
							throw exception("Invalid mode of deleting a source");
					}
				}
			}
	
	};
	
	
	
	
	class CommandMoveSink : public CommandWorkingWithExsistingTree
	{
		
		public:
		
			virtual void execute (TokensDeque & tokens) const
			{
				ensureIfThereAreSomeTree();


				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception & ex) { throw exception(ex.what()); }
		
				if (args.name == "")
					args.name = requestNameAndGet();
				if (args.newParentName == "")
					args.newParentName = requestNewParentNameAndGet();
		
				ConnectSinkTo(args);
		
				reportExecution(args);
			}
		
		
		
		
		private:
		
			struct Arguments
			{
				string name = "";
				string newParentName = "";
				optional<MotionMode> mode;
				optional<string> newSinksParentName = "";
			};



			Arguments parseArguments (TokensDeque & tokens) const
			{
				Arguments args;
	
				if (tokens.empty())    return args;
	
	
				auto handeledArg = tokens.front();
	
				if (!isMotionModeString(handeledArg))
				{
					args.name = handeledArg;

					tokens.pop_front();
					if (tokens.empty())    return args;
					handeledArg = tokens.front();
				}
	
				if (isMotionModeString(handeledArg))
				{
					args.mode = parseDeletingMode(handeledArg);
	
					tokens.pop_front();
					if (tokens.empty())    return args;
					handeledArg = tokens.front();
				}
	
				if (!isMotionModeString(handeledArg))
				{
					args.newParentName = handeledArg;
	
					tokens.pop_front();
					if (tokens.empty())    return args;
					handeledArg = tokens.front();
				}
	
				if (!isMotionModeString(handeledArg))
				{
					args.newSinksParentName = handeledArg;
	
					tokens.pop_front();
					if (tokens.empty())    return args;
				}


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

			void ConnectSinkTo (const Arguments & args) const
			{
				if ( activePowerTree->isLoadExsist(args.name) )
					activePowerTree->moveLoad(args.name, args.newParentName);
				else
				{
					switch (*args.mode)
					{
						case MotionMode::WITH_DESCES:
							activePowerTree->moveSubnet(args.name, args.newParentName);
							break;

						case MotionMode::FREE_DESCES:
							activePowerTree->moveNode(args.name, args.newParentName);
							break;

						case MotionMode::RECONNECT_DESCES:
							activePowerTree->moveNode(args.name, args.newParentName, *args.newSinksParentName);
							break;


						default:
							;
					}
				}
			}

			void reportExecution (const Arguments & args) const
			{
				string type = toStr( activePowerTree->getNodeType(args.name) );
				cout << capitalize(type) << " \"" << args.name << "\" has been connected to the " 
					 << toStr( activePowerTree->getNodeType(args.newParentName) ) << " \"" << args.newParentName << "\"";

				switch (*args.mode)
				{
					case MotionMode::WITH_DESCES:
						cout << " with its sinks." << endl;
						return;

					case MotionMode::FREE_DESCES:
						cout << ". All sinks are left free." << endl;
						return;

					case MotionMode::RECONNECT_DESCES:
						cout << ". All sinks have been connected to the " << toStr( activePowerTree->getNodeType(*args.newSinksParentName) ) 
							 << " \"" << *args.newSinksParentName << "\"." << endl;
						return;


					default:
						;
				}
			}
		
};
	
	
	
	
	class CommandDisconnectSink : public CommandWorkingWithExsistingTree
{
	
	public:
	
		virtual void execute (TokensDeque & tokens) const
		{
			ensureIfThereAreSomeTree();


			Arguments args;
			try { args = parseArguments(tokens); }
			catch (exception & ex) { throw exception(ex.what()); }


			DisconnectSink(args);
		
			reportExecution(args);
		}
	
	
	
	
	private:
	
		struct Arguments
		{
			string name = "";
			optional<MotionMode> mode;
			optional<string> newSinksParentName = "";
		};



		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;
	
			if (tokens.empty())    return args;
	
	
			auto handeledArg = tokens.front();
	
			if (!isMotionModeString(handeledArg))
			{
				args.name = handeledArg;

				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}
	
			if (isMotionModeString(handeledArg))
			{
				args.mode = parseDeletingMode(handeledArg);
	
				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}
	
			if (!isMotionModeString(handeledArg))
			{
				args.newSinksParentName = handeledArg;
	
				tokens.pop_front();
				if (tokens.empty())    return args;
			}
	

			throw exception("Too many arguments for this command");
		}

		void DisconnectSink (const Arguments & args) const
		{
			if ( activePowerTree->isLoadExsist(args.name) )
				activePowerTree->freeLoad(args.name);
			else
			{
				switch (*args.mode)
				{
					case MotionMode::WITH_DESCES:
						activePowerTree->freeSubnet(args.name);
						break;

					case MotionMode::FREE_DESCES:
						activePowerTree->freeNode(args.name);
						break;

					case MotionMode::RECONNECT_DESCES:
						activePowerTree->freeNode(args.name, *args.newSinksParentName);
						break;


					default:
						;
				}
			}
		}

		void reportExecution (const Arguments & args) const
		{
			string type = toStr( activePowerTree->getNodeType(args.name) );
			cout << capitalize(type) << " \"" << args.name << "\" has been disconnected ";

			switch (*args.mode)
			{
				case MotionMode::WITH_DESCES:
					cout << " with its sinks." << endl;
					return;

				case MotionMode::FREE_DESCES:
					cout << ". All sinks are left free." << endl;
					return;

				case MotionMode::RECONNECT_DESCES:
					cout << ". All sinks have been connected to the " << toStr( activePowerTree->getNodeType(*args.newSinksParentName) ) 
						 << " \"" << *args.newSinksParentName << "\"." << endl;
					return;


				default:
					;
			}
		}
	
};
	
	
	
	
	
	
	
	
	
	
	static const map< string, const shared_ptr<Command> > commandDictionary = { { "cr", make_shared<CommandCreate>()           },
																				{ "rn", make_shared<CommandRename>()           },
																				{ "sv", make_shared<CommandSolve>()            },
																				{ "sr", make_shared<CommandShowResults>()      },
																				{ "ss", make_shared<CommandShowStructure>()    },
	
																				{ "ci", make_shared<CommandCreateInput>()      },
																				{ "cc", make_shared<CommandCreateConverter>()  },
																				{ "cl", make_shared<CommandCreateLoad>()       },
																				{ "mi", make_shared<CommandModifyInput>()      },
																				{ "mc", make_shared<CommandModifyConverter>()  },
																				{ "ml", make_shared<CommandModifyLoad>()       },
																				{ "ms", make_shared<CommandMoveSink>()         },
																				{ "ds", make_shared<CommandDisconnectSink>()   },
																				{ "dn", make_shared<CommandDeleteNode>()       }  };
	
	
	
	
	
	
	
	
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
	
	
	
	
	//void TestCreateArgumentsParsing()
	//{
	//	using Arguments = CommandCreate::Arguments;
	//
	//
	//
	//
	//	{
	//		TokensDeque emptyTokens;
	//
	//		Arguments emptyArgs;
	//
	//		auto emptyOut = cr.parseArguments(emptyTokens);
	//
	//		Assert(emptyArgs == emptyOut, "");
	//	}
	//
	//
	//
	//
	//	{
	//		TokensDeque onlyNameTokens = { "name" };
	//
	//		CommandCreate::Arguments onlyNameArgs;
	//		onlyNameArgs.name = "name";
	//
	//		auto onlyNameOut = cr.parseArguments(onlyNameTokens);
	//
	//		Assert(onlyNameArgs == onlyNameOut, "");
	//	}
	//
	//
	//
	//
	//	{
	//		TokensDeque onlyTypeTokens = { "cur" };
	//
	//		Arguments onlyTypeArgs;
	//		onlyTypeArgs.inputCvType = CvType::CURRENT;
	//
	//		auto onlyTypeOut = cr.parseArguments(onlyTypeTokens);
	//
	//		Assert(onlyTypeArgs == onlyTypeOut, "");
	//	}
	//
	//
	//
	//
	//	{
	//		TokensDeque onlyValueTokens = { "24" };
	//
	//		Arguments onlyValueArgs;
	//		onlyValueArgs.inputCvValue = 24;
	//
	//		auto onlyValueOut = cr.parseArguments(onlyValueTokens);
	//
	//		Assert(onlyValueArgs == onlyValueOut, "");
	//	}
	//}
	





}
