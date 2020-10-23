
#pragma once


#include "config.h"




namespace commands
{

	enum class MotionMode { WITH_DESCES, FREE_DESCES, RECONNECT_DESCES };

	inline bool isMotionModeString (const string & str)
	{
		if (str != "h" && str != "d" && str != "r")    return false;
		return true;
	}



	using command_mnemonic = const string;
	
	
	
	
	bool isCommandToQuit (string );
	void executeCommand (string );

}
