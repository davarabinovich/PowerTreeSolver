
#pragma once


#include "config.h"




namespace commands
{

	enum class MotionMode { FREE_DESCES, WITH_DESCES, RECONNECT_DESCES };

	inline bool isMotionModeString (const string & str)
	{
		if (str != "h" && str != "d" && str != "r")    return false;
		return true;
	}

	inline MotionMode parseMotionMode (const string & str)
	{
		if (!isMotionModeString(str))
			throw exception(  string("\"" + str + "\" is not a mode of deleting").c_str()  );

		if (str == "d")
			return MotionMode::WITH_DESCES;
		if (str == "h")
			return MotionMode::FREE_DESCES;
		return MotionMode::RECONNECT_DESCES;
	}



	using command_mnemonic = const string;
	
	
	
	
	bool isCommandToQuit (string );
	void executeCommand (string );

}
