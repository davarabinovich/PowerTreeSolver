
#include "forest/forest.h"
#include "electric_net.h"





namespace electirc_net
{

	ElectricNet::ElectricNet (string nm)
		: title(nm)    { ; }


	 

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


	void ElectricNet::deleteInput (key name, key newSourceName)
	{
		net.moveAllDesces(name, newSourceName);
		net.popFrontRoot(name);
	}


	void ElectricNet::deleteInput (key name)
	{
		net.popFrontRoot(name);
	}

	void ElectricNet::deleteConverter (key name, key newSourceName)
	{
		net.moveAllDesces(name, newSourceName);
		net.popBackLeaf(name);
	}


	void ElectricNet::deleteConverter (key name)
	{
		net.freeAllDesces(name);
		net.popBackLeaf(name);
	}


	void ElectricNet::deleteLoad (key name)
	{
		net.popBackLeaf(name);
	}


	void ElectricNet::deleteNode (key name, key newSourceName)
	{
		net.moveAllDesces(name, newSourceName);
		net.popBackLeaf(name);
	}


	void ElectricNet::deleteNode (key name)
	{
		net.freeAllDesces(name);
		net.popBackLeaf(name);
	}


	void ElectricNet::deleteSubnet (key headerName)
	{
		net.popBackSubtree(headerName);
	}


	void ElectricNet::deleteAllSinks (key sourceName)
	{
		net.eraseAllDesces(sourceName);
	}


	void ElectricNet::renameNode (key name, key newName)
	{

	}


	void ElectricNet::setSourceCvType (key name, CvType newType)
	{
		auto z = net[name].get();
		auto a = dynamic_cast<Source *>(z);
		/*auto source_ptr = dynamic_pointer_cast<Source>( net[name] );
		source_ptr->type = newType;*/
	}


	//void ElectricNet::setSourceCvValue (key name, double newValue)
	//{
	//	auto source_ptr = dynamic_pointer_cast<Source>( net[name] );
	//	source_ptr->cvValue = newValue;
	//}


	//void ElectricNet::setConverterType (key name, ConverterType newType)
	//{
	//	auto converter_ptr = dynamic_pointer_cast<Converter>( net[name] );
	//	converter_ptr->type = newType;
	//}


	//void ElectricNet::setConverterEfficienct (key name, double newEfficiency)
	//{
	//	auto converter_ptr = dynamic_pointer_cast<Converter>( net[name] );
	//	converter_ptr->efficiency = newEfficiency;
	//}


	//void ElectricNet::setLoadType (key name, LoadType newType)
	//{
	//	auto load_ptr = dynamic_pointer_cast<Load>( net[name] );
	//	load_ptr->type = newType;
	//}


	//void ElectricNet::setLoadResistance (key name, double resistance)
	//{
	//	auto load_ptr = dynamic_pointer_cast<OneParamLoad>( net[name] );
	//	load_ptr->mainParam = resistance;
	//}


	//void ElectricNet::setLoadCurrent (key name, double current)
	//{
	//	auto load_ptr = dynamic_pointer_cast<OneParamLoad>( net[name] );
	//	load_ptr->mainParam = current;
	//}


	//void ElectricNet::setLoadForawrdVoltage (key name, double forwardVoltage)
	//{
	//	auto load_ptr = dynamic_pointer_cast<TwoParamLoad>( net[name] );
	//	load_ptr->mainParam = forwardVoltage;
	//}


	//void ElectricNet::setLoadForwardCurrent (key name, double forwardCurrent)
	//{
	//	auto load_ptr = dynamic_pointer_cast<TwoParamLoad>( net[name] );
	//	load_ptr->secondaryParam = forwardCurrent;
	//}


	//void ElectricNet::setLoadNomPower (key name, double nomPower)
	//{
	//	auto load_ptr = dynamic_pointer_cast<TwoParamLoad>( net[name] );
	//	load_ptr->mainParam = nomPower;
	//}


	//void ElectricNet::setLoadNomVoltage (key name, double nomVoltage)
	//{
	//	auto load_ptr = dynamic_pointer_cast<TwoParamLoad>( net[name] );
	//	load_ptr->secondaryParam = nomVoltage;
	//}




	
	ElectricNet::Source::Source (DeviceType devType, CvType tp, double value)
		: ElectricNode(devType), type(tp), cvValue(value) {;}





	ElectricNet::Input::Input (CvType type, double cvValue)
		: Source(DeviceType::INPUT, type, cvValue) {;}





	ElectricNet::Converter::Converter (CvType cvType, double value, ConverterType tp, double effcnc)
		: Source(DeviceType::CONVERTER, cvType, value), type(tp), efficiency(effcnc) {;}





	ElectricNet::ElectricNode::ElectricNode (DeviceType tp)
		: type(tp) {;}

}
