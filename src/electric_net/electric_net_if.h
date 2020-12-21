
#pragma once


#include <sstream>
#include <functional>


#include "config.h"




using std::ostream;




namespace electric_net
{
	
	enum class DeviceType { INPUT, CONVERTER, LOAD };

	inline string toStr (DeviceType type)
	{
		switch (type)
		{
			case DeviceType::INPUT:
				return "input";
			case DeviceType::CONVERTER:
				return "converter";
			case DeviceType::LOAD:
				return "load";
	
			default:
				throw exception("Invalid type of device");
		}
	}

	inline ostream & operator << (ostream & os, const DeviceType & type)
	{
		return os << toStr(type);
	}



#pragma todo rename to VarType and add inversion
	enum class VarKind { VOLTAGE, CURRENT };
	
	inline ostream & operator << (ostream & os, const VarKind & type)
	{
		switch (type)
		{
			case VarKind::VOLTAGE:
				return os << "voltage";
			case VarKind::CURRENT:
				return os << "current";
	
			default:
				throw exception("Invalid kind of controlled variable");
		}
	}
	
	inline const string operator + (const VarKind & tp, const string & str)
	{
		if (tp == VarKind::VOLTAGE)
			return ("voltage" + str);
		return ("current" + str);
	}
	
	inline const string operator + (const string & str, const VarKind & tp)
	{
		if (tp == VarKind::VOLTAGE)
			return (str + "voltage");
		return (str + "current");
	}

	inline bool isCvTypeString (const string & str)
	{
		if (str[0] != 'c' && str[0] != 'C' && str[0] != 'v' && str[0] != 'V') return false;
		if (str != "cur" && str != "Cur" && str != "current" && str != "Current")
			if (str != "vol" && str != "Vol" && str != "voltage" && str != "Voltage") return false;
		return true;
	}
	
	inline VarKind parseCvType (const string & str)
	{
		if (!isCvTypeString(str))
#pragma todo write exceptions message
			throw exception();

		if (str == "cur" || str == "Cur" || str == "current" || str == "Current") return VarKind::CURRENT;
		return VarKind::VOLTAGE;
	}

	inline string getCvUnitDesignatorStr (const VarKind type)
	{
		switch (type)
		{
			case VarKind::VOLTAGE:
				return "V";
			case VarKind::CURRENT:
				return "A";
	
			default:
				throw exception("Invalid kind of controlled variable");
		}
	}

	inline string getAvUnitDesignatorStr (const VarKind type)
	{
		switch (type)
		{
			case VarKind::VOLTAGE:
				return "A";
			case VarKind::CURRENT:
				return "V";
	
			default:
				throw exception("Invalid kind of controlled variable");
		}
	}
	
	
	
	enum class LoadType { RESISTIVE, CONSTANT_CURRENT, DIODE };
	
