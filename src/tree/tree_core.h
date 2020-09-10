
#pragma once


#include <string>
#include <map>
#include <sstream>
#include <memory>




using namespace std;




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

inline ostream& operator << (ostream & os, const LoadType & type)
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



enum class DeletingMode { WITH_DESCENDANTS, HANG_DESCENDANTS, RECONNECT_DESCENDANTS, NONE };





class PowerTree
{

	public:

		using key = string; 
		
		
		
		PowerTree (key name = "");




	private:
		
		key name;



		class Node
		{
			
		};

		class Sink : public Node
		{
			public:
				virtual double calculateConsumption (double parentCvValue, CvType parentCvType) const = 0;
		};

		class Load : public Sink
		{
			public:
				virtual double calculateConsumption (double parentCvValue, CvType parentCvType) const override = 0;
		};

		class ResistiveLoad : public Load
		{
			public:
				virtual double calculateConsumption (double parentCvValue, CvType parentCvType) const override;


			private:
				double resistance;
		};

		class CurrentLoad : public Load
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
				using DescendantsMap = map< key, shared_ptr<Sink> >;

				double calculateLoad () const;


			protected:

				CvType cvType;
				double cvValue;


			private:
				DescendantsMap descendants;
		};

		class Input : public Source
		{

		};


		class Converter : public Source, Sink
		{
#pragma todo to forbid connect some converters to the converters with current output
			public:
				virtual double calculateConsumption (double parentCvValue, CvType parentCvType) const override;


			protected:
				virtual double recudeLoadToInput (double parentCvValue, CvType parentCvType) const = 0;

				double efficiency = 100.0;


			private:
				double getSelfConsumption (double parentCvValue, CvType parentCvType, double load) const;
		};

		class PulseConverter : public Converter
		{
			protected:
				virtual double reduceLoadToInput (double parentCvValue, CvType parentCvType, double load) const;
		};

		class LinearConverter : public Converter
		{
			protected:
				virtual double reduceLoadToInput (double parentCvValue, CvType parentCvType, double load) const;
		};



		map< key, shared_ptr<Input> > inputs;
		map< key, shared_ptr<Sink> > freeSinks;
		map< key, shared_ptr<Sink> > connectedSinks;




	public:

		using DescendantsMap = Source::DescendantsMap;



		void addInput (key name);
		void addConverter (key name, key parentName = "");
		void addLoad (key name, key parentName = "");

		void moveSubnetTo (key subnetHeadName, key newParentName);
		void disconnectSubnet (key subnetHeadName);

		DescendantsMap & getDescendants (key headName);

		void deleteNode(key name, key descendantsNewParentName = "");
		void deleteSubnet(key name);


		void calculate () const;

};
