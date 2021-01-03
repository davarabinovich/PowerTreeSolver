
#pragma once


#include "config.h"




namespace command_processor
{

	using command_mnemonic = const string;




	enum class MotionMode { FREE_DESCES, WITH_DESCES, RECONNECT_DESCES };

	inline bool isMotionModeString (const string & str)
	{
		if (str != "f" && str != "w" && str != "r")    return false;
		return true;
	}

	inline MotionMode parseMotionMode (const string & str)
	{
		if (!isMotionModeString(str))
			throw exception(  string("\"" + str + "\" is not a mode of deleting").c_str()  );

		if (str == "w")
			return MotionMode::WITH_DESCES;
		if (str == "f")
			return MotionMode::FREE_DESCES;
		return MotionMode::RECONNECT_DESCES;
	}

	
	
	
	void executeCommand (string );

}
