
#include <sstream>
#include <cctype>
#include <cmath>

#include <deque>
#include <set>
#include <map>
#include <memory>
#include <optional>


#include "lib/ciflib.h"


#include "electric_net/electric_net_if.h"
#include "electric_net/electric_net.h"
#include "file_server/file_server.h"


#include "command_processor/command_processor.h"





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
	
	
	


	using Token = string;
	using TokensCollection = deque<Token>;

	




	class Command
	{

		public:

			virtual void operator () (TokensCollection & tokens) const;




		protected:

			enum class MessageMode { STRAIGHT, REVERSE };

			
			struct UserAnswer
			{
				UserAnswer();
				UserAnswer (string content);

				static UserAnswer genInvalidAnswer();

				bool isValid;
				string content;
			};


			struct Args 
			{
				void * args;
			};

			struct IntermediateData
			{

			};

			struct ValidationData 
			{
				ValidationData() {};
				ValidationData(bool isValid);

				ValidationData& operator = (ValidationData example);

				bool isValid = false;
			};

			struct ExecutionData
			{
				ExecutionData() {};
				ExecutionData(bool isSuccess);

				ExecutionData& operator = (ExecutionData example);

				bool isSuccess = false;
			};




			virtual void checkContext() const {}
			virtual Args parseArgs(TokensCollection& tokens) const;
			virtual void complementArgs(Args args) const {};
			virtual const ValidationData isArgsValid(Args args) const;

			virtual const IntermediateData getIntermediateData() const;
			virtual const ExecutionData execute(const Args args) const { return ExecutionData(true); };
			virtual void reportExecution(const Args args) const {};
			virtual void reportError(const ValidationData validData) const {};
			virtual void reportError(const ExecutionData validData) const {};


			void reportTooManyArgs () const;


			UserAnswer suggestEnterParamAndGetStr (string message = "", MessageMode mode = MessageMode::STRAIGHT) const;
			UserAnswer requestParamAndGet(string message = "") const;




			virtual ~Command () {;}

	};
	
	

	
	
	class CommandWorkingWithExsistingTree : public Command
	{
		protected:
			virtual void checkContext () const override;
			void ensureIfThereAreSomeTree () const;

			virtual ~CommandWorkingWithExsistingTree () {;}
	};





	class CommandSave : public CommandWorkingWithExsistingTree
	{

		public:

			virtual void operator () (TokensCollection & tokens) const override
			{
				ensureIfThereAreSomeTree();


				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception& ex) { throw exception(ex.what()); }

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



			Arguments parseArguments (TokensCollection & tokens) const
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


			void updateSystemVariables(Arguments args) const
			{
				if (!args.fileName.empty())
					fileName = args.fileName;

				if (!args.path.empty())
					path = args.path;
			}


			void recordPowerTree(Arguments args) const
			{
				string treeTitle = activePowerTree->getTitle();

				auto [newFileName, newPath] = args;

				if (args.fileName.empty())
					args.fileName = fileName;

				if (args.path.empty())
					args.path = path;

				FileWriter fileWriter(treeTitle, fileName, path);
				WriteNode writeNode(fileWriter);

				activePowerTree->iterateAndExecuteForEach(writeNode);
			}

			FUNCTOR(WriteNode, void, Key nodeName)
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
			FUNCTOR_END_BODY
				WriteNode(FileWriter& genWfstream)
				: wfstream(genWfstream) {
				;
			}

		private:
			FileWriter& wfstream;
			END_FUNCTOR

				void reportExecution(Arguments args) const
			{
				cout << "Power tree \"" << activePowerTree->getTitle()
					<< "\" is been successfully saved to the file \"" << args.fileName << "\"" << endl << endl;
			}


			static void writeLoad(Key loadName, FileWriter& wfstream)
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





	class CommandCreate : public Command
	{

		private:

			static const string name_suggesting_message;




			struct Args 
			{
				string name;
				string firstInputName;
				VarKind firstInputCvKind = VarKind::VOLTAGE;
				double firstInputCvValue = 24;
			}; 
			
			
			

			virtual void checkContext() const override;
			virtual Command::Args parseArgs(TokensCollection& tokens) const override;
			virtual void complementArgs(Command::Args args) const override;

			virtual const ExecutionData execute(const Command::Args args) const override;
			virtual void reportExecution(const Command::Args args) const override;


			bool suggestSaveActiveTree() const;
			void createTreeByArgs(const Args& args) const;
			void createInputByArgs(const Args& args) const;

};





	class CommandRename : public CommandWorkingWithExsistingTree
	{
	
		public:
		
			virtual void operator () (TokensCollection & tokens) const override
			{
				ensureIfThereAreSomeTree();
	
				if (tokens.size() > 1)    throw exception("Too many arguments for this command");
	
				string newName;
				if (tokens.empty())
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
			};
	
	
	
			string requestAndGetNewName () const
			{
				cout << "Enter a new name for this power tree: ";
				Token newName;
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



			virtual ~CommandWithShowingStrucute() {;}

	};





	class CommandSolve : public CommandWithShowingStrucute
	{
	
		public:
		
			virtual void operator () (TokensCollection & tokens) const override
			{
				ensureIfThereAreSomeTree();

				activePowerTree->calculte();
				displayResults();
			}
		
		
		
		
		private:

			void displayResults () const
			{
				displayHeader();
				activePowerTree->iterateAndExecuteForEach(displayResultsForElectricNode);
				cout << endl;
			}

			static void displayResultsForElectricNode (Key nodeName)
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

			static void displayInputResults (InputResults results)
			{
				auto [name, cvKind, value, avValue] = results;

				string output = to_string(value) + getVarKindDesignatorStr(cvKind) + " source \"" + name + "\":\n";
				output += ("   gives " + to_string(avValue) + getVarKindDesignatorStr(!cvKind));

				cout << output << endl << endl;
			}

			static void displayConverterResults (ConverterResults results)
			{
				auto [name, nestingLevel, cvKind, value, type, avValue, inputValue] = results;
				auto shift = generateShift(nestingLevel);

				string output = shift + to_string(value) + getVarKindDesignatorStr(cvKind) + " " + toStr(type) + " dc/dc \"" + name + "\":\n";
				output += (shift + "   gives " + to_string(avValue) + getVarKindDesignatorStr(!cvKind) + "\n");
				output += (shift + "   consumes " + to_string(inputValue) + "A");

				cout << output << endl << endl;
			}

			static void displayLoadResults (Key loadName)
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
				auto [name, nestingLevel, resistance, inputValue, inputVarKind] = results;
				auto shift = generateShift(nestingLevel);
				
				string output = shift + "Load \"" + name + "\" " + to_string(resistance) + "Ohm:" + "\n";
				
				output += shift;
				if (inputVarKind == VarKind::VOLTAGE)
					output += "   consumes ";
				else
					output += "   works by ";
				output += (to_string(inputValue) + getVarKindDesignatorStr(!inputVarKind));

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
		
			virtual void operator () (TokensCollection & tokens) const override
			{
				ensureIfThereAreSomeTree();
	
				displayTreeStructure();
			}
		
		
		
		
		private:

			void displayTreeStructure () const
			{
				displayHeader();
				activePowerTree->iterateAndExecuteForEach(displayElectricNode);
				cout << endl;
			}

			static void displayElectricNode (Key nodeName)
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

			static void displayInput (InputData data)
			{
				auto [name, type, value] = data;
				string output = to_string(value) + getVarKindDesignatorStr(type) + " source \"" + name + "\":   ";
				cout << output << endl;
			}

			static void displayConverter (ConverterData data)
			{
				auto [name, nestingLevel, cvType, value, type, efficiencyParam] = data;
				auto shift = string((nestingLevel-1) * spaces_per_level_shift, ' ');
				
				string output = shift + to_string(value) + getVarKindDesignatorStr(cvType) + " " + toStr(type) + " dc/dc \"" + name + "\"";
				if (type == ConverterType::PULSE)
					output += " (eff. " + to_string(efficiencyParam) + "%):";
				else
					output += " (own cons. " + to_string(1000*efficiencyParam) + "mA):";
				cout << output << endl;
			}

			static void displayLoad (Key loadName)
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
				auto shift = string((nestingLevel - 1) * spaces_per_level_shift, ' ');
				
				string output = shift + "Load \"" + name + "\" " + to_string(resistance) + getMainUnitDesignatorStr(LoadType::RESISTIVE);
				cout << output << endl;
			}

			static void displayConstantCurrentLoad (ConstantCurrentLoadData data)
			{
				auto [name, nestingLevel, current] = data;
				auto shift = string((nestingLevel - 1) * spaces_per_level_shift, ' ');
				
				string output = shift + "Load \"" + name + "\" " + to_string(current) + getMainUnitDesignatorStr(LoadType::CONSTANT_CURRENT);
				cout << output << endl;
			}

			static void displayDiodeLoad (DiodeLoadData data)
			{
				auto [name, nestingLevel, forwardVoltage, forwardCurrent] = data;
				auto shift = string((nestingLevel - 1) * spaces_per_level_shift, ' ');
				
				string output = shift + "Load \"" + name + "\" " + to_string(forwardVoltage) + getMainUnitDesignatorStr(LoadType::DIODE)
					                  + " (forw. cur. " + to_string(forwardCurrent) + getAddUnitDesignatorStr(LoadType::DIODE) + ")";
				cout << output << endl;
			}
		
	};





	class CommandCreateInput : public CommandWorkingWithExsistingTree
	{
		
		public:
		
			virtual void operator () (TokensCollection & tokens) const override
			{
				ensureIfThereAreSomeTree();
	
	
				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception & ex) { throw exception(ex.what()); }
		
				if (args.name.empty())
					args.name = suggestEnterNameAndGet();
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
		
		
		
			Arguments parseArguments (TokensCollection & tokens) const
			{
				Arguments args;
	
				if (tokens.empty())    return args;
	
	
				auto handeledArg = tokens.front();
				
				if ( !isVarKindString(handeledArg) && !isFloatNumberString(handeledArg) )
				{
					args.name = handeledArg;
	
					tokens.pop_front();
					if (tokens.empty())    return args;
					handeledArg = tokens.front();
				}
	
				if (isVarKindString(handeledArg))
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
	
	
		
	
			string suggestEnterNameAndGet () const
			{
				string name = "";
				cout << "Do you want to set a name for the new input?" << endl;
				string answer; getline(cin, answer);

				if (answer == "yes" || answer == "Yes" || answer == "y" || answer == "Y")
				{
					getline(cin, name);
					return name;
				}
				else if (answer != "no" && answer != "No" && answer != "n" && answer != "N")
					throw exception("Invalid answer");

				name = activePowerTree->getDefaultNodeName(DeviceType::INPUT);
				return name;
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
		
		private:
		
			static const string name_suggesting_message;
			static const string parent_suggesting_message;

			static const string cv_value_requiresting_message_template;




			struct Args
			{
				string name;

				VarKind cvKind = VarKind::VOLTAGE;
				double cvValue = NAN;
				ConverterType type = ConverterType::PULSE;
				double efficiencyParam = 100.0;

				string parentName;
			};
		
		


			virtual Command::Args parseArgs(TokensCollection& tokens) const override;
			virtual void complementArgs(Command::Args args) const override;

			virtual const Command::ExecutionData execute(const Command::Args args) const override;
			virtual void reportExecution(const Command::Args args) const override;


			void createConverterByArgs (const Args & args) const;
		
	};


	
	
	
	class CommandCreateLoad : public CommandWorkingWithExsistingTree
	{
		
		public:
		
			virtual void operator () (TokensCollection & tokens) const override
			{
				ensureIfThereAreSomeTree();
	
	
				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception & ex) { throw exception(ex.what()); }
	
				if (args.name.empty())
					args.name = suggestEnterNameAndGet();
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
		
	
	
			Arguments parseArguments (TokensCollection & tokens) const
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

			string suggestEnterNameAndGet () const
			{
				string name = "";
				cout << "Do you want to set a name for the new tree?" << endl;
				string answer; getline(cin, answer);

				if (answer == "yes" || answer == "Yes" || answer == "y" || answer == "Y")
				{
					getline(cin, name);
					return name;
				}
				else if (answer != "no" && answer != "No" && answer != "n" && answer != "N")
					throw exception("Invalid answer");

				name = activePowerTree->getDefaultNodeName(DeviceType::LOAD);
				return name;
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
		
			virtual void operator () (TokensCollection & tokens) const override
			{
				ensureIfThereAreSomeTree();
	
	
				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception & ex) { throw exception(ex.what()); }
	
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
		
		
		
			Arguments parseArguments (TokensCollection & tokens) const
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
						string Key = extractKeyFromToken(token);
						if (Key == "n")
						{
							if (args.newName)    
								continue;
	
							args.newName = extractParamFromToken(token);
						}
						else if (Key == "u")
						{
							if (args.cvType)    
								continue;
	
							args.cvType = parseVarKind(extractParamFromToken(token));
						}
						else if (Key == "v")
						{
							if (args.cvValue)    
								continue;
	
							args.cvValue = strToDouble(extractParamFromToken(token));
						}
						else
							throw exception(  string("Unrecognized parameter \"" + Key).c_str()  );
					}
					else
					{
						if (isVarKindString(token))
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
					throw exception("This is not a parameter with a Key");
	
				const auto charEqual_it = find(token.begin(), token.end(), '=');
				string result = string(token.begin(), charEqual_it);
				return result;
			}
	
			string extractParamFromToken (const string & token) const
			{
				if (!isParamWithKey(token))
					throw exception("This is not a parameter with a Key");
	
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
		
			virtual void operator () (TokensCollection & tokens) const override
			{
				ensureIfThereAreSomeTree();
	
	
				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception & ex) { throw exception(ex.what()); }
		
	
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
				optional<double> efficiencyParam;
			};
		
		
		
			Arguments parseArguments (TokensCollection & tokens) const
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
						Token key = extractKeyFromToken(token);
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
						else if (key == "e" || key == "c")
						{
							if (args.efficiencyParam)    continue;
	
							args.efficiencyParam = strToDouble(extractParamFromToken(token));
						}
						else
							throw exception(string("Unrecognized parameter \"" + key).c_str());
					}
					else
					{
						if (isVarKindString(token))
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
							if (args.efficiencyParam)
							{
								args.efficiencyParam = strToDouble(token);
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
					throw exception("This is not a parameter with a Key");
		
				const auto charEqual_it = find(token.begin(), token.end(), '=');
				string result = string(token.begin(), charEqual_it);
				return result;
			}
		
			string extractParamFromToken (const string & token) const
			{
				if (!isParamWithKey(token))
					throw exception("This is not a parameter with a Key");
		
				const auto charEqual_it = find(token.begin(), token.end(), '=');
				string result = string(charEqual_it + 1, token.end());
				return result;
			}
		
		
			void modifyConverterParams (const Arguments & args) const
			{
				if (args.cvType)
					activePowerTree->setSourceCvType(args.currentName, *args.cvType);
				if (args.cvValue)
					activePowerTree->setSourceCvValue(args.currentName, *args.cvValue);
				if (args.type)
					activePowerTree->setConverterType(args.currentName, *args.type);
				if (args.efficiencyParam)
					activePowerTree->setConverterEfficiencyParam(args.currentName, *args.efficiencyParam);
	
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
				if (args.efficiencyParam)
				{
					cout << endl << "    Efficiency - " << *args.efficiencyParam << " %";
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
		
			virtual void operator () (TokensCollection & tokens) const override
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
		
		
		
			Arguments parseArguments (TokensCollection & tokens) const
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
						Token Key = extractKeyFromToken(token);
						if (Key == "n")
						{
							if (args.newName)    continue;
	
							args.newName = extractParamFromToken(token);
						}
						else if (Key == "t")
						{
							if (args.type)    continue;
	
							args.type = parseLoadType(extractParamFromToken(token));
						}
						else if (Key == "v")
						{
							if (args.value)    continue;
	
							args.value = strToDouble(extractParamFromToken(token));
						}
						else if (Key == "a")
						{
							if (args.addValue)    continue;
	
							args.addValue = strToDouble(extractParamFromToken(token));
						}
						else
							throw exception(string("Unrecognized parameter \"" + Key).c_str());
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
					throw exception("This is not a parameter with a Key");
		
				const auto charEqual_it = find(token.begin(), token.end(), '=');
				string result = string(token.begin(), charEqual_it);
				return result;
			}
		
			string extractParamFromToken (const string & token) const
			{
				if (!isParamWithKey(token))
					throw exception("This is not a parameter with a Key");
		
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
		
			virtual void operator () (TokensCollection & tokens) const override
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
		
		
		
			Arguments parseArguments (TokensCollection & tokens) const
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
							throw exception("Invalid mode of deleting a source");
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
		
			virtual void operator () (TokensCollection & tokens) const override
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



			Arguments parseArguments (TokensCollection & tokens) const
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
							throw exception("Invalid mode of deleting a source");
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
	
		virtual void operator () (TokensCollection & tokens) const override
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



		Arguments parseArguments (TokensCollection & tokens) const
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
					throw exception("Invalid mode of deleting a source");
			}
		}
	
};





	class CommandLoad : public Command
	{

		public:

			virtual void operator () (TokensCollection & tokens) const override
			{
				if (isThereSomeTree())
				{
					bool needToSaveActiveTree = suggestSaveActiveTree();
					if (needToSaveActiveTree)
					{
						CommandSave commandSave;
						TokensCollection savingTokens;
						commandSave(savingTokens);
					}
				}


				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception & ex) { throw exception(ex.what()); }

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



			virtual void checkContext() const override;

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

			Arguments parseArguments (TokensCollection & tokens) const
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
					activePowerTree->addConverter(node.name, data.type, data.cvKind, data.value, data.efficiencyParam);
				else
					activePowerTree->addConverter(node.name, data.commonSinkData.parentName, data.type, data.cvKind, data.value, 
					                                         data.efficiencyParam);
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

			virtual void operator () (TokensCollection & tokens) const override
			{
				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception & ex) { throw exception(ex.what()); }

				setPath(args);
				reportExecution(args);
			}




		private:

			struct Arguments
			{
				string path;
			};




			virtual void checkContext() const override;

			Arguments parseArguments (TokensCollection & tokens) const
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





	class NodeCopyingCommand : public CommandWorkingWithExsistingTree
	{
		
		protected:
		
			struct Arguments
			{
				string exampleName = "";
				string newNodeName = "";
				string parentName = "";
			};
		
		
		
			void copyNode (const Arguments & args) const
			{
				auto type = activePowerTree->getNodeType(args.exampleName);
		
				switch (type)
				{
					case DeviceType::INPUT:
					{
						auto params = activePowerTree->getInputData(args.exampleName);
						activePowerTree->addInput(args.newNodeName, params.cvKind, params.value);
						break;
					}
		
					case DeviceType::CONVERTER:
					{
						auto params = activePowerTree->getConverterData(args.exampleName);
						if (args.parentName.empty())
							activePowerTree->addConverter(args.newNodeName, params.type, params.cvKind, params.value, params.efficiency);
						else
							activePowerTree->addConverter(args.newNodeName, args.parentName, params.type, params.cvKind, params.value, params.efficiency);
						break;
					}
		
					case DeviceType::LOAD:
					{
						auto loadType = activePowerTree->getLoadType(args.exampleName);
		
						switch (loadType)
						{
							case LoadType::RESISTIVE:
							{
								auto params = activePowerTree->getResistiveLoadData(args.exampleName);
								if (args.parentName.empty())
									activePowerTree->addLoad(args.newNodeName, LoadType::RESISTIVE, params.resistance);
								else
									activePowerTree->addLoad(args.newNodeName, args.parentName, LoadType::RESISTIVE, params.resistance);
		
								break;
							}
		
							case LoadType::CONSTANT_CURRENT:
							{
								auto params = activePowerTree->getConstantCurrentLoadData(args.exampleName);
								if (args.parentName.empty())
									activePowerTree->addLoad(args.newNodeName, LoadType::CONSTANT_CURRENT, params.current);
								else
									activePowerTree->addLoad(args.newNodeName, args.parentName, LoadType::CONSTANT_CURRENT, params.current);
		
								break;
							}
		
							case LoadType::DIODE:
							{
								auto params = activePowerTree->getDiodeLoadData(args.exampleName);
								if (args.parentName.empty())
									activePowerTree->addLoad(args.newNodeName, LoadType::DIODE, params.forwardVoltage, params.forwardCurrent);
								else
									activePowerTree->addLoad(args.newNodeName, args.parentName, LoadType::DIODE, params.forwardVoltage, params.forwardCurrent);
		
								break;
							}
		
							default:
								throw exception("Invalid type of load");
						}
		
						break;
					}
		
					default:
						throw exception("Invalid type of device");
				}
		
			}



			virtual ~NodeCopyingCommand () {;}
		
	};





	class CommandCopyNode : public NodeCopyingCommand
	{

		public:

			virtual void operator () (TokensCollection & tokens) const override
			{
				ensureIfThereAreSomeTree();


				Arguments args;
				try { args = parseArguments(tokens); }
				catch (exception & ex) { throw exception(ex.what()); }

				if (args.exampleName == "")
					args.exampleName = requestExampleNameAndGet();
				if (args.newNodeName == "")
					args.newNodeName = requestNewNodeNameAndGet();

				if (args.parentName == "")
					args.parentName = suggestSpecifyParentAndGet();

				copyNode(args);

				reportExecution(args);
			}




		private:

			Arguments parseArguments (TokensCollection & tokens) const
			{
				Arguments args;
				Token handeledArg;
				
				if (tokens.empty())    return args;
				handeledArg = tokens.front();
				args.exampleName = handeledArg;
				tokens.pop_front();

				if (tokens.empty())    return args;
				handeledArg = tokens.front();
				args.newNodeName = handeledArg;
				tokens.pop_front();

				if (tokens.empty())    return args;
				handeledArg = tokens.front();
				args.parentName = handeledArg;
				tokens.pop_front();

				if (tokens.empty())    return args;
				throw exception("Too many arguments for this command");
			}

			string requestExampleNameAndGet () const
			{
				cout << "Please enter name of example to been copied" << endl;
				string name; getline(cin, name);
				return name;
			}

			string requestNewNodeNameAndGet () const
			{
				cout << "Please enter name of new node" << endl;
				string name; getline(cin, name);
				return name;
			}

			string suggestSpecifyParentAndGet () const
			{
				string parentName = "";
				cout << "Do you want to leave the new node unconnected?" << endl;
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

			void reportExecution (const Arguments & args) const
			{
				auto type = activePowerTree->getNodeType(args.exampleName);
				cout << "The " << type << " \"" << args.newNodeName << "\" is copied from the one \"" << args.exampleName << "\"";

				if (type != DeviceType::INPUT)
				{
					cout << " and";

					if (args.parentName.empty())
						cout << "has been left free";
					else
						cout << " connected to the " << activePowerTree->getNodeType(args.parentName) << " \"" << args.parentName << "\"" 
						     << endl << endl;
				}
			}

	};

	
	
	
	
	
	
	


	static const map<string, const shared_ptr<Command>> commandDictionary = { { "cr", make_shared<CommandCreate>()          },
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
																			  { "cn", make_shared<CommandCopyNode>()        },
																			  
																			  { "sv", make_shared<CommandSave>()            },
																			  { "ld", make_shared<CommandLoad>()            },
																			  { "cd", make_shared<CommandChangeDirectory>() }  };
	
	
	
	
	
	
	
	


	TokensCollection tokenize (const string & commandStr)
	{
		TokensCollection tokens;

		bool isActuallyWordRead = false;
		Token actualWord;
		for (const auto actualChar : commandStr)
		{
			if (isActuallyWordRead)
			{
				if (actualChar != ' ')
					tokens.back() += actualChar;
				else
					isActuallyWordRead = false;
			}
			else
			{
				if (actualChar != ' ')
				{
					isActuallyWordRead = true;
					tokens.push_back(Token(1, actualChar));
				}
			}
		}

		return tokens;
	}
	
	void executeCommand (string enteredCommand)
	{
		auto tokens = tokenize(enteredCommand);
		string commandMnemonic = tokens.front();
		tokens.pop_front();
	
		shared_ptr<Command> currentCommand;
		try { currentCommand = commandDictionary.at(commandMnemonic); }
		catch (out_of_range) { throw exception("Unrecognized command"); }
	
		(*currentCommand)(tokens);
	}











#ifdef DEBUG
extern shared_ptr<electric_net::ElectricNet> readTreeFromFile (string name, string path)
{
	TokensCollection tokens = { name, path };
	CommandLoad cl;
	cl(tokens);
	shared_ptr<ElectricNet> destination = dynamic_pointer_cast<ElectricNet>(activePowerTree);
	return destination;
}


extern void writeTreeToFile (string name, string path, shared_ptr<electric_net::ElectricNet> source)
{
	activePowerTree = source;
	TokensCollection tokens = { name, path };
	CommandSave sv;
	sv(tokens);
}


extern void resetTree ()
{
	activePowerTree.reset();
	fileName = "";
	path = "";
}
#endif








Command::UserAnswer::UserAnswer ()
	: isValid(true), content("")    {;}


Command::UserAnswer::UserAnswer (string genContent)
	: content(genContent)    {;}



Command::UserAnswer Command::UserAnswer::genInvalidAnswer ()
{
	UserAnswer invalidAnswer("");
	invalidAnswer.isValid = false;
	return invalidAnswer;
}










Command::ValidationData::ValidationData (bool genIsValid)
	: isValid(genIsValid)    {;}




Command::ValidationData & Command::ValidationData::operator = (ValidationData example)
{
	isValid = example.isValid;
	return *this;
}










Command::ExecutionData::ExecutionData (bool genIsSuccess)
	: isSuccess(genIsSuccess) {;}




Command::ExecutionData & Command::ExecutionData::operator = (ExecutionData example)
{
	isSuccess = example.isSuccess;
	return *this;
}










void Command::operator () (TokensCollection & tokens) const
{
	checkContext();


	auto args = parseArgs(tokens);
	if (!tokens.empty())
	{
		reportTooManyArgs();
		return;
	}

	complementArgs(args);
	const auto validData = isArgsValid(args);
	if (!validData.isValid)
	{
		reportError(validData);
		return;
	}
	
	const auto IntermediateData = getIntermediateData();
	const auto executionData = execute(args);
	if (!executionData.isSuccess)
	{
		reportError(executionData);
		return;
	}

	reportExecution(args);
}




Command::Args Command::parseArgs (TokensCollection & tokens) const
{
	return Args();
}


const Command::ValidationData Command::isArgsValid (Args args) const
{
	return ValidationData(true);
}


const Command::IntermediateData Command::getIntermediateData () const
{
	return IntermediateData();
}


void Command::reportTooManyArgs() const
{
	cout << "Too many arguments for this command";
}


Command::UserAnswer Command::suggestEnterParamAndGetStr (string message, MessageMode mode) const
{
	string param_str = "";
	cout << message << endl;
	string answer; getline(cin, answer);

	if (mode == MessageMode::STRAIGHT)
	{
		if (isNo(answer))
			return UserAnswer("");

		if (!isYes(answer))
			return UserAnswer::genInvalidAnswer();
	}
	else
	{
		if (isYes(answer))
			return UserAnswer("");

		if (!isNo(answer))
			return UserAnswer::genInvalidAnswer();
	}

	getline(cin, param_str);
	return param_str;
}


Command::UserAnswer Command::requestParamAndGet (string message) const
{
	cout << message << endl;
	string answer; getline(cin, answer);
	return answer;
}




const string CommandCreate::name_suggesting_message = "Do you want to set a name for the new tree?";




void CommandCreate::checkContext () const
{
	if (isThereSomeTree())
	{
		bool needToSaveActiveTree = suggestSaveActiveTree();
		if (needToSaveActiveTree)
		{
			CommandSave commandSave;
			TokensCollection savingTokens;
			commandSave(savingTokens);
		}
	}
}


Command::Args CommandCreate::parseArgs (TokensCollection & tokens) const
{
	static Args args;


	auto kind_it = find_if(tokens.begin(), tokens.end(), isVarKindString);
	if (kind_it != tokens.end())
	{
		args.firstInputCvKind = parseVarKind(*kind_it);
		tokens.erase(kind_it);
	}
	
	auto value_it = find_if(tokens.begin(), tokens.end(), isFloatNumberString);
	if (value_it != tokens.end())
	{
		args.firstInputCvValue = strToDouble(*value_it);
		tokens.erase(value_it);
	}

	if (tokens.empty())
		return { &args };
	args.name = tokens.front();
	tokens.pop_front();

	if (tokens.empty())
		return { &args };
	args.firstInputName = tokens.front();
	tokens.pop_front();

	return { &args };
}


void CommandCreate::complementArgs (Command::Args rawArgs) const
{
	auto & args = *( reinterpret_cast<Args*>(rawArgs.args) );


	if (args.name.empty())
	{
		auto answer = suggestEnterParamAndGetStr(name_suggesting_message);
		if (answer.isValid && !answer.content.empty())
			args.name = answer.content;
		else
			args.name = default_tree_name;
	}

	if (args.firstInputName.empty())
		args.firstInputName = default_frist_input_name;
}


const Command::ExecutionData CommandCreate::execute (const Command::Args rawArgs) const
{
	AUTO_CONST_REF args = *( reinterpret_cast<Args*>(rawArgs.args) );


	createTreeByArgs(args);
	fileName = args.name;

	createInputByArgs(args);

	
	return ExecutionData(true);
}


void CommandCreate::reportExecution (const Command::Args rawArgs) const
{
	AUTO_CONST_REF args = *( reinterpret_cast<Args*>(rawArgs.args) );


	auto [name, firstInputName, firstInputCvKind, firstInputCvValue] = args;

	cout << "A new power three \"" << name << "\" with a " << firstInputCvKind << " input \"" << firstInputName << "\"" << " "
		 << firstInputCvValue << " " << getVarKindDesignatorStr(firstInputCvKind) << " has been created";
	cout << endl << endl;
}


bool CommandCreate::suggestSaveActiveTree () const
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


void CommandCreate::createTreeByArgs (const Args & args) const
{
	string name = args.name;
	if (name == "")
		name = default_tree_name;

	activePowerTree = make_shared<PowerTree>(name);
}


void CommandCreate::createInputByArgs (const Args & args) const
{
	string name = args.firstInputName;
	if (name == "")
		name = activePowerTree->getDefaultNodeName(DeviceType::INPUT);

	activePowerTree->addInput(name, args.firstInputCvKind, args.firstInputCvValue);
}










void CommandWorkingWithExsistingTree::checkContext () const
{
	ensureIfThereAreSomeTree();
}


void CommandWorkingWithExsistingTree::ensureIfThereAreSomeTree () const
{
	if (isThereSomeTree())	return;
	throw exception("There is no power tree. Create or load a tree");
}










const string CommandCreateConverter::name_suggesting_message = "Do you want to set a name for the new converter?";
const string CommandCreateConverter::parent_suggesting_message = "Do you want to leave the new converter unconnected?";

const string CommandCreateConverter::cv_value_requiresting_message_template = "Please enter a value of ";




Command::Args CommandCreateConverter::parseArgs (TokensCollection & tokens) const
{
	static Args args;


	auto kind_it = find_if(tokens.begin(), tokens.end(), isVarKindString);
	if (kind_it != tokens.end())
	{
		args.cvKind = parseVarKind(*kind_it);
		tokens.erase(kind_it);
	}

	auto type_it = find_if(tokens.begin(), tokens.end(), isConverterTypeString);
	if (type_it != tokens.end())
	{
		args.type = parseConverterType(*type_it);
		tokens.erase(type_it);
	}
	
	auto value_it = find_if(tokens.begin(), tokens.end(), isFloatNumberString);
	if (value_it != tokens.end())
	{
		args.cvValue = strToDouble(*value_it);
		tokens.erase(value_it);
	}

	auto efficiency_it = find_if(tokens.begin(), tokens.end(), isFloatNumberString);
	if (efficiency_it != tokens.end())
	{
		args.efficiencyParam = strToDouble(*efficiency_it);
		tokens.erase(efficiency_it);
	}

	if (tokens.empty())
		return { &args };
	args.name = tokens.front();
	tokens.pop_front();

	if (tokens.empty())
		return { &args };
	args.parentName = tokens.front();
	tokens.pop_front();

	return { &args };
}


void CommandCreateConverter::complementArgs (Command::Args rawArgs) const
{
	auto & args = *( reinterpret_cast<Args*>(rawArgs.args) );


	if (args.name.empty())
	{
		auto answer = suggestEnterParamAndGetStr(name_suggesting_message);
		if (answer.isValid && !answer.content.empty())
			args.name = answer.content;
		else
			args.name = activePowerTree->getDefaultNodeName(DeviceType::CONVERTER);
	}

	if (isnan(args.cvValue))
	{
		auto answer = requestParamAndGet(cv_value_requiresting_message_template + args.cvKind);
		args.cvValue = strToDouble(answer.content);
	}

	if (args.parentName.empty())
	{
		auto answer = suggestEnterParamAndGetStr(parent_suggesting_message, MessageMode::REVERSE);
		if (answer.isValid && !answer.content.empty())
			args.parentName = answer.content;
		else
			args.parentName = default_tree_name;
	}
}


const Command::ExecutionData CommandCreateConverter::execute (const Command::Args rawArgs) const
{
	AUTO_CONST_REF args = *( reinterpret_cast<Args*>(rawArgs.args) );

	createConverterByArgs(args);

	return ExecutionData(true);
}


void CommandCreateConverter::reportExecution (const Command::Args rawArgs) const
{
	AUTO_CONST_REF args = *( reinterpret_cast<Args*>(rawArgs.args) );


	auto [name, cvKind, cvValue, type, efficiencyParam, parentName] = args;
	bool isFree = args.parentName.empty();

	cout << "A new ";
	if (isFree)
		cout << "free ";
	cout << args.type << " converter \"" << name << "\" " << args.cvValue << " " << getVarKindDesignatorStr(args.cvKind) << " is created";
	if (!isFree)
		cout << " at the " << toStr(activePowerTree->getNodeType(args.parentName)) << " \"" << args.parentName << "\"";
	cout << endl << endl;
}




void CommandCreateConverter::createConverterByArgs (const Args & args) const
{
	if (args.parentName == "")
		activePowerTree->addConverter(args.name, args.type, args.cvKind, args.cvValue, args.efficiencyParam);
	else
		activePowerTree->addConverter(args.name, args.parentName, args.type, args.cvKind, args.cvValue, args.efficiencyParam);
}










void CommandLoad::checkContext () const
{

}









void CommandChangeDirectory::checkContext() const
{

}