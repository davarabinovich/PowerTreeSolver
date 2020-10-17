
#pragma once


#include <string>
#include <map>
#include <sstream>


#include "lib/ciflib.h"





using namespace std;




namespace electirc_net
{
#pragma todo these must be not here
enum class CvType { VOLTAGE, CURRENT };

inline ostream & operator << (ostream & os, const CvType & type)
{
	switch (type)
	{
	case CvType::VOLTAGE:
		return os << "voltage";
	case CvType::CURRENT:
		return os << "current";

	default:
		throw exception("Invalid type of controlled variable");
	}
}

inline const string operator + (const CvType & tp, const string & str)
{
	if (tp == CvType::VOLTAGE)
		return ("voltage" + str);
	return ("current" + str);
}

inline const string operator + (const string & str, const CvType & tp)
{
	if (tp == CvType::VOLTAGE)
		return (str + "voltage");
	return (str + "current");
}


#pragma todo add diode load
enum class LoadType { RESISTIVE, CONSTANT_CURRENT, ENERGY };

inline ostream & operator << (ostream & os, const LoadType & type)
{
	switch (type)
	{
	case LoadType::RESISTIVE:
		return os << "resistive";
	case LoadType::CONSTANT_CURRENT:
		return os << "current";
	case LoadType::ENERGY:
		return os << "power";

	default:
		throw exception("Invalid type of load");
	}
}

inline const string operator + (const LoadType & tp, const string & str)
{
	if (tp == LoadType::RESISTIVE)
		return ("resistive" + str);
	if (tp == LoadType::CONSTANT_CURRENT)
		return ("current" + str);
	return ("power" + str);
}

inline const string operator + (const string & str, const LoadType & tp)
{
	if (tp == LoadType::RESISTIVE)
		return (str + "resistive");
	if (tp == LoadType::CONSTANT_CURRENT)
		return (str + "current");
	return (str + "power");
}



enum class ConverterType { PULSE, LINEAR };

inline ostream & operator << (ostream & os, const ConverterType & type)
{
	switch (type)
	{
	case ConverterType::PULSE:
		return os << "pulse";
	case ConverterType::LINEAR:
		return os << "linear";

	default:
		throw exception("Invalid type of converter");
	}
}



enum class DeletingMode { WITH_DESCES, HANG_DESCES, RECONNECT_DESCES, NONE };





class PowerTree
{

};

}
