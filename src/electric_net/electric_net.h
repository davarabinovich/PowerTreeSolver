
#pragma once


#include <string>
#include <map>
#include <sstream>
#include <variant>


#include "forest/forest.h"
#include "lib/ciflib.h"





using namespace std;




namespace electirc_net
{
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
	

	
	
	using key = string;
	class ElectricNet
	{
	
		public:
			
			//using key = string;



			ElectricNet (string name);



			void addInput (key name, CvType type = CvType::VOLTAGE, double cvValue = 0.0);
			void addConverter (key name, ConverterType type = ConverterType::PULSE, CvType cvType = CvType::VOLTAGE, 
							   double cvValue = 0.0, double efficiency = 100.0);
			void addResistiveLoad (key name, double resistance);
			void addConstantCurrentLoad (key name, double current);
			void addDiodeLoad (key name, double forwardVoltage, double forwardCurrent);
			void addEnergyLoad (key name, double nomPower, double nomVoltage);

			void deleteInput (key name, key newSourceName);
			void deleteInput (key name);
			void deleteConverter (key name, key newSourceName);
			void deleteConverter (key name);
			void deleteLoad (key name);
			void deleteNode (key name, key newSourceName);
			void deleteNode (key name);
			void deleteSubnet (key headerName);
			void deleteAllSinks (key sourceName);

			void moveConverter (key name, key newSourceName);
			void moveConverter (key name, key newSourceName, key newSinksSourceName);
			void freeConverter (key name);
			void freeConverter (key name, key newSinksSourceName);
			void moveLoad (key name, key newSourceName);
			void freeLoad (key name);
			void moveSubnet (key headerName, key newSourceName);
			void freeSubnet (key headerName);
			void moveNode (key name, key newSourceName);
			void moveNode (key name, key newSourceName, key newSinksSourceName);
			void freeNode (key name);
			void freeNode (key name, key newSinksSourceName);

			void renameNode (key name, key newName);
			void setSourceCvType (key name, CvType newType);
			void setSourceCvValue (key name, double value);
			void setConverterType (key name, ConverterType type);
			void setConverterEfficienct (key name, double efficiency);
			void setLoadType (key name, LoadType type);
			void setLoadResistance (key name, double resistance);
			void setLoadCurrent (key name, double current);
			void setLoadForawrdVoltage (key name, double forwardVoltage);
			void setLoadForwardCurrent (key name, double forwardCurrent);
			void setLoadNomPower (key name, double nomPower);
			void setLoadNomVoltage (key name, double nomVoltage);

			void calculte ();



			~ElectricNet();




		private:
				
			struct Source
			{
				CvType type = CvType::VOLTAGE;
				double value = 0.0;
			};

			struct Input
			{
				Source source;
			};

			struct Converter
			{
				Source source;
				ConverterType type = ConverterType::PULSE;
				double efficiency = 100.0;
			};

			struct OneParamLoad
			{
				double mainParam = 100.0;
			};
			
			struct TwoParamLoad
			{
				double secondaryParam = 100.0;
			};

			struct Load
			{
				LoadType type = LoadType::CONSTANT_CURRENT;
				variant<OneParamLoad, TwoParamLoad> params = OneParamLoad();
			};


			struct ElectricNode
			{
				variant<Input, Converter, Load> nodeParams;
			};

			

			
			string name;

			Forest<string, ElectricNode> net;

	};
	
}
