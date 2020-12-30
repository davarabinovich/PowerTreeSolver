
#include "electric_net.h"




namespace electric_net
{

	ElectricNet::ElectricNet (string nm)
		: title(nm)    { ; }


	 

	void ElectricNet::addInput (key name, VarKind type, double cvValue)
	{
		isStoragedResultsActual = false;

#pragma todo all checking
		auto newInput_ptr = make_shared<Input>(type, cvValue);
		net.addRoot(name, newInput_ptr);
	}

	void ElectricNet::addConverter (key name, key sourceName, ConverterType type, VarKind cvType, double cvValue, double efficiency)
	{
		isStoragedResultsActual = false;

		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.pushBackLeaf(name, sourceName, newConverter_ptr);
	}


	void ElectricNet::addConverter (key name, ConverterType type, VarKind cvType, double cvValue, double efficiency)
	{
		isStoragedResultsActual = false;

		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.addRoot(name, newConverter_ptr);
	}


	void ElectricNet::insertConverter (key name, key sourceName, ConverterType type, VarKind cvType, double cvValue, double efficiency)
	{
		isStoragedResultsActual = false;

		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.insertDesc(name, sourceName, newConverter_ptr);
	}


	void ElectricNet::insertConverter (key name, key sourceName, key sinkName, ConverterType type, VarKind cvType, double cvValue, double efficiency)
	{
		isStoragedResultsActual = false;

		auto newConverter_ptr = make_shared<Converter>(cvType, cvValue, type, efficiency);
		net.insertDesc(name, sourceName, sinkName, newConverter_ptr);
	}


	void ElectricNet::addLoad (key name, key sourceName, LoadType type, double param)
	{
		if (type == LoadType::DIODE)     throw exception("Diode load must have two parameters");


		isStoragedResultsActual = false;

		auto newLoad_ptr = make_shared<OneParamLoad>(type, param);
		net.pushBackLeaf(name, sourceName, newLoad_ptr);
	}


	void ElectricNet::addLoad (key name, LoadType type, double param)
	{
		if (type == LoadType::DIODE)     throw exception("Diode load must have two parameters");


		isStoragedResultsActual = false;

		auto newLoad_ptr = make_shared<OneParamLoad>(type, param);
		net.addRoot(name, newLoad_ptr);
	}


	void ElectricNet::addLoad (key name, key sourceName, LoadType type, double mainParam, double secondaryParam)
	{
		if (type == LoadType::RESISTIVE)     throw exception("Resistive load can have only one parameter");
		if (type == LoadType::CONSTANT_CURRENT)    throw exception("Constant current load can have only one parameter");


		isStoragedResultsActual = false;

		auto newLoad_ptr = make_shared<TwoParamsLoad>(type, mainParam, secondaryParam);
		net.pushBackLeaf(name, sourceName, newLoad_ptr);
	}


	void ElectricNet::addLoad (key name, LoadType type, double mainParam, double secondaryParam)
	{
		if (type == LoadType::RESISTIVE)     throw exception("Resistive load can have only one parameter");
		if (type == LoadType::CONSTANT_CURRENT)    throw exception("Constant current load can have only one parameter");


		isStoragedResultsActual = false;

		auto newLoad_ptr = make_shared<TwoParamsLoad>(type, mainParam, secondaryParam);
		net.addRoot(name, newLoad_ptr);
	}


	void ElectricNet::deleteInput (key name, key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveAllDesces(name, newSourceName);
		net.popFrontRoot(name);
	}


	void ElectricNet::deleteInput (key name)
	{
		isStoragedResultsActual = false;

		net.popFrontRoot(name);
	}

	void ElectricNet::deleteConverter (key name, key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveAllDesces(name, newSourceName);
		net.popBackLeaf(name);
	}


	void ElectricNet::deleteConverter (key name)
	{
		isStoragedResultsActual = false;

		net.freeAllDesces(name);
		net.popBackLeaf(name);
	}


	void ElectricNet::deleteLoad (key name)
	{
		isStoragedResultsActual = false;

		net.popBackLeaf(name);
	}


