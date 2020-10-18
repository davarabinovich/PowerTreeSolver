
#include "forest/forest.h"
#include "electric_net.h"





namespace electirc_net
{

	ElectricNet::ElectricNet (string nm)
		: name(nm)    { ; }


	 

	void ElectricNet::addInput (key name, CvType type, double cvValue)
	{
		if (name == "") throw exception("Name of input can't be empty");

		ElectricNode a();
		net.addRoot(name, a);

	}


	void ElectricNet::addConverter (key name, ConverterType type, CvType cvType, double cvValue, double efficiency)
	{

	}


	void ElectricNet::addResistiveLoad (key name, double resistance)
	{

	}


	void ElectricNet::addConstantCurrentLoad (key name, double current)
	{

	}


	void ElectricNet::addDiodeLoad (key name, double forwardVoltage, double forwardCurrent)
	{

	}


	void ElectricNet::addEnergyLoad (key name, double nomPower, double nomVoltage)
	{

	}

}
