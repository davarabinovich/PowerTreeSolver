
#pragma once


#include <iostream>
#include <string>
#include <ctype.h>




using std::cout;
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


inline void shiftSpaces (unsigned spaces_qty)
{
	for (; spaces_qty != 0; spaces_qty--)
		cout << " ";
}


inline void scrollInteratorToNewWord_unsafe(string::const_iterator & char_it)
{
	while (*char_it == ' ') char_it++;
}


inline double strToDouble (string str)
{
	double number = atof(str.c_str());
	if ( (number == 0.0) && (str != "0.0") )
		throw exception("There is no floating-point number");
	return number;
}


inline string capitalize (string str, bool needsCapitalize = true)
{
	string result = str;
	if (needsCapitalize)
		str[0] = std::toupper(str[0]);

	return result;
}


inline bool isYes (string str)
{
	if ( (str == "y") || (str == "Y") || (str == "yes") || (str == "Yes") )
		return true;
	return false;
}


inline bool isNo (string str)
{
	if ((str == "n") || (str == "N") || (str == "no") || (str == "No"))
		return true;
	return false;
}


inline bool isBool_str (string str)
{
	if (isYes(str) || isNo(str))
		return true;
	return false;
}
