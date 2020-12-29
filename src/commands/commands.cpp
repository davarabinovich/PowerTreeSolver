
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
#include "file_server/file_server.h"


#include "commands.h"




using namespace std;
using namespace electric_net;
using namespace file_server;










// Dependencies injection is there
using PowerTree = ElectricNet;
static shared_ptr<ElectricNet_If> activePowerTree;

bool isThereSomeTree ()
{ 
	bool result = !(activePowerTree == nullptr);
	return result;
}




static string fileName;
static string path;










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
				fileName = args.name;

				createInputByArgs(args);

				reportExecution(args);
			}




		private:

			struct Arguments
			{
				string name = "";
				string inputName = "";
				VarKind inputCvType = VarKind::VOLTAGE;
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
					args.inputCvType = parseVarKind(handeledArg);

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
					args.inputCvType = parseVarKind(handeledArg);

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
				args.inputCvType = parseVarKind(handeledArg);

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
				
				activePowerTree = make_shared<PowerTree>(name);
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
				if (args.inputCvType == VarKind::CURRENT)
					cvType = "current"; 

				bool isCvValuePresent = false;
				string cvUnit = "V";
				if (!isnan(args.inputCvValue))
				{
					isCvValuePresent = true;
					if (args.inputCvType == VarKind::CURRENT)
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
		
	

	
	
	class CommandWithShowingStrucute : public CommandWorkingWithExsistingTree
	{
		
		protected:
			
			static const unsigned spaces_per_level_shift = 12;



			void displayHeader () const
			{
				string treeTitle = activePowerTree->getTitle();
				cout << "Structure of power net \"" << treeTitle << "\":" << endl << endl;
			}

	};





	class CommandSolve : public CommandWithShowingStrucute
	{
	
		public:
		
			virtual void execute (TokensDeque & tokens) const override
			{
				ensureIfThereAreSomeTree();

				activePowerTree->calculte();
				displayResults();
			}
		
		
		
		
		private:

			#pragma todo decide what kind of functional object is the best
			class DisplayResultsForElectricNode
			{
				public:
					void operator () (key nodeName)
					{
						auto nodeType = activePowerTree->getNodeType(nodeName);
						switch (nodeType)
						{
							case DeviceType::INPUT:
							{
								auto inputResults = activePowerTree->getInputResults(nodeName);
								displayInputResults(inputResults);
								break;
							}

							case DeviceType::CONVERTER:
							{
								auto converterResults = activePowerTree->getConverterResults(nodeName);
								displayConverterResults(converterResults);
								break;
							}

							case DeviceType::LOAD:
							{
								displayLoadResults(nodeName);
								break;
							}

							default:
								throw exception("Invalid type of device");

						}
					}
			};
			const DisplayResultsForElectricNode displayResultsForElectricNode;



			void displayResults () const
			{
				displayHeader();
				activePowerTree->iterateAndExecuteForEach(displayResultsForElectricNode);
				cout << endl;
			}


			static void displayInputResults (InputResults results)
			{
				auto [name, type, value, avValue] = results;

				string output = to_string(value) + getCvUnitDesignatorStr(type) + " source \"" + name + "\":\n";
				output += ("   gives " + to_string(avValue) + getAvUnitDesignatorStr(type));

				cout << output << endl << endl;
			}


			static void displayConverterResults (ConverterResults results)
			{
				auto [name, nestingLevel, cvType, value, type, avValue, inputValue] = results;
				auto shift = generateShift(nestingLevel);

				string output = shift + to_string(value) + getCvUnitDesignatorStr(cvType) + " " + toStr(type) + " dc/dc \"" + name + "\":\n";
				output += (shift + "   gives " + to_string(avValue) + getAvUnitDesignatorStr(cvType) + "\n");
				output += (shift + "   consumes " + to_string(inputValue) + "A");

				cout << output << endl << endl;
			}


			static void displayLoadResults (key loadName)
			{
				auto type = activePowerTree->getLoadType(loadName);
				switch (type)
				{
					case LoadType::RESISTIVE:
					{
						auto loadResults = activePowerTree->getResistiveLoadResults(loadName);
						displayResistiveLoadResults(loadResults);
						break;
					}
				
					case LoadType::CONSTANT_CURRENT:
					{
						auto loadResults = activePowerTree->getConstantCurrentLoadResults(loadName);
						displayConstantCurrentLoadResults(loadResults);
						break;
					}
				
					case LoadType::DIODE:
					{
						auto loadData = activePowerTree->getDiodeLoadResults(loadName);
						displayDiodeLoadResults(loadData);
						break;
					}
				
					default:
							throw exception("Invalid type of load");
				}
			}


			static void displayResistiveLoadResults (ResistiveLoadResults results)
			{
				auto [name, nestingLevel, resistance, inputValue, inputVarType] = results;
				auto shift = generateShift(nestingLevel);
				
				string output = shift + "Load \"" + name + "\" " + to_string(resistance) + "Ohm:" + "\n";
				
				output += shift;
				if (inputVarType == VarKind::VOLTAGE)
					output += "   consumes ";
				else
					output += "   works by ";
				output += (to_string(inputValue) + getAvUnitDesignatorStr(inputVarType));

				cout << output << endl << endl;
			}


			static void displayConstantCurrentLoadResults (ConstantCurrentLoadResults results)
			{
				auto [name, nestingLevel, current, inputVoltage] = results;
				auto shift = generateShift(nestingLevel);
				
				string output = shift + "Load \"" + name + "\" " + to_string(current) + "A";

				cout << output << endl << endl;
			}


			static void displayDiodeLoadResults (DiodeLoadResults metadata)
			{
				auto [name, nestingLevel, voltage, current] = metadata;
				auto shift = generateShift(nestingLevel);
				
				string output = shift + "Diode \"" + name + "\" " + to_string(voltage) + "V, " + to_string(current) + "A";

				cout << output << endl << endl;
			}


			static string generateShift (unsigned shifts_qty)
			{
				auto shift = string(shifts_qty*spaces_per_level_shift, ' ');
				return shift;
			}

	};
	




	class CommandShowStructure : public CommandWithShowingStrucute
	{
		
		public:
		
			virtual void execute (TokensDeque & tokens) const
			{
				ensureIfThereAreSomeTree();
	
				displayTreeStructure();
			}
		
		
		
		
		private:

			#pragma todo decide what kind of functional object is the best
			class DisplayElectricNode
			{
				public:
					void operator () (key nodeName)
					{
						auto nodeType = activePowerTree->getNodeType(nodeName);
						switch (nodeType)
						{
							case DeviceType::INPUT:
							{
								auto inputData = activePowerTree->getInputData(nodeName);
								displayInput(inputData);
								break;
							}

							case DeviceType::CONVERTER:
							{
								auto converterData = activePowerTree->getConverterData(nodeName);
								displayConverter(converterData);
								break;
							}

							case DeviceType::LOAD:
							{
								displayLoad(nodeName);
								break;
							}

							default:
								throw exception("Invalid type of device");

						}
					}
			};
			const DisplayElectricNode displayElectricNode;



			void displayTreeStructure () const
			{
				displayHeader();
				activePowerTree->iterateAndExecuteForEach(displayElectricNode);
				cout << endl;
			}


			static void displayInput (InputData data)
			{
				auto [name, type, value] = data;
				string output = to_string(value) + getCvUnitDesignatorStr(type) + " source \"" + name + "\":   ";
				cout << output << endl;
			}


			static void displayConverter (ConverterData data)
			{
				auto [name, nestingLevel, cvType, value, type, efficiency] = data;
				auto shift = string(nestingLevel*spaces_per_level_shift, ' ');
				
				string output = shift + to_string(value) + getCvUnitDesignatorStr(cvType) + " " + toStr(type) + " dc/dc \"" + name + "\"";
				if (type == ConverterType::PULSE)
					output += " (eff. " + to_string(efficiency) + "%):";
				cout << output << endl;
			}


			static void displayLoad (key loadName)
			{
				auto type = activePowerTree->getLoadType(loadName);
				switch (type)
				{
					case LoadType::RESISTIVE:
					{
						auto loadData = activePowerTree->getResistiveLoadData(loadName);
						displayResistiveLoad(loadData);
						break;
					}
				
					case LoadType::CONSTANT_CURRENT:
					{
						auto loadData = activePowerTree->getConstantCurrentLoadData(loadName);
						displayConstantCurrentLoad(loadData);
						break;
					}
				
					case LoadType::DIODE:
					{
						auto loadData = activePowerTree->getDiodeLoadData(loadName);
						displayDiodeLoad(loadData);
						break;
					}
				
					default:
							throw exception("Invalid type of load");
				}
			}


			static void displayResistiveLoad (ResistiveLoadData data)
			{
				auto [name, nestingLevel, resistance] = data;
				auto shift = string(nestingLevel*spaces_per_level_shift, ' ');
				
				string output = shift + "Load \"" + name + "\" " + to_string(resistance) + getMainUnitDesignatorStr(LoadType::RESISTIVE);
				cout << output << endl;
			}


			static void displayConstantCurrentLoad (ConstantCurrentLoadData data)
			{
				auto [name, nestingLevel, current] = data;
				auto shift = string(nestingLevel*spaces_per_level_shift, ' ');
				
				string output = shift + "Load \"" + name + "\" " + to_string(current) + getMainUnitDesignatorStr(LoadType::CONSTANT_CURRENT);
				cout << output << endl;
			}


			static void displayDiodeLoad (DiodeLoadData data)
			{
				auto [name, nestingLevel, forwardVoltage, forwardCurrent] = data;
				auto shift = string(nestingLevel*spaces_per_level_shift, ' ');
				
				string output = shift + "Load \"" + name + "\" " + to_string(forwardVoltage) + getMainUnitDesignatorStr(LoadType::DIODE)
					                  + " (forw. cur. " + to_string(forwardCurrent) + getAddUnitDesignatorStr(LoadType::DIODE) + ")";
				cout << output << endl;
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
			VarKind cvType = VarKind::VOLTAGE;
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
				args.cvType = parseVarKind(handeledArg);

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


	

		double requestCvValue (const VarKind type) const
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
			if (args.cvType == VarKind::CURRENT)
				cvType = "current";

			bool isCvValuePresent = false;
			string cvUnit = "V";
			if (!isnan(args.cvValue))
			{
				isCvValuePresent = true;
				if (args.cvType == VarKind::CURRENT)
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
				VarKind cvType = VarKind::VOLTAGE;
				double cvValue = NAN;
	
				ConverterType type = ConverterType::PULSE;
				double efficiency = 100.0;
	
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
					args.cvType = parseVarKind(handeledArg);
	
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
	
	
			double requestCvValue (const VarKind type) const
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
				if (args.cvType == VarKind::CURRENT)
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
				catch (exception & ex) { throw exception(ex.what()); }
	
				if (args.name == "")
					args.name = requestAndGetNewName();
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
	
					if (tokens.empty())    return args;
					handeledArg = tokens.front();
	
					if (isFloatNumberString(handeledArg))
					{
						args.addValue = strToDouble(handeledArg);
	
						tokens.pop_front();
						if (tokens.empty())    return args;
					}
				}
	
				args.parentName = tokens.front();
				tokens.pop_front();
				if (tokens.empty())    return args;
	
				throw exception("There is at least one invalid argument");
			}
	
			string requestAndGetNewName () const
			{
				cout << "Enter a name of new load: ";
				string newName;
				getline(cin, newName);
				return newName;
			}
	
			double requestValue (const LoadType type) const
			{
				cout << "Enter a value of ";
				string mainParam; 
				switch (type)
				{
					case LoadType::RESISTIVE:
						mainParam = "resistance";
						break;
	
					case LoadType::CONSTANT_CURRENT:
						mainParam = "current";
						break;
	
					case LoadType::DIODE:
						mainParam = "forward voltage";
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
	
					case (LoadType::DIODE):
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
				else if (args.type == LoadType::DIODE)
					valueUnit = "V";
	
	
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
				optional<VarKind> cvType;
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
	
							args.cvType = parseVarKind(extractParamFromToken(token));
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
								args.cvType = parseVarKind(token);
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
				if (args.cvType)
					activePowerTree->setSourceCvType(args.currentName, *args.cvType);
				if (args.cvValue)
					activePowerTree->setSourceCvValue(args.currentName, *args.cvValue);
				if (args.newName)
					activePowerTree->renameNode(args.currentName, *args.newName);
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
					if (*args.cvType == VarKind::CURRENT)
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
			optional<VarKind> cvType;
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
	
						args.cvType = parseVarKind(extractParamFromToken(token));
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
							args.cvType = parseVarKind(token);
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
#pragma todo split structue to separate values;
			if (args.cvType)
				activePowerTree->setSourceCvType(args.currentName, *args.cvType);
			if (args.cvValue)
				activePowerTree->setSourceCvValue(args.currentName, *args.cvValue);
			if (args.type)
				activePowerTree->setConverterType(args.currentName, *args.type);
			if (args.efficiency)
				activePowerTree->setConverterEfficiency(args.currentName, *args.efficiency);

			if (args.newName)
				activePowerTree->renameNode(args.currentName, *args.newName);
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
				if (*args.cvType == VarKind::CURRENT)
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
				catch (exception & ex) { throw exception(ex.what()); }
		
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
				for (const auto & token : tokens)
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
						else if (key == "a")
						{
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
	
					case LoadType::DIODE:
					{
						if (args.value)
							activePowerTree->setLoadForawrdVoltage(actualName, *args.value);
						if (args.addValue)
							activePowerTree->setLoadForwardCurrent(actualName, *args.addValue);
						return;
					}
				}
				
	
			}
		
			void reportExecution (const Arguments & args) const
			{
				cout << "Parameters of load \"" << args.currentName << "\" is changed: ";
		
				string actualName = args.currentName;
				if (args.newName)
				{ 
					actualName = *args.newName;
					cout << endl << "    Name - \"" << actualName << "\"";
				}
	
				LoadType actualType = activePowerTree->getLoadType(actualName);
				if (args.type)
					cout << endl << "    Type - " << actualType;
				
				if (args.value)
				{
					string paramStr = capitalize( getValueTypeStr(actualType) );
					cout << endl << "    " << paramStr << " - " << *args.value;
		
					string valueUnit = getMainUnitDesignatorStr(actualType);
					cout << " " << valueUnit;
				}
	
				if (args.addValue)
				{
					string paramStr = capitalize( getAddValueTypeStr(actualType) );
					cout << endl << "    " << paramStr << " - " << *args.addValue;
		
					string valueUnit = getAddUnitDesignatorStr(actualType);
					cout << " " << valueUnit;
				}
		
				cout << endl;
			}
		
			void reportNonexsistentLoad (const string & name) const
			{
				cout << "A load \"" << name << "\" doesn't exsist." << endl;
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

				Params params;
				fillParams(args, params);
	
				deleteNode(args, params);
				reportExecution(args, params);
			}
		
		
		
		
		private:
		
			struct Arguments
			{
				string name = "";
				optional<MotionMode> mode;
				optional<string> newParentName;
			};

			struct Params
			{
				DeviceType type;
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
				if (isMotionModeString(handeledArg))
				{
					args.mode = parseMotionMode(handeledArg);
	
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
	
			void fillParams (const Arguments & args, Params & params) const
			{
				params.type = activePowerTree->getNodeType(args.name);
			}

			void deleteNode (const Arguments & args, const Params & params) const
			{
				if ( params.type == DeviceType::LOAD )
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
	

	
			void reportExecution (const Arguments & args, const Params & params) const
			{
				string type = toStr(params.type);
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
		
				connectSinkTo(args);
		
				reportExecution(args);
			}
		
		
		
		
		private:
		
			struct Arguments
			{
				string name = "";
				string newParentName = "";
				optional<MotionMode> mode = MotionMode::WITH_DESCES;
				optional<string> newSinksParentName = "";
			};



			Arguments parseArguments (TokensDeque & tokens) const
			{
				Arguments args;
				bool isModeSpecifiedExplicitly = false;
	
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
					isModeSpecifiedExplicitly = true;
					args.mode = parseMotionMode(handeledArg);
	
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

					if ( isModeSpecifiedExplicitly && (args.mode != MotionMode::RECONNECT_DESCES) )
						throw exception ("New parent for descendants can't be specified when motion modei is not \"Reconnect\"");

					args.mode = MotionMode::RECONNECT_DESCES;
	
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

			void connectSinkTo (const Arguments & args) const
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

				if (type == "load")
				{
					cout << "." << endl;
					return;
				}

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
			optional<MotionMode> mode = MotionMode::WITH_DESCES;
			optional<string> newSinksParentName = "";
		};



		Arguments parseArguments (TokensDeque & tokens) const
		{
			Arguments args;
			bool isModeSpecifiedExplicitly = false;
	
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
				isModeSpecifiedExplicitly = true;
				args.mode = parseMotionMode(handeledArg);
	
				tokens.pop_front();
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
			}
	
			if (!isMotionModeString(handeledArg))
			{
				args.newSinksParentName = handeledArg;

				if ( isModeSpecifiedExplicitly && (args.mode != MotionMode::RECONNECT_DESCES) )
						throw exception ("New parent for descendants can't be specified when motion modei is not \"Reconnect\"");

				args.mode = MotionMode::RECONNECT_DESCES;
	
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

			if (type == "load")
			{
				cout << "." << endl;
				return;
			}

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





	class CommandSave : public CommandWorkingWithExsistingTree
	{

		public:

			virtual void execute (TokensDeque & tokens) const override
			{
				ensureIfThereAreSomeTree();


				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception & ex) { throw exception(ex.what()); }

				updateSystemVariables(args);
				recordPowerTree(args);
				reportExecution(args);
			}




		private:

			struct Arguments
			{
				string fileName;
				string path;
			};



			Arguments parseArguments (TokensDeque & tokens) const
			{
				if (tokens.empty())    return Arguments();


				Arguments args;

				if (tokens.size() == 1)
				{
					auto handeledArg = tokens.front();

					if (isBackSlashInString(handeledArg))
						args.path = handeledArg;
					else
						args.fileName = handeledArg;
				}
				else if (tokens.size() == 2)
				{
					auto firstArg = tokens[0];
					auto secondArg = tokens[1];

					if (isBackSlashInString(secondArg))
					{
						args.path = secondArg;
						args.fileName = firstArg;
					}
					else
					{
						args.path = firstArg;
						args.fileName = secondArg;
					}
				}
				else
					throw exception("Too many arguments for this command");

				return args;
			}


			void updateSystemVariables (Arguments args) const
			{
				if (!args.fileName.empty())
					fileName = args.fileName;

				if (!args.path.empty())
					path = args.path;
			}


			void recordPowerTree (Arguments args) const
			{
				string treeTitle = activePowerTree->getTitle();

				auto [newFileName, newPath] = args;

				if (args.fileName.empty())
					args.fileName = fileName;

				if (args.path.empty())
					args.path = path;

				FileWriter fileWriter(treeTitle, fileName, path);




				class WriteNode
				{
					public:
						WriteNode (FileWriter & genWfstream)
							: wfstream(genWfstream) {;}

						void operator () (key nodeName) 
						{
							auto nodeType = activePowerTree->getNodeType(nodeName);
							switch (nodeType)
							{
								case DeviceType::INPUT:
								{
									auto inputData = activePowerTree->getInputData(nodeName);
									wfstream << inputData; 
									break;
								}

								case DeviceType::CONVERTER:
								{
									auto converterData = activePowerTree->getConverterData(nodeName);
									wfstream << converterData; 
									break;
								}

								case DeviceType::LOAD:
								{
									writeLoad(nodeName, wfstream);
									break;
								}

								default:
									throw exception("Invalid type of device");

							}
						}

					private:
						FileWriter & wfstream;
				};
				WriteNode writeNode(fileWriter);




				activePowerTree->iterateAndExecuteForEach(writeNode);
			}


			void reportExecution (Arguments args) const
			{
				cout << "Power tree \"" << activePowerTree->getTitle() 
					 << "\" is been successfully saved to the file \"" << args.fileName << "\"" << endl << endl;
			}


			static void writeLoad (key loadName, FileWriter & wfstream)
			{
				auto type = activePowerTree->getLoadType(loadName);
				switch (type)
				{
					case LoadType::RESISTIVE:
					{
						auto loadData = activePowerTree->getResistiveLoadData(loadName);
						wfstream << loadData;
						break;
					}
				
					case LoadType::CONSTANT_CURRENT:
					{
						auto loadData = activePowerTree->getConstantCurrentLoadData(loadName);
						wfstream << loadData;
						break;
					}
				
					case LoadType::DIODE:
					{
						auto loadData = activePowerTree->getDiodeLoadData(loadName);
						wfstream << loadData;
						break;
					}
				
					default:
							throw exception("Invalid type of load");
				}
			}

	};





	class CommandLoad : public Command
	{

		public:

			virtual void execute (TokensDeque & tokens) const override
			{
				if (isThereSomeTree())
				{
					bool needToSaveActiveTree = suggestSaveActiveTree();
					if (needToSaveActiveTree)
					{
						CommandSave commandSave;
						TokensDeque savingTokens;
						commandSave.execute(savingTokens);
					}
				}


				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception& ex) { throw exception(ex.what()); }

				if (args.fileName.empty())
					args.fileName = requestFileNameAndGet();

				updateSystemVariables(args);
				loadTree(args);
				reportExecution(args);
			}




		private:

			struct Arguments
			{
				string fileName;
				string path;

				string title;
			};



			bool suggestSaveActiveTree () const
			{
				string activeTreeTitle = activePowerTree->getTitle();
				cout << "Do you want to save the power tree \"" << activeTreeTitle << "\"?" << endl;
				string answer; getline(cin, answer);

				if (answer == "yes" || answer == "Yes" || answer == "y" || answer == "Y")
					return true;
				else if (answer == "no" || answer == "No" || answer == "n" || answer == "N")
					return false;
				throw exception("Invalid answer");
			}


			Arguments parseArguments (TokensDeque & tokens) const
			{
				if (tokens.empty())    return Arguments();


				Arguments args;

				if (tokens.size() == 1)
				{
					auto handeledArg = tokens.front();

					if (isBackSlashInString(handeledArg))
						args.path = handeledArg;
					else
						args.fileName = handeledArg;
				}
				else if (tokens.size() == 2)
				{
					auto firstArg = tokens[0];
					auto secondArg = tokens[1];

					if (isBackSlashInString(secondArg))
					{
						args.path = secondArg;
						args.fileName = firstArg;
					}
					else
					{
						args.path = firstArg;
						args.fileName = secondArg;
					}
				}
				else
					throw exception("Too many arguments for this command");

				return args;
			}


			string requestFileNameAndGet() const
			{
				cout << "Please enter name of file to be loaded" << endl;
				string newName; getline(cin, newName);
				return newName;
			}


			void updateSystemVariables (Arguments args) const
			{
				fileName = args.fileName;
				
				if (!args.path.empty())
					path = args.path;
			}


			void loadTree (Arguments & args) const
			{
				FileReader rstream(fileName, path);

				string title = rstream.getTitle();
				args.title = title;
				activePowerTree = make_shared<PowerTree>(title);

				while (rstream.hasUnreadNode())
				{
					ReadNode node;
					rstream >> node;
					createNode(node);
				}
			}


			void createNode (ReadNode & node) const
			{
				auto type = node.type;
				switch (type)
				{
					case DeviceType::INPUT:
						createInputByParams(node);
						break;

					case DeviceType::CONVERTER:
						createConverterByParams(node);
						break;

					case DeviceType::LOAD:
						createLoadByParams(node);
						break;


					default:
						throw exception("Invalid type of device");
				}
			}


			void createInputByParams (ReadNode & node) const
			{
				auto data = get<ReadInput>(node.data);
				activePowerTree->addInput(node.name, data.cvKind, data.value);
			}


			void createConverterByParams (ReadNode & node) const
			{
				auto data = get<ReadConverter>(node.data);

				if (data.commonSinkData.parentName.empty())
					activePowerTree->addConverter(node.name, data.type, data.cvKind, data.value, data.efficiency);
				else
					activePowerTree->addConverter(node.name, data.commonSinkData.parentName, data.type, data.cvKind, data.value, 
					                                         data.efficiency);
			}


			void createLoadByParams (ReadNode & node) const
			{
				auto data = get<ReadLoad>(node.data);
				auto type = data.type;

				switch (type)
				{
					case LoadType::RESISTIVE:
					case LoadType::CONSTANT_CURRENT:
						if (data.commonSinkData.parentName.empty())
							activePowerTree->addLoad(node.name, data.type, data.mainParam);
						else
							activePowerTree->addLoad(node.name, data.commonSinkData.parentName, data.type, data.mainParam);
						break;
						
					case LoadType::DIODE:
						if (data.commonSinkData.parentName.empty())
							activePowerTree->addLoad(node.name, data.type, data.mainParam);
						else
							activePowerTree->addLoad(node.name, data.commonSinkData.parentName, data.type, data.mainParam, 
							                                    data.additionalParam);
						break;

					default:
						throw exception("Invalid type of load");
				}
			}


			void reportExecution (Arguments & args) const
			{
				cout << "A new power three \"" << args.title << "\" is successfully loaded" << endl << endl;
			}

	};





	class CommandChangeDirectory : public Command
	{

		public:

			virtual void execute (TokensDeque & tokens) const override
			{
				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception& ex) { throw exception(ex.what()); }

				setPath(args);
				reportExecution(args);
			}




		private:

			struct Arguments
			{
				string path;
			};



			Arguments parseArguments (TokensDeque & tokens) const
			{
				if (tokens.empty())    throw exception("Title of new directory should be specified");

				Arguments args;
				args.path = tokens.front();
				return args;

				throw exception("Too many arguments for this command");
			}


			void setPath (Arguments args) const
			{
				string newPath = args.path;

				size_t pathSize = newPath.size();
				wchar_t lastChar = newPath[pathSize - 1];
				if (lastChar != '\\')
					newPath += '\\';

				path = newPath;
			}


			void reportExecution (Arguments & args) const
			{
				cout << "Active directory has been successfully changed" << endl << endl;
			}

	};
	
	
	
	
	
	
	
	
	
	
	static const map< string, const shared_ptr<Command> > commandDictionary = { { "cr", make_shared<CommandCreate>()          },
																				{ "rn", make_shared<CommandRename>()          },
																				{ "sl", make_shared<CommandSolve>()           },
																				{ "ss", make_shared<CommandShowStructure>()   },
	
																				{ "ci", make_shared<CommandCreateInput>()     },
																				{ "cc", make_shared<CommandCreateConverter>() },
																				{ "cl", make_shared<CommandCreateLoad>()      },
																				{ "mi", make_shared<CommandModifyInput>()     },
																				{ "mc", make_shared<CommandModifyConverter>() },
																				{ "ml", make_shared<CommandModifyLoad>()      },
																				{ "ms", make_shared<CommandMoveSink>()        },
																				{ "ds", make_shared<CommandDisconnectSink>()  },
																				{ "dn", make_shared<CommandDeleteNode>()      },
	
																			    { "sv", make_shared<CommandSave>()            },
																				{ "ld", make_shared<CommandLoad>()            },
																				{ "cd", make_shared<CommandChangeDirectory>() }  };
	
	
	
	
	
	
	
	


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

}











#pragma todo there follows workarounds for testing; they should be removed
extern void readTreeFromFile (string name, string path, shared_ptr<ElectricNet_If> destination)
{
	using namespace commands;



	TokensDeque tokens = { name, path };
	CommandLoad cl;
	cl.execute(tokens);
	destination = activePowerTree;
}


extern void writeTreeToFile (string name, string path, shared_ptr<ElectricNet_If> source)
{
	using namespace commands;



	activePowerTree = source;
	TokensDeque tokens = { name, path };
	CommandSave sv;
	sv.execute(tokens);
}
