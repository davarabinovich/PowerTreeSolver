
#include "electric_net.h"




namespace electric_net
{

	ElectricNet::ElectricNet (string nm)
		: title(nm)    { ; }


	 

	void ElectricNet::addInput (key name, VarKind type, double cvValue)
	{
#pragma all checking
		auto newInput_ptr = make_shared<Input>(type, cvValue);
		net.addRoot(name, newInput_ptr);
	}

	void ElectricNet::addConverter (key name, key sourceName, ConverterType type, VarKind cvType, double cvValue, double efficiency)
	{
		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.pushBackLeaf(name, sourceName, newConverter_ptr);
	}


	void ElectricNet::addConverter (key name, ConverterType type, VarKind cvType, double cvValue, double efficiency)
	{
		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.addRoot(name, newConverter_ptr);
	}


	void ElectricNet::insertConverter (key name, key sourceName, ConverterType type, VarKind cvType, double cvValue, double efficiency)
	{
		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.insertDesc(name, sourceName, newConverter_ptr);
	}


	void ElectricNet::insertConverter (key name, key sourceName, key sinkName, ConverterType type, VarKind cvType, double cvValue, double efficiency)
	{
		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.insertDesc(name, sourceName, sinkName, newConverter_ptr);
	}


	void ElectricNet::addLoad (key name, key sourceName, LoadType type, double param)
	{
		if (type == LoadType::DIODE)     throw exception("Diode load must have two parameters");
		if (type == LoadType::ENERGY)    throw exception("Energy load must have two parameters");

		auto newLoad_ptr = make_shared<OneParamLoad>(type, param);
		net.pushBackLeaf(name, sourceName, newLoad_ptr);
	}


	void ElectricNet::addLoad (key name, LoadType type, double param)
	{
		if (type == LoadType::DIODE)     throw exception("Diode load must have two parameters");
		if (type == LoadType::ENERGY)    throw exception("Energy load must have two parameters");

		auto newLoad_ptr = make_shared<OneParamLoad>(type, param);
		net.addRoot(name, newLoad_ptr);
	}


	void ElectricNet::addLoad (key name, key sourceName, LoadType type, double mainParam, double secondaryParam)
	{
		if (type == LoadType::RESISTIVE)     throw exception("Resistive load can have only one parameter");
		if (type == LoadType::CONSTANT_CURRENT)    throw exception("Constant current load can have only one parameter");

		auto newLoad_ptr = make_shared<TwoParamLoad>(type, mainParam, secondaryParam);
		net.pushBackLeaf(name, sourceName, newLoad_ptr);
	}


