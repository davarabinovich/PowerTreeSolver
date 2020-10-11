
#pragma once


#include <string>




using std::string;
using std::exception;




#define AUTO_CONST_REF auto const &




inline void ensureIsNameNotEmpty (string name, string itemType_str = "")
{
	if (name != "")    return;

	string message;
	if (itemType_str == "")
		message = string("Name can't be empry");
	else
		message = string("Name of " + itemType_str + " can't be empty");

	throw exception(message.c_str());
}
