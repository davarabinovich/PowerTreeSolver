
#pragma once


#include <sstream>


#include "config.h"




using std::ostream;




namespace electric_net
{

	enum class DeletingMode { WITH_DESCES, HANG_DESCES, RECONNECT_DESCES, NONE };
	
	
	
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
	
	
	
	enum class LoadType { RESISTIVE, CONSTANT_CURRENT, ENERGY, DIODE };
	
	inline ostream & operator << (ostream & os, const LoadType & type)
{
	switch (type)
	{
		case LoadType::RESISTIVE:
			return os << "resistive";
		case LoadType::CONSTANT_CURRENT:
			return os << "constant current";
		case LoadType::ENERGY:
			return os << "energy";
		case LoadType::DIODE:
			return os << "diode";

	default:
		throw exception("Invalid type of load");
	}
}
	
	inline const string operator + (const LoadType & tp, const string & str)
{
	if (tp == LoadType::RESISTIVE)
		return ("resistive" + str);
	if (tp == LoadType::CONSTANT_CURRENT)
		return ("constant current" + str);
	if (tp == LoadType::DIODE)
		return ("diode" + str);
	return ("power" + str);
}
	
	inline const string operator + (const string & str, const LoadType & tp)
{
	if (tp == LoadType::RESISTIVE)
		return (str + "resistive");
	if (tp == LoadType::CONSTANT_CURRENT)
		return (str + "current");
	if (tp == LoadType::DIODE)
		return (str + "diode");
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




	class ElectricNet_If
	{
		
		public:

			using key = string;

			
			
			virtual void addInput (key name, CvType type = CvType::VOLTAGE, double cvValue = 0.0) = 0;
			virtual void addConverter (key name, key sourceName, ConverterType type = ConverterType::PULSE, CvType cvType = CvType::VOLTAGE, 
							   double cvValue = 0.0, double efficiency = 100.0) = 0;
			virtual void addConverter (key name, ConverterType type = ConverterType::PULSE, CvType cvType = CvType::VOLTAGE, double cvValue = 0.0,
							   double efficiency = 100.0) = 0;
			virtual void insertConverter (key name, key sourceName, ConverterType type = ConverterType::PULSE, CvType cvType = CvType::VOLTAGE, 
							      double cvValue = 0.0, double efficiency = 100.0) = 0;
			virtual void insertConverter (key name, key sourceName, key sinkName, ConverterType type = ConverterType::PULSE, 
								  CvType cvType = CvType::VOLTAGE, double cvValue = 0.0, double efficiency = 100.0) = 0;
			virtual void addLoad (key name, key sourceName, LoadType type, double param) = 0;
			virtual void addLoad (key name, LoadType type, double param) = 0;
			virtual void addLoad (key name, key sourceName, LoadType type, double mainParam, double secondaryParam) = 0;
			virtual void addLoad (key name, LoadType type, double mainParam, double secondaryParam) = 0;

			virtual void deleteInput (key name, key newSourceName) = 0;
			virtual void deleteInput (key name) = 0;
			virtual void deleteConverter (key name, key newSourceName) = 0;
			virtual void deleteConverter (key name) = 0;
			virtual void deleteLoad (key name) = 0;
			virtual void deleteNode (key name, key newSourceName) = 0;
			virtual void deleteNode (key name) = 0;
			virtual void deleteSubnet (key headerName) = 0;
			virtual void deleteAllSinks (key sourceName) = 0;

			virtual void moveConverter (key name, key newSourceName) = 0;
			virtual void moveConverter (key name, key newSourceName, key newSinksSourceName) = 0;
			virtual void freeConverter (key name) = 0;
			virtual void freeConverter (key name, key newSinksSourceName) = 0;
			virtual void moveLoad (key name, key newSourceName) = 0;
			virtual void freeLoad (key name) = 0;
			virtual void moveSubnet (key headerName, key newSourceName) = 0;
			virtual void freeSubnet (key headerName) = 0;
			virtual void moveNode (key name, key newSourceName) = 0;
			virtual void moveNode (key name, key newSourceName, key newSinksSourceName) = 0;
			virtual void freeNode (key name) = 0;
			virtual void freeNode (key name, key newSinksSourceName) = 0;

			virtual void renameNode (key name, key newName) = 0;
			virtual void setSourceCvType (key name, CvType newType) = 0;
			virtual void setSourceCvValue (key name, double value) = 0;
			virtual void setConverterType (key name, ConverterType type) = 0;
			virtual void setConverterEfficiency (key name, double efficiency) = 0;
			virtual void setLoadType (key name, LoadType type) = 0;
			virtual void setLoadResistance (key name, double resistance) = 0;
			virtual void setLoadCurrent (key name, double current) = 0;
			virtual void setLoadForawrdVoltage (key name, double forwardVoltage) = 0;
			virtual void setLoadForwardCurrent (key name, double forwardCurrent) = 0;
			virtual void setLoadNomPower (key name, double nomPower) = 0;
			virtual void setLoadNomVoltage (key name, double nomVoltage) = 0;

			virtual string getTitle () = 0;
			virtual void rename (string newTitle) = 0;

			virtual void calculte () = 0;



			virtual ~ElectricNet_If () {;}

	};

}
