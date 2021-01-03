
#include <sstream>
#include <cctype>
#include <cmath>

#include <deque>
#include <set>
#include <map>
#include <memory>
#include <optional>


#include "lib/ciflib.h"


#include "electric_net/electric_net.h"


#include "command_processor/command_processor.h"







bool CommandProcessor::isThereSomeActiveTree () const
{
	bool result = !(activePowerTree == nullptr);
	return result;
}
	
	
	
	
	

	







	
	
	
	
	
	
	


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
	
	
	
	
	
	
	
	


	TokensDeque tokenize (const string & commandStr)
	{
		TokensDeque tokens;

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
	TokensDeque tokens = { name, path };
	CommandLoad cl;
	cl(tokens);
	shared_ptr<ElectricNet> destination = dynamic_pointer_cast<ElectricNet>(activePowerTree);
	return destination;
}


extern void writeTreeToFile (string name, string path, shared_ptr<electric_net::ElectricNet> source)
{
	



	activePowerTree = source;
	TokensDeque tokens = { name, path };
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
