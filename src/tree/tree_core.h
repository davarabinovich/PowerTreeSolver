
#pragma once


#include <string>
#include <map>
#include <sstream>
#include <memory>




using namespace std;




#pragma todo carry over to library
inline void ensureIsNameNotEmpty (string name, string itemType_str = "")
{
	if (name != "")    return;
	
	string message;
	if (itemType_str == "")
		message = string("Name can't be empry");
	else
		message = string("Name of " + itemType_str + " can't be empty");
		
	throw exception( message.c_str() );
}





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



enum class DeletingMode { WITH_DESCENDANTS, HANG_DESCENDANTS, RECONNECT_DESCENDANTS, NONE };





class PowerTree
{

	public:

		PowerTree (string name = "");




	private:

		using key = string;


		
		key name;



		class Node
		{
			public:
				virtual string getTypeOfNode_str () const;
		};

		class Sink : public Node
		{
			public:
				virtual double calculateConsumption (double parentCvValue, CvType parentCvType) const = 0;
		};

		class Load : public Sink
		{
			public:
				string getTypeOfNode_str() const override;

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

				void connectNewDescendant (key newDescendantName, shared_ptr<Sink> sink_ptr);
				double calculateLoad () const;


			protected:
				CvType cvType;
				double cvValue;


			private:
				DescendantsMap descendants;
		};

		class Input : public Source
		{
			public:
				string getTypeOfNode_str() const override;
		};


		class Converter : public Source, Sink
		{
#pragma todo to forbid connect some converters to the converters with current output
#pragma todo remove all constructors, as well as for load, node, sink and source
			public:
				string getTypeOfNode_str() const override;

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



		map< key, shared_ptr<Input>     > inputs;
		map< key, shared_ptr<Converter> > converters;
		map< key, shared_ptr<Load>      > loads;




	public:

#pragma todo the following must be const
		void addInput (key name);
		void addConverter (key name, key parentName = "", ConverterType type = ConverterType::PULSE);
		void addLoad (key name, key parentName = "");

		void moveSubnetTo (key subnetHeadName, key newParentName);
		void disconnectSubnet (key subnetHeadName);

		void moveAllDescendantsTo (key parentName, key newParentName);
		void disconnectAllDescendants (key parentName);

		void deleteNode (key name, key descendantsNewParentName = "");
		void deleteSubnet (key name);


		void calculate () const;



		void ensureIsNodeNotExsisting (key name) const;
		void ensureIsSourceExsisting (key name) const;



		bool isSuchSource (key name) const;
		bool isSuchInput (key name) const;
		bool isSuchConverter (key name) const;
		bool IsSuchLoad (key name) const;
};
