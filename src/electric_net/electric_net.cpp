
#include "forest/forest.h"
#include "electric_net.h"





namespace electirc_net
{

	ElectricNet::ElectricNet (string nm)
		: name(nm)    { ; }


	 

	void ElectricNet::addInput (key name, CvType type, double cvValue)
	{
#pragma all checking
		auto newInput_ptr = make_shared<Input>(type, cvValue);
		net.addRoot(name, newInput_ptr);
	}

	void ElectricNet::addConverter (key name, key sourceName, ConverterType type, CvType cvType, double cvValue, double efficiency)
	{
		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.pushBackLeaf(name, sourceName, newConverter_ptr);
	}


	void ElectricNet::addConverter (key name, ConverterType type, CvType cvType, double cvValue, double efficiency)
	{
		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.addRoot(name, newConverter_ptr);
	}


	void ElectricNet::insertConverter (key name, key sourceName, ConverterType type, CvType cvType, double cvValue, double efficiency)
	{
		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.insertDesc(name, sourceName, newConverter_ptr);
	}

	void ElectricNet::insertConverter (key name, key sourceName, key sinkName, ConverterType type, CvType cvType, double cvValue, double efficiency)
	{
		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.insertDesc(name, sourceName, sinkName, newConverter_ptr);
	}





	ElectricNet::Source::Source (CvType tp, double value)
		: type(tp), cvValue(value) {;}





	ElectricNet::Input::Input (CvType type, double cvValue)
		: Source(type, cvValue) {;}





	ElectricNet::Converter::Converter (CvType cvType, double value, ConverterType tp, double effcnc)
		: Source(cvType, value), type(tp), efficiency(effcnc) {;}

}
