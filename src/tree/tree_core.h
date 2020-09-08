
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



enum class LoadType { RESISTIVE, CURRENT, POWER };

inline ostream& operator << (ostream & os, const LoadType & type)
{
	switch (type)
	{
	case LoadType::RESISTIVE:
		return os << "resistive";
	case LoadType::CURRENT:
		return os << "current";
	case LoadType::POWER:
		return os << "power";

	default:
		throw exception("Invalid type of load");
	}
}

inline const string operator + (const LoadType & tp, const string & str)
{
	if (tp == LoadType::RESISTIVE)
		return ("resistive" + str);
	if (tp == LoadType::CURRENT)
		return ("current" + str);
	return ("power" + str);
}

inline const string operator + (const string & str, const LoadType & tp)
{
	if (tp == LoadType::RESISTIVE)
		return (str + "resistive");
	if (tp == LoadType::CURRENT)
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

		
	private:
		
		using key = string;



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

		class PowerLoad : public Load
		{


			public:

				virtual double calculateConsumption (double cvValue, CvType parentCvtype) const override;




			private:

				double power;
				double nomVoltatge;

		};


		class Source : public Node
		{
			
			public:

				virtual void calculateLoad () const;




			protected:

				CvType cvType;
				double cvValue;

				mutable double load = NAN;




			private:

				map< key, shared_ptr<Sink> > sinks;

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

				double getSelfConsumption (double parentCvValue, CvType parentCvType) const;

		};


		class PulseConverter : public Converter
		{

			protected:

				virtual double recudeLoadToInput (double parentCvValue, CvType parentCvType) const;

		};

		class LinearConverter : public Converter
		{

			protected:

				virtual double recudeLoadToInput (double parentCvValue, CvType parentCvType) const;

		};







		map< key, shared_ptr<Input> > inputs;




	public:

		void calculate () const;

};
