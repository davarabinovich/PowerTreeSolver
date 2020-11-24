
#pragma once


#include <iostream>
#include <string>
#include <ctype.h>



#pragma todo remove
using namespace std;




#define AUTO_CONST_REF auto const &



#define CUSTOM_EXCEPTION(ex, baseEx)    class ex : public baseEx                                      \
									    {                                                             \
									        public:    ex (const char * str)    : exception(str) {;}  \
									    }                                                             \

#define STRING_MESSAGE_EXCEPTION(ex, baseEx)    class ex : public baseEx                                                          \
										        {                                                                                 \
										            public:    ex (string str)    : baseEx( (" ## ex ## : " + str).c_str() ) {;}  \
										        }                                                                                 \




inline void ensureIsNameNotEmpty (string name, string itemType_str = "")
{
	if (name != "")    return;

	std::string message;
	if (itemType_str == "")
		message = std::string("Name can't be empry");
	else
		message = std::string("Name of " + itemType_str + " can't be empty");

	throw std::exception(message.c_str());
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

	bool isStrNotZero = ( (str != "0.0") && (str != "0") );
	if ( (number == 0.0) && isStrNotZero )
		throw exception("There is no floating-point number");
	return number;
}


inline bool isFloatNumberString (const string & str)
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


inline string capitalize (string str, bool needsCapitalize = true)
{
	string result = str;
	if (needsCapitalize)
		result[0] = (char)std::toupper(result[0]);

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