	void ElectricNet::deleteNode (key name, key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveAllDesces(name, newSourceName);
		net.popBackLeaf(name);
	}


	void ElectricNet::deleteNode (key name)
	{
		isStoragedResultsActual = false;

		net.freeAllDesces(name);

		if ( net.isRoot(name) )
			net.popFrontRoot(name);
		else
			net.popBackLeaf(name);
	}


	void ElectricNet::deleteSubnet (key headerName)
	{
		isStoragedResultsActual = false;

		net.popBackSubtree(headerName);
	}


	void ElectricNet::deleteAllSinks (key sourceName)
	{
		isStoragedResultsActual = false;

		net.eraseAllDesces(sourceName);
	}


	void ElectricNet::moveConverter (key name, key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveNode(name, newSourceName);
	}


	void ElectricNet::moveConverter (key name, key newSourceName, key newSinksSourceName)
	{
		isStoragedResultsActual = false;

		net.moveNode(name, newSourceName, newSinksSourceName);
	}


	void ElectricNet::freeConverter (key name)
	{
		isStoragedResultsActual = false;

		net.freeNode(name);
	}
	 

	void ElectricNet::freeConverter (key name, key newSinksSourceName)
	{
		isStoragedResultsActual = false;

		net.freeNode(name, newSinksSourceName);
	}
	 

	void ElectricNet::moveLoad (key name, key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveLeaf(name, newSourceName);
	}
	 

	void ElectricNet::freeLoad (key name)
	{
		isStoragedResultsActual = false;

		net.freeLeaf(name);
	}


	void ElectricNet::moveSubnet (key headerName, key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveSubtree(headerName, newSourceName);
	}


	void ElectricNet::freeSubnet (key headerName)
	{
		isStoragedResultsActual = false;

		net.freeSubtree(headerName);
	}


	void ElectricNet::moveNode (key name, key newSourceName)
	{
		isStoragedResultsActual = false;

		net.moveNode(name, newSourceName);
	}


	void ElectricNet::moveNode (key name, key newSourceName, key newSinksSourceName)
	{
		isStoragedResultsActual = false;

		net.moveNode(name, newSourceName, newSinksSourceName);
	}


	void ElectricNet::freeNode (key name)
	{
		isStoragedResultsActual = false;

		net.freeNode(name);
	}


	void ElectricNet::freeNode (key name, key newSinksSourceName)
	{
		isStoragedResultsActual = false;

		net.freeNode(name, newSinksSourceName);
	}


	void ElectricNet::renameNode (key name, key newName)
	{
		isStoragedResultsActual = false;

		net.renameNode(name, newName);
	}


	void ElectricNet::setSourceCvType (key name, VarKind newType)
	{
		isStoragedResultsActual = false;

		auto source_ptr = dynamic_pointer_cast<Source>( net[name] );
		source_ptr->cvKind = newType;
	}


	void ElectricNet::setSourceCvValue (key name, double newValue)
	{
		isStoragedResultsActual = false;

		auto source_ptr = dynamic_pointer_cast<Source>( net[name] );
		source_ptr->cvValue = newValue;
	}


	void ElectricNet::setConverterType (key name, ConverterType newType)
	{
		isStoragedResultsActual = false;

		auto converter_ptr = dynamic_pointer_cast<Converter>( net[name] );
		converter_ptr->type = newType;
	}


	void ElectricNet::setConverterEfficiency (key name, double newEfficiency)
	{
		isStoragedResultsActual = false;

		auto converter_ptr = dynamic_pointer_cast<Converter>( net[name] );
		converter_ptr->efficiency = newEfficiency;
	}


	void ElectricNet::setLoadType (key name, LoadType newType)
	{
		isStoragedResultsActual = false;

		auto load_ptr = dynamic_pointer_cast<Load>( net[name] );
		load_ptr->type = newType;
	}


