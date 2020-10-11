
#pragma once


#include <string>
#include <map>
#include <sstream>
#include <memory>


#include "lib/ciflib.h"





using namespace std;





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

	public:

		PowerTree (string name = "");




	private:

		using key = string;


		
		key name;



		class Node
		{ };

		class Sink : public Node
		{
			public:
				virtual double calculateConsumption (double parentCvValue, CvType parentCvType) const = 0;
		};

		class Load : public Sink
		{
			public:
				Load (const Load & otherLoad) = delete;
				Load (Load && otherLoad) = delete;

				virtual double calculateConsumption (double parentCvValue, CvType parentCvType) const override = 0;

			protected:
				Load () {};
		};

		class ResistiveLoad : public Load
		{
			public:
				virtual double calculateConsumption (double parentCvValue, CvType parentCvType) const override;


			private:
				double resistance;
		};

		class ConstantCurrentLoad : public Load
		{
#pragma todo to forbid connect it to the current source
			public:
				virtual double calculateConsumption (double parentCvValue, CvType parentCvtype) const override;


			private:
				double current;
		};

		
		class Source : public Node
		{
			public:
				using DescesDictionary = map< key, shared_ptr<Sink> >;

				Source (const Source & otherSource) = delete;
				Source (Source && otherSource) = delete;

				void connectDesc (key descName, shared_ptr<Sink> sink_ptr);
				void deleteDesc (key descName);
				const DescesDictionary & getAllDesces () const;
				double calculateLoad () const;
				bool isSuchDesc (key descName) const;
				void setCvType (CvType type);
				void setValue (double value);

			protected:
				CvType cvType;
				double cvValue;

				Source () {};

			private:
				DescesDictionary desces;
		};

		class Input : public Source
		{ };

#pragma //"exercises: compiler couldn't convert sh_ptr<Conv> to sh_ptr<Sink> while inheritance from Sink was a private"
		class Converter : public Source, public Sink
		{
#pragma todo to forbid connect some converters to the converters with current output
			public:
				Converter (const Converter & otherConverter) = delete;
				Converter (Converter && otherConverter) = delete;

				virtual double calculateConsumption (double parentCvValue, CvType parentCvType) const override;

			protected:
				Converter () {};

				virtual double reduceLoadToInput (double parentCvValue, double load) const = 0;

				double efficiency = 100.0;

			private:
				double getSelfConsumption (double load) const;
		};

		class PulseConverter : public Converter
		{
			protected:
				virtual double reduceLoadToInput (double parentCvValue, double load) const override;
		};

		class LinearConverter : public Converter
		{
			protected:
				virtual double reduceLoadToInput (double parentCvValue, double load) const override;
		};



		map< key, shared_ptr<Input>     > inputs;
		map< key, shared_ptr<Converter> > converters;
		map< key, shared_ptr<Load>      > loads;




	public:

		void addInput (key name);
		void addConverter (key name, key parentName = "", ConverterType type = ConverterType::PULSE);
		void addLoad (key name, key parentName = "", LoadType type = LoadType::CONSTANT_CURRENT);

		void moveSubnetTo (key subnetHeadName, key newParentName);
		void disconnectSubnet (key headerName);
		void moveAllDescesTo (key parentName, key newParentName);
		void disconnectAllDesces (key parentName);

		void deleteNode (key name, key descesNewParentName = "");
		void deleteSubnet (key headerName);

		void setNodeName (key oldName, key newName);
		void setSourceCvType (key inputName, CvType type, double value = NAN);
		void setSourceValue (key inputName, double value);
		void setConverterType (key converterName, ConverterType type);
		void setConverterEfficiency (key converterName, double efficiency);
		void setLoadValue (key loadName, double value);
		
		

		void calculate () const;


		void validateArgsForNewSink (key name, key parentName) const;
		void writeNewSinkToSource (key sinkName, key parentName, shared_ptr<Sink> newSink_ptr);
		void disconnectSubnetWithoutChecking (key headerName);
		void disconnectSubnetWithoutChecking (key headerName, key parentName);
		void deleteSubnetPointers (key headerName);
		shared_ptr<Source> findParent (key name) const;
		const shared_ptr<Source> findSourceByKey (key name) const;
		const shared_ptr<Sink> findSinkByKey (key name) const;
		const Source::DescesDictionary & findDescesByKey (key name) const;

		void ensureIsNodeExsisting (key name) const;
		void ensureIsNodeNotExsisting (key name) const;
		void ensureIsSourceExsisting (key name) const;
		void ensureIsSinkExsisting (key name) const;
		void ensureIsSecondNotFirstsDesc (key firstName, key secondName) const;
		void ensureIsConvertersDescNotExsisting (key descName, key converterName) const;

		bool isSuchSource (key name) const;
		bool isSuchSink (key name) const;
		bool isSuchInput (key name) const;
		bool isSuchConverter (key name) const;
		bool isSuchLoad (key name) const;

};