	inline ostream & operator << (ostream & os, const LoadType & type)
	{
		switch (type)
		{
			case LoadType::RESISTIVE:
				return os << "resistive";
			case LoadType::CONSTANT_CURRENT:
				return os << "constant current";
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
		return ("diode" + str);
	}
		
	inline const string operator + (const string & str, const LoadType & tp)
	{
		if (tp == LoadType::RESISTIVE)
			return (str + "resistive");
		if (tp == LoadType::CONSTANT_CURRENT)
			return (str + "current");
		return (str + "diode");
	}

	inline bool isLoadTypeString (const string & str)
	{
		if (str != "r" && str != "R" && str != "res" && str != "Res" && str != "resistive" && str != "Resistive")
			if (str != "c" && str != "C" && str != "cur" && str != "Cur" && str != "current" && str != "Current")
				if (str != "d" && str != "D" && str != "di" && str != "Di" && str != "diode" && str != "Diode")
					return false;
		return true;
	}
	
	inline LoadType parseLoadType (const string & str)
	{
		if (!isLoadTypeString(str))
			throw exception("Invalid type of load");

		if (str == "r" || str == "R" || str == "res" || str == "Res" || str == "resistive" || str == "Resistive") 
			return LoadType::RESISTIVE;
		if (str == "c" || str == "C" || str == "cur" || str == "Cur" || str == "current" || str == "Current") 
			return LoadType::CONSTANT_CURRENT;
		return LoadType::DIODE;
	}

	inline string getValueTypeStr (const LoadType type)
	{
		switch (type)
		{
			case LoadType::RESISTIVE:
				return string("resistance");

			case LoadType::CONSTANT_CURRENT:
				return string("current");

			case LoadType::DIODE:
				return string("forward voltage");


			default:
				throw exception("Invalid type of load");
		}
	}

	inline string getMainUnitDesignatorStr (const LoadType type)
	{
		switch (type)
		{
			case LoadType::RESISTIVE:
				return string("Ohm");

			case LoadType::CONSTANT_CURRENT:
				return string("A");

			case LoadType::DIODE:
				return string("V");


			default:
				throw exception("Invalid type of load");
		}
	}

	inline string getAddValueTypeStr (const LoadType type)
	{
		switch (type)
		{
			case LoadType::DIODE:
				return string("forward current");

			case LoadType::RESISTIVE: [[__fallthrough]]
			case LoadType::CONSTANT_CURRENT:
				throw exception("Such a type of load hasn't an additional parameter");


			default:
				throw exception("Invalid type of load");
		}
	}

	inline string getAddUnitDesignatorStr (const LoadType type)
	{
		switch (type)
		{
			case LoadType::DIODE:
				return string("A");

			case LoadType::RESISTIVE: [[__fallthrough]]
			case LoadType::CONSTANT_CURRENT:
				throw exception("Such a type of load hasn't an additional parameter");


			default:
				throw exception("Invalid type of load");
		}
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

	inline bool isConverterTypeString (const string & str)
	{
		if (str != "l" && str != "L" && str != "lin" && str != "Lin" && str != "linear" && str != "Linear")
			if (str != "p" && str != "P" && str != "pul" && str != "Pul" && str != "pulse" && str != "Pulse") return false;
		return true;
	}
	
	inline ConverterType parseConverterType (const string & str)
	{
		if (!isConverterTypeString(str))
#pragma todo write exceptions message
			throw exception();

		if (str == "lin" || str == "Lin" || str == "linear" || str == "Linear") return ConverterType::LINEAR;
		return ConverterType::PULSE;
	}

	inline string toStr (ConverterType type)
	{
		switch (type)
		{
		case ConverterType::PULSE:
			return "pulse";
		case ConverterType::LINEAR:
			return "linear";
	
		default:
			throw exception("Invalid type of converter");
		}
	}



	using key = string;



	struct InputData
	{
		key name;

		VarKind type;
		double value;
	};

	struct ConverterData
	{
		key name;
		unsigned nestingLevel;

		VarKind cvType;
		double value;
		ConverterType type; 
		double efficiency;
	};

	struct ResistiveLoadData
	{
		key name;
		unsigned nestingLevel;

		double resistance;
	};

	struct ConstantCurrentLoadData
	{
		key name;
		unsigned nestingLevel;

		double current;
	};

	struct DiodeLoadData
	{
		key name;
		unsigned nestingLevel;

		double forwardVoltage;
		double forwardCurrent;
	};

	struct EnergyLoadData
	{
		key name;
		unsigned nestingLevel;

		double nominalPower;
		double nominalVoltage;
	};



	struct InputResults
	{
		key name;

		VarKind type;
		double value;

		double avValue;
	};

	struct ConverterResults
	{
		key name;
		unsigned nestingLevel;

		VarKind cvType;
		double value;
		ConverterType type;

		double avValue;
		double inputValue;
	};

	struct ResistiveLoadResults
	{
		key name;
		unsigned nestingLevel;

		double resistance;

		double inputValue;
		VarKind inputVarKind;
	};

	struct ConstantCurrentLoadResults
	{
		key name;
		unsigned nestingLevel;

		double current;
		
		double inputVoltage;
	};

	struct DiodeLoadResults
	{
		key name;
		unsigned nestingLevel;

		double forwardVoltage;
		double forwardCurrent;
	};





	class ElectricNet_If
	{
		
		public:

#pragma todo add const qualificators
			virtual void addInput (key name, VarKind type = VarKind::VOLTAGE, double cvValue = 0.0) = 0;
			virtual void addConverter (key name, key sourceName, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, 
							   double cvValue = 0.0, double efficiency = 100.0) = 0;
			virtual void addConverter (key name, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, double cvValue = 0.0,
							   double efficiency = 100.0) = 0;
			virtual void insertConverter (key name, key sourceName, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, 
							      double cvValue = 0.0, double efficiency = 100.0) = 0;
			virtual void insertConverter (key name, key sourceName, key sinkName, ConverterType type = ConverterType::PULSE, 
								  VarKind cvType = VarKind::VOLTAGE, double cvValue = 0.0, double efficiency = 100.0) = 0;
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
			virtual void setSourceCvType (key name, VarKind newType) = 0;
			virtual void setSourceCvValue (key name, double value) = 0;
			virtual void setConverterType (key name, ConverterType type) = 0;
			virtual void setConverterEfficiency (key name, double efficiency) = 0;
			virtual void setLoadType (key name, LoadType type) = 0;
			virtual void setLoadResistance (key name, double resistance) = 0;
			virtual void setLoadCurrent (key name, double current) = 0;
			virtual void setLoadForawrdVoltage (key name, double forwardVoltage) = 0;
			virtual void setLoadForwardCurrent (key name, double forwardCurrent) = 0;
			
			virtual DeviceType getNodeType (key name) = 0;
			virtual InputData getInputData (key inputName) = 0;
			virtual ConverterData getConverterData (key converterName) = 0;
			virtual ResistiveLoadData getResistiveLoadData (key loadName) = 0;
			virtual ConstantCurrentLoadData getConstantCurrentLoadData (key loadName) = 0;
			virtual DiodeLoadData getDiodeLoadData (key loadName) = 0;
			virtual EnergyLoadData getEnergyLoadData (key loadName) = 0;
			virtual bool isLoadExsist (key name) = 0;
			virtual LoadType getLoadType (key name) = 0;

			virtual string getTitle () = 0;
			virtual void rename (string newTitle) = 0;

#pragma todo make calculate const again
			virtual void calculte () = 0;
			virtual InputResults getInputResults (key inputName) const = 0;
			virtual ConverterResults getConverterResults (key convertertName) const = 0;
			virtual ResistiveLoadResults getResistiveLoadResults (key loadName) const = 0;
			virtual ConstantCurrentLoadResults getConstantCurrentLoadResults (key loadName) const = 0;
			virtual DiodeLoadResults getDiodeLoadResults (key loadName) const = 0;

#pragma todo make this const
			virtual void iterateAndExecuteForEach (function<void (key)> functor) = 0;
			



			virtual ~ElectricNet_If () {;}

	};

}