	void ElectricNet::setLoadResistance (key name, double resistance)
	{
		isStoragedResultsActual = false;

		auto load_ptr = dynamic_pointer_cast<OneParamLoad>( net[name] );
		load_ptr->param = resistance;
	}


	void ElectricNet::setLoadCurrent (key name, double current)
	{
		isStoragedResultsActual = false;

		auto load_ptr = dynamic_pointer_cast<OneParamLoad>( net[name] );
		load_ptr->param = current;
	}


	void ElectricNet::setLoadForawrdVoltage (key name, double forwardVoltage)
	{
		isStoragedResultsActual = false;

		auto load_ptr = dynamic_pointer_cast<TwoParamsLoad>( net[name] );
		load_ptr->mainParam = forwardVoltage;
	}


	void ElectricNet::setLoadForwardCurrent (key name, double forwardCurrent)
	{
		isStoragedResultsActual = false;

		auto load_ptr = dynamic_pointer_cast<TwoParamsLoad>( net[name] );
		load_ptr->secondaryParam = forwardCurrent;
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

		data.cvKind = input->cvKind;
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

		data.cvKind = converter->Source::cvKind;
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
		AUTO_CONST_REF load = dynamic_pointer_cast<TwoParamsLoad>(node);

		data.name = loadName;
		data.nestingLevel = net.getNestingLevel(loadName);

		data.forwardVoltage = load->mainParam;
		data.forwardCurrent = load->secondaryParam;

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
		if (!isStoragedResultsActual)
		{
			updateCalculations();
			isStoragedResultsActual = true;
		}
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
		results.inputVarKind = calcInputVarTypeByParent(loadName);

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

		results.inputVoltage = load->inputValue;

		return results;
	}
		
	
	DiodeLoadResults ElectricNet::getDiodeLoadResults (key loadName) const
	{
		DiodeLoadResults results;

		AUTO_CONST_REF node = net.at(loadName);
		AUTO_CONST_REF load = dynamic_pointer_cast<TwoParamsLoad>(node);

		results.name = loadName;
		results.nestingLevel = net.getNestingLevel(loadName);

		results.forwardVoltage = load->mainParam;
		results.forwardCurrent = load->secondaryParam;

		return results;
	}


	void ElectricNet::iterateAndExecuteForEach (function<void (key)> functor)
	{
		for (AUTO_CONST_REF node : net)
			functor(node.first);
	}


	bool ElectricNet::operator != (const ElectricNet & other) const
	{
		for (auto it = net.begin(); it != net.end(); it++)
		{

			auto isNodeEqualTo = [it](pair<key, Node_ptr> otherPair) -> bool
			{
				if ((*it).first != otherPair.first)
					return false;
				if (!isNodesEqual((*it).second, otherPair.second))
					return false;

				return true;
			};



			if (find_if(other.net.begin(), other.net.end(), isNodeEqualTo) == other.net.end())
				return true;
		}

		return false;
	}




	void ElectricNet::updateCalculations ()
	{
		for (ElectricForest::desces_group_iterator root_it = net.dgbegin(); root_it != net.dgend(); root_it++)
			calculateAndUpdateGivenParams(root_it);
	}


