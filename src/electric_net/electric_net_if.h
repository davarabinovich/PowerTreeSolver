
#pragma once


#include <sstream>
#include <functional>


#include "config.h"




using std::ostream;
using std::function;




namespace electric_net
{
	
	const string default_tree_name = "New power tree";

	const string default_frist_input_name = "in1";
	const string default_input_name_body = "in";
	const string default_converter_name_body = "con";
	const string default_load_name_body = "load";




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

	inline VarKind operator ! (const VarKind & kind)
	{
		if (kind == VarKind::VOLTAGE)
			return VarKind::CURRENT;
		return VarKind::VOLTAGE;
	}

	inline bool isVarKindString (const string & str)
	{
		if (str != "c" && str != "C" && str != "cur" && str != "Cur" && str != "current" && str != "Current")
			if (str != "v" && str != "V" && str != "vol" && str != "Vol" && str != "voltage" && str != "Voltage") return false;
		return true;
	}
	
	inline VarKind parseVarKind (const string & str)
	{
		if (!isVarKindString(str))    throw exception("parseVarKind: String doesn't represent a variable kind");

		if (str == "cur" || str == "Cur" || str == "current" || str == "Current") return VarKind::CURRENT;
		return VarKind::VOLTAGE;
	}

	inline string getVarKindDesignatorStr (const VarKind type)
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
		if (!isLoadTypeString(str))    throw exception("parseLoadType: String doesn't represent a type of load");

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
			if (str != "p" && str != "P" && str != "pul" && str != "Pul" && str != "pulse" && str != "Pulse") 
				return false;
		return true;
	}
	
	inline ConverterType parseConverterType (const string & str)
	{
		if (!isConverterTypeString(str))    throw exception("parseConverterType: String doesn't represent a type of converter");

		if (str == "l" || str == "L" || str == "lin" || str == "Lin" || str == "linear" || str == "Linear")
			return ConverterType::LINEAR;
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










	using Key = string;



	struct InputData
	{
		Key name;

		VarKind cvKind;
		double value;
	};

	struct ConverterData
	{
		Key name;
		unsigned nestingLevel;

		VarKind cvKind;
		double value;
		ConverterType type; 
		double efficiency;
	};

	struct ResistiveLoadData
	{
		Key name;
		unsigned nestingLevel;

		double resistance;
	};

	struct ConstantCurrentLoadData
	{
		Key name;
		unsigned nestingLevel;

		double current;
	};

	struct DiodeLoadData
	{
		Key name;
		unsigned nestingLevel;

		double forwardVoltage;
		double forwardCurrent;
	};



	struct InputResults
	{
		Key name;

		VarKind type;
		double value;

		double avValue;
	};

	struct ConverterResults
	{
		Key name;
		unsigned nestingLevel;

		VarKind cvType;
		double value;
		ConverterType type;

		double avValue;
		double inputValue;
	};

	struct ResistiveLoadResults
	{
		Key name;
		unsigned nestingLevel;

		double resistance;

		double inputValue;
		VarKind inputVarKind;
	};

	struct ConstantCurrentLoadResults
	{
		Key name;
		unsigned nestingLevel;

		double current;
		
		double inputVoltage;
	};

	struct DiodeLoadResults
	{
		Key name;
		unsigned nestingLevel;

		double forwardVoltage;
		double forwardCurrent;
	};





	class ElectricNet_If
	{
		
		public:

			virtual void addInput (Key name, VarKind type = VarKind::VOLTAGE, double cvValue = 0.0) = 0;
			virtual void addConverter (Key name, Key sourceName, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, 
							           double cvValue = 0.0, double efficiency = 100.0) = 0;
			virtual void addConverter (Key name, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, double cvValue = 0.0,
							           double efficiencyParam = 100.0) = 0;
			virtual void insertConverter (Key name, Key sourceName, ConverterType type = ConverterType::PULSE, VarKind cvType = VarKind::VOLTAGE, 
							              double cvValue = 0.0, double efficiency = 100.0) = 0;
			virtual void insertConverter (Key name, Key sourceName, Key sinkName, ConverterType type = ConverterType::PULSE, 
								          VarKind cvType = VarKind::VOLTAGE, double cvValue = 0.0, double efficiency = 100.0) = 0;
			virtual void addLoad (Key name, Key sourceName, LoadType type, double param) = 0;
			virtual void addLoad (Key name, LoadType type, double param) = 0;
			virtual void addLoad (Key name, Key sourceName, LoadType type, double mainParam, double secondaryParam) = 0;
			virtual void addLoad (Key name, LoadType type, double mainParam, double secondaryParam) = 0;

			virtual void deleteInput (Key name, Key newSourceName) = 0;
			virtual void deleteInput (Key name) = 0;
			virtual void deleteConverter (Key name, Key newSourceName) = 0;
			virtual void deleteConverter (Key name) = 0;
			virtual void deleteLoad (Key name) = 0;
			virtual void deleteNode (Key name, Key newSourceName) = 0;
			virtual void deleteNode (Key name) = 0;
			virtual void deleteSubnet (Key headerName) = 0;
			virtual void deleteAllSinks (Key sourceName) = 0;

			virtual void moveConverter (Key name, Key newSourceName) = 0;
			virtual void moveConverter (Key name, Key newSourceName, Key newSinksSourceName) = 0;
			virtual void freeConverter (Key name) = 0;
			virtual void freeConverter (Key name, Key newSinksSourceName) = 0;
			virtual void moveLoad (Key name, Key newSourceName) = 0;
			virtual void freeLoad (Key name) = 0;
			virtual void moveSubnet (Key headerName, Key newSourceName) = 0;
			virtual void freeSubnet (Key headerName) = 0;
			virtual void moveNode (Key name, Key newSourceName) = 0;
			virtual void moveNode (Key name, Key newSourceName, Key newSinksSourceName) = 0;
			virtual void freeNode (Key name) = 0;
			virtual void freeNode (Key name, Key newSinksSourceName) = 0;

			virtual void renameNode (Key name, Key newName) = 0;
			virtual void setSourceCvType (Key name, VarKind newType) = 0;
			virtual void setSourceCvValue (Key name, double value) = 0;
			virtual void setConverterType (Key name, ConverterType type) = 0;
			virtual void setConverterEfficiencyParam (Key name, double efficiencyParam) = 0;
			virtual void setLoadType (Key name, LoadType type) = 0;
			virtual void setLoadResistance (Key name, double resistance) = 0;
			virtual void setLoadCurrent (Key name, double current) = 0;
			virtual void setLoadForawrdVoltage (Key name, double forwardVoltage) = 0;
			virtual void setLoadForwardCurrent (Key name, double forwardCurrent) = 0;
			
			virtual DeviceType getNodeType (Key name) const = 0;
			virtual InputData getInputData (Key inputName) const = 0;
			virtual ConverterData getConverterData (Key converterName) const = 0;
			virtual ResistiveLoadData getResistiveLoadData (Key loadName) const = 0;
			virtual ConstantCurrentLoadData getConstantCurrentLoadData (Key loadName) const = 0;
			virtual DiodeLoadData getDiodeLoadData (Key loadName) const = 0;
			virtual bool isLoadExsist (Key name) const = 0;
			virtual LoadType getLoadType (Key name) const = 0;

			virtual string getTitle () const = 0;
			virtual void rename (string newTitle) = 0;

			virtual void calculte () const = 0;
			virtual InputResults getInputResults (Key inputName) const = 0;
			virtual ConverterResults getConverterResults (Key convertertName) const = 0;
			virtual ResistiveLoadResults getResistiveLoadResults (Key loadName) const = 0;
			virtual ConstantCurrentLoadResults getConstantCurrentLoadResults (Key loadName) const = 0;
			virtual DiodeLoadResults getDiodeLoadResults (Key loadName) const = 0;

			virtual void iterateAndExecuteForEach (function<void (Key)> functor) const = 0;

			virtual Key getDefaultNodeName (DeviceType type) const = 0;
			



			virtual ~ElectricNet_If () {;}

	};

}