	void ElectricNet::addLoad (key name, LoadType type, double mainParam, double secondaryParam)
	{
		if (type == LoadType::RESISTIVE)     throw exception("Resistive load can have only one parameter");
		if (type == LoadType::CONSTANT_CURRENT)    throw exception("Constant current load can have only one parameter");

		auto newLoad_ptr = make_shared<TwoParamLoad>(type, mainParam, secondaryParam);
		net.addRoot(name, newLoad_ptr);
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

		if ( net.isRoot(name) )
			net.popFrontRoot(name);
		else
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


	void ElectricNet::moveConverter (key name, key newSourceName)
	{
		net.moveNode(name, newSourceName);
	}


	void ElectricNet::moveConverter (key name, key newSourceName, key newSinksSourceName)
	{
		net.moveNode(name, newSourceName, newSinksSourceName);
	}


	void ElectricNet::freeConverter (key name)
	{
		net.freeNode(name);
	}
	 

	void ElectricNet::freeConverter (key name, key newSinksSourceName)
	{
		net.freeNode(name, newSinksSourceName);
	}
	 

	void ElectricNet::moveLoad (key name, key newSourceName)
	{
		net.moveLeaf(name, newSourceName);
	}
	 

	void ElectricNet::freeLoad (key name)
	{
		net.freeLeaf(name);
	}


	void ElectricNet::moveSubnet (key headerName, key newSourceName)
	{
		net.moveSubtree(headerName, newSourceName);
	}


	void ElectricNet::freeSubnet (key headerName)
	{
		net.freeSubtree(headerName);
	}


	void ElectricNet::moveNode (key name, key newSourceName)
	{
		net.moveNode(name, newSourceName);
	}


	void ElectricNet::moveNode (key name, key newSourceName, key newSinksSourceName)
	{
		net.moveNode(name, newSourceName, newSinksSourceName);
	}


	void ElectricNet::freeNode (key name)
	{
		net.freeNode(name);
	}


	void ElectricNet::freeNode (key name, key newSinksSourceName)
	{
		net.freeNode(name, newSinksSourceName);
	}


	void ElectricNet::renameNode (key name, key newName)
	{
		net.renameNode(name, newName);
	}


	void ElectricNet::setSourceCvType (key name, VarKind newType)
	{
		auto source_ptr = dynamic_pointer_cast<Source>( net[name] );
		source_ptr->cvKind = newType;
	}


	void ElectricNet::setSourceCvValue (key name, double newValue)
	{
		auto source_ptr = dynamic_pointer_cast<Source>( net[name] );
		source_ptr->cvValue = newValue;
	}


	void ElectricNet::setConverterType (key name, ConverterType newType)
	{
		auto converter_ptr = dynamic_pointer_cast<Converter>( net[name] );
		converter_ptr->type = newType;
	}


	void ElectricNet::setConverterEfficiency (key name, double newEfficiency)
	{
		auto converter_ptr = dynamic_pointer_cast<Converter>( net[name] );
		converter_ptr->efficiency = newEfficiency;
	}


	void ElectricNet::setLoadType (key name, LoadType newType)
	{
		auto load_ptr = dynamic_pointer_cast<Load>( net[name] );
		load_ptr->type = newType;
	}


	void ElectricNet::setLoadResistance (key name, double resistance)
	{
		auto load_ptr = dynamic_pointer_cast<OneParamLoad>( net[name] );
		load_ptr->param = resistance;
	}


	void ElectricNet::setLoadCurrent (key name, double current)
	{
		auto load_ptr = dynamic_pointer_cast<OneParamLoad>( net[name] );
		load_ptr->param = current;
	}


	void ElectricNet::setLoadForawrdVoltage (key name, double forwardVoltage)
	{
		auto load_ptr = dynamic_pointer_cast<TwoParamLoad>( net[name] );
		load_ptr->mainParam = forwardVoltage;
	}


	void ElectricNet::setLoadForwardCurrent (key name, double forwardCurrent)
	{
		auto load_ptr = dynamic_pointer_cast<TwoParamLoad>( net[name] );
		load_ptr->secondaryParam = forwardCurrent;
	}


	void ElectricNet::setLoadNomPower (key name, double nomPower)
	{
		auto load_ptr = dynamic_pointer_cast<TwoParamLoad>( net[name] );
		load_ptr->mainParam = nomPower;
	}


	void ElectricNet::setLoadNomVoltage (key name, double nomVoltage)
	{
		auto load_ptr = dynamic_pointer_cast<TwoParamLoad>( net[name] );
		load_ptr->secondaryParam = nomVoltage;
	}


	DeviceType ElectricNet::getNodeType (key name)
	{
		auto node_ptr = net.at(name);
		auto type = node_ptr->type;
		return type;
	}


	InputData ElectricNet::getInputData (key inputName)
	{
		InputData data;

		AUTO_CONST_REF node = net.at(inputName);
		AUTO_CONST_REF input = dynamic_pointer_cast<Input>(node);

		data.name = inputName;

		data.type = input->cvKind;
		data.value = input->cvValue;

		return data;
	}


	ConverterData ElectricNet::getConverterData (key converterName)
	{
		ConverterData data;

		AUTO_CONST_REF node = net.at(converterName);
		AUTO_CONST_REF converter = dynamic_pointer_cast<Converter>(node);

		data.name = converterName;
		data.nestingLevel = net.getNestingLevel(converterName);

		data.cvType = converter->Source::cvKind;
		data.value = converter->cvValue;
		data.type = converter->type;
		data.efficiency = converter->efficiency;

		return data;
	}


	ResistiveLoadData ElectricNet::getResistiveLoadData (key loadName)
	{
		ResistiveLoadData data;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<OneParamLoad>(node);

		data.name = loadName;
		data.nestingLevel = net.getNestingLevel(loadName);

		data.resistance = load->param;

		return data;
	}


	ConstantCurrentLoadData ElectricNet::getConstantCurrentLoadData (key loadName)
	{
		ConstantCurrentLoadData data;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<OneParamLoad>(node);

		data.name = loadName;
		data.nestingLevel = net.getNestingLevel(loadName);

		data.current = load->param;

		return data;
	}


	DiodeLoadData ElectricNet::getDiodeLoadData (key loadName)
	{
		DiodeLoadData data;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<TwoParamLoad>(node);

		data.name = loadName;
		data.nestingLevel = net.getNestingLevel(loadName);

		data.forwardVoltage = load->mainParam;
		data.forwardCurrent = load->secondaryParam;

		return data;
	}


	EnergyLoadData ElectricNet::getEnergyLoadData (key loadName)
	{
		EnergyLoadData data;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<TwoParamLoad>(node);

		data.name = loadName;
		data.nestingLevel = net.getNestingLevel(loadName);

		data.nominalPower = load->mainParam; 
		data.nominalVoltage = load->secondaryParam; 

		return data;
	}


	bool ElectricNet::isLoadExsist (key name)
	{
		if (net.isExsist(name))
		{
			auto type = net.at(name) ->type;
			if (type == DeviceType::LOAD)
				return true;
		}
		return false;
	}


	LoadType ElectricNet::getLoadType (key name)
	{
		auto load_ptr = dynamic_pointer_cast<Load>( net[name] );
		auto type = load_ptr->type;
		return type;
	}


	string ElectricNet::getTitle ()
	{
		return title;
	}


	void ElectricNet::rename (string newTitle)
	{
		title = newTitle;
	}


	void ElectricNet::calculte ()
	{
		updateCalculations();
	}


	InputResults ElectricNet::getInputResults (key inputName) const
	{
		InputResults results;

		AUTO_CONST_REF node = net.at(inputName);
		AUTO_CONST_REF input = dynamic_pointer_cast<Input>(node);

		results.name = inputName;

		results.type = input->cvKind;
		results.value = input->cvValue;

		results.avValue = input->avValue;

		return results;
	}


	ConverterResults ElectricNet::getConverterResults (key convertertName) const
	{
		ConverterResults results;

		AUTO_CONST_REF node = net.at(convertertName);
		AUTO_CONST_REF converter = dynamic_pointer_cast<Converter>(node);

		results.name = convertertName;
		results.nestingLevel = net.getNestingLevel(convertertName);

		results.cvType = converter->Source::cvKind;
		results.value = converter->cvValue;
		results.type = converter->type;

		results.avValue = converter->avValue;
		results.inputValue = converter->inputValue;

		return results;
	}


	ResistiveLoadResults ElectricNet::getResistiveLoadResults (key loadName) const
	{
		ResistiveLoadResults results;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<OneParamLoad>(node);

		results.name = loadName;
		results.nestingLevel = net.getNestingLevel(loadName);

		results.resistance = load->param;

		results.inputValue = load->inputValue;
		results.inputVarType = calcInputVarTypeByParent(loadName);

		return results;
	}


	ConstantCurrentLoadResults ElectricNet::getConstantCurrentLoadResults (key loadName) const
	{
		ConstantCurrentLoadResults results;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<OneParamLoad>(node);

		results.name = loadName;
		results.nestingLevel = net.getNestingLevel(loadName);

		results.current = load->param;

		return results;
	}
		
		
	void ElectricNet::iterateAndExecuteForEach (function<void (key)> functor)
	{
		for (AUTO_CONST_REF node : net)
			functor(node.first);
	}




	void ElectricNet::updateCalculations ()
	{
		for (ElectricForest::desces_group_iterator root_it = net.dgbegin(); root_it != net.dgend(); root_it++)
			calculateAndUpdateGivenParams(root_it);
	}


	double ElectricNet::calculateAndUpdateGivenParams (Desc_it source_it)
	{
		double accOutputCurrent(0.0);
		for (ElectricForest::desces_group_iterator desc_it = net.dgbegin((*source_it).first); 
			                                       desc_it != net.dgend((*source_it).first); desc_it++)
			accOutputCurrent += calculateConsumption (desc_it, source_it);

		writeAvValueToSource(accOutputCurrent, (*source_it).first);

		return accOutputCurrent;
	}
	

	void ElectricNet::writeAvValueToSource (double newAvValue, key sourceName)
	{
		AUTO_CONST_REF source = dynamic_pointer_cast<Source>(net.at(sourceName));
		source->avValue = newAvValue;
	}
	

	void ElectricNet::writeInputValueToConverter (double newInputValue, key converterName)
	{
		AUTO_CONST_REF source = dynamic_pointer_cast<Converter>(net.at(converterName));
		source->inputValue = newInputValue;
	}


	void ElectricNet::writeInputValueToLoad (double newInputValue, key loadName)
	{
		AUTO_CONST_REF source = dynamic_pointer_cast<OneParamLoad>(net.at(loadName));
		source->inputValue = newInputValue;
	}


	double ElectricNet::calculateConsumption (Desc_it sink_it, Desc_it source_it)
	{
		DeviceType type = (*sink_it).second->type;

		switch (type)
		{
			case DeviceType::CONVERTER:
			{
				calculateAndUpdateGivenParams(sink_it);
				double inputCurrent = reduceOutputToInput(sink_it, source_it);
				writeInputValueToConverter(inputCurrent, (*sink_it).first);
				return inputCurrent;
			}

			case DeviceType::LOAD:
			{
				double loadCurrent = calculateLoadConsumption(sink_it, source_it);
				return loadCurrent;
			}
		}
		return 1;
	}


	double ElectricNet::reduceOutputToInput (Desc_it sink_it, Desc_it source_it)
	{
		auto sink = dynamic_pointer_cast<Converter>( (*sink_it).second );
		auto source = dynamic_pointer_cast<Source>( (*source_it).second );

		double outputVoltage(sink->cvValue);
		double outputCurrent(sink->avValue);
		double efficiency(sink->efficiency);
		double inputVoltage(source->cvValue);

		double inputCurrent = 100 * (outputVoltage * outputCurrent) / (efficiency * inputVoltage);
		return inputCurrent;
	}


	double ElectricNet::calculateLoadConsumption (Desc_it sink_it, Desc_it source_it)
	{
		auto sink = dynamic_pointer_cast<OneParamLoad>( (*sink_it).second );
		auto source = dynamic_pointer_cast<Source>( (*source_it).second );

		double resistance(sink->param);
		double voltage(source->cvValue);

		double loadCurrent = voltage / resistance;
		writeInputValueToLoad(loadCurrent, (*sink_it).first);
		return loadCurrent;
	}


	VarKind ElectricNet::calcInputVarTypeByParent (key loadName) const
	{
		auto parent_ptr = net.getParent(loadName);
		auto type = dynamic_pointer_cast<Source>(parent_ptr)->cvKind;
		return type;
	}

	





	ElectricNet::ElectricNode::ElectricNode (DeviceType tp)
		: type(tp)    {;}





	ElectricNet::Source::Source (DeviceType devType, VarKind tp, double value)
		: ElectricNode(devType), cvKind(tp), cvValue(value)    {;}





	ElectricNet::Input::Input (VarKind type, double cvValue)
		: Source(DeviceType::INPUT, type, cvValue)    {;}





	ElectricNet::Converter::Converter (VarKind cvType, double value, ConverterType tp, double effcnc)
		: Source(DeviceType::CONVERTER, cvType, value), type(tp), efficiency(effcnc)    {;}





	ElectricNet::Load::Load (LoadType tp)
		: ElectricNode(DeviceType::LOAD), type(tp)    {;}





	ElectricNet::OneParamLoad::OneParamLoad (LoadType tp, double param)
		: Load(tp), param(param)    
	{
		if (tp == LoadType::DIODE)     throw exception("Diode load must have two parameters");
		if (tp == LoadType::ENERGY)    throw exception("Energy load must have two parameters");
	}
	




	ElectricNet::TwoParamLoad::TwoParamLoad(LoadType tp, double firstParam, double secondParam)
		: Load(tp), mainParam(firstParam), secondaryParam(secondParam)    
	{
		if (tp == LoadType::RESISTIVE)    throw exception("Resistive load can have only one parameter");
		if (tp == LoadType::CONSTANT_CURRENT)    throw exception("Constant current load can have only one parameter");
	}

}