	double ElectricNet::calculateAndUpdateGivenParams (Desc_it source_it)
	{
		auto sourceVarKind = dynamic_pointer_cast<Source>((*source_it).second)->cvKind;

		switch (sourceVarKind)
		{
			case VarKind::VOLTAGE:
			{
				double accOutputCurrent = 0.0;
				for (ElectricForest::desces_group_iterator desc_it = net.dgbegin((*source_it).first); 
					                                       desc_it != net.dgend((*source_it).first); desc_it++)
					accOutputCurrent += calculateConsumption (desc_it, source_it);

				writeAvValueToSource(accOutputCurrent, (*source_it).first);

				return accOutputCurrent;
			}

			case VarKind::CURRENT:
			{
				auto firstDesc_it = net.dgbegin((*source_it).first);
				auto descesType = dynamic_pointer_cast<Load>((*firstDesc_it).second) ->type;

				switch (descesType)
				{
					case LoadType::RESISTIVE:
					{
						double accLoadsTotalConductivity = 0.0;
						for (ElectricForest::desces_group_iterator desc_it = net.dgbegin((*source_it).first); 
							                                       desc_it != net.dgend((*source_it).first); desc_it++)
						{
							double loadResistance = dynamic_pointer_cast<OneParamLoad>((*desc_it).second) ->param;
							accLoadsTotalConductivity += 1 / loadResistance;
						}
						
						double sourceCurrent = dynamic_pointer_cast<Converter>((*source_it).second) ->cvValue;
						double outputVoltage = sourceCurrent / accLoadsTotalConductivity;
						writeAvValueToSource(outputVoltage, (*source_it).first);
						
						for (ElectricForest::desces_group_iterator desc_it = net.dgbegin((*source_it).first);
							                                       desc_it != net.dgend((*source_it).first); desc_it++)
						{
							double loadResistance = dynamic_pointer_cast<OneParamLoad>((*desc_it).second) ->param;
							double loadCurrent = outputVoltage / loadResistance;
							writeInputValueToResistiveLoad(loadCurrent, (*desc_it).first);
						}
						
						return outputVoltage;
					}

					case LoadType::CONSTANT_CURRENT:
					{
						throw exception("\"calculateAndUpdateGivenParams\": Constant current load can't be supplied from current source");
					}

					case LoadType::DIODE:
					{
						double forwardVoltage = dynamic_pointer_cast<TwoParamsLoad>((*firstDesc_it).second) ->mainParam;
						return forwardVoltage;
					}


					default:
						throw exception("Invalid type of load");
				}
			}


			default:
				throw exception("invalind kind of controlled variable");
		}
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


	void ElectricNet::writeInputValueToResistiveLoad (double newInputValue, key loadName)
	{
		AUTO_CONST_REF source = dynamic_pointer_cast<OneParamLoad>(net.at(loadName));
		source->inputValue = newInputValue;
	}


	double ElectricNet::calculateConsumption (Desc_it consumer_it, Desc_it source_it)
	{
		DeviceType type = (*consumer_it).second->type;

		switch (type)
		{
			case DeviceType::CONVERTER:
			{
				calculateAndUpdateGivenParams(consumer_it);
				double inputValue = reduceOutputToInput(consumer_it, source_it);
				writeInputValueToConverter(inputValue, (*consumer_it).first);
				return inputValue;
			}

			case DeviceType::LOAD:
			{
				double loadCurrent = calculateLoadConsumptionDrivenByVoltageSource(consumer_it, source_it);
				return loadCurrent;
			}


			default:
				throw exception("Input doesn't consume");
		}
	}


	double ElectricNet::reduceOutputToInput (Desc_it sink_it, Desc_it source_it)
	{
		auto source = dynamic_pointer_cast<Source>( (*source_it).second );
		auto converter = dynamic_pointer_cast<Converter>( (*sink_it).second );
		auto converterType = converter->type;
		auto converterVarKind = converter->cvKind;

		double outputPower = (converter->cvValue) * (converter->avValue);
		double efficiency = converter->efficiency;
		double sourcesCv = source->cvValue;

		double inputValue;
		switch (converterType)
		{
			case ConverterType::PULSE:
				inputValue = 100 * outputPower / (efficiency * sourcesCv);
				break;

			case ConverterType::LINEAR:
				switch (converterVarKind)
				{
					case VarKind::VOLTAGE:
						inputValue = converter->avValue;
						break;

					case VarKind::CURRENT:
						inputValue = converter->cvValue;
						break;


					default:
						throw exception("Invalind kind of controlled variable");
				}
				break;


			default:
				throw exception("Invalid type of converter");
		}
		
		return inputValue;
	}


	double ElectricNet::calculateLoadConsumptionDrivenByVoltageSource (Desc_it load_it, Desc_it source_it)
	{
		auto loadType = dynamic_pointer_cast<Load>((*load_it).second) ->type;

		switch (loadType)
		{
			case LoadType::RESISTIVE:
			{
				double resistance = dynamic_pointer_cast<OneParamLoad>((*load_it).second) ->param;
				double voltage = dynamic_pointer_cast<Source>((*source_it).second) ->cvValue;

				double loadCurrent = voltage / resistance;
				writeInputValueToResistiveLoad(loadCurrent, (*load_it).first);
				return loadCurrent;
			}

			case LoadType::CONSTANT_CURRENT:
			{
				auto current = dynamic_pointer_cast<OneParamLoad>((*load_it).second) ->param;
				return current;
			}

			case LoadType::DIODE:
			{
				auto current = dynamic_pointer_cast<TwoParamsLoad>((*load_it).second) ->secondaryParam;
				return current;
			}

			default:
				throw exception("Invalid type of load");
		}
	}


	VarKind ElectricNet::calcInputVarTypeByParent (key loadName) const
	{
		auto parent_ptr = net.getParent(loadName);
		auto type = dynamic_pointer_cast<Source>(parent_ptr)->cvKind;
		return type;
	}




	bool ElectricNet::isNodesEqual (Node_ptr first_ptr, Node_ptr second_ptr)
	{
		const auto & first = *first_ptr;
		const auto & second = *second_ptr;

		if (first.type != second.type)
			return false;

		switch (first.type)
		{
			case DeviceType::INPUT:
			{
				const auto & firstInput = dynamic_cast<const Input &>(first);
				const auto & secondInput = dynamic_cast<const Input &>(second);

				if (firstInput.cvKind != secondInput.cvKind)
					return false;
				if (firstInput.cvValue != secondInput.cvValue)
					return false;

				break;
			}

			case DeviceType::CONVERTER:
			{
				const auto & firstConverter = dynamic_cast<const Converter &>(first);
				const auto & secondConverter = dynamic_cast<const Converter &>(second);

				if (firstConverter.cvKind != secondConverter.cvKind)
					return false;
				if (firstConverter.cvValue != secondConverter.cvValue)
					return false;
				if (firstConverter.type != secondConverter.type)
					return false;

				if (firstConverter.type == ConverterType::PULSE)
					if (firstConverter.efficiency != secondConverter.efficiency)
						return false;


				break;
			}

			case DeviceType::LOAD:
			{
				const auto & firstLoad = dynamic_cast<const Load &>(first);
				const auto & secondLoad = dynamic_cast<const Load &>(second);

				if (firstLoad.type != secondLoad.type)
					return false;

				switch (firstLoad.type)
				{
					case LoadType::RESISTIVE: [[__fallthrough]]
					case LoadType::CONSTANT_CURRENT:
					{
						const auto & firstOneParamLoad = dynamic_cast<const OneParamLoad &>(first);
						const auto & secondOneParamLoad = dynamic_cast<const OneParamLoad &>(second);

						if (firstOneParamLoad.param != secondOneParamLoad.param)
							return false;


						break;
					}

					case LoadType::DIODE:
					{
						const auto & firstTwoParamsLoad = dynamic_cast<const TwoParamsLoad &>(first);
						const auto & secondTwoParamsLoad = dynamic_cast<const TwoParamsLoad &>(second);

						if (firstTwoParamsLoad.mainParam != secondTwoParamsLoad.mainParam)
							return false;
						if (firstTwoParamsLoad.secondaryParam != secondTwoParamsLoad.secondaryParam)
							return false;


						break;
					}
						

					default:
						throw exception("Invalid type of load");
				}

				
				break;
			}


			default:
				throw exception("Invalid type of device");
		}

		return true;
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
	}
	




	ElectricNet::TwoParamsLoad::TwoParamsLoad(LoadType tp, double firstParam, double secondParam)
		: Load(tp), mainParam(firstParam), secondaryParam(secondParam)    
	{
		if (tp == LoadType::RESISTIVE)    throw exception("Resistive load can have only one parameter");
		if (tp == LoadType::CONSTANT_CURRENT)    throw exception("Constant current load can have only one parameter");
	}

}
